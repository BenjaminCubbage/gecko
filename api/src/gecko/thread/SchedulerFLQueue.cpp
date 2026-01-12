#include "gecko/thread/SchedulerFLQueue.h"

namespace Gecko::API::Thread
{
    SchedulerFLQueue::SchedulerFLQueue(ThreadPool* threadPool)
        : m_threadPool(threadPool)
    {
        for (size_t i = 0; i < m_tasks.size(); ++i)
        {
            m_tasks[i].handle.index  = i;
            m_tasks[i].handle.ticket = Ticket_Free;
        }

        Reset();
    }

    bool SchedulerFLQueue::QueueTask(TimePoint due,
                                     std::function<void ()>&& func,
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
                    task.poolHandle && m_threadPool->IsJobCompleted(*task.poolHandle))
                {
                    RemoveTask(task.handle);
                }
            }

            // No jobs were completed; we really are out of room
            if (!m_freeHead)
                return false;
        }

        Task *task = m_freeHead;
        m_freeHead = m_freeHead->nextFree;

        task->handle.ticket = ++m_nextTicket;
        task->isFree   = false;
        task->nextFree = nullptr;

        task->due  = due;
        task->func = std::move(func);

        if (!m_nextDue || *task->due < *m_nextDue->due)
        {
            *outIsNextDue = true;
            m_nextDue     = task;
        }

        *outHandle = task->handle;
        m_end = std::max(m_end, task->handle.index + 1);

        return true;
    }

    bool SchedulerFLQueue::RemoveTask(const TaskHandle& handle, bool *outWasNextDue)
    {
        Task* task = &m_tasks[handle.index];
        assert(handle.index < m_tasks.size());
        
        if (task->isFree || handle.ticket != task->handle.ticket)
            return false;

        task->handle.ticket = Ticket_Free;

        task->isFree   = true;
        task->nextFree = m_freeHead;
        m_freeHead     = task;

        task->due        = std::nullopt;
        task->poolHandle = std::nullopt;
        task->func       = {};

        if (outWasNextDue != nullptr)
            *outWasNextDue = task == m_nextDue;

        if (task == m_nextDue)
            RescanNextDueAndFLEnd(m_end);

        return true;
    }

    bool SchedulerFLQueue::PushToPool(const TaskHandle& handle)
    {
        Task* task = &m_tasks[handle.index];

        if (!task->due || handle.ticket != task->handle.ticket)
            return false;

        ThreadPool::JobHandle poolHandle;

        if (m_threadPool->ScheduleJob(task->func, &poolHandle) !=
            ThreadPool::Result::OK) [[unlikely]]
        {
            // todo(ben): Right now, we just destroy tasks that couldn't
            // be scheduled. In the future we might consider some sort of
            // retry system.

            RemoveTask(handle);
            return false;
        }

        task->func       = {};
        task->due        = std::nullopt;
        task->poolHandle = poolHandle;

        if (task == m_nextDue)
            RescanNextDueAndFLEnd(m_end);

        return true;
    }

    bool SchedulerFLQueue::QueryTask(const TaskHandle& handle,
                                     std::optional<TimePoint>* outDue,
                                     std::optional<ThreadPool::JobHandle>* outPoolHandle)
    {
        assert(handle.index < m_tasks.size());
        Task* task = &m_tasks[handle.index];

        if (task->handle.ticket != handle.ticket)
            return false;

        if (task->poolHandle && m_threadPool->IsJobCompleted(*task->poolHandle))
        {
            RemoveTask(handle);
            return false;
        }

        *outDue        = task->due;
        *outPoolHandle = task->poolHandle;
        return true;
    }

    bool SchedulerFLQueue::NextDue(TaskHandle* outHandle, TimePoint* outDue) const
    {
        assert(outHandle);
        assert(outDue);
        assert(m_nextDue == nullptr || m_nextDue->due);

        if (!m_nextDue)
            return false;

        *outHandle = m_nextDue->handle;
        *outDue    = *m_nextDue->due;
        return true;
    }

    void SchedulerFLQueue::Reset()
    {
        for (size_t i = 0; i < m_tasks.size() - 1; ++i)
        {
            m_tasks[i].handle.ticket = Ticket_Free;

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

    void SchedulerFLQueue::RescanNextDueAndFLEnd(size_t searchEnd)
    {
        m_end     = 0;
        m_nextDue = nullptr;

        for (size_t i = 0; i < searchEnd; ++i)
        {
            if (m_tasks[i].isFree)
                continue;
            
            m_end = i + 1;

            if (!m_tasks[i].due)
                continue;

            if (m_nextDue != nullptr)
            {
                assert(m_nextDue->due);

                if (*m_tasks[i].due < *m_nextDue->due)
                    m_nextDue = &m_tasks[i];
            }
            else
                m_nextDue = &m_tasks[i];
        }
    }
}