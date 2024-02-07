#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_
#include <core/base.h>
#include <core/system.h>

namespace core
{

class EventQueue
{
public:
	~EventQueue()
	{
		delete[] payBuf_;
	}
    EventQueue()
    {
    	payBuf_ = new void*[DATA_QUEUE_SIZE];
#ifndef RELEASE
    	if(payBuf_ == nullptr)Error_Handler();
#endif
    }
    uint16_t getMaxSizeEv(){return this->maxSizeEv_;}
    uint16_t getMaxSizePay(){return this->maxSizePay_;}
	inline bool next()
	{
		if (size_ == 0) return false;
        uint8_t index = pop_();
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

    inline void post(uint8_t index, void* payloadAddr = nullptr)
    {
    	if (size_ == capacity_ || paySize_ == payCapacity_)	Error_Handler();	// 1 of 2 Queues Full
#ifndef RELEASE
    	if(size_ > maxSizeEv_)maxSizeEv_ = size_;
    	if(paySize_ > maxSizePay_)maxSizePay_ = paySize_;
#endif
        DISABLE_INTERRUPT;
        push_(index);
        if(payloadAddr != nullptr) pushPay_(payloadAddr);
        ENABLE_INTERRUPT;
    }

    inline void* popPayload()
    {
    	if (paySize_ == 0)	Error_Handler();	// Payload Queue Empty
    	return popPay_();
    }

private:
    inline void push_(uint8_t val)
    {
		buffer_[rear_] = val;
		rear_ = (rear_ + 1) == capacity_ ? 0 : (rear_ + 1);
		size_++;
    }

    inline uint8_t pop_()
    {
		uint8_t data = buffer_[front_];
		front_ = (front_ + 1) == capacity_ ? 0 : (front_ + 1);
		size_--;
		return data;
    }

    inline void pushPay_(void* val)
    {
		payBuf_[payRear_] = val;
		payRear_ = (payRear_ + 1) == payCapacity_ ? 0 : (payRear_ + 1);
		paySize_++;
	}

    inline void* popPay_()
    {
		void* data = payBuf_[payFront_];
		payFront_ = (payFront_ + 1) == payCapacity_ ? 0 : (payFront_ + 1);
		paySize_--;
		return data;
    }

private:
    void** payBuf_ = nullptr;
    uint8_t payCapacity_ = DATA_QUEUE_SIZE;
    uint8_t payFront_ = 0;
    uint8_t payRear_ = 0;
    uint8_t paySize_ = 0;

    uint8_t buffer_[EVENT_QUEUE_SIZE];
    uint8_t capacity_ = EVENT_QUEUE_SIZE;
    uint8_t front_ = 0;
    uint8_t rear_ = 0;
    uint8_t size_ = 0;

private:
    Event* events_[EVENT_POOL_SIZE];
    uint8_t maxSizeEv_ = 0;
    uint8_t maxSizePay_ = 0;
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
    friend class Strand;
};

}

#endif /* CORE_EVENT_QUEUE_H_ */
