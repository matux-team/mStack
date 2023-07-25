#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_
#include <core/base.h>
#include <core/system.h>

#define EVENT_POOL_SIZE     128
#define EVENT_QUEUE_SIZE	2048

namespace core
{

class EventQueue//: public AbstractEventQueue
{
public:
	virtual ~EventQueue(){}
    EventQueue(){}
	inline bool next()
	{
		if (inPtr_ == outPtr_) return false;
        container_t container_ = pop_();
        if (container_.index_ < poolSize_)
        {
            Event* e = events_[container_.index_];
            e->execute();
        }
		return true;
	}

    inline void post(container_t container)
    {
        uint16_t avail = size_ + outPtr_ - inPtr_;
        if (avail > size_) avail -= size_;
        if (avail < 1) return;

        push_(container);
    }

//    inline void postByte(uint8_t index, uint8_t c)
//    {
//        uint16_t avail = size_ + outPtr_ - inPtr_;
//        if (avail > size_) avail -= size_;
//        if (avail < 2) return;
//
//        DISABLE_INTERRUPT;
//        push_(index);
//        push_(c);
//        ENABLE_INTERRUPT;
//    }

//    inline void pushFixed(uint8_t index, uint8_t* data, uint8_t size) override
//    {
//        uint16_t avail = size_ + outPtr_ - inPtr_;
//        if (avail > size_) avail -= size_;
//        if (avail < size+1) return;
//
//        DISABLE_INTERRUPT;
//        push_(index);
//        for (int i=0;i<size;i++)
//        {
//            push_(data[i]);
//        }
//        ENABLE_INTERRUPT;
//    }
//
//    inline void popFixed(uint8_t* data, uint8_t size) override
//    {
//        for (int i=0;i<size;i++)
//        {
//            data[i] = pop_();
//        }
//    }
//
//    inline void pushBuffer(uint8_t index, uint8_t* data, uint8_t size) override
//    {
//        uint16_t avail = size_ + outPtr_ - inPtr_;
//        if (avail > size_) avail -= size_;
//        if (avail < size+2) return;
//
//        DISABLE_INTERRUPT;
//        push_(index);
//        push_(size);
//        for (int i=0;i<size;i++)
//        {
//            push_(data[i]);
//        }
//        ENABLE_INTERRUPT;
//    }
//
//    inline void popBuffer(uint8_t* data, uint8_t& size) override
//    {
//        size = pop_();
//        for (int i=0;i<size;i++)
//        {
//            data[i] = pop_();
//        }
//    }

private:
    inline void push_(container_t val)
    {
        *(inPtr_) = val;
        inPtr_++;
        if (inPtr_ == last_) inPtr_ = first_;
    }

    inline container_t pop_()
    {
    	container_t ret = *(outPtr_);
		outPtr_++;
		if (outPtr_ == last_) outPtr_ = first_;
		return ret;
    }

private:
    container_t buffer_[EVENT_QUEUE_SIZE];
    uint16_t size_ = EVENT_QUEUE_SIZE;
    container_t* first_ = buffer_;
    container_t* last_ = buffer_ + EVENT_QUEUE_SIZE;
    container_t* inPtr_ = buffer_;
    container_t* outPtr_ = buffer_;
private:
    Event* events_[EVENT_POOL_SIZE];
    uint8_t poolSize_ = 0;

    uint8_t registerEvent_(Event* event)
    {
        events_[poolSize_] = event;
        if (poolSize_>= EVENT_POOL_SIZE)
        {
            /*TODO: warning here*/
        	Error_Handler();
        }
        return poolSize_++;
    }

    friend class Event;
    friend class Engine;
    friend class Strand;
};

}

#endif /* CORE_EVENT_QUEUE_H_ */
