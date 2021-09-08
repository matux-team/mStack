#ifndef EX_EVENT_H
#define EX_EVENT_H
#include <core/engine.h>
#include <core/event.h>
#include <console/controller.h>

COMPONENT(ex, Blink)
	M_EVENT(buttonPressed)
	M_TASK(blink)

	U_ACTION(Command::START ,start)
	U_ACTION(Command::STOP ,stop)
	U_INTEGER(Command::INTERVAL, interval)
public:
	enum class Command
	{
		INTERVAL=100,
		START,
		STOP
	};
private:
	void ledToggle_();
	void ledOn_();
	void ledOff_();
	uint32_t interval_ = 500;
	bool on_ = false;
COMPONENT_END

#endif // CONTROLLER_H
