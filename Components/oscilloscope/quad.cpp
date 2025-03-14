#include <oscilloscope/quad.h>
#include <console/controller.h>

osc::Quad::Quad(uint8_t c1, uint16_t c2, uint16_t c3, uint16_t c4)
{
    c1_.channel = c1;
    c2_.channel = c2;
    c3_.channel = c3;
    c4_.channel = c4;
}

void osc::Quad::thresholding_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4)
{
    if (threshold_ > 0)
    {
        total_ = 0;
        state_ = &osc::Quad::probing_;
        return;
    }

    if (v1 < min_) min_ = v1;
    if (v1 > max_) max_ = v1;
    if (total_++ > 1000)
    {
        threshold_ = (min_+max_)/2;
        min_ = 65535;
        max_ = 0;
        total_ = 0;
        console::Controller::instance().printf("Auto detect threshold:%d", threshold_);
        state_ = &osc::Quad::probing_;
    }
}

void osc::Quad::probing_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4)
{
    if (++total_ > 1000)
    {
        total_ = 0;
        threshold_ = -1;
        state_ = &osc::Quad::thresholding_;
    }

    if ((c1_.lastVal < threshold_) && (v1 >= threshold_))
    {
        state_ = &osc::Quad::first_;
    }
}

void osc::Quad::first_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4)
{
    c1_.reset(v1);
    c2_.reset(v2);
    c3_.reset(v3);
    c4_.reset(v4);
    total_ = 1;
    state_ = &osc::Quad::adding_;
}

void osc::Quad::adding_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4)
{
    c1_.add(v1);
    c2_.add(v2);
    c3_.add(v3);
    c4_.add(v4);

    total_++;
    if ((total_ > 1000) || (c1_.index > 1199) || (c2_.index > 1199)|| (c3_.index > 1199)|| (c4_.index > 1199))
    {
        state_ = &osc::Quad::idle_;
        total_ = 0;
        c1_.prepareFlush();
        c2_.prepareFlush();
        c3_.prepareFlush();
        c4_.prepareFlush();
        c1FlushEvent_.post();
    }
}

void osc::Quad::c1Flush_()
{
    if (c1_.flush())
    {
        c2FlushEvent_.post();
    }
    else
    {
        c1FlushEvent_.post();
    }
}

void osc::Quad::c2Flush_()
{
    if (c2_.flush())
    {
        c3FlushEvent_.post();
    }
    else
    {
        c2FlushEvent_.post();
    }
}

void osc::Quad::c3Flush_()
{
    if (c3_.flush())
    {
        c4FlushEvent_.post();
    }
    else
    {
        c3FlushEvent_.post();
    }
}

void osc::Quad::c4Flush_()
{
    if (c4_.flush())
    {
        state_ = &osc::Quad::probing_;
    }
    else
    {
        c4FlushEvent_.post();
    }
}
