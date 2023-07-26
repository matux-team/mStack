#ifndef CORE_BASE_H
#define CORE_BASE_H
#include <stdint.h>
#include "main.h"

#define _M_MACRO_2(_1,_2,NAME,...) NAME
#define _M_MACRO_3(_1,_2,_3,NAME,...) NAME
#define _M_MACRO_4(_1,_2,_3,_4,NAME,...) NAME

enum EventStatus : uint8_t {POST_FAILED = 0, POST_SUCCESS, ALLOCATION_FAILED};

namespace core
{

class Component
{
public:
    virtual void init(){/*empty*/}
    virtual ~Component(){}
};

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
