#include <console/controller.h>
#include <console/driver.h>
#include <console/hal.h>
#include <console/define.h>
#include "usart.h"

void console::_HAL::init()
{
	CONSOLE_INIT;
	LL_USART_EnableIT_RXNE (CONSOLE_PORT);
	LL_USART_EnableIT_ERROR(CONSOLE_PORT);
	LL_USART_DisableIT_TC(CONSOLE_PORT);
	LL_USART_DisableIT_TXE(CONSOLE_PORT);
}

bool console::_HAL::txReady()
{
	return (LL_USART_IsActiveFlag_TXE(CONSOLE_PORT));
}

void console::_HAL::write(uint8_t c)
{
	LL_USART_TransmitData8(CONSOLE_PORT, c);
}

CONSOLE_ISR_HANDLER()
{
	if(LL_USART_IsActiveFlag_RXNE(CONSOLE_PORT) && LL_USART_IsEnabledIT_RXNE(CONSOLE_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(CONSOLE_PORT);
		console::Driver::instance().post(c);
	}
	else if(LL_USART_IsActiveFlag_TC(CONSOLE_PORT))
	{
		LL_USART_DisableIT_TC(CONSOLE_PORT);
	}
	else if(LL_USART_IsActiveFlag_TXE(CONSOLE_PORT))
	{
		LL_USART_DisableIT_TXE(CONSOLE_PORT);
	}
}
