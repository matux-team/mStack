#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <stm32f1xx.h>
#define DISABLE_INTERRUPT   __disable_irq()
#define ENABLE_INTERRUPT    __enable_irq()
#define WAIT_FOR_INTERUPT   __WFI()
#define NO_OPERATION		__NOP()

#define EVENT_POOL_SIZE     128
#define EVENT_QUEUE_SIZE	2048

void systemInit();

#endif // SYSTEM_H
