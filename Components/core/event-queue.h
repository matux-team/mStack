#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_
#include <core/base.h>
#include <core/system.h>

namespace core
{

class EventQueue: public AbstractEventQueue
{
public:
	virtual ~EventQueue(){}
    EventQueue(){}
    uint16_t getMinAvail(){return this->minimumAvail_;}
	inline bool next()
	{
		if (inPtr_ == outPtr_) return false;
        static uint8_t index = pop_();
        if (index < poolSize_)
        {
            Event* e = events_[index];
            e->execute();
        }
#ifndef RELEASE
        else
        {
        	Error_Handler();
        }
#endif
		return true;
	}

    inline void post(uint8_t index)
    {
        uint16_t avail = size_ + outPtr_ - inPtr_; 	// maximum avail = size_ - 1;
        if (avail > size_) avail -= (size_);		// here we have to subtract 1 but we don't do that
#ifndef RELEASE
        if (avail < minimumAvail_) minimumAvail_ = avail;
        if (avail < 2)	Error_Handler();			// and here we compare with 2 instead of 1.
#else
		if (avail < 2) return;
#endif
        DISABLE_INTERRUPT;
        push_(index);
        ENABLE_INTERRUPT;
    }

    inline void pushFixed(uint8_t index, void* payloadAddr) override
    {
    	static uint8_t size = sizeof(payloadAddr);
    	uint8_t* ptr = (uint8_t*)payloadAddr;
        uint16_t avail = size_ + outPtr_ - inPtr_;
        if (avail > size_) avail -= size_;
#ifndef RELEASE
        if (avail < minimumAvail_) minimumAvail_ = avail;
        if (avail < 2 + size) Error_Handler();
#else
        if (avail < size + 2) return;
#endif
        DISABLE_INTERRUPT;
        push_(index);
        for (int i=0;i<size;i++)
        {
            push_(ptr[i]);
        }
        ENABLE_INTERRUPT;
    }

    inline void popFixed(uint8_t* data) override
    {
        for (uint16_t i = 0; i < sizeof(void*); i++)
        {
            data[i] = pop_();
        }
    }

private:
    inline void push_(uint8_t val)
    {
        *(inPtr_) = val;
        inPtr_++;
        if (inPtr_ == last_) inPtr_ = first_;
    }

    inline uint8_t pop_()
    {
    	uint8_t ret = *(outPtr_);
		outPtr_++;
		if (outPtr_ == last_) outPtr_ = first_;
		return ret;
    }

private:
    uint8_t buffer_[EVENT_QUEUE_SIZE];
    uint16_t size_ = EVENT_QUEUE_SIZE;
    uint8_t* first_ = buffer_;
    uint8_t* last_ = buffer_ + EVENT_QUEUE_SIZE;
    uint8_t* inPtr_ = buffer_;
    uint8_t* outPtr_ = buffer_;
private:
    Event* events_[EVENT_POOL_SIZE];
    uint16_t minimumAvail_ = EVENT_QUEUE_SIZE;
    uint8_t poolSize_ = 0;

    uint8_t registerEvent_(Event* event)
    {
        events_[poolSize_] = event;
        if (poolSize_>= EVENT_POOL_SIZE)
        {
            /*Over Event Pool*/
        	Error_Handler();
        }
        return poolSize_++;
    }

    friend class Event;
    friend class Engine;
};

}

#endif /* CORE_EVENT_QUEUE_H_ */
