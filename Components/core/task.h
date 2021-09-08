#ifndef CORE_TASK_H
#define CORE_TASK_H
#include <core/base.h>

namespace core
{

class Task
{
public:
    typedef void (Component::*Handler) ();
    Task(Component* component, Handler handler);
    void start(uint32_t interval, int32_t loop = -1);
    void stop();
    bool running(){return (loop_==0);}
private:
    void run_();
    Task* next_ = nullptr;
    uint32_t interval_;
    uint64_t nextTick_;
    int32_t loop_ = 0;
    Component *component_ = nullptr;
    Handler handler_;
    friend class Engine;
    friend class TickEvent;
};

}

#define M_TASK(name)\
private:\
    core::Task name##Task_ = core::Task(this, (core::Task::Handler)&CLASS::name##Handler##_);\
    void name##Handler##_();\

#define M_TASK_HANDLER(cls,name) void cls::name##Handler##_()

#endif // TASK_H
