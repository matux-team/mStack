#include <core/system.h>
#include <core/engine.h>
#include "gpio.h"

extern "C" void SystemClock_Config(void);

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	core::Engine::instance().tick();
}

void systemInit()
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	LL_SYSTICK_EnableIT();
}
