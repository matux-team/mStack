#ifndef CORE_SIGNAL_H
#define CORE_SIGNAL_H
#include <core/event.h>

namespace core
{

class EmptySignalOne
{
public:
    void connect(EmptyEvent* event){this->event_ = event;}
    void disconnect(){event_ = nullptr;}
    inline void emit(){if (event_ != nullptr) event_->post();}
private:
    EmptyEvent* event_ = nullptr;
};

template <typename EV, typename E>
class SignalOne
{
public:
    void connect(EV* event){this->event_ = event;}
    void disconnect(){event_ = nullptr;}
    inline void emit(E e){if (event_ != nullptr) event_->post(e);}
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
    inline void emit()
    {
        for (Connection* it = connections_; it!=nullptr; it=it->next)
        {
            it->event->post();
        }
    }
};

template <typename EV, typename E>
class SignalMany: public BaseSignalMany<EV>
{
public:
    inline void emit(E e)
    {
        for (auto it = this->connections_; it!=nullptr; it=it->next)
        {
            it->event->post(e);
        }
    }
};

}

#define M_SIGNAL_ONE(name)\
public:\
    core::EmptySignalOne name##Signal;

#define M_SIGNAL_MANY(name)\
public:\
    core::EmptySignalMany name##Signal;

#define M_FIXED_SIGNAL_ONE(type, name)\
public:\
    core::SignalOne<core::FixedEvent<type>, type> name##Signal;

#define M_FIXED_SIGNAL_MANY(type, name)\
public:\
    core::SignalMany<core::FixedEvent<type>, type> name##Signal;

#define M_OBJECT_SIGNAL_ONE(type, name)\
public:\
    core::SignalOne<core::ObjectEvent<type>, type> name##Signal;

#define M_OBJECT_SIGNAL_MANY(type, name)\
public:\
    core::SignalMany<core::ObjectEvent<type>, type> name##Signal;

#endif // SIGNAL_H
