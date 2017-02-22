#ifndef __uart_h__
#define __uart_h__

#include <stdio.h>

#include "GPIO.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

class Console {
public:
    USART_TypeDef* USARTx;
    int baudrate;

    Console(USART_TypeDef* USARTx, int baudrate):
        USARTx(USARTx), baudrate(baudrate) {}

    void configure() {
        // Map USART2 to A.09
        // TODO: Generalize this setup for all possible USART pins
        configure_GPIO(PA9, UP, ALT, GPIO_AF_USART1);

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        USART_InitTypeDef USART_InitStructure;
        // Initialize USART
        USART_InitStructure.USART_BaudRate = this->baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx;
        USART_Init(USARTx, &USART_InitStructure);

        USART_Cmd(USARTx, ENABLE);
    }

    int putcharx(int ch) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, (uint8_t)ch);
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
