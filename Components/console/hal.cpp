#include <console/controller.h>
#include <console/driver.h>
#include <console/hal.h>
#include "usart.h"
#include "dma.h"

console::Driver& console_ = console::Driver::instance();

void console::_HAL::init()
{
	MX_DMA_Init();
	CONSOLE_INIT;

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_6,
						 (uint32_t)Driver::instance().txQueue(),
						 LL_USART_DMA_GetRegAddr(CONSOLE_PORT),
						 LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_6));
//	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_6, TX_BUF_SIZE);

	/* Enable DMA transfer complete/error interrupts  */
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_6);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_STREAM_6);

	/* Enable DMA TX Interrupt */
	LL_USART_EnableDMAReq_TX(CONSOLE_PORT);

	LL_USART_EnableIT_RXNE(CONSOLE_PORT);
	LL_USART_EnableIT_ERROR(CONSOLE_PORT);
	LL_USART_DisableIT_TC(CONSOLE_PORT);
	LL_USART_DisableIT_TXE(CONSOLE_PORT);
}

//bool console::_HAL::txReady()
//{
//	return (LL_USART_IsActiveFlag_TXE(CONSOLE_PORT));
//}
//
//void console::_HAL::write(uint8_t c)
//{
//	LL_USART_TransmitData8(CONSOLE_PORT, c);
//}

CONSOLE_ISR_HANDLER()
{
	if(LL_USART_IsActiveFlag_RXNE(CONSOLE_PORT) && LL_USART_IsEnabledIT_RXNE(CONSOLE_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(CONSOLE_PORT);
		console_.receiveEvent.post(c);
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

extern "C" void DMA1_Stream6_IRQHandler(void)
{
	if (LL_DMA_IsActiveFlag_TC6(DMA1))
	{
		LL_DMA_ClearFlag_TC6(DMA1);
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);
		console_.sendEvent.post();
	}
	else if (LL_DMA_IsActiveFlag_TE6(DMA1))
	{
		/* Call Error function */
		LL_DMA_ClearFlag_TE6(DMA1);
	}
}
