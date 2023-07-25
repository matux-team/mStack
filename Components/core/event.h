#ifndef CORE_EVENT_H
#define CORE_EVENT_H
#include <stdint.h>
#include <core/queue.h>
#include <core/base.h>
#include <core/engine.h>
#include <core/mem-pool.h>

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
    void execute(){/*empty*/}
    NullEvent(){};
};

class EmptyEvent: public Event
{
public:
    typedef void (Component::*Handler) ();
    EmptyEvent(Component* component, Handler handler):component_(component), handler_(handler){}
    void post()
    {
        core::Engine::instance().events().post(container_);
    }
private:
    void execute() override
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
    FixedEvent(Component* component, Handler handler)
    {
    	component_ = component;
    	handler_ = handler;
//    	Declare MemPool with correspond type here
    	pool_ = new MemPool<E>(3);	//FIXME: 3 is constant
    }
    void post(const E& e)
    {
    	container_.payload_ = pool_->Alloc();
    	memcpy(container_.payload_, &e, sizeof(E));

    	core::Engine::instance().events().post(container_);
    }
protected:
    void execute() override
    {
    	E* e = (E*)container_.payload_;
        (component_->*handler_)(*e);
    }

    inline void execute_(E* e){(component_->*handler_)(*e);}
    Component *component_ = nullptr;
    Handler handler_;
    MemPool<E>* pool_;
    friend class Strand;
};

//template<typename E>
//class ObjectEvent: public Event
//{
//public:
//    typedef void (Component::*Handler) (const E&);
//    ObjectEvent(Component* component, Handler handler): component_(component), handler_(handler){}
//    void post(const E& e)
//    {
//        e.push(core::Engine::instance().events());
//    }
//protected:
//    void execute(core::AbstractEventQueue* queue) override
//    {
//        E e;
//        if (e.pop(core::Engine::instance().events()))
//        {
//            (component_->*handler_)(e);
//        }
//    }
//
//    Component *component_ = nullptr;
//    Handler handler_;
//    friend class Strand;
//};

//typedef FixedEvent<uint8_t> ByteEvent;
//
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
