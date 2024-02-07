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
    uint16_t available() const {
        if (inPtr_ >= outPtr_) {
            return size_ - (inPtr_ - outPtr_) - 1;
        } else {
            return (outPtr_ - inPtr_) - 1;
        }
    }

    bool isFull() const {return (inPtr_ == outPtr_ - 1) || (inPtr_ == last_ - 1 && outPtr_ == first_);}
    bool isEmpty() const {return (inPtr_ == outPtr_);}

    void reset()
    {
        inPtr_ = first_;
        outPtr_ = first_;
    }

    void push(uint8_t val)
    {
    	if((inPtr_ == outPtr_ - 1) || (inPtr_ == last_ - 1 && outPtr_ == first_)) return;
		*inPtr_ = val;
		inPtr_ = (inPtr_ == last_ - 1) ? first_ : inPtr_ + 1;
    }

    uint8_t pop()
    {
    	if (outPtr_ != inPtr_)	//Queue not empty
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
