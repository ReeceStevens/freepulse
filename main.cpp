
/* stm32f4_discovery.h is located in Utilities/STM32F4-Discovery
 * and defines the GPIO Pins where the leds are connected.
 * Including this header also includes stm32f4xx.h and
 * stm32f4xx_conf.h, which includes stm32f4xx_gpio.h
 */
#include "stm32f4_discovery.h"
#include <stdint.h>
/* We make code more readable by giving the led-pins
 * symbolic names depending on the led color.
 * The values LED4_PIN etc are defined in stm32f4_discovery.h
 * as GPIO_Pin_12 etc.
 */
#define GREEN  LED4_PIN
#define ORANGE LED3_PIN
#define RED    LED5_PIN
#define BLUE   LED6_PIN
#define ALL_LEDS (GREEN | ORANGE | RED | BLUE) // all leds

#define PAUSE_LONG  4000000L
#define PAUSE_SHORT 1000000L

#define LEDS_GPIO_PORT (GPIOD)

static uint16_t leds[LEDn] = {GREEN, ORANGE, RED, BLUE};

GPIO_InitTypeDef GPIO_InitStructure;

static void delay(__IO uint32_t nCount)
{
    while(nCount--)
        __asm("nop"); // do nothing
}

/* Initialize the GPIOD port.
 * See also the comments beginning stm32f4xx_gpio.c
 * (found in the library)
 */
static void setup_leds(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = ALL_LEDS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);
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
	delay(PAUSE_SHORT);
	GPIO_ResetBits(GPIOA, GPIO_Pin_10);
	delay(PAUSE_SHORT);
}

/* Turn on the color leds one after another.
 * The order of the leds is defined in the array leds above.
 * The functions GPIO_SetBits and GPIO_ResetBits are declared
 * in stm32f4xx_gpio.h and implemented in stm32f4xx_gpio.c.
 *
 * You might want to look at the implementation in order to see
 * how this works.
 * Basically, this works by using the memory map mechanism of the
 * Cortex-M4: the pins of the GPIO port D are mapped to special
 * memory addresses which these function write to.
 * The exact addresses are represented by the fields of the
 * GPIO_TypeDef structure (that is by their offsets).
 * See also the structure GPIO_TypeDef in stm32f4xx.h.
 */
static void led_round(void)
{
    int i;
    for (i = 0; i < LEDn; i++)
    {
        /* turn on led */
        GPIO_SetBits(LEDS_GPIO_PORT, leds[i]);
        /* wait a while */
        delay(PAUSE_LONG);
        /* turn off all leds */
        GPIO_ResetBits(LEDS_GPIO_PORT, ALL_LEDS);
    }
}

/* Turn all leds on and off 4 times. */
static void flash_all_leds(void)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        /* Turn on all user leds */
        GPIO_SetBits(LEDS_GPIO_PORT, ALL_LEDS);
        /* Wait a short time */
        delay(PAUSE_SHORT);
        /* Turn off all leds */
        GPIO_ResetBits(LEDS_GPIO_PORT, ALL_LEDS);
        /* Wait again before looping */
        delay(PAUSE_SHORT);
    }
}

int main(void)
{
    //setup_leds();
	external_IO_setup();
    while (1)
    {
        //led_round();
		external_LED();
        //flash_all_leds();
    }

    return 0; // never returns actually
}
