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
#include "gecko/thread/SchedulerFLQueue.h"
#include "gecko/thread/ThreadPool.h"

namespace Gecko::API::Thread
{
    // intent(Scheduler): To provide a simple way to schedule tasks
    // to be run in a thread pool after a given duration of time
    // has elapsed.
    class Scheduler
    {
    public:
        using TaskHandle = SchedulerFLQueue::TaskHandle;
        using TimePoint  = SchedulerFLQueue::TimePoint;

        enum class Result
        {
            Success = 0,

            StartDenied_NotIdle,

            ScheduleDenied_NotStarted,
            ScheduleDenied_PoolNotStarted,
            ScheduleFailed_FLQueueFull,
            ScheduleFailed_ThreadPoolQueueFull,
            ScheduleFailed_PoolError,

            JoinDenied_AlreadyJoined,

            CancelFailed_CurrentlyRunning,
            CancelFailed_AlreadyCompleted,

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

        Scheduler(ThreadPool* pool)
            : m_pool{ pool }, m_queue{ m_pool } { }

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
        Result ScheduleJobAfter(std::chrono::nanoseconds delay, std::function<void ()>&& func, TaskHandle *outTask);
        Result ScheduleJobAt(TimePoint at, std::function<void ()>&& func, TaskHandle *outTask);
        Result CancelJob(const TaskHandle& task);
        Result CancelJobOrWait(const TaskHandle& task);
        inline Result ShutdownSlow() { return Shutdown(false); }
        inline Result ShutdownFast() { return Shutdown(true); }
        Result Join();
        static inline TimePoint Now() { return TimePoint::clock::now(); }

    private:
        Result Shutdown(bool fast);
        void ThreadLoop();

        ThreadPool* m_pool;
        SchedulerFLQueue m_queue;

        State m_state{ State::NotRunning };
        std::optional<std::jthread> m_schedulerThread;

        std::mutex m_mutex;
        std::condition_variable m_cv;
    };
}