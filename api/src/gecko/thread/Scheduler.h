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
    /*
        intent(Scheduler): To provide a simple way to schedule tasks
        to be run in a thread pool after a given duration of time
        has elapsed.

        Basically, there's the thread pool, which runs the tasks, and
        then there's the scheduler thread, whose only job it is is to
        wait for tasks to be ready to run.
    */
    class Scheduler
    {
      public:
        using TaskHandle = SchedulerFLQueue::TaskHandle;
        using TimePoint  = SchedulerFLQueue::TimePoint;

        enum class Result
        {
            OK = 0,

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

        /*
            Start the scheduler if currently stopped and joined.

            Most of these methods assume the thread pool is actively
            running. If it isn't, nothing horrible should happen, but
            the queue will eventually fill up and new tasks will be
            rejected.
        */
        Result Start();

        /*
            Schedule job after a set amount of time.
        */
        Result ScheduleJobAfter(std::chrono::nanoseconds delay, std::function<void ()>&& func, TaskHandle *outTask);

        /*
            Schedule a job to run at the given time point.
        */
        Result ScheduleJobAt(TimePoint at, std::function<void ()>&& func, TaskHandle *outTask);

        /*
            Cancel the associated job, if it isn't yet completed. If it
            is currently running, do nothing.
        */
        Result CancelJob(const TaskHandle& task);

        /*
            Cancel the associated job, if it isn't yet completed. If it
            is currently running in the thread pool, wait for it to
            complete.
        */
        Result CancelJobOrWait(const TaskHandle& task);

        /*
            Stop the scheduler once all tasks are pushed to the pool.
        */
        inline Result ShutdownSlow() { return Shutdown(false); }

        /*
            Stop the scheduler and cancel all queued tasks.
        */
        inline Result ShutdownFast() { return Shutdown(true); }

        /*
            Join the scheduler to the current thread. This must be
            called before attempting to restart the scheduler.

            This does not wait for all jobs to complete; it waits for
            all threads to be queued to the thread pool, at which point
            the scheduler thread will be ready to join.
        */
        Result Join();

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