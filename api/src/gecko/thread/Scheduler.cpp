#include "gecko/thread/Scheduler.h"
#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace Gecko::API::Thread
{
    Scheduler::Result Scheduler::Start()
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                return Result::StartDenied_NotIdle;

            case State::NotRunning:
                assert(!m_schedulerThread);

                m_state = State::Running;
                m_schedulerThread.emplace(&Scheduler::ThreadLoop, this);
                return Result::OK;

            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::StartDenied_NotIdle;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::ScheduleJobAfter(std::chrono::nanoseconds delay,
                                                  std::function<void ()>&& func,
                                                  TaskHandle *outTask)
    {
        return ScheduleJobAt(TimePoint::clock::now() + delay, std::move(func), outTask);
    }

    Scheduler::Result Scheduler::ScheduleJobAt(TimePoint at,
                                               std::function<void ()>&& func,
                                               TaskHandle *outTask)
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                {
                    bool isNextDue{};

                    if (!m_queue.QueueTask(at, std::move(func), outTask, &isNextDue))
                        return Result::ScheduleFailed_FLQueueFull;

                    if (isNextDue)
                        m_cv.notify_one();

                    return Result::OK;
                }

            case State::NotRunning:
            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::ScheduleDenied_NotStarted;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::CancelJob(const TaskHandle& task)
    {
        std::unique_lock lk{ m_mutex };

        std::optional<ThreadPool::JobHandle> poolHandle;
        std::optional<TimePoint>             due;

        if (!m_queue.QueryTask(task, &due, &poolHandle))
            return Result::CancelFailed_AlreadyCompleted;

        if (!due)
            return Result::CancelFailed_CurrentlyRunning;

        bool wasNextDue{};
        m_queue.RemoveTask(task, &wasNextDue);

        if (wasNextDue)
            m_cv.notify_one();

        return Result::OK;
    }

    Scheduler::Result Scheduler::CancelJobOrWait(const TaskHandle& task)
    {
        std::unique_lock lk{ m_mutex };

        std::optional<ThreadPool::JobHandle> poolHandle;
        std::optional<TimePoint>             due;

        if (!m_queue.QueryTask(task, &due, &poolHandle))
            return Result::CancelFailed_AlreadyCompleted;

        assert(due.has_value() ^ poolHandle.has_value());

        if (poolHandle)
            m_pool->WaitForJobCompletion(*poolHandle);
        
        m_queue.RemoveTask(task);
        return Result::OK;
    }

    Scheduler::Result Scheduler::Join()
    {
        std::unique_lock lk{ m_mutex };

        if (!m_schedulerThread || !m_schedulerThread->joinable())
            return Result::JoinDenied_AlreadyJoined;

        assert(m_schedulerThread->get_id() != std::this_thread::get_id() &&
                "Join() should never be called from the scheduler thread.");

        auto thread = std::move(*m_schedulerThread);
        m_schedulerThread.reset();

        lk.unlock();
        thread.join();
        lk.lock();

        m_state = State::NotRunning;
        return Result::OK;
    }

    Scheduler::Result Scheduler::Shutdown(bool fast)
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                m_state = fast
                    ? State::ShuttingDownFast
                    : State::ShuttingDownSlow;
                m_cv.notify_one();
                return Result::OK;

            case State::NotRunning:
            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::ShutdownDenied_NotRunning;
        }

        std::terminate();
    }

    void Scheduler::ThreadLoop()
    {
        std::unique_lock lk{ m_mutex };

        while (true)
        {
            SchedulerFLQueue::TaskHandle task;
            TimePoint due;

            const bool hasDue = m_queue.NextDue(&task, &due);
            const auto wake = hasDue ? due : TimePoint::max();

            const bool timedOut =
                !m_cv.wait_until(lk, wake, [&] {
                    return
                        !hasDue && m_queue.AnyUnpooledTasks() ||
                        hasDue && !m_queue.IsNextDue(task) ||
                        m_state == State::ShuttingDownFast ||
                        m_state == State::ShuttingDownSlow && !m_queue.AnyUnpooledTasks();
                });

            if (m_state == State::Running ||
                m_state == State::ShuttingDownSlow)
            {
                if (timedOut && hasDue)
                    m_queue.PushToPool(task);
            }

            if ((m_state == State::ShuttingDownSlow && !m_queue.AnyUnpooledTasks()) ||
                (m_state == State::ShuttingDownFast))
            {
                m_queue.ClearTasks();
                return;
            }
        }
    }
}