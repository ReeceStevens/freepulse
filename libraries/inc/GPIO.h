#ifndef __gpio_h__
#define __gpio_h__

#include "stm32f4xx_gpio.h"

#define HIGH 1
#define LOW 0

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

/* GPIO functions */
enum Pu_Pd { UP, DOWN, NO_PU_PD};
enum GPIO_Mode { INPUT, OUTPUT, ANALOG, ALT };

void configure_GPIO(Pin_Num pn, Pu_Pd resistor, GPIO_Mode mode);
void configure_GPIO(Pin_Num pn, Pu_Pd resistor, GPIO_Mode mode, uint8_t GPIO_AF);
int initialize_ADCs(void);
void digitalWrite(Pin_Num pn, int state);
int digitalRead(Pin_Num pn);
int analogRead(Pin_Num pn);

#endif
