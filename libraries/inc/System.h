/*
 * System.h
 *
 * Classes and methods that abstract away some of the 
 * register-level pushing around for GPIO and other 
 * peripherals.
 */
#ifndef __System_h__
#define __System_h__


#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#define HIGH 1
#define LOW 0
#define PAUSE_LONG  40
#define PAUSE_SHORT 10

enum Pin_Num {
	PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
	PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15
};

class Pin {
public:
	GPIO_TypeDef* GPIOx;
	int physical_pin;
	Pin(){
		this->GPIOx = 0;
		this->physical_pin = 0;
	}
	Pin(Pin& other_pin){
		this->GPIOx = other_pin.GPIOx;
		this->physical_pin = other_pin.physical_pin;
	}
	Pin(Pin_Num pin){
		switch(pin) {
			case PA0:
				physical_pin = 0;
				GPIOx = GPIOA;
				break;
			case PA1:
				physical_pin = 1;
				GPIOx = GPIOA;
				break;
			case PA2:
				physical_pin = 2;
				GPIOx = GPIOA;
				break;
			case PA3:
				physical_pin = 3;
				GPIOx = GPIOA;
				break;
			case PA4:
				physical_pin = 4;
				GPIOx = GPIOA;
				break;
			case PA5:
				physical_pin = 5;
				GPIOx = GPIOA;
				break;
			case PA6:
				physical_pin = 6;
				GPIOx = GPIOA;
				break;
			case PA7:
				physical_pin = 7;
				GPIOx = GPIOA;
				break;
			case PA8:
				physical_pin = 8;
				GPIOx = GPIOA;
				break;
			case PA9:
				physical_pin = 9;
				GPIOx = GPIOA;
				break;
			case PA10:
				physical_pin = 10;
				GPIOx = GPIOA;
				break;
			case PA11:
				physical_pin = 11;
				GPIOx = GPIOA;
				break;
			case PA12:
				physical_pin = 12;
				GPIOx = GPIOA;
				break;
			case PA13:
				physical_pin = 13;
				GPIOx = GPIOA;
				break;
			case PA14:
				physical_pin = 14;
				GPIOx = GPIOA;
				break;
			case PA15:
				physical_pin = 15;
				GPIOx = GPIOA;
				break;
			case PB0:
				physical_pin = 0;
				GPIOx = GPIOB;
				break;
			case PB1: 
				physical_pin = 1;
				GPIOx = GPIOB;
				break;
			case PB2:
				physical_pin = 2;
				GPIOx = GPIOB;
				break;
			case PB3:
				physical_pin = 3;
				GPIOx = GPIOB;
				break;
			case PB4:
				physical_pin = 4;
				GPIOx = GPIOB;
				break;
			case PB5:
				physical_pin = 5;
				GPIOx = GPIOB;
				break;
			case PB6:
				physical_pin = 6;
				GPIOx = GPIOB;
				break;
			case PB7:
				physical_pin = 7;
				GPIOx = GPIOB;
				break;
			case PB8:
				physical_pin = 8;
				GPIOx = GPIOB;
				break;
			case PB9:
				physical_pin = 9;
				GPIOx = GPIOB;
				break;
			case PB10:
				physical_pin = 10;
				GPIOx = GPIOB;
				break;
			case PB11:
				physical_pin = 11;
				GPIOx = GPIOB;
				break;
			case PB12:
				physical_pin = 12;
				GPIOx = GPIOB;
				break;
			case PB13:
				physical_pin = 13;
				GPIOx = GPIOB;
				break;
			case PB14:
				physical_pin = 14;
				GPIOx = GPIOB;
				break;
			case PB15:
				physical_pin = 15;
				GPIOx = GPIOB;
				break;
			case PC0:
				physical_pin = 0;
				GPIOx = GPIOC;
				break;
			case PC1:
				physical_pin = 1;
				GPIOx = GPIOC;
				break;
			case PC2:
				physical_pin = 2;
				GPIOx = GPIOC;
				break;
			case PC3:
				physical_pin = 3;
				GPIOx = GPIOC;
				break;
			case PC4:
				physical_pin = 4;
				GPIOx = GPIOC;
				break;
			case PC5:
				physical_pin = 5;
				GPIOx = GPIOC;
				break;
			case PC6:
				physical_pin = 6;
				GPIOx = GPIOC;
				break;
			case PC7:
				physical_pin = 7;
				GPIOx = GPIOC;
				break;
			case PC8:
				physical_pin = 8;
				GPIOx = GPIOC;
				break;
			case PC9:
				physical_pin = 9;
				GPIOx = GPIOC;
				break;
			case PC10:
				physical_pin = 10;
				GPIOx = GPIOC;
				break;
			case PC11:
				physical_pin = 11;
				GPIOx = GPIOC;
				break;
			case PC12: 
				physical_pin = 12;
				GPIOx = GPIOC;
				break;
			case PC13: 
				physical_pin = 13;
				GPIOx = GPIOC;
				break;
			case PC14:
				physical_pin = 14;
				GPIOx = GPIOC;
				break;
            case PC15:
				physical_pin = 15;
				GPIOx = GPIOC;
				break;

		}
	}
};

