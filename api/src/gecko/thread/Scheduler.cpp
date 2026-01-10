#include "gecko/thread/Scheduler.h"
#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace Gecko::API::Thread
{
    Scheduler::ScheduleBuffer::ScheduleBuffer()
    {
        for (size_t i = 0; i < m_tasks.size(); ++i)
            m_tasks[i].index = i;

        Reset();
    }

    bool Scheduler::ScheduleBuffer::AddTask(ThreadPool* threadPool,
                                            TimePoint due,
                                            TaskFunc&& func,
                                            TaskHandle* outHandle,
                                            bool* outIsNextDue)
    {
        assert(outHandle && outIsNextDue);
        *outIsNextDue = false;

        if (!m_freeHead)
        {
            // note(ben): It is 3:00 in the morning and I just need
            // to get this done, so this is a super naive implementation.

            // Find all jobs that are completed but not yet marked as
            // free and mark them as free.
            for (size_t i = 0; i < m_end; ++i)
            {
                if (auto& task = m_tasks[i];
                    task.poolHandle && threadPool->IsJobCompleted(*task.poolHandle))
                {
                    bool unused{};
                    RemoveTask(&task, &unused);
                }
            }

            // No jobs were completed; we really are out of room
            if (!m_freeHead)
                return false;
        }

        Task *task = m_freeHead;
        m_freeHead = m_freeHead->nextFree;

        task->ticket   = ++m_nextTicket;
        task->isFree   = false;
        task->nextFree = nullptr;

        task->due  = due;
        task->func = std::move(func);

        if (!m_nextDue || *task->due < *m_nextDue->due)
        {
            *outIsNextDue = true;
            m_nextDue     = task;
        }

        outHandle->index  = task->index;
        outHandle->ticket = task->ticket;

        m_end = std::max(m_end, task->index + 1);
        return true;
    }

    void Scheduler::ScheduleBuffer::RemoveTask(Task* task, bool *outWasNextDue)
    {
        task->ticket   = Ticket_Free;
        task->isFree   = true;
        task->nextFree = m_freeHead;
        m_freeHead     = task;

        task->due        = std::nullopt;
        task->poolHandle = std::nullopt;
        task->func       = {};

        if (task == m_nextDue)
        {
            GetNextDue(m_tasks, m_end, &m_nextDue, &m_end);
            *outWasNextDue = true;
        }

        *outWasNextDue = false;
    }

    void Scheduler::ScheduleBuffer::MarkTaskPushedToPool(Task* task,
                                                         const ThreadPool::JobHandle& poolHandle)
    {
        task->due        = std::nullopt;
        task->poolHandle = poolHandle;

        if (task == m_nextDue)
            GetNextDue(m_tasks, m_end, &m_nextDue, &m_end);
    }

    void Scheduler::ScheduleBuffer::Reset()
    {
        for (size_t i = 0; i < m_tasks.size() - 1; ++i)
        {
            m_tasks[i].isFree   = true;
            m_tasks[i].nextFree = &m_tasks[i + 1];

            m_tasks[i].func = {};
            m_tasks[i].due  = {};
        }

        m_tasks.back().isFree   = true;
        m_tasks.back().nextFree = nullptr;

        m_tasks.back().func = {};
        m_tasks.back().due  = {};

        m_end      = 0;
        m_freeHead = &m_tasks[0];
        m_nextDue  = nullptr;
    }

    void Scheduler::ScheduleBuffer::GetNextDue(std::array<Task, ScheduleQueueSize>& arr,
                                               size_t end,
                                               Task** outNextDue,
                                               size_t* outNewEnd)
    {
        *outNextDue = nullptr;
        *outNewEnd  = 0;

        for (size_t i = 0; i < end; ++i)
        {
            if (arr[i].isFree)
                continue;

            *outNewEnd = i + 1;

            if (!arr[i].due)
                continue;

            if (!*outNextDue || *arr[i].due < *(*outNextDue)->due)
                *outNextDue = &arr[i];
        }
    }

    Scheduler::Result Scheduler::Start()
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                return Result::StartDenied_NotIdle;

            case State::NotRunning:
                assert(!m_thread && "m_thread should always be nullopt when in the NotRunning state.");

                m_state = State::Running;
                m_thread.emplace(&Scheduler::ThreadLoop, this);
                return Result::Success;

            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::StartDenied_NotIdle;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::ScheduleJobAfter(std::chrono::nanoseconds delay,
                                                  TaskFunc&& func,
                                                  TaskHandle *outHandle)
    {
        return ScheduleJobAt(TimePoint::clock::now() + delay, std::move(func), outHandle);
    }

    Scheduler::Result Scheduler::ScheduleJobAt(TimePoint at,
                                               TaskFunc&& func,
                                               TaskHandle *outHandle)
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                {
                    bool isNextDue{};

                    if (!m_buffer.AddTask(m_pool, at, std::move(func), outHandle, &isNextDue))
                        return Result::ScheduleFailed_QueueFull;

                    if (isNextDue)
                        m_cv.notify_one();

                    return Result::Success;
                }

            case State::NotRunning:
            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::ScheduleDenied_NotStarted;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::CancelJob(const TaskHandle& handle)
    {
        std::unique_lock lk{ m_mutex };

        Task* task;

        if (!m_buffer.TryGetTaskFromHandle(handle, &task))
            return Result::Success;

        if (task->due)
        {
            bool wasNextDue{};
            m_buffer.RemoveTask(task, &wasNextDue);

            if (wasNextDue)
                m_cv.notify_one();

            return Result::Success;
        }
        
        return Result::CancelFailed_NotScheduled;

        lk.unlock();
        m_pool->WaitForJobCompletion(*task->poolHandle);

        return Result::Success;
    }

    Scheduler::Result Scheduler::CancelJobOrWait(const TaskHandle& handle)
    {

        switch (auto r = CancelJob(handle))
        {
            case Result::Success:
                return Result::Success;

            case Result::CancelFailed_NotScheduled:
                if (Task* task; m_buffer.TryGetTaskFromHandle(handle, &task))
                {
                    if (!m_pool->IsJobCompleted(*task->poolHandle))
                    {
                        m_pool->WaitForJobCompletion(*task->poolHandle);
                        return Result::Success;
                    }
                }

                return Result::CancelOrWaitFailed_AlreadyCompleted;

            default:
                return r;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::Join()
    {
        std::jthread thread;

        {
            std::unique_lock lk{ m_mutex };

            if (!m_thread || !m_thread->joinable())
                return Result::JoinDenied_AlreadyJoined;

            assert(m_thread->get_id() != std::this_thread::get_id() &&
                   "Join() should never be called from the scheduler thread.");

            thread = std::move(*m_thread);
            m_thread.reset();
        }

        thread.join();

        {
            std::unique_lock lk{ m_mutex };
            m_state = State::NotRunning;
        }

        return Result::Success;
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
                return Result::Success;

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
            Task* task = nullptr;
            const bool hasDue = m_buffer.NextTaskDue(&task);

            const auto wake =
                (hasDue && task && task->due)
                    ? *task->due
                    : TimePoint::max();

            const bool timedOut =
                !m_cv.wait_until(lk, wake, [this, &task] {
                    Task* newTask = nullptr;
                    if (m_buffer.NextTaskDue(&newTask) && newTask != task)
                        return true;

                    return 
                        m_state == State::ShuttingDownFast ||
                        m_state == State::ShuttingDownSlow && !m_buffer.AnyTasks();
                });

            if ((m_state == State::Running || m_state == State::ShuttingDownSlow) &&
                timedOut && task)
            {
                ThreadPool::JobHandle poolHandle;
                if (m_pool->ScheduleJob(task->func, &poolHandle) == ThreadPool::Result::Success)
                {
                    m_buffer.MarkTaskPushedToPool(task, poolHandle);
                }
                else
                {
                    bool unused{};
                    m_buffer.RemoveTask(task, &unused);
                }
            }

            if ((m_state == State::ShuttingDownSlow && !m_buffer.AnyTasks()) ||
                m_state == State::ShuttingDownFast)
            {
                m_buffer.ClearTasks();
                return;
            }
        }
    }
}