#include "gecko/thread/ThreadPool.h"
#include <assert.h>
#include <optional>

namespace Gecko::API::Thread
{
    ThreadPool::~ThreadPool()
    {
        m_execState.store(ExecState::Terminating, std::memory_order_release);

        MainState expected = MainState::Stopped;
        if (m_mainState.compare_exchange_strong(
            expected, MainState::Halted, std::memory_order_acq_rel))
        {
            return;
        }

        m_mainState.wait(MainState::Starting, std::memory_order_acquire);

        try
        {
            (void)Stop();
            (void)Join();
        }
        catch (...) { }
    }

    ThreadPool::Result ThreadPool::Start()
    {
        if (m_execState.load(std::memory_order_acquire) == ExecState::Terminating)
            return Result::StartDenied_Terminating;

        MainState expected = MainState::Stopped;
        if (!m_mainState.compare_exchange_strong(
            expected, MainState::Starting, std::memory_order_acq_rel))
        {
            return Result::StartDenied_NotStopped;
        }

        /*
            note(ben): From here, we must leave "Starting" no matter what
            (success or failure), and we must notify_all() so the destructor
            can't hang.
        */
        try
        {
            for (size_t i = 0; i < m_threads.size(); ++i)
                m_threads[i].emplace(&ThreadPool::ThreadLoop, this);
        }
        catch (...)
        {
            /*
                note(ben): This is a best-effort rollback: Perform roughly
                the work of Stop(), except don't touch the threads that threw.
            */
            m_mainState.store(MainState::Stopping, std::memory_order_release);
            m_mainState.notify_all();

            m_sem.release(static_cast<std::ptrdiff_t>(m_threads.size()));

            for (auto& t : m_threads)
            {
                if (t && t->joinable())
                    t->join();
                t.reset();
            }

            m_joinState.store(JoinState::Joined, std::memory_order_release);
            m_joinState.notify_all();

            m_mainState.store(MainState::Stopped, std::memory_order_release);
            m_mainState.notify_all();
            return Result::StartFailed_ThreadCreate;
        }

        m_joinState.store(JoinState::NotJoined, std::memory_order_release);
        m_mainState.store(MainState::Running,   std::memory_order_release);
        m_mainState.notify_all();
        return Result::OK;
    }

    ThreadPool::Result ThreadPool::Stop()
    {
        MainState expected = MainState::Running;
        if (!m_mainState.compare_exchange_strong(
            expected, MainState::Stopping, std::memory_order_acq_rel))
            return Result::StopDenied_NotStarted;

        std::unique_lock<std::shared_mutex> lk{ m_scheduling };
        m_sem.release(static_cast<std::ptrdiff_t>(m_threads.size()));
        return Result::OK;
    }

    ThreadPool::Result ThreadPool::ScheduleJob(std::function<void ()>& func, JobHandle* outHandle)
    {
        if (m_mainState.load(std::memory_order_relaxed) != MainState::Running)
            return Result::ScheduleDenied_NotStarted;

        std::shared_lock<std::shared_mutex> lk{ m_scheduling };

        if (m_mainState.load(std::memory_order_acquire) != MainState::Running)
            return Result::ScheduleDenied_NotStarted;

        decltype(m_jobs)::EnqTicket enqueueTicket;
        if (!m_jobs.TryEnqueueAcquire(&enqueueTicket))
            return Result::ScheduleFailed_QueueFull;

        size_t ticketID{ m_nextTicketID.fetch_add(1) };
        size_t ticketIndex{ ticketID & (QueueSize - 1) };

        for (;; ++ticketIndex)
        {
            size_t swap = NPos;
            if (m_inflightTickets[ticketIndex].compare_exchange_weak(
                swap, ticketID, std::memory_order_relaxed, std::memory_order_relaxed))
                break;
        }

        outHandle->ticketID    = ticketID;
        outHandle->ticketIndex = ticketIndex;

        m_jobs.EnqueuePublish(enqueueTicket, Job {
            .func   = std::move(func),
            .handle = *outHandle
        });

        m_sem.release();
        return Result::OK;
    }

    ThreadPool::Result ThreadPool::WaitForJobCompletion(const JobHandle& handle) noexcept
    {
        assert(handle.ticketIndex < m_inflightTickets.size());
        m_inflightTickets[handle.ticketIndex].wait(handle.ticketID, std::memory_order_acquire);
        return Result::OK;
    }

    bool ThreadPool::IsJobCompleted(const JobHandle& handle) const noexcept
    {
        return m_inflightTickets[handle.ticketIndex].load(std::memory_order_acquire) != handle.ticketID;
    }

    ThreadPool::Result ThreadPool::Join()
    {
        JoinState expected = JoinState::NotJoined;
        if (m_joinState.compare_exchange_strong(
            expected, JoinState::Joining, std::memory_order_acq_rel))
        {
            for (auto& t : m_threads)
            {
                if (t && t->joinable())
                    t->join();
                t.reset();
            }

            /*
                Publish join completion first (prevents Start() from seeing 
                Stopped early)
            */
            m_joinState.store(JoinState::Joined, std::memory_order_release);
            m_joinState.notify_all();

            /*
                Then publish stopped (Start() gates on this)
            */
            m_mainState.store(MainState::Stopped, std::memory_order_release);
            m_mainState.notify_all();

            return Result::OK;
        }

        if (expected == JoinState::Joining)
        {
            m_joinState.wait(JoinState::Joining, std::memory_order_acquire);
            return Result::OK;
        }

        return Result::JoinDenied_AlreadyJoined;
    }

    void ThreadPool::ThreadLoop() noexcept
    {
        for (;;)
        {
            m_sem.acquire();

            for (;;)
            {
                if (auto job = m_jobs.TryDequeue())
                {
                    try { job->func(); } catch (...) { }

                    m_inflightTickets[job->handle.ticketIndex].store(
                        NPos, std::memory_order_release);

                    m_inflightTickets[job->handle.ticketIndex].notify_all();

                    if (m_sem.try_acquire())
                        continue;

                    break;
                }

                if (m_mainState.load(std::memory_order_acquire) == MainState::Stopping)
                    return;

                break;
            }
        }
    }
}