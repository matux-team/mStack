#ifndef CONSOLE_HAL_H
#define CONSOLE_HAL_H
#include <core/base.h>

#define UART_PORT			USART2
#define UART_ISR_HANDLER()	extern "C" void USART2_IRQHandler(void)

HAL_DEF(console, )
    static void init();
HAL_END

#endif // HAL_H
