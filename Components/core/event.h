#ifndef CORE_EVENT_H
#define CORE_EVENT_H
#include <stdint.h>
#include <core/queue.h>
#include <core/base.h>
#include <core/engine.h>

namespace core
{

class EventQueue;
class Strand;

class NullEvent: public Event
{
public:
    static NullEvent* instance()
    {
        static NullEvent event;
        return &event;
    }
private:
    virtual void execute(core::AbstractEventQueue* queue){/*empty*/}
    NullEvent(){};
};

class EmptyEvent: public Event
{
public:
    typedef void (Component::*Handler) ();
    EmptyEvent(Component* component, Handler handler):component_(component), handler_(handler){}
    void post()
    {
        core::Engine::instance().events().post(index_);
    }
private:
    void execute(core::AbstractEventQueue* queue) override
    {
        (component_->*handler_)();
    }

    void execute_(){(component_->*handler_)();}
    Component *component_ = nullptr;
    Handler handler_;
    friend class Strand;
};

template <typename E>
class FixedEvent: public Event
{
public:
    typedef void (Component::*Handler) (const E&);
    FixedEvent(Component* component, Handler handler): component_(component), handler_(handler){}
    void post(const E& e)
    {
        uint8_t* ptr = (uint8_t*)&e;
        core::Engine::instance().events().pushFixed(index_,ptr,sizeof (E));
    }
protected:
    void execute(core::AbstractEventQueue* queue) override
    {
        E e;
        uint8_t* ptr = (uint8_t*)&e;
        queue->popFixed(ptr, sizeof (E));
        (component_->*handler_)(e);
    }

    inline void execute_(E* e){(component_->*handler_)(*e);}
    Component *component_ = nullptr;
    Handler handler_;
    friend class Strand;
};

template<typename E>
class ObjectEvent: public Event
{
public:
    typedef void (Component::*Handler) (const E&);
    ObjectEvent(Component* component, Handler handler): component_(component), handler_(handler){}
    void post(const E& e)
    {
        e.push(core::Engine::instance().events());
    }
protected:
    void execute(core::AbstractEventQueue* queue) override
    {
        E e;
        if (e.pop(core::Engine::instance().events()))
        {
            (component_->*handler_)(e);
        }
    }

    Component *component_ = nullptr;
    Handler handler_;
    friend class Strand;
};

typedef FixedEvent<uint8_t> ByteEvent;

}

#define M_EVENT(...) _M_MACRO_2(__VA_ARGS__, _M_FIXED_EVENT, _M_EVENT)(__VA_ARGS__)
#define M_EVENT_HANDLER(...) _M_MACRO_3(__VA_ARGS__, _M_FIXED_EVENT_HANDLER, _M_EVENT_HANDLER)(__VA_ARGS__)

#define _M_EVENT(name)\
public:\
    core::EmptyEvent name##Event = core::EmptyEvent(this, (core::EmptyEvent::Handler)&CLASS::name##Handler##_);\
private:\
    void name##Handler##_();

#define _M_FIXED_EVENT(name, type)\
public:\
    core::FixedEvent<type> name##Event = core::FixedEvent<type>(this, (core::FixedEvent<type>::Handler)&CLASS::name##Handler##_);\
private:\
    void name##Handler##_(const type& event);

#define _M_EVENT_HANDLER(cls,name) void cls::name##Handler##_()
#define _M_FIXED_EVENT_HANDLER(cls, name, type) void cls::name##Handler##_(const type& event)

#endif // MODEL_H
