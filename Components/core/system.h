#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <stm32f4xx.h>
#define DISABLE_INTERRUPT   __disable_irq()
#define ENABLE_INTERRUPT    __enable_irq()
#define WAIT_FOR_INTERUPT   __WFI()
#define NO_OPERATION		__NOP()

#define EVENT_POOL_SIZE     24
#define EVENT_QUEUE_SIZE	64

void systemInit();

#endif // SYSTEM_H
