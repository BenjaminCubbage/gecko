#pragma once
#include <atomic>
#include <cstddef>
#include <functional>
#include <limits>
#include <mutex>
#include <queue>
#include <semaphore>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <vector>
#include "MPMCQueue.h"

namespace Gecko::API::Thread
{
    /*
        intent(ThreadPool): This is just a horrible implementation, please
        don't even look at it.

        My initial goal was to make a lock-free queue for tasks to be pushed
        and digested by some fixed set of running threads, but the result
        was an overly-complicated spider's web of atomic control flow, and we
        end up locking in Schedule() anyways (albeit with a shared_lock).

        This might be okay if Schedule is an _extremely_ hot path and you
        absolutely need not only high throughput but also _stable_
        throughput. _Maybe_. The state machine is also well-defined and
        predictable, destruction-before-join works as expected (it joins
        for you), and in general the interface should be nice to work with
        now that it's done.

        But _oh my god_ is it difficult to reason about and refactor. I guess
        the moral of the story is don't try too hard to be clever, because
        you might just end up making something that's both needlessly complex
        and slower than a simple std::mutex anyways.

        It's pretty cool, though.
    */
    class ThreadPool
    {
    public:
        /*
            Number of threads.
        */
        static constexpr size_t ThreadCount{ 4 };

        /*
            Maximum number of simultaneously-queued tasks.
        */
        static constexpr size_t QueueSize{ 256 };

        static constexpr size_t MaxInflightTickets{ ThreadCount + QueueSize };

        static constexpr size_t NPos{ (std::numeric_limits<size_t>::max)() };

        static_assert((QueueSize > 1) & !(QueueSize & (QueueSize - 1)), "QueueSize must be a power of two.");

        enum class Result
        {
            OK = 0,

            /* Start() */
            StartDenied_Terminating,
            StartDenied_NotStopped,
            StartFailed_ThreadCreate,

            /* Stop() */
            StopDenied_NotStarted,

            /* Schedule() */
            ScheduleDenied_NotStarted,
            ScheduleFailed_QueueFull,

            /* Join() */
            JoinDenied_AlreadyJoined
        };

        struct JobHandle
        {
            size_t ticketID;
            size_t ticketIndex;
        };

    private:
        struct Job
        {
            std::function<void ()> func;
            JobHandle handle;
        };

        enum class MainState
        {
            Starting, Running, Stopping, Stopped, Halted
        };

        enum class JoinState
        {
            NotJoined, Joining, Joined
        };

        enum class ExecState
        {
            Alive, Terminating
        };

    public:
        ThreadPool()
        {
            for (size_t i = 0; i < m_inflightTickets.size(); ++i)
                m_inflightTickets[i] = NPos;
        }

        /*
            Gracefully join all threads once all tasks have been executed.
        */
        ~ThreadPool();

        /*
            Start the thread pool, if currently stopped and joined.
        */
        Result Start();

        /*
            Don't allow any more tasks to be queued into the thread pool.
            When all tasks are completed, the thread pool is considered
            stopped.
        */
        Result Stop();

        /*
            Schedule a job to be completed, if currently started.
        */
        Result ScheduleJob(std::function<void ()>& func, JobHandle* outHandle);

        /*
            Wait for the job to be completed if it is running.
        */
        Result WaitForJobCompletion(const JobHandle& handle) noexcept;

        /*
            Returns true if the job is completed.
            Otherwise, false.
        */
        bool IsJobCompleted(const JobHandle& handle) const noexcept;

        /*
            Join all threads, once stopped.

            It's okay to call this method from multiple threads
            simultaneously. In that case, all threads will wait for
            completion.

            If not running or already joined, returns immediately.
        */
        Result Join();

    private:
        void ThreadLoop() noexcept;

        /* Scheduling gate: shared for Schedule(), unique for Stop() */
        std::shared_mutex m_scheduling{};

        /* FSM */
        std::atomic<MainState> m_mainState{ MainState::Stopped };
        std::atomic<JoinState> m_joinState{ JoinState::Joined  };
        std::atomic<ExecState> m_execState{ ExecState::Alive   };

        /* Work + wakeups */
        MPMCQueue<Job, QueueSize> m_jobs{};
        std::counting_semaphore<std::numeric_limits<int32_t>::max()> m_sem{ 0 };

        /* Track individual jobs (npos = free slot) */
        std::array<std::atomic<size_t>, MaxInflightTickets> m_inflightTickets;
        std::atomic<size_t> m_nextTicketID;

        /* Worker threads */
        std::array<std::optional<std::thread>, ThreadCount> m_threads{};
    };
}
