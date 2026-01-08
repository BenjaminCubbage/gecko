#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace Gecko::API::Thread
{
    class ThreadPool
    {
    public:
        ThreadPool(int threadCount)
        {
            for (int i = 0; i < threadCount; ++i)
                m_threads.emplace_back(&ThreadPool::ThreadLoop, this);
        }

        ~ThreadPool()
        {
            {
                std::unique_lock lk{ m_jobMutex };
                m_joining = true;
            }
            
            m_jobSignal.notify_all();

            for (auto& thread : m_threads)
                thread.join();
        }

        ThreadPool(ThreadPool&& other) noexcept            = default;
        ThreadPool& operator=(ThreadPool&& other) noexcept = default;

        ThreadPool(const ThreadPool& other)            = delete;
        ThreadPool& operator=(const ThreadPool& other) = delete;

        void Schedule(const std::function<void()>& job);
        
    private:
        std::mutex                         m_jobMutex;
        std::condition_variable            m_jobSignal;
        std::vector<std::function<void()>> m_jobs;
        std::vector<std::thread>           m_threads;
        std::atomic<bool>                  m_joining;

        void ThreadLoop();
    };
}