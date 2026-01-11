#pragma once
#include <array>
#include <assert.h>
#include <chrono>
#include <functional>
#include <optional>
#include "gecko/thread/ThreadPool.h"

namespace Gecko::API::Thread
{
    // intent(SchedulerFLQueue): To act as a free-list data structure
    // optimized for fast insertion and removal of scheduled tasks,
    // and caching of the next scheduled task.

    // This implementation is not designed to be thread-safe and the
    // user is responsible for enforcing mutual exclusion.
    class SchedulerFLQueue
    {
    public:
        static constexpr const size_t Ticket_Free{ std::numeric_limits<size_t>::max() };
        static constexpr const size_t ScheduleQueueSize{ 512 };
        static_assert(ScheduleQueueSize > 0, "ScheduleQueueSize must be > 0");

        using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

        struct TaskHandle
        {
            size_t index;
            size_t ticket;

            bool operator ==(const TaskHandle& other) const noexcept
            {
                return ticket == other.ticket;
            };
        };

    private:
        struct Task
        {
            // Self-aware handle
            TaskHandle handle;

            // Free list
            bool isFree{ true };
            Task *nextFree{ nullptr };

            // Function
            // This is invalid once moved into the pool
            std::function<void()> func;

            // One or the other
            std::optional<TimePoint>             due;
            std::optional<ThreadPool::JobHandle> poolHandle;
        };

    public:
        SchedulerFLQueue(ThreadPool* threadPool);

        SchedulerFLQueue           (SchedulerFLQueue&&) = default;
        SchedulerFLQueue& operator=(SchedulerFLQueue&&) = default;
        SchedulerFLQueue           (const SchedulerFLQueue&) = delete;
        SchedulerFLQueue& operator=(const SchedulerFLQueue&) = delete;

        // Returns false if the buffer was full, otherwise true
        bool QueueTask(TimePoint due,
                       std::function<void()>&& func,
                       TaskHandle* outHandle,
                       bool* outIsNextDue);

        bool RemoveTask(const TaskHandle& handle, bool *outWasNextDue = nullptr);

        bool PushToPool(const TaskHandle& handle);

        bool QueryTask(const TaskHandle& handle,
                       std::optional<TimePoint>* outDue,
                       std::optional<ThreadPool::JobHandle>* outPoolHandle);

        bool NextDue(TaskHandle* outHandle,
                     TimePoint* outDue) const;

        inline bool IsNextDue(const TaskHandle& handle) const noexcept
        {
            return m_nextDue != nullptr && handle == m_nextDue->handle;
        }

        inline void ClearTasks() { Reset(); }

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
