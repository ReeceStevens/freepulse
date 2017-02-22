#ifndef __timer_h__
#define __timer_h__

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

enum TimerChannel {
    tim2, tim3, tim4
};

void enable_timer(double frequency, TimerChannel timx);

#endif
