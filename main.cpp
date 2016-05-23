
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
#define PAUSE_LONG  4000000L
#define PAUSE_SHORT 1000000L

#define LEDS_GPIO_PORT (GPIOD)

static uint16_t leds[LEDn] = {GREEN, ORANGE, RED, BLUE};

GPIO_InitTypeDef GPIO_InitStructure;
SPI_InitTypeDef SPI_InitStruct;


static void delay(__IO uint32_t nCount)
{
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
	delay(4000000L);
	GPIO_ResetBits(GPIOA, GPIO_Pin_10);
	delay(4000000L);
}

static void spi_Init(void){
	// Initialize alternate function pins A5, A6, and A7
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); // SCLK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); // MISO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); // MOSI

	// Initialize CS pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure); // CS

	GPIOC->BSRRL |= GPIO_Pin_4

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
	SPI_Init(SPI1, &SPI_InitStruct); 

}

uint8_t SPI1_send(uint8_t data){
	SPI1->DR = data; // write data to be transmitted to the SPI data register
	while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(SPI1->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return SPI1->DR; // return received data from SPI data register
}

// Compatibility functions to use Arduino RA8875 Library

void writeCommand(uint8_t reg) {
	SPI_Cmd(SPI1, reg);
}

void writeData(uint8_t data) {
	SPI1_send(data);
}

uint8_t readData(void) {
	uint8_t rx_data = SPI1_send(NULL);
	return rx_data;
}

int main(void)
{
	external_IO_setup();
	spi_Init();
    while (1)
    {
		external_LED();
    }

    return 0; 
}
