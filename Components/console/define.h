#ifndef CONSOLE_DEFINE_H_
#define CONSOLE_DEFINE_H_

/*CONTROLLER*/
#define TIME_PLOT	10	//ms

/*DRIVER*/
#define TX_BUF_SIZE   		256
#define HEADER_INDICATOR	0xFE
#define FOOTER_INDICATOR	0xFD
#define MAX_PACKET_LENGTH	64

/*HAL*/
#define CONSOLE_PORT			USART2
#define CONSOLE_INIT			MX_USART2_UART_Init()
#define CONSOLE_ISR_HANDLER()	extern "C" void USART2_IRQHandler(void)

#define DMA_MODULE				DMA1
#define DMA_CHANNEL				LL_DMA_STREAM_6
#define DMA_ISR_HANDLER()		extern "C" void DMA1_Stream6_IRQHandler(void)


#endif /* CONSOLE_DEFINE_H_ */
