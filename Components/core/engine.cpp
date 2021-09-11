#include "engine.h"
#include <core/system.h>
#include <core/event.h>
#include <console/log.h>

core::Event::Event()
{
    index_ = core::Engine::instance().events().registerEvent_(this);
}

core::Engine::Engine(): Event(0)
{
    events_.registerEvent_(this);
    systemInit();
}

void core::Engine::run()
{
    while (true)
    {
        if (events_.next()) continue;
        WAIT_FOR_INTERUPT;
    }
}

void core::Engine::registerTask_(Task* task)
{
    task->next_ = tasks_;
    tasks_ = task;
}

void core::Engine::startTask_(Task* task)
{
    Task* prev = nullptr;
    for (Task* it = tasks_; it!=nullptr; it = it->next_)
    {
        if (it == task)
        {
            if (prev == nullptr) tasks_ = it->next_;
            else prev->next_ = it->next_;
            task->next_ = activeTasks_;
            activeTasks_ = task;
            break;
        }
        prev = it;
    }
    if (nextTick_ > task->nextTick_) nextTick_ = task->nextTick_;
}

void core::Engine::stopTask_(Task* task)
{
    Task* prev = nullptr;
    for (Task* it = activeTasks_; it!=nullptr; it = it->next_)
    {
        if (it == task)
        {
            if (prev == nullptr) activeTasks_ = it->next_;
            else prev->next_ = it->next_;
            task->next_ = tasks_;
            tasks_ = task;
            break;
        }
        prev = it;
    }
}

void core::Engine::delay(uint32_t t)
{
    auto timeout = tickCount_ + t;
    while (tickCount_ < timeout){NO_OPERATION;}
}

void core::Engine::execute(core::AbstractEventQueue* queue)
{
    uint64_t min = LAST_TICK;
    Task* it = activeTasks_;
    Task* next;
    while (it!=nullptr)
    {
        next = it->next_;
        if (min > it->nextTick_) min = it->nextTick_;
        if (tickCount_ >= it->nextTick_) it->run_();
        it = next;
    }
    nextTick_ = min;
}
