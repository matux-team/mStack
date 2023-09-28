#include <console/log.h>
#include "gpio.h"

#include <math.h>
#include "example.h"
#define PI 3.14159265358979323846

void ex::Test::init()
{
    for (int i=0;i<400;i++)
    {
        double v = ((double)i/200) * PI;
        int16_t sine = (int16_t)(sin(v)*512.0);
        int16_t cosine = (int16_t)(cos(v)*512.0);
        sine_[i] = sine + 512;
        cosine_[i] = cosine + 512;
    }

    plotTask_.start(1); //100Hz
    oscilloscopeTask_.start(1);

    emptySignal.connect(&emptySignalReceivedEvent);
    fixedSignal.connect(&fixedSignalReceivedEvent);

	timeoutTask_.start(100);
	SM_START(StartUp, 5);
}

M_TASK_HANDLER(ex::Test, plot)
{
	EventStatus a;
	a = emptySignal.emit();
	if(a != EventStatus::POST_SUCCESS)
	{
		LOG_PRINTF("WRONG");
	}

	a = fixedSignal.emit(cosine_[angle_]);
	if(a != EventStatus::POST_SUCCESS)
	{
		LOG_PRINTF("WRONG");
	}

	if (++angle_>=400) angle_=0;
}

M_TASK_HANDLER(ex::Test, oscilloscope)
{
	static uint16_t angle;
    //singlePlot(sine_[angle] + rand()%10);
//    dualPlot(sine_[angle_] + rand()%10, cosine_[angle_] + rand()%10);
    //triplePlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5);
    quadPlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5, sine_[angle]/2 + rand()%5);
    if (++angle>=400) angle=0;
}

M_TASK_HANDLER(ex::Test, timeout)
{
	EventStatus a;
	a = emptyEventEvent.post();
	if(a != EventStatus::POST_SUCCESS)
	{
		LOG_PRINTF("WRONG");
	}

	SM_POST(Event::TIMEOUT);
}

M_EVENT_HANDLER(ex::Test, emptyEvent)
{
//	LOG_PRINTF("EmptyEvent");
}

M_EVENT_HANDLER(ex::Test, fixedEvent, uint32_t)
{
	LOG_PRINTF("FixedEvent %d", event);
}

M_EVENT_HANDLER(ex::Test, emptySignalReceived)
{
    console::Controller::instance().plot(0, sine_[angle_]);
}

M_EVENT_HANDLER(ex::Test, fixedSignalReceived, uint16_t)
{
	console::Controller::instance().plot(1, event);
}

U_ACTION_HANDLER(ex::Test, start)
{
	LOG_PRINTF("START");
	LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
}

U_ACTION_HANDLER(ex::Test, stop)
{
	LOG_PRINTF("STOP");
}

U_INTEGER_HANDLER(ex::Test, integer)
{
	LOG_PRINTF("Integer: %d", value);
}

U_TEXT_HANDLER(ex::Test, name)
{
	int i = 0;
	for(; i < length; i++)
	{
		name_[i] = data[i];
	}
	name_[i] = 0;
	LOG_PRINTF("%s", name_);
}
