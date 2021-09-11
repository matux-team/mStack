#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H
#include <core/queue.h>
#include <core/event-queue.h>
#include <core/utils.h>
#include <core/task.h>

#define LAST_TICK       0xFFFFFFFFFFFFFFFF

namespace core
{
class Event;
class Task;

class Engine: public Event
{
public:
    ~Engine(){}
    void run();
    inline void tick(); /*must be called in timer interrupt or SysTick interrupt, 1ms usually*/
    inline uint64_t tickCount(){return tickCount_;}
    void delay(uint32_t t); //t in ms, WARNING: this function is blocking, use in some limited context only
    EventQueue& events(){return events_;}
    static Engine& instance()
    {
        static Engine engine;
        return engine;
    }

private:
    void execute(core::AbstractEventQueue* queue) override;
    Engine();
    EventQueue events_;
    Task* tasks_ = nullptr;
    Task* activeTasks_ = nullptr;
    void registerTask_(Task* task);
    void startTask_(Task* task);
    void stopTask_(Task* task);
    volatile uint64_t tickCount_ = 0;
    uint64_t nextTick_ = LAST_TICK;

private:
    friend class Task;
};

inline void core::Engine::tick()
{
    if (++tickCount_ >= nextTick_) events_.post(index_);
}

}

#endif // ENGINE_H
