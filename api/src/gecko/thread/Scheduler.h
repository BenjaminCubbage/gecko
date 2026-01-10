#pragma once
#include <array>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <limits>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include "gecko/thread/ThreadPool.h"

namespace Gecko::API::Thread
{
    // intent(Scheduler): To provide a simple way to schedule tasks
    // to be run in a thread pool after a given duration of time
    // has elapsed.
    class Scheduler
    {
    public:
        static constexpr const size_t Ticket_Free{ std::numeric_limits<size_t>::max() };

        static constexpr const size_t ScheduleQueueSize{ 256 };

        static_assert(ScheduleQueueSize > 0, "ScheduleQueueSize must be > 0");

        // intent(ScheduleBuffer): To act as a free-list data structure
        // optimized for fast insertion and removal of scheduled tasks,
        // and caching of the next scheduled task.

        // This implementation is not designed to be thread-safe and the
        // user is responsible for enforcing mutual exclusion.
        class ScheduleBuffer
        {
        public:
            typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

            struct TaskHandle
            {
                size_t index;
                size_t ticket;
                bool operator ==(const TaskHandle& other) const { return other.ticket == ticket; };
            };

            struct Task
            {
                // Self-aware index
                size_t index;
                size_t ticket{ Ticket_Free };

                // Free list
                bool isFree{ true };
                Task *nextFree{ nullptr };

                // Function
                // This is invalid once moved into the pool
                std::function<void()> func;

                // If due is not nullopt, the task is still scheduled
                // and not yet pushed to the pool.
                // If poolHandle is not nullopt, the task is pushed to
                // the pool, and may or may not be completed yet.
                std::optional<TimePoint>             due;
                std::optional<ThreadPool::JobHandle> poolHandle;
            };

            ScheduleBuffer();

            // Returns false if the buffer was full, otherwise true
            bool AddTask(ThreadPool* threadPool,
                         TimePoint due,
                         std::function<void()>&& func,
                         TaskHandle* outHandle,
                         bool* outIsNextDue);

            void RemoveTask(Task* task, bool *outWasNextDue);

            void MarkTaskPushedToPool(Task* task,
                                      const ThreadPool::JobHandle& poolHandle);

            inline void ClearTasks() { Reset(); }

            [[nodiscard]]
            inline bool TryGetTaskFromHandle(const TaskHandle& handle,
                                             Task** outTask)
            {
                if (m_tasks[handle.index].ticket == handle.ticket)
                {
                    *outTask = &m_tasks[handle.index];
                    return true;
                }

                return false;
            }

            [[nodiscard]]
            inline bool NextTaskDue(Task** task) const
            {
                return *task = m_nextDue;
            }

            [[nodiscard]]
            inline bool AnyTasks() const noexcept 
            { 
                return m_nextDue != nullptr; 
            }

        private:
            void Reset();

            static void GetNextDue(std::array<Task, ScheduleQueueSize>& arr,
                                   size_t end,
                                   Task** outNextDue,
                                   size_t* outNewEnd);

            void UpdateNextDue();

            Task *m_freeHead;
            Task *m_nextDue;

            size_t m_end;

            std::array<Task, ScheduleQueueSize> m_tasks;

            size_t m_nextTicket{ 0 };
        };

        typedef ScheduleBuffer::Task       Task;
        typedef ScheduleBuffer::TaskHandle TaskHandle;
        typedef std::function<void ()>     TaskFunc;
        typedef ScheduleBuffer::TimePoint  TimePoint;

        enum class Result
        {
            Success = 0,

            StartDenied_NotIdle,

            ScheduleDenied_NotStarted,
            ScheduleDenied_PoolNotStarted,
            ScheduleFailed_QueueFull,
            ScheduleFailed_ThreadPoolQueueFull,
            ScheduleFailed_PoolError,

            JoinDenied_AlreadyJoined,

            CancelFailed_NotScheduled,

            CancelOrWaitFailed_AlreadyCompleted,

            ShutdownDenied_NotRunning
        };

        enum class State
        {
            NotRunning,
            Running,
            ShuttingDownFast,
            ShuttingDownSlow
        };

        Scheduler(ThreadPool* pool) : m_pool(pool) {}

        ~Scheduler()
        {
            ShutdownFast();
            Join();
        }

        Scheduler           (Scheduler&&) = default;
        Scheduler& operator=(Scheduler&&) = default;

        Scheduler           (const Scheduler&) = delete;
        Scheduler& operator=(const Scheduler&) = delete;

        Result Start();
        Result ScheduleJobAfter(std::chrono::nanoseconds delay, TaskFunc&& func, TaskHandle *outHandle);
        Result ScheduleJobAt(TimePoint at, TaskFunc&& func, TaskHandle *outHandle);
        Result CancelJob(const TaskHandle& handle);
        Result CancelJobOrWait(const TaskHandle& handle);
        inline Result ShutdownSlow() { return Shutdown(false); }
        inline Result ShutdownFast() { return Shutdown(true); }
        Result Join();
        static inline TimePoint Now() { return TimePoint::clock::now(); }

    private:
        Result Shutdown(bool fast);
        void ThreadLoop();

        ThreadPool* m_pool;

        State m_state{ State::NotRunning };
        std::optional<std::jthread> m_thread;

        std::mutex m_mutex;
        std::condition_variable m_cv;

        ScheduleBuffer m_buffer;
    };
}