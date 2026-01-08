#include "gecko/thread/ThreadPool.h"

namespace Gecko::API::Thread
{
    void ThreadPool::ThreadLoop()
    {
        for(;;)
        {
            std::function<void()> job;

            {
                std::unique_lock lk{ m_jobMutex };
                m_jobSignal.wait(lk, [this] {
                    return !m_jobs.empty() || m_joining;
                });
                
                // Don't join until all jobs are completed.
                if (m_joining && m_jobs.empty())
                    return;
                
                job = m_jobs.back();
                m_jobs.pop_back();
            }
            
            job();
        }
    }
    
    void ThreadPool::Schedule(const std::function<void()>& job)
    {
        {
            std::unique_lock lk{ m_jobMutex };
            m_jobs.push_back(job);
        }
        m_jobSignal.notify_one();
    }
}