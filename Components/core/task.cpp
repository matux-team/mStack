#include <core/task.h>
#include <core/engine.h>

core::Task::Task(Component* component, Handler handler): component_(component), handler_(handler)
{
    core::Engine::instance().registerTask_(this);
}

void core::Task::start(uint32_t interval, int32_t loop)
{
    core::Engine& engine = core::Engine::instance();
    this->interval_ = interval;
    this->nextTick_ = engine.tickCount_ + interval;
    this->loop_ = loop;
    engine.startTask_(this);
}

void core::Task::stop()
{
	loop_ = 0;
    core::Engine::instance().stopTask_(this);
}

void core::Task::run_()
{
    if (--loop_ == 0)
    {
        core::Engine::instance().stopTask_(this);
    }
    else
    {
        nextTick_ += interval_;
        if (loop_ < 0) loop_ = -1;
    }

    (component_->*handler_)();
}

