#include <console/log.h>
#include <example/blink/blink.h>
#include "gpio.h"

U_ACTION_HANDLER(ex::Blink, start)
{
	blinkTask_.start(interval_);
	LOG_PRINT("Blink Stopped");
}

U_ACTION_HANDLER(ex::Blink, stop)
{
	blinkTask_.start(interval_);
	LOG_PRINT("Blink Started");
}

U_INTEGER_HANDLER(ex::Blink, interval)
{
	if (value > 0 || value < 2000) interval_ = value;
	LOG_PRINTF("Interval = %d", interval_);
}

M_TASK_HANDLER(ex::Blink, blink)
{
	this->ledToggle_();
}

M_EVENT_HANDLER(ex::Blink, buttonPressed)
{
	LOG_PRINT("Button Pressed");
	if (blinkTask_.running())
	{
		blinkTask_.stop();
		ledOn_();
	}
	else
	{
		ledToggle_();
	}
}

void ex::Blink::ledToggle_()
{
	if (on_) ledOff_();
	else ledOn_();
}

void ex::Blink::ledOn_()
{
	LOG_PRINT("LED is on");
	on_ = true;
	/*TODO: write pin here*/
	LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
}

void ex::Blink::ledOff_()
{
	LOG_PRINT("LED is off");
	on_ = false;
	/*TODO: write pin here*/
	LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);
}
