#ifndef CORE_MACHINE_H
#define CORE_MACHINE_H
#include <core/engine.h>
#include <core/event.h>

namespace core {

class Machine: public core::Component
{
public:
    typedef void (Machine::*State)();
protected:
    void NullState(){}
    State nextState_ = nullptr;
    State currentState_ = &Machine::NullState;
    virtual bool check_(uint8_t event, State next){return false;}
    virtual void start_(State state){currentState_ = state;}
protected:
    const static uint8_t ENTER = 0xFE;
    const static uint8_t EXIT = 0xFF;
};

class SimpleMachine: public core::Machine
{
public:
    void postEvent(uint8_t event)
    {
        postEvent_.post(event);
    }

    void execute(uint8_t& event)
    {
        nextState_ = nullptr;
        nextEvent_ = event; (this->*currentState_)();
        if (nextState_ != nullptr && nextState_ != currentState_)
        {
            nextEvent_ = EXIT; (this->*currentState_)();
            currentState_ = nextState_;
            nextEvent_ = ENTER; (this->*currentState_)();
        }
    }
protected:
    uint8_t nextEvent_ = 0;
protected:
    core::ByteEvent postEvent_ = core::ByteEvent(this, (core::ByteEvent::Handler)&SimpleMachine::execute);

    bool check_(uint8_t input, State state) override
    {
        if (nextEvent_ != input) return false;
        nextState_ = state;
        return true;
    }

    void start_(State s) override
    {
        currentState_ = s;
        nextEvent_ = ENTER;
        (this->*currentState_)();
    }
};

class ByteMachine: public core::Machine
{
public:
    void post(uint8_t c)
    {
        postEvent_.post(c);
    }
protected:
    uint8_t data_;
    core::ByteEvent postEvent_ = core::ByteEvent(this, (core::ByteEvent::Handler)&ByteMachine::execute_);
    void execute_(uint8_t& c)
    {
        data_ = c;
        nextState_ = nullptr;
        (this->*currentState_)();
        if (nextState_ != nullptr) currentState_ = nextState_;
    }
    void start_(State s) override
    {
        currentState_ = s;
    }
};

}

#define SM_SIMPLE(module, name) namespace module{class name: public core::SimpleMachine{\
using CLASS = module::name;\
public: static name& instance(){static name instance;return instance;}\
private:\
    virtual ~name(){}\
    name(){}

#define STATE_DEF(name) void name();
#define STATE_BODY(name) void name()

#define TRANSITION_(ev,st)\
if (nextState_ != nullptr) return;\
if (check_((uint8_t)ev, (core::Machine::State)&st))

#define ENTER_() if (nextEvent_ == ENTER)
#define EXIT_() if (nextEvent_ == EXIT)

#define SM_SWITCH(state) nextState_ = (core::Machine::State)&state
#define SM_START(state) this->start_((core::Machine::State)&state)

#define SM_BYTE(module, name) namespace module{class name: public core::ByteMachine{\
using CLASS = module::name;\
public: static name& instance(){static name instance;return instance;}\
private:\
    virtual ~name(){}\
    name(){}

#define SM_END };}

#endif // CORE_MACHINE_H
