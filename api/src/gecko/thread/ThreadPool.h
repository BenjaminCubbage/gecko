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
#include "MPMCJobQueue.h"

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
        using Job = std::function<void()>;

        static constexpr const size_t MaxQueueSize{ 256 };

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

    private:
        enum class MainState
        {
            Starting, Started, Stopping, Stopped, Halted
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
        ThreadPool(size_t threadCount) 
            : m_threads(std::vector<std::optional<std::thread>>(threadCount)) {}

        ~ThreadPool();
        Result Start();
        Result Stop();
        Result Schedule(Job& job);
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
        MPMCJobQueue<MaxQueueSize> m_jobs{};
        std::counting_semaphore<std::numeric_limits<int32_t>::max()> m_sem{ 0 };

        // Workers
        std::size_t m_threadCount{ 0 };
        std::vector<std::optional<std::thread>> m_threads{};
    };
}
