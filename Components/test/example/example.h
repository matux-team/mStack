#ifndef TEST_EXAMPLE_EXAMPLE_H_
#define TEST_EXAMPLE_EXAMPLE_H_

#include <core/engine.h>
#include <core/event.h>
#include <core/task.h>
#include <core/machine.h>
#include <core/signal.h>
#include <console/controller.h>
#include <oscilloscope/quad.h>

MACHINE(ex, Test)

	M_TASK(timeout)
	M_TASK(plot)
	M_TASK(oscilloscope)
	M_EVENT(emptyEvent)
	M_EVENT(fixedEvent, uint32_t)

	O_QUAD(quad, 2,3,4,5)

	M_SIGNAL(empty)
	M_SIGNAL(fixed, uint16_t)

	M_EVENT(emptySignalReceived)
	M_EVENT(fixedSignalReceived, uint16_t)

	U_ACTION(101, start)
	U_ACTION(102, stop)
	U_INTEGER(100, integer)
	U_TEXT(103, name)

public:
	void init();

private:
	enum class Event{TIMEOUT = 0};
private:
	STATE_DEF(StartUp)
	STATE_DEF(Running)
	STATE_DEF(Pause)

private:
	uint32_t count_;

	uint32_t angle_ = 0;
    uint16_t sine_[400];
    uint16_t cosine_[400];
    char name_[32];

MACHINE_END

#endif /* TEST_EXAMPLE_EXAMPLE_H_ */
