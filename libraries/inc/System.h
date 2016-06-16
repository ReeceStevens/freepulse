/*
 * System.h
 *
 * Classes and methods that abstract away some of the 
 * register-level pushing around for GPIO and other 
 * peripherals.
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define PA0 Pin(A0)
#define PA1 Pin(PA1)
#define PA2 Pin(PA2)
#define PA3 Pin(PA3)
#define PA4 Pin(PA4)
#define PA5 Pin(PA5)
#define PA6 Pin(PA6)
#define PA7 Pin(PA7)
#define PA8 Pin(PA8)
#define PA9 Pin(PA9)
#define PA10 Pin(A10)
#define PA11 Pin(A11)
#define PA12 Pin(A12)
#define PA13 Pin(A13)
#define PA14 Pin(A14)
#define PB0 Pin(PB0)
#define PB1 Pin(PB1)
#define PB2 Pin(PB2)
#define PB3 Pin(PB3)
#define PB4 Pin(PB4)
#define PB5 Pin(PB5)
#define PB6 Pin(PB6)
#define PB7 Pin(PB7)
#define PB8 Pin(PB8)
#define PB9 Pin(PB9)
#define PB10 Pin(B10)
#define PB11 Pin(B11)
#define PB12 Pin(B12)
#define PB13 Pin(B13)
#define PB14 Pin(B14)
#define PC0 Pin(PC0)
#define PC1 Pin(PC1)
#define PC2 Pin(PC2)
#define PC3 Pin(PC3)
#define PC4 Pin(PC4)
#define PC5 Pin(PC5)
#define PC6 Pin(PC6)
#define PC7 Pin(PC7)
#define PC8 Pin(PC8)
#define PC9 Pin(PC9)
#define PC10 Pin(C10)
#define PC11 Pin(C11)
#define PC12 Pin(C12)
#define PC13 Pin(C13)
#define PC14 Pin(C14)


enum Pin_Num {
	A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
	B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14,
	C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14
};

class Pin {
public:
	Pin_Num pin;
	GPIO_TypeDef* GPIOx;
	int physical_pin;
	Pin(Pin_Num pin){
		switch(pin) {
			case A0:
				physical_pin = 0;
			case A1:
				physical_pin = 1;
			case A2:
				physical_pin = 2;
			case A3:
				physical_pin = 3;
			case A4:
				physical_pin = 4;
			case A5:
				physical_pin = 5;
			case A6:
				physical_pin = 6;
			case A7:
				physical_pin = 7;
			case A8:
				physical_pin = 8;
			case A9:
				physical_pin = 9;
			case A10:
				physical_pin = 10;
			case A11:
				physical_pin = 11;
			case A12:
				physical_pin = 12;
			case A13:
				physical_pin = 13;
			case A14:
				physical_pin = 14;
				GPIOx = GPIOA;
				break;
			case B0:
				physical_pin = 0;
			case B1: 
				physical_pin = 1;
			case B2:
				physical_pin = 2;
			case B3:
				physical_pin = 3;
			case B4:
				physical_pin = 4;
			case B5:
				physical_pin = 5;
			case B6:
				physical_pin = 6;
			case B7:
				physical_pin = 7;
			case B8:
				physical_pin = 8;
			case B9:
				physical_pin = 9;
			case B10:
				physical_pin = 10;
			case B11:
				physical_pin = 11;
			case B12:
				physical_pin = 12;
			case B13:
				physical_pin = 13;
			case B14:
				physical_pin = 14;
				GPIOx = GPIOB;
				break;
			case C0:
				physical_pin = 0;
			case C1:
				physical_pin = 1;
			case C2:
				physical_pin = 2;
			case C3:
				physical_pin = 3;
			case C4:
				physical_pin = 4;
			case C5:
				physical_pin = 5;
			case C6:
				physical_pin = 6;
			case C7:
				physical_pin = 7;
			case C8:
				physical_pin = 8;
			case C9:
				physical_pin = 9;
			case C10:
				physical_pin = 10;
			case C11:
				physical_pin = 11;
			case C12: 
				physical_pin = 12;
			case C13: 
				physical_pin = 13;
			case C14:
				physical_pin = 14;
				GPIOx = GPIOC;
				break;
		}
	};
};

enum Pu_Pd { UP, DOWN, NONE};
enum GPIO_Mode { INPUT, OUTPUT, ALT };
void configure_GPIO(Pin pin, Pu_Pd resistor, GPIO_Mode mode) {
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
		case NONE:
			GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	}
    GPIO_Init(pin.GPIOx, &GPIO_InitStructure); 	
};
