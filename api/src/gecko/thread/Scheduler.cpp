#include "gecko/thread/Scheduler.h"
#include <algorithm>
#include <stdexcept>

namespace Gecko::API::Thread
{
    Scheduler::ScheduleBuffer::ScheduleBuffer()
    {
        for (size_t i = 0; i < m_tasks.size(); ++i)
            m_tasks[i].index = i;

        Reset();
    }

    bool Scheduler::ScheduleBuffer::AddTask(TimePoint due,
                                            std::function<void ()>&& func,
                                            bool* outIsNextDue)
    {
        if (!outIsNextDue)
            throw std::runtime_error("outIsNextDue cannot be null");

        *outIsNextDue = false;

        if (!m_freeHead)
            return false;

        Task *task = m_freeHead;
        m_freeHead = m_freeHead->nextFree;

        task->isFree   = false;
        task->nextFree = nullptr;

        task->due  = due;
        task->func = std::move(func);

        if (!m_nextDue || task->due < m_nextDue->due)
        {
            *outIsNextDue = true;
            m_nextDue     = task;
        }

        m_end = std::max(m_end, task->index + 1);
        return true;
    }

    void Scheduler::ScheduleBuffer::RemoveTask(Task *task)
    {
        if (!task)
            throw std::runtime_error("task cannot be null");

        task->isFree   = true;
        task->nextFree = m_freeHead;
        m_freeHead     = task;

        task->due  = {};
        task->func = {};

        if (m_nextDue == task)
            m_nextDue = nullptr;

        size_t newEnd{ 0 };
        for (size_t i = 0; i < m_end; ++i)
        {
            if (m_tasks[i].isFree)
                continue;

            newEnd = i + 1;
            if (!m_nextDue || m_tasks[i].due < m_nextDue->due)
                m_nextDue = &m_tasks[i];
        }

        m_end = newEnd;
    }

    void Scheduler::ScheduleBuffer::Reset()
    {
        for (size_t i = 0; i < m_tasks.size() - 1; ++i)
        {
            m_tasks[i].isFree   = true;
            m_tasks[i].nextFree = &m_tasks[i + 1];

            m_tasks[i].func = {};
            m_tasks[i].due  = {};
        }

        m_tasks.back().isFree   = true;
        m_tasks.back().nextFree = nullptr;

        m_tasks.back().func = {};
        m_tasks.back().due  = {};

        m_end      = 0;
        m_freeHead = &m_tasks[0];
        m_nextDue  = nullptr;
    }

    Scheduler::Result Scheduler::Start()
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                return Result::AlreadyRunning;

            case State::NotRunning:
                m_state = State::Running;

                if (m_thread)
                    throw std::logic_error("Thread not yet joined in the NotRunning state");

                m_thread.emplace(&Scheduler::ThreadLoop, this);
                return Result::Success;

            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::IsShuttingDown;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::ScheduleNow(std::function<void ()>&& func)
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                m_pool->Schedule(std::move(func));
                return Result::Success;

            case State::NotRunning:
                return Result::NotRunning;

            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::IsShuttingDown;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::ScheduleAfter(std::chrono::nanoseconds delay, 
                                              std::function<void ()>&& func)
    {
        return ScheduleAt(TimePoint::clock::now() + delay, std::move(func));
    }

    Scheduler::Result Scheduler::ScheduleAt(TimePoint at,
                                            std::function<void ()>&& func)
    {
        if (at < TimePoint::clock::now())
            return ScheduleNow(std::move(func));

        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                {
                    bool isNextDue{ false };

                    if (!m_buffer.AddTask(at, std::move(func), &isNextDue))
                        return Result::BufferFull;

                    if (isNextDue)
                        m_cv.notify_one();
                }
                return Result::Success;

            case State::NotRunning:
                return Result::NotRunning;

            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::IsShuttingDown;
        }

        std::terminate();
    }

    Scheduler::Result Scheduler::Join()
    {
        std::jthread thread;

        {
            std::unique_lock lk{ m_mutex };

            if (!m_thread || !m_thread->joinable())
                return Result::AlreadyJoined;

            if (m_thread->get_id() == std::this_thread::get_id())
                throw std::logic_error("Tried to join thread with itself");

            thread = std::move(*m_thread);
            m_thread.reset();
        }

        thread.join();

        {
            std::unique_lock lk{ m_mutex };
            m_state = State::NotRunning;
        }

        return Result::Success;
    }

    Scheduler::Result Scheduler::Shutdown(bool fast)
    {
        std::unique_lock lk{ m_mutex };
        switch (m_state)
        {
            case State::Running:
                m_state = fast
                    ? State::ShuttingDownFast
                    : State::ShuttingDownSlow;
                m_cv.notify_one();
                return Result::Success;

            case State::NotRunning:
                return Result::NotRunning;

            case State::ShuttingDownSlow:
            case State::ShuttingDownFast:
                return Result::IsShuttingDown;
        }

        std::terminate();
    }

    void Scheduler::ThreadLoop()
    {
        std::unique_lock lk{ m_mutex };

        while (true)
        {
            Task* task{ m_buffer.NextTaskDue() };
            bool timerFinished =
                !m_cv.wait_until(lk, task ? task->due : TimePoint::max(), [this, task] {
                    return m_buffer.NextTaskDue() != task ||
                           m_state == State::ShuttingDownFast ||
                           m_state == State::ShuttingDownSlow && !m_buffer.AnyTasks();
                });

            if (m_state == State::Running ||
                m_state == State::ShuttingDownSlow)
            {
                if (timerFinished)
                {
                    m_mutex.unlock();
                    m_pool->Schedule(std::move(task->func));
                    m_mutex.lock();
                    m_buffer.RemoveTask(task);
                }
            }

            if (m_state == State::ShuttingDownSlow && !m_buffer.AnyTasks() ||
                m_state == State::ShuttingDownFast)
            {
                m_buffer.ClearTasks();
                return;
            }
        }
    }
}