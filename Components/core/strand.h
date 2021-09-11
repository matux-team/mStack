#ifndef CORE_STRAND_H
#define CORE_STRAND_H
#include <core/event.h>
#include <core/queue.h>
#include <core/system.h>
#include <core/event-queue.h>
#include <core/engine.h>
#include <core/task.h>

namespace core
{

class Strand: public core::Component, public AbstractEventQueue
{
public:
    Strand(core::Queue& queue): queue_(queue){}

    void post(EmptyEvent* event, ByteEvent* finished = nullptr)
    {
        if (queue_.available() < 3) return;

        DISABLE_INTERRUPT;
        if (finished!=nullptr)
        {
            queue_.push(CALLBACK);
            queue_.push(finished->index_);
        }
        else queue_.push(VOID);
        queue_.push(event->index_);
        ENABLE_INTERRUPT;
        next_();
    }

    template<typename E>
    void post(FixedEvent<E>* event, E e, ByteEvent* finished = nullptr)
    {
        if (queue_.available() < sizeof(E) + 3) return;
        DISABLE_INTERRUPT;
        if (finished!=nullptr)
        {
            queue_.push(CALLBACK);
            queue_.push(finished->index_);
        }
        else queue_.push(VOID);
        queue_.push(event->index_);
        uint8_t* ptr = (uint8_t*)&e;
        for (size_t i=0; i<sizeof(E); i++)
        {
            queue_.push(ptr[i]);
        }
        ENABLE_INTERRUPT;
        next_();
    }

    template<typename E>
    void post(ObjectEvent<E>* event, E e, ByteEvent* finished = nullptr)
    {
        if (queue_.available() < 3) return;
        DISABLE_INTERRUPT;
        if (finished!=nullptr)
        {
            queue_.push(CALLBACK);
            queue_.push(finished->index_);
        }
        else queue_.push(VOID);
        event->push(this);
        ENABLE_INTERRUPT;
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
private:
    inline void popFixed(uint8_t* data, uint8_t size) override
    {
        for (int i=0;i<size;i++)
        {
            data[i] = queue_.pop();
        }
    }

    inline void pushFixed(uint8_t index, uint8_t* data, uint8_t size) override
    {
        /*TODO*/
    }

    inline void pushBuffer(uint8_t index, uint8_t* data, uint8_t size)  override
    {
        /*TODO*/
    }

    inline void popBuffer(uint8_t* data, uint8_t& size)  override
    {
        /*TODO*/
    }

    enum EventType
    {
        VOID=1, CALLBACK, DELAY
    };

    void next_()
    {
        if (busy_ || queue_.empty()) return;
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
        else if (type == VOID)
        {
            finished_ = nullptr;
            uint8_t index = queue_.pop();
            if (index < events_.poolSize_)
            {
                Event* e = events_.events_[index];
                e->execute(this);
            }
        }
        else if (type == CALLBACK)
        {
            uint8_t index = queue_.pop();
            if (index < events_.poolSize_) finished_ = (ByteEvent*)events_.events_[index];
            else finished_ = nullptr;

            index = queue_.pop();
            if (index < events_.poolSize_)
            {
                Event* e = events_.events_[index];
                e->execute(this);
            }
        }
    }
private:
    core::Queue& queue_;
    bool busy_ = false;
    core::ByteEvent* finished_ = nullptr;
    core::EmptyEvent executeEvent_ = core::EmptyEvent(this, (EmptyEvent::Handler)&Strand::execute_);
    core::EventQueue& events_ = core::Engine::instance().events();
    core::Task timer_ = core::Task(this,(core::Task::Handler)&Strand::timeout_);
};

}

#define M_STRAND(name, size)\
private:\
    QUEUE_DEF(name##Queue, size)\
public:\
    core::Strand name##Strand = core::Strand(name##Queue##_);

#endif // STRAND_H
