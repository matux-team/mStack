#ifndef CORE_BASE_H
#define CORE_BASE_H
#include <stdint.h>
#include "main.h"

#define _M_MACRO_2(_1,_2,NAME,...) NAME
#define _M_MACRO_3(_1,_2,_3,NAME,...) NAME

namespace core
{

//class AbstractEventQueue
//{
//public:
//    virtual void pushFixed(uint8_t index, uint8_t* data, uint8_t size)=0;
//    virtual void popFixed(uint8_t* data, uint8_t size)=0;
//    virtual void pushBuffer(uint8_t index, uint8_t* data, uint8_t size)=0;
//    virtual void popBuffer(uint8_t* data, uint8_t& size)=0;
//};

class Component
{
public:
    virtual void init(){/*empty*/}
    virtual ~Component(){}
};

//class Event
//{
//public:
//    Event();
//    virtual ~Event(){};
//protected:
//    virtual void execute(core::AbstractEventQueue* queue) = 0;
//    Event(uint8_t index): index_(index){}
//    uint8_t index_;
//    friend class EventQueue;
//    friend class Strand;
//};

typedef struct Container
{
	uint8_t index_;
	void* payload_;
}container_t;

class Event
{
public:
    Event();
    virtual ~Event(){};

protected:
    virtual void execute(){};
    Event(uint8_t index)
    {
    	container_.index_ = index;
    	container_.payload_ = nullptr;
    }
    Event(uint8_t index, void* func)
    {
    	container_.index_ = index;
    	container_.payload_ = func;
    }
	container_t container_;
    friend class EventQueue;
    friend class Strand;
};

}

#define COMPONENT(module, name)\
namespace module{\
class name: public core::Component{\
using CLASS = module::name;\
public: static name& instance(){static name instance;return instance;}\
private:\
    virtual ~name(){}\
    name(){}

#define COMPONENT_END };}
#define COMPONENT_REG(module, name) module::name::instance().init()

#define HAL_DEF(module)\
namespace module{\
class HAL{\
public:

#define HAL_END };}

#endif // COMPONENT_H
