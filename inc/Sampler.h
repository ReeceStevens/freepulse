#ifndef __Sampler_h__
#define __Sampler_h__

#include "Logger.h"
#include "CircleBuffer.h"
#include "Vector.h"
#include "GPIO.h"

#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"

class LineInterrupt {
private:
    uint8_t EXTI_PortSource;
    uint8_t EXTI_PinSource;
    uint8_t EXTI_Line;
    IRQn_Type EXTI_InterruptNum;

    void get_exti_interrupt_from_pin(Pin pin) {
        EXTI_Line = (0x0001 << pin.physical_pin);
        if (pin.physical_pin >= 10) {
            EXTI_InterruptNum = EXTI15_10_IRQn;
        } else if (pin.physical_pin >= 5) {
            EXTI_InterruptNum = EXTI9_5_IRQn;
        } else {
            switch(pin.physical_pin) {
                case 4:
                    EXTI_InterruptNum = EXTI4_IRQn;
                    break;
                case 3:
                    EXTI_InterruptNum = EXTI3_IRQn;
                    break;
                case 2:
                    EXTI_InterruptNum = EXTI2_IRQn;
                    break;
                case 1:
                    EXTI_InterruptNum = EXTI1_IRQn;
                    break;
                case 0:
                    EXTI_InterruptNum = EXTI0_IRQn;
                    break;
                default:
                    logger(l_error, "Invalid interrupt pin.");
                    break;
            }
        }
    }

public:
    LineInterrupt() {}

    LineInterrupt(LineInterrupt& other_interrupt) {
        this->EXTI_PortSource = other_interrupt.EXTI_PortSource;
        this->EXTI_PinSource = other_interrupt.EXTI_PinSource;
        this->EXTI_Line = other_interrupt.EXTI_Line;
        this->EXTI_InterruptNum = other_interrupt.EXTI_InterruptNum;
    }

    LineInterrupt(Pin_Num interrupt_pin) {
        Pin pin(interrupt_pin);
        if (pin.GPIOx == GPIOA) { EXTI_PortSource = EXTI_PortSourceGPIOA; }
        else if (pin.GPIOx == GPIOB) { EXTI_PortSource = EXTI_PortSourceGPIOB; }
        else if (pin.GPIOx == GPIOC) { EXTI_PortSource = EXTI_PortSourceGPIOC; }
        else { logger(l_error, "Invalid GPIO Port for interrupt (use A-C)."); }
        EXTI_PinSource = uint8_t(pin.physical_pin);
        get_exti_interrupt_from_pin(pin);
    }

    void enable() {
        EXTI_InitTypeDef EXTI_InitStruct;
        NVIC_InitTypeDef NVIC_InitStruct;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
        SYSCFG_EXTILineConfig(EXTI_PortSource, EXTI_PinSource); 

        EXTI_InitStruct.EXTI_Line = EXTI_Line;
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_Init(&EXTI_InitStruct);

        NVIC_InitStruct.NVIC_IRQChannel = EXTI_InterruptNum;
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02; // TODO: Define sampler priority
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
    }
};

class Sampler {
private:
    CircleBuffer<uint32_t>* buf;
    bool can_sample = false;
    uint32_t (*sample_fcn) (void);
    Pin_Num interrupt_pin;
    LineInterrupt* intr;
   
public:
    Sampler(uint32_t (*sample_fcn)(void), Pin_Num interrupt_pin, int buffer_size): sample_fcn(sample_fcn), interrupt_pin(interrupt_pin) {
        intr = new LineInterrupt(interrupt_pin);
        buf = new CircleBuffer<uint32_t>(buffer_size);
    }

    /*
     * Enables the sampler to begin 
     */
    void begin() {
        intr->enable();
        can_sample = true;
    }

    /*
     * This function must only be called from the interrupt
     * handler context. Access must be guarded by the 
     * can_sample flag since this function could preempt
     * `load_samples_into_buffer`.
     *
     * TODO: Should signal filters be implemented in the sampler?
     * If so, `filter()` should be called here.
     */
    void sample() {
        if (can_sample) {
            buf->add(sample_fcn());
        }
    }

    /*
     * Must lock the current buffer and return a copy
     * in order to guarantee the data is not modified
     * by further sampling during copy.
     */
    void load_samples_into_buffer(Vector<uint32_t>& safe_buffer) {
        can_sample = false;
        buf->copy_to_vector(safe_buffer);
        can_sample = true;
    }
};



#endif
