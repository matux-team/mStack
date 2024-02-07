/*
 * strand.h
 *
 *  Created on: Feb 6, 2024
 *      Author: thanh.nguyen16
 */

#ifndef CORE_STRAND_H_
#define CORE_STRAND_H_

#include <core/event.h>
#include <core/queue.h>
#include <core/system.h>
#include <core/event-queue.h>
#include <core/engine.h>
#include <core/task.h>

namespace core
{

class Strand: public core::Component
{
public:
    Strand(core::Queue& queue): queue_(queue){}

    void post(EmptyEvent* event, ByteEvent* finished = nullptr)
    {
    	if (queue_.available() < 3) return;

		DISABLE_INTERRUPT;
		if (finished == nullptr) queue_.push(VOID_EMPTY);
		else
		{
			queue_.push(CALLBACK_EMPTY);
			queue_.push(finished->index_);
		}
		queue_.push(event->index_);
		ENABLE_INTERRUPT;
		next_();

    }

    template<typename E>
    void post(FixedEvent<E>* event, const E& e, ByteEvent* finished = nullptr)
    {
    	static uint8_t min = 3 + sizeof(void*);
        if (queue_.available() < min) return;
        DISABLE_INTERRUPT;
        if (finished == nullptr)	queue_.push(VOID_FIXED);
        else
        {
            queue_.push(CALLBACK_FIXED);
            queue_.push(finished->index_);
        }
        queue_.push(event->index_);
        //Allocate
        void *p = event->pool_->Alloc();
#ifndef RELEASE
    	if(p  == nullptr) Error_Handler();	// Cannot Allocate, Pool Over
#else
    	if(p  == nullptr) return;
#endif
    	memcpy(p, &e, sizeof(E));
        //Push Payload to queue
        uint8_t* ptr = (uint8_t*)((uint32_t)&p);
        for (size_t i = 0; i < sizeof(void*); i++)
        {
            queue_.push(ptr[i]);
        }
        ENABLE_INTERRUPT;
        next_();
    }

    void delay(uint32_t ms)
    {
        if (queue_.available()<5) return;
        DISABLE_INTERRUPT;
        queue_.push(DELAY);
        queue_.push((ms >> 24) & 0xFF);
        queue_.push((ms >> 16) & 0xFF);
        queue_.push((ms >> 8) & 0xFF);
        queue_.push(ms & 0xFF);
        ENABLE_INTERRUPT;
        next_();
    }

    void done()
    {
        busy_ = false;
        next_();
    }

    void done(uint8_t error)
    {
        busy_ = false;
        //if (finished_ != nullptr) finished_->post();
        if (finished_ != nullptr) finished_->execute_(&error);
        next_();
    }

    enum EventType
    {
        VOID_EMPTY=1, CALLBACK_EMPTY,
		VOID_FIXED, CALLBACK_FIXED,
		DELAY
    };

    void next_()
    {
        if (busy_ || queue_.isEmpty()) return;
        executeEvent_.post();
        busy_ = true;
    }

    void timeout_()
    {
        busy_ = false;
        next_();
    }

    void execute_()
    {
        uint8_t type = queue_.pop();
        if (type == DELAY)
        {
            uint32_t time = queue_.pop();
            time <<= 8; time += queue_.pop();
            time <<= 8; time += queue_.pop();
            time <<= 8; time += queue_.pop();
            timer_.start(time,1);
            finished_ = nullptr;
        }
        else if (type == VOID_EMPTY)
        {
            finished_ = nullptr;
            uint8_t index = queue_.pop();
            if (index < events_.poolSize_)
            {
            	Event* e = events_.events_[index];
                e->execute();
            }
        }
        else if (type == CALLBACK_EMPTY)
        {
            uint8_t index = queue_.pop();
            if (index < events_.poolSize_) finished_ = (ByteEvent*)events_.events_[index];
            else finished_ = nullptr;
            index = queue_.pop();
            if (index < events_.poolSize_)
            {
            	Event* e = events_.events_[index];
                e->execute();
            }
        }
        else if (type == VOID_FIXED)
        {
            finished_ = nullptr;
            uint8_t index = queue_.pop();

            uint32_t payload = 0;
            uint8_t* ptr = (uint8_t*)&payload;

            for(uint8_t i = 0; i < sizeof(void*); i++)
            {
            	ptr[i] = queue_.pop();
            }

            if (index < events_.poolSize_)
            {
                Event* e = events_.events_[index];
                e->execute_((void*)payload, true);
            }
        }
        else if (type == CALLBACK_FIXED)
        {
            uint8_t index = queue_.pop();
            if (index < events_.poolSize_) finished_ = (ByteEvent*)events_.events_[index];
            else finished_ = nullptr;

            uint32_t payload = 0;
            uint8_t* ptr = (uint8_t*)&payload;

            for(uint8_t i = 0; i < sizeof(void*); i++)
            {
            	ptr[i] = queue_.pop();
            }
            index = queue_.pop();
            if (index < events_.poolSize_)
            {
                Event* e = events_.events_[index];
                e->execute_((void*)payload, true);
            }
        }
    }
private:
    Queue& queue_;
    bool busy_ = false;
    ByteEvent* finished_ = nullptr;
    EmptyEvent executeEvent_ = EmptyEvent(this, (EmptyEvent::Handler)&Strand::execute_);
    EventQueue& events_ = Engine::instance().events();
    Task timer_ = Task(this,(Task::Handler)&Strand::timeout_);
};

}

#define M_STRAND(name, size)\
private:\
    QUEUE_DEF(name##Queue, size)\
public:\
    core::Strand name##Strand = core::Strand(name##Queue##_);

#endif /* CORE_STRAND_H_ */
