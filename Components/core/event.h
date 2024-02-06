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
    void execute(){/*empty*/}
    NullEvent(){};
};

class EmptyEvent: public Event
{
public:
    typedef void (Component::*Handler) ();
    EmptyEvent(Component* component, Handler handler):component_(component), handler_(handler){}
    inline void post()
    {
    	Engine::instance().events().post(index_);
    }
private:
    inline void execute() override
    {
        (component_->*handler_)();
    }

    inline void execute_(){(component_->*handler_)();}

    Component *component_ = nullptr;
    Handler handler_;
    friend class Strand;
    friend class EmptySignalOne;
    friend class EmptySignalMany;
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
    		if(pool_ == nullptr) Error_Handler();	// Cannot Allocating
    	}
    }

    void post(const E& e)
    {
		if(pool_ == nullptr) Error_Handler();	// Pool still not be allocated

    	DISABLE_INTERRUPT;
    	void* p = pool_->Alloc();
    	ENABLE_INTERRUPT;

#ifndef RELEASE
    	if(p  == nullptr) Error_Handler();	// Cannot Allocate, Pool Over
#else
    	if(p  == nullptr) return;
#endif
    	memcpy(p, &e, sizeof(E));

    	Engine::instance().events().post(index_, p);
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
    inline void execute() override
    {
    	void* payload = Engine::instance().events().popPayload();
        (component_->*handler_)(*((E*)payload));
        pool_->Free(payload);
    }

    inline void execute_(E* e){(component_->*handler_)(*e);}

    Component *component_ = nullptr;
    Handler handler_ = nullptr;
    MemPool<E>* pool_ = nullptr;
    friend class Strand;
    // Add this friend declaration for SignalOne
    template <typename EV, typename EventE>
    friend class SignalOne;
    template <typename EV, typename EventE>
    friend class SignalMany;
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
