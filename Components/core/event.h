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
    EventStatus post()
    {
    	return core::Engine::instance().events().post(container_);
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
    FixedEvent(Component* component, Handler handler, uint8_t numOfMem = 3)	// 3 is just default value
    {
    	component_ = component;
    	handler_ = handler;
    	//Declare MemPool with correspond type here
    	pool_ = new MemPool<E>(numOfMem);
    }
    EventStatus post(const E& e)
    {
    	void* p = pool_->Alloc();
    	if(p  == nullptr)	// Cannot Allocate, Pool Over
    	{
    		return EventStatus::ALLOCATION_FAILED;
    	}
    	container_.payload_ = p;

    	memcpy(container_.payload_, &e, sizeof(E));

    	return core::Engine::instance().events().post(container_);
    }
protected:
    void execute() override
    {
        (component_->*handler_)(*((E*)container_.payload_));
        pool_->Free(container_.payload_);
    }

    inline void execute_(E* e){(component_->*handler_)(*e);}
    Component *component_ = nullptr;
    Handler handler_;
    MemPool<E>* pool_;
    friend class Strand;
};

typedef FixedEvent<uint8_t> ByteEvent;

}

#define M_EVENT(...) _M_MACRO_3(__VA_ARGS__,_M_FIXED_NUM_EVENT, _M_FIXED_EVENT, _M_EVENT)(__VA_ARGS__)
#define M_EVENT_HANDLER(...) _M_MACRO_4(__VA_ARGS__, _M_FIXED_NUM_EVENT, _M_FIXED_EVENT_HANDLER, _M_EVENT_HANDLER)(__VA_ARGS__)

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

#define _M_FIXED_NUM_EVENT(name, type, num)\
public:\
    core::FixedEvent<type> name##Event = core::FixedEvent<type>(this, (core::FixedEvent<type>::Handler)&CLASS::name##Handler##_, num);\
private:\
    void name##Handler##_(const type& event);

#define _M_EVENT_HANDLER(cls,name) void cls::name##Handler##_()
#define _M_FIXED_EVENT_HANDLER(cls, name, type) void cls::name##Handler##_(const type& event)

#endif // MODEL_H
