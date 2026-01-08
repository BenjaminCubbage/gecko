#pragma once
#include <array>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include "gecko/thread/ThreadPool.h"

namespace Gecko::API::Thread
{
    // intent(Scheduler): To provide a simple way to schedule tasks
    // to be run in a thread pool after a given duration of time
    // has elapsed
    class Scheduler
    {
    public:
        static constexpr const size_t ScheduleBufferSize{ 256 };

        static_assert(ScheduleBufferSize > 0, "ScheduleBufferSize must be > 0");

        // intent(ScheduleBuffer): To act as a free-list data structure
        // optimized for fast insertion and removal of scheduled tasks,
        // and caching of the next scheduled task.
        // This implementation is not designed to be thread-safe and the
        // user is responsible for enforcing mutual exclusion.
        class ScheduleBuffer
        {
        public:
            typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

            struct Task
            {
                // Self-aware index
                size_t index;

                // Free list
                bool isFree{ true };
                Task *nextFree{ nullptr };

                TimePoint due;
                std::function<void ()> func;
            };

            ScheduleBuffer();

            // Returns false if the buffer was full, otherwise true
            bool AddTask(TimePoint due, std::function<void ()>&& func, bool* outIsNextDue);

            void RemoveTask(Task *task);

            inline void ClearTasks() { Reset(); }
            [[nodiscard]] inline Task* NextTaskDue() const { return m_nextDue; }
            [[nodiscard]] bool AnyTasks() const { return m_end != 0; }

        private:
            void Reset();

            size_t m_end;
            Task *m_freeHead;
            Task *m_nextDue;

            std::array<Task, ScheduleBufferSize> m_tasks;
        };

        typedef ScheduleBuffer::Task      Task;
        typedef ScheduleBuffer::TimePoint TimePoint;

        enum class Result
        {
            Success = 0,
            NotRunning,
            AlreadyRunning,
            IsShuttingDown,
            BufferFull,
            AlreadyJoined,
            NotYetJoined,
            UnknownError
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
        Result ScheduleNow(std::function<void ()>&& func);
        Result ScheduleAfter(std::chrono::nanoseconds delay, std::function<void ()>&& func);
        Result ScheduleAt(TimePoint at, std::function<void ()>&& func);
        inline Result ShutdownSlow() { return Shutdown(false); }
        inline Result ShutdownFast() { return Shutdown(true); }
        Result Join();

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