#pragma once
#include <array>
#include <atomic>
#include <functional>
#include <optional>
#include <utility>

namespace Gecko::API::Thread
{
    /*
        intent(MPMCQueue): To have a simple lock-free, bounded MPMC
        queue for multithreaded use.

        This is based on a bounded MPMC algorithm by Dmitry Vyukov.
    */
    template<typename T, size_t Capacity>
    class MPMCQueue
    {
      private:
        static_assert((Capacity > 1) & !(Capacity & (Capacity - 1)), "Capacity must be a power of two.");

        struct Cell
        {
            std::atomic<size_t> seq;
            T data;
        };

      public:
        struct EnqTicket
        {
            size_t t;
            Cell* cell;
        };

        MPMCQueue           (const MPMCQueue&) = delete;
        MPMCQueue& operator=(const MPMCQueue&) = delete;
        MPMCQueue           (MPMCQueue&&) noexcept = default;
        MPMCQueue& operator=(MPMCQueue&&) noexcept = default;

        MPMCQueue()
        {
            for (size_t i = 0; i < m_jobs.size(); ++i)
                m_jobs[i].seq = i;
        }

        /*
            Try to dequeue an item. Returns std::nullopt if
            nothing found.
        */
        std::optional<T>
        TryDequeue()
        {
            for (;;)
            {
                size_t h = m_head.load(std::memory_order_relaxed);
                Cell& job = m_jobs[h & (Capacity - 1)];

                size_t seq = job.seq.load(std::memory_order_acquire);

                if (seq <= h)
                    return std::nullopt;

                if (seq > h + 1)
                    continue;

                if (!m_head.compare_exchange_strong(
                        h, h + 1,
                        std::memory_order_relaxed,
                        std::memory_order_relaxed))
                    continue;

                auto data = std::move(job.data);
                job.seq.store(h + Capacity, std::memory_order_release);
                return std::move(data);
            }
        }

        /*
            Reserve a slot in the queue, but don't mark it as "ready"
            until EnqueueCommit() is called.

            Separating EnqueueReserve and EnqueueCommit allows the
            caller to:

            (1) Reserve a queue cell (ensuring capacity exists) without
                making it visible to consumers yet, then
            (2) perform any prerequisite work (e.g., reserving a
                completion slot) and fully initialize the cell, and finally
            (3) publish the cell with a release-store so consumers may
                digest it.

            Once a slot is reserved, it should be committed quickly; the
            head cannot move beyond an uncommitted reservation, and the
            queue will eventually halt on the uncommitted enqueue.

            Thus, only light work--or none at all--should be performed
            between reservation and commiting.
        */
        bool TryEnqueueAcquire(EnqTicket* outTicket)
        {
            for (;;)
            {
                size_t t = m_tail.load(std::memory_order_relaxed);
                Cell& job = m_jobs[t & (Capacity - 1)];

                size_t seq = job.seq.load(std::memory_order_acquire);

                if (seq < t)
                    return false;

                if (seq > t)
                    continue;

                if (!m_tail.compare_exchange_strong(t, t + 1,
                                                    std::memory_order_relaxed,
                                                    std::memory_order_relaxed))
                    continue;

                *outTicket = {
                    .t    = t,
                    .cell = &job
                };

                return true;
            }
        }

        /*
            Mark the acquired ticket as "ready".
        */
        void EnqueuePublish(EnqTicket ticket, T&& data)
        {
            ticket.cell->data = std::forward<T>(data);
            ticket.cell->seq.store(ticket.t + 1, std::memory_order_release);
        }

      private:
        alignas(64) std::atomic<size_t> m_head{ 0 };
        alignas(64) std::atomic<size_t> m_tail{ 0 };
        std::array<Cell, Capacity> m_jobs;
    };
}