#include "gecko/thread/ThreadPool.h"
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

        // note(ben): From here, we must leave "Starting" no matter what
        // (success or failure), and we must notify_all() so the destructor
        // can't hang.
        try
        {
            for (size_t i = 0; i < m_threads.size(); ++i)
                m_threads[i].emplace(&ThreadPool::ThreadLoop, this);
        }
        catch (...)
        {
            // note(ben): This is a best-effort rollback: Perform roughly
            // the work of Stop(), except don't touch the threads that threw.
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
        m_mainState.store(MainState::Started,   std::memory_order_release);
        m_mainState.notify_all();
        return Result::Success;
    }

    ThreadPool::Result ThreadPool::Stop()
    {
        MainState expected = MainState::Started;
        if (!m_mainState.compare_exchange_strong(
            expected, MainState::Stopping, std::memory_order_acq_rel))
            return Result::StopDenied_NotStarted;

        std::unique_lock<std::shared_mutex> lk{ m_scheduling };
        m_sem.release(static_cast<std::ptrdiff_t>(m_threads.size()));
        return Result::Success;
    }

    ThreadPool::Result ThreadPool::Schedule(Job& job)
    {
        if (m_mainState.load(std::memory_order_acquire) != MainState::Started)
            return Result::ScheduleDenied_NotStarted;

        std::shared_lock<std::shared_mutex> lk{ m_scheduling };

        if (m_mainState.load(std::memory_order_acquire) != MainState::Started)
            return Result::ScheduleDenied_NotStarted;

        if (!m_jobs.TryEnqueue(job))
            return Result::ScheduleFailed_QueueFull;

        m_sem.release();
        return Result::Success;
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

            // Publish join completion first (prevents Start() from seeing Stopped early)
            m_joinState.store(JoinState::Joined, std::memory_order_release);
            m_joinState.notify_all();

            // Then publish stopped (Start() gates on this)
            m_mainState.store(MainState::Stopped, std::memory_order_release);
            m_mainState.notify_all();

            return Result::Success;
        }

        if (expected == JoinState::Joining)
        {
            m_joinState.wait(JoinState::Joining, std::memory_order_acquire);
            return Result::Success;
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
                    try { (*job)(); } catch (...) { }

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