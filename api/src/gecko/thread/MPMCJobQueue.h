#pragma once
#include <array>
#include <atomic>
#include <functional>
#include <optional>
#include <utility>

namespace Gecko::API::Thread
{
    // intent(MPMCJobQueue): To have a simple lock-free, bounded MPMC queue
    // for storing the jobs in Scheduler. It can also be adapted for storing
    // other types of data later if need be.

    // This is based on a bounded MPMC algorithm by Dmitry Vyukov
    template<size_t Capacity>
    class MPMCJobQueue
    {
    private:
        static_assert((Capacity > 1) & !(Capacity & (Capacity - 1)), "Capacity must be a power of two.");
    
        struct Job
        {
            std::atomic<size_t> seq;
            std::function<void ()> func;
        };

    public:
        MPMCJobQueue           (const MPMCJobQueue&) = delete;
        MPMCJobQueue& operator=(const MPMCJobQueue&) = delete;
        MPMCJobQueue           (MPMCJobQueue&&) noexcept = default;
        MPMCJobQueue& operator=(MPMCJobQueue&&) noexcept = default;

        MPMCJobQueue()
        {
            for (size_t i = 0; i < m_jobs.size(); ++i)
                m_jobs[i].seq = i;
        }

        std::optional<std::function<void ()>>
        TryDequeue()
        {
            for (;;)
            {
                size_t h = m_head.load(std::memory_order_relaxed);
                Job& job = m_jobs[h & (Capacity - 1)];

                size_t seq = job.seq.load(std::memory_order_acquire);

                if (seq <= h)
                    return std::nullopt;

                if (seq > h + 1)
                    continue;

                if (!m_head.compare_exchange_strong(h, h + 1,
                                                    std::memory_order_relaxed,
                                                    std::memory_order_relaxed))
                    continue;

                auto func = std::move(job.func);
                job.seq.store(h + Capacity, std::memory_order_release);
                return std::move(func);
            }
        }
            
        bool TryEnqueue(std::function<void ()>& func)
        {
            for (;;)
            {
                size_t t = m_tail.load(std::memory_order_relaxed);
                Job& job = m_jobs[t & (Capacity - 1)];

                size_t seq = job.seq.load(std::memory_order_acquire);

                if (seq < t)
                    return false;

                if (seq > t)
                    continue;

                if (!m_tail.compare_exchange_strong(t, t + 1,
                                                    std::memory_order_relaxed,
                                                    std::memory_order_relaxed))
                    continue;

                job.func = std::move(func);
                job.seq.store(t + 1, std::memory_order_release);
                return true;
            }
        }

    private:
        alignas(64) std::atomic<size_t> m_head{ 0 };
        alignas(64) std::atomic<size_t> m_tail{ 0 };
        std::array<Job, Capacity> m_jobs;
    };
}