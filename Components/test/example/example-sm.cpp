#include <console/log.h>
#include <test/example/example.h>

STATE_BODY(ex::Test::StartUp)
{
	ENTER_()
	{
//		LOG_PRINTF("START UP");
		timeoutTask_.start(1000, 1);
	}
	TRANSITION_(Event::TIMEOUT, Running){}
//	EXIT_()
//	{
//		LOG_PRINTF("EXIT START UP");
//	}
}

STATE_BODY(ex::Test::Running)
{
	ENTER_()
	{
//		LOG_PRINTF("RUNNING");
		SM_POST(Event::TEST);
	}
	TRANSITION_(Event::TEST)
	{
		fixedEventEvent.post(count_++);
		timeoutTask_.start(1000,1);
	}
	TRANSITION_(Event::TIMEOUT, Pause){}
//	EXIT_()
//	{
//		LOG_PRINTF("EXIT RUNNING");
//	}
}

STATE_BODY(ex::Test::Pause)
{
	ENTER_()
	{
//		LOG_PRINTF("PAUSE");
	}
	TRANSITION_(Event::TIMEOUT, StartUp){}
//	EXIT_()
//	{
//		LOG_PRINTF("EXIT PAUSE");
//	}
}
