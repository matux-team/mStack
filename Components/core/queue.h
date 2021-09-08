#ifndef CORE_QUEUE_H
#define CORE_QUEUE_H
#include <stdint.h>

namespace core
{

class Queue
{
public:
    Queue(uint8_t* buf, uint16_t size)
    {
        first_ = buf;
        size_ = size;
        last_ = first_ + size;
        inPtr_ = first_;
        outPtr_ = first_;
    }
    uint16_t available()
    {
    	uint16_t ret = size_+outPtr_-inPtr_;
    	if (ret > size_) ret -= size_;
    	return ret;
    }
    bool notEmpty(){return (inPtr_ != outPtr_);}
    bool empty(){return (inPtr_ == outPtr_);}

    void reset()
    {
        inPtr_ = first_;
        outPtr_ = first_;
    }

    void push(uint8_t val)
    {
    	uint8_t* next = inPtr_ + 1;
    	if (next == last_) next = first_;
		if (next!=outPtr_) //queue not full
		{
			*inPtr_ = val;
			inPtr_ = next;
		}
    }

    uint8_t pop()
    {
    	if (outPtr_ != inPtr_)
		{
			uint8_t ret = *outPtr_;
			outPtr_++;
			if (outPtr_ == last_) outPtr_ = first_;
			return ret;
		}
        return 0;
    }
private:
    uint16_t size_;
    uint8_t* first_;
    uint8_t* last_;
    uint8_t* inPtr_;
    uint8_t* outPtr_;
};

}

#define QUEUE_DEF(name, size)\
uint8_t name##QueueBuffer_[size];\
core::Queue name##_ = core::Queue(name##QueueBuffer_, size);

#endif // QUEUE_H