enum Pu_Pd { UP, DOWN, NO_PU_PD};
enum GPIO_Mode { INPUT, OUTPUT, ANALOG, ALT };

static void configure_GPIO(Pin_Num pn, Pu_Pd resistor, GPIO_Mode mode) {
	Pin pin(pn);
	GPIO_InitTypeDef GPIO_InitStructure;
	if (pin.GPIOx == GPIOA) { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); }
	else if (pin.GPIOx == GPIOB) { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); }
	else { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); }
    GPIO_InitStructure.GPIO_Pin   = 1 << pin.physical_pin;
	switch(mode) {
		case INPUT:
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
			break;
		case OUTPUT:
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
			break;
		case ANALOG:
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
			break;
		case ALT:
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	}
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	switch(resistor) {
		case UP:
			GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
			break;
		case DOWN:
			GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
			break;
		case NO_PU_PD:
			GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	}
    GPIO_Init(pin.GPIOx, &GPIO_InitStructure); 	
};

static int adcInit(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);

	ADC_InitTypeDef ADC_init_structure;
	ADC_init_structure.ADC_Resolution = ADC_Resolution_12b;
	ADC_init_structure.ADC_ScanConvMode = DISABLE;
	ADC_init_structure.ADC_ContinuousConvMode = DISABLE; 
	ADC_init_structure.ADC_ExternalTrigConv = DISABLE;
	ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_init_structure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1,&ADC_init_structure);
	ADC_Cmd(ADC1,ENABLE);
	return 0;
}

static void digitalWrite(Pin_Num pn, int state) {
	Pin pin(pn);
	uint16_t pin_addr = (1 << (pin.physical_pin));
	switch(state) {
		case 0:
			pin.GPIOx->BSRRH |= pin_addr;
			break;
		default:
			pin.GPIOx->BSRRL |= pin_addr;
	}
};

static void analogWrite(Pin_Num pn, int value); // TODO: will need this for NIBP valve

static int digitalRead(Pin_Num pn) {
	Pin pin(pn);
	uint16_t pin_addr = (1 << (pin.physical_pin));
	if (pin.GPIOx->IDR & pin_addr) { return 1; }
	else { return 0; }
};

static int analogRead(Pin_Num pn){
	configure_GPIO(pn, NO_PU_PD, ANALOG);
	uint8_t channel;
	switch(pn) {
		case PA0:
			channel = ADC_Channel_0;
			break;
		case PA1:
			channel = ADC_Channel_1;
			break;
		case PA2:
			channel = ADC_Channel_2;
			break;
		case PA3:
			channel = ADC_Channel_3;
			break;
		case PA4:
			channel = ADC_Channel_4;
			break;
		case PA5:
			channel = ADC_Channel_5;
			break;
		case PA6:
			channel = ADC_Channel_6;
			break;
		case PA7:
			channel = ADC_Channel_7;
			break;
		case PB0:
			channel = ADC_Channel_8;
			break;
		case PB1:
			channel = ADC_Channel_9;
			break;
		case PC0:
			channel = ADC_Channel_10;
			break;
		case PC1:
			channel = ADC_Channel_11;
			break;
		case PC2:
			channel = ADC_Channel_12;
			break;
		case PC3:
			channel = ADC_Channel_13;
			break;
		case PC4:
			channel = ADC_Channel_14;
			break;
		case PC5:
			channel = ADC_Channel_15;
			break;
		default:
			return 0;
	}
	ADC_RegularChannelConfig(ADC1,channel,1,ADC_SampleTime_144Cycles);
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { };
	return ADC_GetConversionValue(ADC1); 
}; 

