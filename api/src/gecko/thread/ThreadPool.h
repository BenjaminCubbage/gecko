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
    // intent(ThreadPool): This is just a horrible implementation, please
    // don't even look at it.

    // My initial goal was to make a lock-free queue for tasks to be pushed
    // and digested by some fixed set of running threads, but the result
    // was an overly-complicated hodgepodge of atomic control flow, and we 
    // end up locking in Schedule() anyways (albeit with a shared_lock).

    // This might be okay if Schedule is an _extremely_ hot path and you
    // absolutely need not only high throughput but also _stable_
    // throughput. _Maybe_. The state machine is also well-defined and 
    // predictable, destruction-before-join works as expected (it joins
    // for you), and in general the interface should be nice to work with
    // now that it's done.

    // But _oh my god_ is it difficult to work with and refactor. I guess
    // the moral of the story is don't try too hard to be clever, because
    // you might just end up making something that's slower than a simple
    // std::mutex anyways.
    class ThreadPool
    {
    public:
        // intent(ThreadCount, MaxQueueSize): These are constexpr because 
        // I only ever need one thread pool in my whole application.
        static constexpr size_t ThreadCount{ 4 };
        static constexpr size_t MaxQueueSize{ 256 };

        static_assert((MaxQueueSize > 1) & !(MaxQueueSize & (MaxQueueSize - 1)), "MaxQueueSize must be a power of two.");
        static constexpr size_t MaxInflightTickets{ ThreadCount + MaxQueueSize };
        static constexpr size_t InflightTicket_Free{ std::numeric_limits<size_t>::max() };

        enum class Result
        {
            Success = 0,

            // Start()
            StartDenied_Terminating,
            StartDenied_NotStopped,
            StartFailed_ThreadCreate,

            // Stop()
            StopDenied_NotStarted,

            // Schedule()
            ScheduleDenied_NotStarted,
            ScheduleFailed_QueueFull,

            // Join()
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
            Running, Terminating
        };

    public:
        ThreadPool()
        {
            for (size_t i = 0; i < m_inflightTickets.size(); ++i)
                m_inflightTickets[i] = InflightTicket_Free;
        }

        ~ThreadPool();
        Result Start();
        Result Stop();
        Result ScheduleJob(std::function<void ()>& func, JobHandle* outHandle);
        Result WaitForJobCompletion(const JobHandle& handle) noexcept;
        bool IsJobCompleted(const JobHandle& handle) const noexcept;
        Result Join();

    private:
        void ThreadLoop() noexcept;

        // Scheduling gate: shared for Schedule(), unique for Stop()
        std::shared_mutex m_scheduling{};

        // FSM
        std::atomic<MainState> m_mainState{ MainState::Stopped };
        std::atomic<JoinState> m_joinState{ JoinState::Joined };
        std::atomic<ExecState> m_execState{ ExecState::Running };

        // Work + wakeups
        MPMCQueue<Job, MaxQueueSize> m_jobs{};
        std::counting_semaphore<std::numeric_limits<int32_t>::max()> m_sem{ 0 };

        // Track individual job statuses
        // size_t{ -1 } means the slot is free
        std::array<std::atomic<size_t>, MaxInflightTickets> m_inflightTickets;
        std::atomic<size_t> m_nextTicketID;

        // Workers
        std::array<std::optional<std::thread>, ThreadCount> m_threads{};
    };
}
