#include <console/controller.h>
#include <console/driver.h>
#include <console/hal.h>
#include "usart.h"
#include "dma.h"

console::Driver& console_ = console::Driver::instance();

void console::_HAL::init()
{
	MX_DMA_Init();
	MX_USART2_UART_Init();

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1,
						 (uint32_t)Driver::instance().txQueue(),
						 LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT),
						 LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1));
//	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, TX_BUF_SIZE);

	/* Enable DMA transfer complete/error interrupts  */
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_1);

	/* Enable DMA TX Interrupt */
	LL_USART_EnableDMAReq_TX(USART2);

	LL_USART_EnableIT_RXNE(UART_PORT);
	LL_USART_EnableIT_ERROR(UART_PORT);
	LL_USART_DisableIT_TC(UART_PORT);
	LL_USART_DisableIT_TXE(UART_PORT);
}

bool console::_HAL::txReady()
{
	return (LL_USART_IsActiveFlag_TXE(UART_PORT));
}

void console::_HAL::write(uint8_t c)
{
	LL_USART_TransmitData8(UART_PORT, c);
}

UART_ISR_HANDLER()
{
	if(LL_USART_IsActiveFlag_RXNE(UART_PORT) && LL_USART_IsEnabledIT_RXNE(UART_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(UART_PORT);
		console_.receiveEvent.post(c);
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

extern "C" void DMA1_Channel1_IRQHandler(void)
{
	if (LL_DMA_IsActiveFlag_TC1(DMA1))
	{
		LL_DMA_ClearFlag_TC1(DMA1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
		console_.sendEvent.post();
	}
	else if (LL_DMA_IsActiveFlag_TE1(DMA1))
	{
		/* Call Error function */
		LL_DMA_ClearFlag_TE1(DMA1);
	}
}
