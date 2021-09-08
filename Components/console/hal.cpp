#include <console/controller.h>
#include <console/driver.h>
#include <console/hal.h>
#include "usart.h"

#define UART_PORT			USART1
#define UART_ISR_HANDLER()	extern "C" void USART1_IRQHandler(void)

void console::HAL::init()
{
	MX_USART1_UART_Init();
	LL_USART_EnableIT_RXNE(UART_PORT);
	LL_USART_EnableIT_ERROR(UART_PORT);
	LL_USART_DisableIT_TC(UART_PORT);
	LL_USART_DisableIT_TXE(UART_PORT);
}

bool console::HAL::txReady()
{
	return (LL_USART_IsActiveFlag_TXE(UART_PORT));
}

void console::HAL::write(uint8_t c)
{
	LL_USART_TransmitData8(UART_PORT, c);
}

UART_ISR_HANDLER()
{
	if(LL_USART_IsActiveFlag_RXNE(UART_PORT) && LL_USART_IsEnabledIT_RXNE(UART_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(UART_PORT);
		console::Driver::instance().post(c);
	}
	else if(LL_USART_IsActiveFlag_TC(UART_PORT))
	{
		LL_USART_DisableIT_TC(UART_PORT);
	}
	else if(LL_USART_IsActiveFlag_TXE(UART_PORT))
	{
		LL_USART_DisableIT_TXE(UART_PORT);
	}
}
