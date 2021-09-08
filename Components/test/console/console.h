#ifndef BLINK_CONTROLLER_H
#define BLINK_CONTROLLER_H
#include <core/engine.h>
#include <console/controller.h>
#include <core/queue.h>
#include <oscilloscope/single.h>
#include <oscilloscope/dual.h>
#include <oscilloscope/triple.h>
#include <oscilloscope/quad.h>
#include <core/strand.h>

COMPONENT(test, Console)
	M_TASK(toggle);
	M_TASK(plot)
    U_ACTION(console::Controller::CommandType::SyncFields, sync)
    U_ACTION(101, start)
    U_ACTION(102, stop)
    U_INTEGER(100, interval)
    U_TEXT(103,name)
    U_ACTION(104,hello)
    M_TASK(oscilloscope)
    //O_SINGLE(single,2)
    //O_DUAL(dual,2,3)
    //O_TRIPLE(triple,2,3,4)
    O_QUAD(quad,2,3,4,5)

	M_TASK(strand)
	M_EVENT(testStrand)
	M_FIXED_EVENT(finished, uint8_t)
	M_STRAND(myStrand, 64);
public:
    void init() override;
private:
    uint32_t interval_ = 200;
    char name_[256];

    uint16_t sine_[400];
    uint16_t cosine_[400];
public:
    enum Command
    {
        Interval=100,
        Start,
        Stop,
        Name,
        Hello,
        Started,
        Stopped,

    };
COMPONENT_END

#endif // CONTROLLER_H
