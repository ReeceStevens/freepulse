#ifndef __uart_h__
#define __uart_h__

#include <stdio.h>

#include "GPIO.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

enum USART_Channel {
    USART_1, USART_2, USART_6
};

enum USART_Mode {
    USART_TX, USART_RX
};

class USART_Interface {
private:
    USART_TypeDef* USARTx;
    USART_Channel usart_channel;
    USART_Mode mode;
    int baudrate;

    int putcharx(int ch) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, (uint8_t)ch);
        return ch;
    }

    char getcharx() {
        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET);
        return (char) USART_ReceiveData(USART1);
    }

    int strlen(const char* str) {
        int i = 0;
        while (str[i]) {
            i += 1;
        }
        return i;
    }

public:

    USART_Interface(USART_Channel usart_channel, USART_Mode mode, int baudrate):
        usart_channel(usart_channel), mode(mode), baudrate(baudrate) {}

    // Map USART2 to A.09
    void configure() {
        USART_TypeDef* USARTx;
        switch(usart_channel) {
            case USART_1:
                /*
                 * Alternate pins for USART1:
                 *    TX    RX
                 *   ------------
                 *   PA9  | PA10
                 *   PB6  | PB7
                 *   PA15 | PB3
                 */
                USARTx = USART1;
                if (mode == USART_TX) {
                    configure_GPIO(PA9, UP, ALT, GPIO_AF_USART1);
                } else {
                    configure_GPIO(PA10, UP, ALT, GPIO_AF_USART1);
                }
                RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
                break;
            case USART_2:
                USARTx = USART2;
                if (mode == USART_TX) {
                    configure_GPIO(PA2, UP, ALT, GPIO_AF_USART2);
                } else {
                    configure_GPIO(PA3, UP, ALT, GPIO_AF_USART2);
                }
                RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
                break;
            case USART_6:
                USARTx = USART6;
                if (mode == USART_TX) {
                    configure_GPIO(PA11, UP, ALT, GPIO_AF_USART6);
                } else {
                    configure_GPIO(PA12, UP, ALT, GPIO_AF_USART6);
                }
                RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
                break;
        }
        // Initialize USART
        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate = this->baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = mode == USART_TX ? USART_Mode_Tx : USART_Mode_Rx;
        USART_Init(USARTx, &USART_InitStructure);
        USART_Cmd(USARTx, ENABLE);
    }

    void print(const char* message) {
        int len = strlen(message);
        for (int i = 0; i < len; i ++) {
            putcharx(message[i]);
        }
    }

    void print(const int value) {
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
