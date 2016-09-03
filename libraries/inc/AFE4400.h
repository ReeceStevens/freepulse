/*
 * Registers and library interface for pulse oximetry
 * analog front end chip AFE4400
 */

#include <stdio.h>
#include "SPI.h"

/* See Register Map, p.49 of docs */
#define LED2STC         0x01
#define LED2ENDC        0x02
#define LED2LEDSTC      0x03
#define LED2LEDENDC     0x04
#define ALED2STC        0x05
#define ALED2ENDC       0x06

#define LED1STC         0x07
#define LED1ENDC        0x08
#define LED1LEDSTC      0x09
#define LED1LEDENDC     0x0A
#define ALED1STC        0x0B
#define ALED1ENDC       0x0C

#define LED2CONVST      0x0D
#define LED2CONVEND     0x0E
#define ALED2CONVST     0x0F
#define ALED2CONVEND    0x10

#define LED1CONVST      0x11
#define LED1CONVEND     0x12
#define ALED1CONVST     0x13
#define ALED1CONVEND    0x14

#define ADCRSTSTCT0     0x15
#define ADCRSTENDCT0    0x16
#define ADCRSTSTCT1     0x17
#define ADCRSTENDCT1    0x18
#define ADCRSTSTCT2     0x19
#define ADCRSTENDCT2    0x1A
#define ADCRSTSTCT3     0x1B
#define ADCRSTENDCT3    0x1C

#define PRPCOUNT        0x1D

#define CONTROL1        0x1E
#define TIAGAIN         0x20
#define TIA_AMB_GAIN    0x21
#define LEDCNTRL        0x22
#define CONTROL2        0x23

#define ALARM           0x29
#define LED2VAL         0X2A
#define ALED2VAL        0x2B
#define LED1VAl         0x2C
#define ALED1VAL        0x2D
#define LED2ALED2VAL   0x2E
#define LED1ALED1VAL   0x2F

#define DIAG            0x30

class PulseOx {
private:
    SPI_Interface* SPI;

public:
   
    // TODO: Still need to add the CS pin!
    PulseOx(SPI_Interface* SPI): SPI(SPI) {
        // Must perform software reset (SW_RST)
    }

    /*
     * writeData() - Put 24-bit data into a target register
     */
    void writeData(uint8_t target_register, uint32_t data) {
        SPI->transfer(target_register);
        uint8_t first_transfer = (uint8_t) (data >> 16);
        uint8_t second_transfer = (uint8_t) (data >> 8);
        uint8_t third_transfer = (uint8_t) (data);
        SPI->transfer(first_transfer); /* Bits 23-16 */
        SPI->transfer(second_transfer); /* Bits 15-8 */
        SPI->transfer(third_transfer); /* Bits 7-0 */
    }

    /*
     * readData() - Read the 24-bit data inside a register
     */
    uint32_t readData(uint8_t target_register) {
        writeData(CONTROL1, 0x00000001);
        SPI->transfer(target_register);
        // Send dummy data to read the next 24 bits
        uint8_t first_transfer = SPI->transfer(0x00);
        uint8_t second_transfer = SPI->transfer(0x00);
        uint8_t third_transfer = SPI->transfer(0x00);
        uint32_t register_data = (first_transfer << 16) + (second_transfer << 8) + third_transfer;
        return register_data;
    }

    /*
     * calibrate() -- Calibrate the TIA gain and LED drive current
     * before taking a measurement. Required to compensate for differing
     * ambient light conditions, etc.
     */
    uint32_t calibrate(){
        // 1. Set R_f to 1MOhm
        //    TIA_AMB_GAIN: RF_LED[2:0] set to 110
        //    (see p.64 in docs for all Rf options

        // 2. Set LED drive current to 5mA
        //    LEDCNTRL (0x22)
        //      LED1[15:8], LED2[7:0]
        //    Formula:
        //         LED_Register_value
        //         ------------------  *  50 mA = current
        //              256
        
    }

};
