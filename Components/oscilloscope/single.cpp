#include <oscilloscope/single.h>
#include <console/controller.h>

osc::Single::Single(uint8_t channel)
{
    c_.channel = channel;
}

void osc::Single::thresholding_(uint16_t v)
{
    if (threshold_ > 0)
    {
        total_ = 0;
        state_ = &osc::Single::probing_;
        return;
    }

    if (v < min_)
        min_ = v;
    if (v > max_)
        max_ = v;
    if (total_++ > MARGIN_TOTAL_SIZE)
    {
        threshold_ = (min_ + max_) / 2;
        min_ = MAX_THRESHOLD;
        max_ = MIN_THRESHOLD;
        total_ = 0;
        console::Controller::instance().printf("Auto detect threshold:%d",
                threshold_);
        state_ = &osc::Single::probing_;
    }
}

void osc::Single::probing_(uint16_t v)
{
    if (++total_ > MARGIN_TOTAL_SIZE)
    {
        total_ = 0;
        threshold_ = -1;
        state_ = &osc::Single::thresholding_;
    }

    if ((c_.lastVal < threshold_) && (v >= threshold_))
    {
        state_ = &osc::Single::first_;
    }
}

void osc::Single::first_(uint16_t v)
{
    c_.reset(v);
    total_ = 1;
    state_ = &osc::Single::adding_;
}

void osc::Single::adding_(uint16_t v)
{
    c_.add(v);
    total_++;
    if ((total_ > MARGIN_TOTAL_SIZE) || (c_.index > MARGIN_BUFFER_INDEX - 3))
    {
        state_ = &osc::Single::idle_;
        total_ = 0;
        c_.prepareFlush();
        flushEvent_.post();
    }
}

void osc::Single::flush_()
{
    if (c_.flush())
    {
        state_ = &osc::Single::probing_;
    }
    else
    {
        flushEvent_.post();
    }
}
