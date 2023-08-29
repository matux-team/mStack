#ifndef CORE_SIGNAL_H
#define CORE_SIGNAL_H
#include <core/event.h>
#include <core/base.h>

namespace core
{

class EmptySignalOne
{
public:
    void connect(EmptyEvent* event){this->event_ = event;}
    void disconnect(){event_ = nullptr;}
    inline EventStatus emit()
    {
    	if (event_ != nullptr) return event_->post();
    	return EventStatus::POST_FAILED;
    }
private:
    EmptyEvent* event_ = nullptr;
};

template <typename EV, typename E>
class SignalOne
{
public:
    void connect(EV* event){this->event_ = event;}
    void disconnect(){event_ = nullptr;}
    inline EventStatus emit(E& e)
    {
    	if (event_ != nullptr) return event_->post(e);
    	return EventStatus::POST_FAILED;
    }
private:
    EV* event_ = nullptr;
};

template<class E>
class BaseSignalMany
{
public:
    void connect(E* event)
    {
    	for (Connection* it = connections_; it!=nullptr; it=it->next)
    	{
    		if (it->event == event) return;
    	}

        Connection* con = new Connection;
        con->event = event;
        con->next = connections_;
        connections_ = con;
    }

    void disconnect(E* event)
    {
        Connection* pre = nullptr;
        for (Connection* it = connections_; it!=nullptr; it=it->next)
        {
            if (it->event == event)
            {
                if (pre == nullptr) connections_ = it->next;
                else pre->next = it->next;
                delete it;
                return;
            }
            pre = it;
        }
    }
protected:
    struct Connection
    {
        E* event;
        Connection* next;
    };
    Connection* connections_ = nullptr;
};

class EmptySignalMany: public BaseSignalMany<EmptyEvent>
{
public:
    inline EventStatus emit()
    {
        for (Connection* it = connections_; it!=nullptr; it=it->next)
        {
        	if( it->event->post() != EventStatus::POST_SUCCESS) return EventStatus::POST_FAILED;
        }
        return EventStatus::POST_SUCCESS;
    }
};

template <typename EV, typename E>
class SignalMany: public BaseSignalMany<EV>
{
public:
    inline EventStatus emit(E e)
    {
        for (auto it = this->connections_; it!=nullptr; it=it->next)
        {
			EventStatus status;
			status = it->event->post(e);
			if( status != EventStatus::POST_SUCCESS) return status;
        }
        return EventStatus::POST_SUCCESS;
    }
};

}

#define M_SIGNAL(...) _M_MACRO_2(__VA_ARGS__,_M_FIXED_SIGNAL_ONE, _M_SIGNAL_ONE)(__VA_ARGS__)
#define M_SIGNAL_MANY(...) _M_MACRO_2(__VA_ARGS__,_M_FIXED_SIGNAL_MANY, _M_SIGNAL_MANY)(__VA_ARGS__)

#define _M_SIGNAL_ONE(name)\
public:\
    core::EmptySignalOne name##Signal;

#define _M_FIXED_SIGNAL_ONE(name,type)\
public:\
    core::SignalOne<core::FixedEvent<type>, type> name##Signal;

#define _M_SIGNAL_MANY(name)\
public:\
    core::EmptySignalMany name##Signal;

#define _M_FIXED_SIGNAL_MANY(name,type)\
public:\
    core::SignalMany<core::FixedEvent<type>, type> name##Signal;

#endif // SIGNAL_H
