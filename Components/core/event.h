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

class NullEvent: public Event
{
public:
    static NullEvent* instance()
    {
        static NullEvent event;
        return &event;
    }
private:
    void execute(void* func){/*empty*/}
    NullEvent(){};
};

class EmptyEvent: public Event
{
public:
    typedef void (Component::*Handler) ();
    EmptyEvent(Component* component, Handler handler):component_(component), handler_(handler){}
    EventStatus post()
    {
    	container_t con = {
    			.index_ = this->index_,
    			.payload_ = nullptr,
    	};
    	return Engine::instance().events().post(con);
    }
private:
    void execute(void* func) override
    {
        (component_->*handler_)();
    }

    Component *component_ = nullptr;
    Handler handler_;
};

template <typename E>
class FixedEvent: public Event
{
public:
    typedef void (Component::*Handler) (const E&);
    FixedEvent(Component* component, Handler handler, uint8_t numOfMem = 0)	// 0 is just default value
    {
    	component_ = component;
    	handler_ = handler;
    	if(numOfMem != 0)
    	{
        	// Declare MemPool with correspond type here
        	pool_ = new MemPool<E>(numOfMem);
#ifndef RELEASE
		if(pool_ == nullptr) Error_Handler();	// Cannot Allocating
#endif
    	}
    }
    EventStatus post(const E& e)
    {
#ifndef RELEASE
		if(pool_ == nullptr) Error_Handler();	// Pool still not be allocated
#else
		if(pool_ == nullptr) return EventStatus::ALLOCATION_FAILED;
#endif
    	DISABLE_INTERRUPT;
    	void* p = pool_->Alloc();
    	ENABLE_INTERRUPT;

#ifndef RELEASE
    		if(p  == nullptr) Error_Handler();	// Cannot Allocate, Pool Over
#else
    		if(p  == nullptr) return EventStatus::ALLOCATION_FAILED;
#endif
    	memcpy(p, &e, sizeof(E));

    	container_t con = {
    			.index_ = this->index_,
				.payload_ = p,
    	};

    	return Engine::instance().events().post(con);
    }

    bool allocate(uint8_t size)
    {
    	if(pool_ == nullptr)
    	{
    		pool_ = new MemPool<E>(size);
    		return true;
    	}
    	else
    	{
    		return false;
    	}
    }
protected:
    void execute(void* func) override
    {
        (component_->*handler_)(*((E*)func));
        pool_->Free(func);
    }

    Component *component_ = nullptr;
    Handler handler_;
    MemPool<E>* pool_ = nullptr;
};

typedef FixedEvent<uint8_t> ByteEvent;

}

#define M_EVENT(...) _M_MACRO_3(__VA_ARGS__,_M_FIXED_NUM_EVENT, _M_FIXED_EVENT, _M_EVENT)(__VA_ARGS__)
#define M_EVENT_HANDLER(...) _M_MACRO_3(__VA_ARGS__, _M_FIXED_EVENT_HANDLER, _M_EVENT_HANDLER)(__VA_ARGS__)

#define _M_EVENT(name)\
public:\
    core::EmptyEvent name##Event = core::EmptyEvent(this, (core::EmptyEvent::Handler)&CLASS::name##Handler##_);\
private:\
    void name##Handler##_();

#define _M_FIXED_EVENT(name, type)\
public:\
    core::FixedEvent<type> name##Event = core::FixedEvent<type>(this, (core::FixedEvent<type>::Handler)&CLASS::name##Handler##_, 3);\
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
