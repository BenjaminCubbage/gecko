#include "gtest/gtest.h"
#include "gecko/thread/ThreadPool.h"
#include <thread>
#include <atomic>
#include <chrono>

namespace Gecko::API::Test
{
    TEST(ThreadPool, ThreadsExecuteInAnOrderlyishWay)
    {
        // Ordering not guaranteed, but so likely that I'm going to be
        // optimistic here and say that this test should always pass.

        std::atomic<int> a{ 0 };

        {
            ThreadPool pool{ 6 };

            pool.Schedule([&] {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                a = a + 1;
            });
            
            pool.Schedule([&] {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                a = a * 2;
            });
            
            pool.Schedule([&] {
                std::this_thread::sleep_for(std::chrono::milliseconds(90));
                a = a + 2;
            });
        }

        EXPECT_EQ(a, 4);
    }
    
    TEST(ThreadPool, ThreadsCompleteAllWorkBeforeJoined)
    {
        std::atomic<int> a{ 0 };

        {
            ThreadPool pool{ 6 };

            for (int i = 0; i < 50; ++i)
                pool.Schedule([i, &a] {
                    std::this_thread::sleep_for(std::chrono::milliseconds(i));
                    a++;
                });
        }

        EXPECT_EQ(a, 50);
    }
}