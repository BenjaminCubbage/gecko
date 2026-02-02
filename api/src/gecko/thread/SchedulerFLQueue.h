#pragma once
#include <array>
#include <assert.h>
#include <chrono>
#include <functional>
#include <optional>
#include "gecko/thread/ThreadPool.h"

namespace Gecko::API::Thread
{
    /*
        intent(SchedulerFLQueue): To act as a free-list data structure
        optimized for fast insertion and removal of scheduled tasks,
        and caching of the next scheduled task.

        This implementation is not designed to be used by the Scheduler
        object. It is not thread-safe and the user is responsible for
        enforcing mutual exclusion.
    */
    class SchedulerFLQueue
    {
      public:
        static constexpr const size_t ScheduleQueueSize{ 512 };

        static constexpr const size_t Ticket_Free{ std::numeric_limits<size_t>::max() };

        static_assert(ScheduleQueueSize > 0, "ScheduleQueueSize must be > 0");

        using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

        struct TaskHandle
        {
            size_t index;
            size_t ticket;

            inline bool operator==(const TaskHandle& other) const
            {
                return other.index  == index &&
                       other.ticket == ticket;
            }
        };

      private:
        struct Task
        {
            /* Self-aware handle */
            TaskHandle handle;

            /* Free list */
            bool isFree{ true };
            Task *nextFree{ nullptr };

            /* This is invalid once moved into the pool */
            std::function<void()> func;

            /* One or the other */
            std::optional<TimePoint>             due;
            std::optional<ThreadPool::JobHandle> poolHandle;
        };

      public:
        SchedulerFLQueue(ThreadPool* threadPool);

        SchedulerFLQueue           (SchedulerFLQueue&&) = default;
        SchedulerFLQueue& operator=(SchedulerFLQueue&&) = default;
        SchedulerFLQueue           (const SchedulerFLQueue&) = delete;
        SchedulerFLQueue& operator=(const SchedulerFLQueue&) = delete;


        /*
            Create a task and queue it.

            Returns true if the buffer wasn't full.
            Otherwise, false.
        */
        bool QueueTask(TimePoint due,
                       std::function<void()>&& func,
                       TaskHandle* outHandle,
                       bool* outIsNextDue);

        /*
            Remove the associated task if it exists.

            Returns true if the task was found.
            Otherwise, false.
        */
        bool RemoveTask(const TaskHandle& handle, bool *outWasNextDue = nullptr);

        /*
            Push the associated task to the pool.

            Returns true if the task was found and not already in the
            pool.
            Otherwise, false.
        */
        bool PushToPool(const TaskHandle& handle);

        /*
            Get data about the associated task.

            Returns true and sets either outDue or outPoolHandle if the
            task was found
            Otherwise, false.
        */
        bool QueryTask(const TaskHandle& handle,
                       std::optional<TimePoint>* outDue,
                       std::optional<ThreadPool::JobHandle>* outPoolHandle);

        /*
            Get the next task due if it exists.

            Returns true and sets outHandle and outDue if there is an
            upcoming task.
            Otherwise, false.
        */
        bool NextDue(TaskHandle* outHandle,
                     TimePoint* outDue) const;

        inline bool IsNextDue(const TaskHandle& handle) const noexcept
        {
            return m_nextDue != nullptr && handle.ticket == m_nextDue->handle.ticket;
        }

        /*
            Remove all queued tasks and reset the queue.
        */
        inline void ClearTasks() { Reset(); }

        /*
            Returns true if there are any unpooled tasks.
            Otherwise, false.
        */
        inline bool AnyUnpooledTasks() const noexcept
        {
            return m_nextDue != nullptr;
        }

      private:
        void Reset();

        void RescanNextDueAndFLEnd(size_t searchEnd);

        void UpdateNextDue();

        // Non-owning
        ThreadPool* m_threadPool;

        // Monotonic ticket counter
        size_t m_nextTicket{ 0 };

        // FL implementation
        Task *m_freeHead;
        Task *m_nextDue;
        size_t m_end;
        std::array<Task, ScheduleQueueSize> m_tasks;
    };
}
