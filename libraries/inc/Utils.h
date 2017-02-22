#ifndef __utils_h__
#define __utils_h__

#include "stm32f4xx.h"
#include "stm32f4xx_syscfg.h"

#define PAUSE_LONG  40
#define PAUSE_SHORT 10

static void delay(__IO long usecs)
{
    volatile int delay_tally = (int) (usecs * 3.125); // Constant determined experimentally
    for (int i = 0; i < delay_tally; i ++) { __asm("nop"); }
}

#endif
