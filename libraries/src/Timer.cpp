#include "Timer.h"

void configure_nvic_table(TimerChannel timx);
TIM_TypeDef* get_timer_structure(TimerChannel timx);
void initialize_timer_settings(double frequency, TIM_TypeDef* TIMx);
void start_timer(TIM_TypeDef* TIMx);


void enable_timer(double frequency, TimerChannel timx) {
    configure_nvic_table(timx);
    TIM_TypeDef* TIMx = get_timer_structure(timx);
    initialize_timer_settings(frequency, TIMx);
    start_timer(TIMx);    
}


void configure_nvic_table(TimerChannel timx) {
    NVIC_InitTypeDef NVIC_InitStructure;
    switch(timx){
        case tim2:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // APB1 clock speed = 42 MHz
            NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
            break;
        case tim3:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // APB1 clock speed = 42 MHz
            NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
            break;
        case tim4:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // APB1 clock speed = 42 MHz
            NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
            break;
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

TIM_TypeDef* get_timer_structure(TimerChannel timx) {
    switch(timx){
        case tim2:
            return TIM2;
        case tim3:
            return TIM3;
        case tim4:
            return TIM4;
    }
}

void initialize_timer_settings(double frequency, TIM_TypeDef* TIMx) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 100 - 1; // 100 MHz Clock down to 1 MHz (adjust per your clock)
    TIM_TimeBaseStructure.TIM_Period = (1000000/frequency) - 1; // 1 MHz down to sampling_rate Hz 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
}

void start_timer(TIM_TypeDef* TIMx) {
    TIM_Cmd(TIMx, ENABLE);
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
    TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
    TIM_SetCounter(TIMx, 0);
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
}
