#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <stm32f4xx.h>
#define DISABLE_INTERRUPT   __disable_irq()
#define ENABLE_INTERRUPT    __enable_irq()
#define WAIT_FOR_INTERUPT   __WFI()
#define NO_OPERATION		__NOP()

void systemInit();

#endif // SYSTEM_H
