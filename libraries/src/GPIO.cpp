#include "GPIO.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"


/* GPIO Configuration */

void enable_gpio_rcc_clock(Pin pin);
void set_gpio_mode(GPIO_InitTypeDef* GPIO_InitStructure, Pin pin, Pu_Pd resistor, GPIO_Mode mode);

void configure_GPIO(Pin_Num pn, Pu_Pd resistor, GPIO_Mode mode) {
    Pin pin(pn);
    enable_gpio_rcc_clock(pin);
    GPIO_InitTypeDef GPIO_InitStructure;
    set_gpio_mode(&GPIO_InitStructure, pin, resistor, mode);
    GPIO_Init(pin.GPIOx, &GPIO_InitStructure);
}

/*
 * configure_GPIO is overloaded to allow for the addition of an alternate function identifier.
 */
void configure_GPIO(Pin_Num pn, Pu_Pd resistor, GPIO_Mode mode, uint8_t GPIO_AF) {
    Pin pin(pn);
    configure_GPIO(pn, resistor, mode);
    GPIO_PinAFConfig(pin.GPIOx, pin.physical_pin, GPIO_AF);
}


void enable_gpio_rcc_clock(Pin pin) {
    if (pin.GPIOx == GPIOA) { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); }
    else if (pin.GPIOx == GPIOB) { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); }
    else { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); }
}

void set_gpio_mode(GPIO_InitTypeDef* GPIO_InitStructure, Pin pin, Pu_Pd resistor, GPIO_Mode mode) {
    GPIO_InitStructure->GPIO_Pin   = 1 << pin.physical_pin;
    GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure->GPIO_OType = GPIO_OType_PP;
    switch(mode) {
        case INPUT:
            GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_IN;
            break;
        case OUTPUT:
            GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_OUT;
            break;
        case ANALOG:
            GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_AN;
            break;
        default:
            GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_AF;
            break;
    }
    switch(resistor) {
        case UP:
            GPIO_InitStructure->GPIO_PuPd  = GPIO_PuPd_UP;
            break;
        case DOWN:
            GPIO_InitStructure->GPIO_PuPd  = GPIO_PuPd_DOWN;
            break;
        case NO_PU_PD:
            GPIO_InitStructure->GPIO_PuPd  = GPIO_PuPd_NOPULL;
    }
}

int initialize_ADCs(void) {
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

/* GPIO Operation */

uint8_t get_adc_channel(Pin_Num pn);

int digitalRead(Pin_Num pn) {
    Pin pin(pn);
    uint16_t pin_addr = (1 << (pin.physical_pin));
    if (pin.GPIOx->IDR & pin_addr) { return 1; }
    else { return 0; }
}

void digitalWrite(Pin_Num pn, int state) {
    Pin pin(pn);
    uint16_t pin_addr = (1 << (pin.physical_pin));
    switch(state) {
        case 0:
            pin.GPIOx->BSRRH |= pin_addr;
            break;
        default:
            pin.GPIOx->BSRRL |= pin_addr;
    }
}

int analogRead(Pin_Num pn) {
    configure_GPIO(pn, NO_PU_PD, ANALOG);
    uint8_t channel = get_adc_channel(pn);
    ADC_RegularChannelConfig(ADC1,channel,1,ADC_SampleTime_144Cycles);
    ADC_SoftwareStartConv(ADC1);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { };
    return ADC_GetConversionValue(ADC1); 

}

uint8_t get_adc_channel(Pin_Num pn) {
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
    return channel;
}
