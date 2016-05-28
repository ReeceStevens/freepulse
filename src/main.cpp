
/* stm32f4_discovery.h is located in Utilities/STM32F4-Discovery
 * and defines the GPIO Pins where the leds are connected.
 * Including this header also includes stm32f4xx.h and
 * stm32f4xx_conf.h, which includes stm32f4xx_gpio.h
 */
#define NULL 0

#include "stm32f4_discovery.h"
#include <stdint.h>
#include "Adafruit_RA8875.h"
/* We make code more readable by giving the led-pins
 * symbolic names depending on the led color.
 * The values LED4_PIN etc are defined in stm32f4_discovery.h
 * as GPIO_Pin_12 etc.
 */
#define PAUSE_LONG  4000000L
#define PAUSE_SHORT 1000000L

//extern void delay(uint32_t nCount);

GPIO_InitTypeDef GPIO_InitStructure;
SPI_InitTypeDef SPI_InitStruct;

// A very rough delay function. Not highly accurate, but
// we are only using it for stalling until a peripheral is 
// initialized.
static void delay(uint32_t nCount)
{
	nCount *= 100; // clock speed (100 MHz) * (1 second / 1e6 microseconds)  = 100 cycles / microsecond
    while(nCount--)
        __asm("nop"); // do nothing
}

static void external_IO_setup(void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void external_LED(void) {
	GPIO_SetBits(GPIOA, GPIO_Pin_10);
	delay(100);
	GPIO_ResetBits(GPIOA, GPIO_Pin_10);
	delay(100);
}

int main(void)
{
	external_IO_setup();
    while (1)
    {
		external_LED();
    }

    return 0; 
}
