#ifndef _ecg_h_
#define _ecg_h_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CircleFifo.h"
#include "interface.h"
#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "Vector.h"

class ECGReadout : public ScreenElement {
private:
	int fifo_size;
	int avg_size;
	int avg_cursor;
	int display_cursor;
	int pin;
	CircleFifo<int> fifo;
	int scaling_factor;
	Vector<int> avg_queue;
	int trace_color;
	int background_color;

public:	
	int last_val = 0;

    ECGReadout(int row, int column, int len, int width, int pin, 
				int trace_color, int background_color, Adafruit_RA8875* tft_interface):
				ScreenElement(row,column,len,width,tft_interface), pin(pin), trace_color(trace_color),
			   	background_color(background_color) {
		fifo_size = real_width;
		fifo = CircleFifo<int>(fifo_size);
		avg_size = 10;
		avg_cursor = 0;
		display_cursor = 0;
		avg_queue = Vector<int>(avg_size);
		scaling_factor = real_len;			
	}

	static void configure(void) {
		adc_init();
		isr_config();
	}

	static void adc_init(void) {
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1; 
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
		//RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN,ENABLE);

		//ADC_DeInit();

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
		//Enable ADC conversion
		ADC_Cmd(ADC1,ENABLE);
	}

	static uint16_t analogRead(){
		ADC_RegularChannelConfig(ADC1,ADC_Channel_11,1,ADC_SampleTime_144Cycles);
		ADC_SoftwareStartConv(ADC1);//Start the conversion
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { };//Processing the conversion
		return ADC_GetConversionValue(ADC1); //Return the converted data
	}

	static void isr_config() {
		/* TIM3 clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // APB1 clock speed = 42 MHz
		NVIC_InitTypeDef NVIC_InitStructure;
		/* Enable the TIM3 gloabal Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // 1 MHz down to 1 KHz (1 ms)
		//TIM_TimeBaseStructure.TIM_Period = 65535; // 1 MHz down to 1 KHz (1 ms)
		TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // 42 MHz Clock down to 1 MHz (adjust per your clock)
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		TIM_Cmd(TIM3, ENABLE);
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM_SetCounter(TIM3, 0);
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	}

    void draw_border(void){
	    tft_interface->drawRect(coord_x,coord_y,real_width,real_len,trace_color);
    }

    void draw(void){
        tft_interface->fillRect(coord_x,coord_y,real_width,real_len,background_color);
		draw_border();
    }

	void read(void) {
		int ecg_data = analogRead();
		fifo.add(ecg_data);
		/* if (avg_cursor < avg_size) { */
		/* 	int input = analogRead(); */
		/* 	//int input = 500; */
		/* 	avg_queue[avg_cursor] = input; */
		/* 	avg_cursor += 1; */
		/* } else { */
		/* 	avg_cursor = 0; */
		/* 	int avg; */
		/* 	for (int i = 0; i < avg_size; i ++ ) { */
		/* 		avg += avg_queue[i]; */
		/* 	} */
		/* 	avg /= avg_size; */
		/* 	fifo.add(avg); */
		/* } */	
	}

	void display_signal(void) {
		// Get newest value from fifo
		//noInterrupts();
		//int new_val = fifo[0];
		int new_val = fifo.newest();
		//interrupts();
		int threshold = 5;
		int display = new_val * real_len;
		display /= 4096;
		if (display > real_len) { display = real_len; }
		else if (display < 0) { display = 0; }
		int old_display = last_val * real_len;
		old_display /= 4096;
		if (old_display > real_len) { old_display = real_len; }
		else if (old_display < 0) { old_display = 0; }
		tft_interface->drawFastVLine(coord_x + display_cursor, coord_y, real_len, background_color);
		if ((display - old_display > threshold) || (display - old_display < -threshold)) {
			tft_interface->drawLine(coord_x + display_cursor, coord_y + real_len - old_display, coord_x + display_cursor, coord_y + real_len - display, trace_color);
		} else {
			tft_interface->drawPixel(coord_x + display_cursor, coord_y + real_len - display, trace_color);
		}
		display_cursor = CircleFifo<int>::mod(display_cursor+1, real_width); // Advance display cursor
		tft_interface->drawFastVLine(coord_x + display_cursor, coord_y, real_len, RA8875_WHITE); // Draw display cursor
		last_val = new_val;
	}

};
#endif
