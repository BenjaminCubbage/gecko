#include "gtest/gtest.h"
#include "gecko/thread/ThreadPool.h"
#include <thread>
#include <atomic>
#include <chrono>

namespace Gecko::API::Test
{
    using Gecko::API::Thread::ThreadPool;

    TEST(ThreadPool, ThreadsExecuteInAnOrderlyishWay)
    {
        // Ordering not guaranteed, but so likely that I'm going to be
        // optimistic here and say that this test should always pass.

        std::atomic<int> a{ 0 };

        ThreadPool pool;
        ThreadPool::JobHandle unused;

        pool.Start();

        std::function<void ()> j1 = [&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            a = a + 1;
        };

        std::function<void ()> j2 = [&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            a = a * 2;
        };

        std::function<void ()> j3 = [&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            a = a + 2;
        };

        pool.ScheduleJob(j1, &unused);
        pool.ScheduleJob(j2, &unused);
        pool.ScheduleJob(j3, &unused);

        pool.Stop();
        pool.Join();
        EXPECT_EQ(a, 4);
    }
    
    TEST(ThreadPool, ThreadsCompleteAllWorkBeforeJoined)
    {
        std::atomic<int> a{ 0 };

        ThreadPool pool;
        ThreadPool::JobHandle unused;
        
        pool.Start();

        for (int i = 0; i < 50; ++i)
        {
            std::function<void ()> fn = [i, &a] {
                std::this_thread::sleep_for(std::chrono::milliseconds(i));
                a++;
            };

            pool.ScheduleJob(fn, &unused);
        }

        pool.Stop();
        pool.Join();
        EXPECT_EQ(a, 50);
    }
}