static void delay(__IO long usecs)
{
	volatile int delay_tally = (int) (usecs * 3.125); // Constant determined experimentally
	for (int i = 0; i < delay_tally; i ++) { __asm("nop"); }
}

enum TimerChannel {
	tim2, tim3, tim4
};

class Sampler {
public: 
	int sampling_rate;
	TIM_TypeDef* TIMx;
	TimerChannel timx;

	Sampler(double sampling_rate, TimerChannel timx):
		sampling_rate(sampling_rate), timx(timx) {
		switch(timx){
			case tim2:
				this->TIMx = TIM2;
				break;
			case tim3:
				this->TIMx = TIM3;
				break;
			case tim4:
				this->TIMx = TIM4;
				break;
		}
	}

	void enable(){
		NVIC_InitTypeDef NVIC_InitStructure;

		switch(this->timx){
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
		
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		//TIM_TimeBaseStructure.TIM_Period = 65535; // 1 MHz down to 1 KHz (1 ms)
		TIM_TimeBaseStructure.TIM_Prescaler = 100 - 1; // 100 MHz Clock down to 1 MHz (adjust per your clock)
		TIM_TimeBaseStructure.TIM_Period = (1000000/this->sampling_rate) - 1; // 1 MHz down to sampling_rate Hz 
		/* TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // 1 MHz down to sampling_rate Hz */ 
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(this->TIMx, &TIM_TimeBaseStructure);
		TIM_Cmd(this->TIMx, ENABLE);
		TIM_ITConfig(this->TIMx, TIM_IT_Update, ENABLE);

		TIM_ClearITPendingBit(this->TIMx, TIM_IT_Update);
		TIM_SetCounter(this->TIMx, 0);
		TIM_ITConfig(this->TIMx, TIM_IT_Update, ENABLE);
	}
};

class Console {
public:
	USART_TypeDef* USARTx;
	int baudrate;

	Console(USART_TypeDef* USARTx, int baudrate):
		USARTx(USARTx), baudrate(baudrate) {}

	void configure() {
		/* RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); */
		/* Configure USART2 Tx (PA.02) as alternate function push-pull */
		/* GPIO_InitTypeDef GPIO_InitStructure; */
		/* GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; */
		/* GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; */
		/* GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; */
		/* GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; */
		/* GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; */
		/* GPIO_Init(GPIOA, &GPIO_InitStructure); */
		configure_GPIO(PA2, UP, ALT);
		// Map USART2 to A.02
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		USART_InitTypeDef USART_InitStructure;
		// Initialize USART
		USART_InitStructure.USART_BaudRate = this->baudrate;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx;
		/* Configure USART */
		USART_Init(USARTx, &USART_InitStructure);
		/* Enable the USART */
		USART_Cmd(USARTx, ENABLE);
	}

	int putcharx(int ch) {
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, (uint8_t)ch);
		return ch;
	}

	int strlen(const char* str) {
		int i = 0;
		while (str[i]) {
			i += 1;
		}
		return i;
	}

	int strlen(char* str) {
		int i = 0;
		while (str[i]) {
			i += 1;
		}
		return i;
	}

	void print(const char* message) {
		int len = strlen(message);
		for (int i = 0; i < len; i ++) {
			putcharx(message[i]);
		}
	}

	void print(int value) {
		char message[20];
		sprintf(message, "%d", value);
		int len = 0;
		while (message[len]) {
			len += 1;
		}
		for (int i = 0; i < len; i ++) {
			putcharx((int) (message[i]));
		}
	}
};

#endif
