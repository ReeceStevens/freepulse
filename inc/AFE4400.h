#ifndef __AFE4400_h__
#define __AFE4400_h__

#include "Logger.h"
#include "GPIO.h"
#include "SPI.h"
#include "Utils.h"

/* Useful commands */
#define SW_RST          0x04

/* See Register Map, p.49 of docs */
#define CONTROL0        0x00
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
#define LED1VAL         0x2C
#define ALED1VAL        0x2D
#define LED2_ALED2VAL   0x2E
#define LED1_ALED1VAL   0x2F

#define DIAG            0x30

enum device_state {
    uninitialized, idle, busy, error
};


class AFE4400 {
private:
    SPI_Interface* SPI;
    Pin_Num cs, rst, adc_rdy, adc_pdn;
    device_state state;

    /*
     * writeData() - Put 24-bit data into a target register
     */
    void writeData(uint8_t target_register, uint32_t data) {
        digitalWrite(cs, LOW);
        SPI->transfer(target_register);
        uint8_t first_transfer = (uint8_t) ((data >> 16) & 0xFF);
        uint8_t second_transfer = (uint8_t) ((data >> 8) & 0xFF);
        uint8_t third_transfer = (uint8_t) ((data) & 0xFF);
        SPI->transfer(first_transfer); /* Bits 23-16 */
        SPI->transfer(second_transfer); /* Bits 15-8 */
        SPI->transfer(third_transfer); /* Bits 7-0 */
        digitalWrite(cs, HIGH);
    }

    /*
     * readData() - Read the 24-bit data inside a register
     */
    uint32_t readData(uint8_t target_register) {
        writeData(CONTROL0, 0x00000001);
        digitalWrite(cs, LOW);
        SPI->transfer(target_register);
        // Send dummy data to read the next 24 bits
        uint32_t first_transfer = SPI->transfer(0x00);
        uint32_t second_transfer = SPI->transfer(0x00);
        uint32_t third_transfer = SPI->transfer(0x00);
        uint32_t register_data = (first_transfer << 16) + (second_transfer << 8) + third_transfer;
        digitalWrite(cs, HIGH);
        writeData(CONTROL0, 0x00000000);
        return register_data;
    }


    /*
     * self_check()
     * Perform multiple reads to the CONTROL1 register and logs the reads to console.
     * Finally, checks to ensure CONTROL1 register is nonzero, since it is defined to
     * always have a nonzero value.
     */
    bool self_check() {
        logger(l_info, "Beginning Self Check... \n");
        for (int i = 0; i < 5; i++) {
            uint16_t control_data = readData(CONTROL1);
            logger(l_info, "%d\n", control_data);
        }
        bool success = readData(CONTROL1) != 0;
        if (success) { logger(l_info, "SpO2 self check passed."); }
        else { logger(l_error, "SpO2 self check failed."); state = error; }
        return success;
    }

    void power_cycle() {
        digitalWrite(rst, HIGH);
        digitalWrite(adc_pdn, LOW);
        delay(100000);
        digitalWrite(adc_pdn, HIGH);
        delay(100000);
        digitalWrite(rst, LOW);
        delay(100000);
        digitalWrite(rst, HIGH);
        delay(100000);
    }

    void initialize_pins() {
        configure_GPIO(cs, NO_PU_PD, OUTPUT);
        configure_GPIO(rst, NO_PU_PD, OUTPUT);
        configure_GPIO(adc_pdn, NO_PU_PD, OUTPUT);
        configure_GPIO(adc_rdy, NO_PU_PD, INPUT);
        digitalWrite(cs, HIGH);
        digitalWrite(adc_pdn, LOW);
        digitalWrite(rst, HIGH);
    }

    /*
     * From datasheet, p.31
     * Sets all pulse timing edges
     * Assumes a pulse repetition frequency of 500 Hz
     */
    void configure_pulse_timings(void) {
        writeData(CONTROL0, 0x000000);
        writeData(CONTROL1, 0x0102); // Enable timers
        writeData(CONTROL2, 0x020100);
        writeData(LED2STC, 6050);
        writeData(LED2ENDC, 7998);
        writeData(LED2LEDSTC, 6000);
        writeData(LED2LEDENDC, 7999);
        writeData(ALED2STC, 50);
        writeData(ALED2ENDC, 1998);
        writeData(LED1STC, 2050);
        writeData(LED1ENDC, 3998);
        writeData(LED1LEDSTC, 2000);
        writeData(LED1LEDENDC, 3999);
        writeData(ALED1STC, 4050);
        writeData(ALED1ENDC, 5998);
        writeData(LED2CONVST, 4);
        writeData(LED2CONVEND, 1999);
        writeData(ALED2CONVST, 2004);
        writeData(ALED2CONVEND, 3999);
        writeData(LED1CONVST, 4004);
        writeData(LED1CONVEND, 5999);
        writeData(ALED1CONVST, 6004);
        writeData(ALED1CONVEND, 7999);
        writeData(ADCRSTSTCT0, 0);
        writeData(ADCRSTENDCT0, 3);
        writeData(ADCRSTSTCT1, 2000);
        writeData(ADCRSTENDCT1, 2003);
        writeData(ADCRSTSTCT2, 4000);
        writeData(ADCRSTENDCT2, 4003);
        writeData(ADCRSTSTCT3, 6000);
        writeData(ADCRSTENDCT3, 6003);
        writeData(PRPCOUNT, 7999);
        setCancellationFilters(0x06);
        setLEDCurrent(0x1A);
    }

public:

    AFE4400(SPI_Interface* SPI, Pin_Num cs, Pin_Num rst, Pin_Num adc_rdy, Pin_Num adc_pdn):
        SPI(SPI), cs(cs), rst(rst), adc_rdy(adc_rdy), adc_pdn(adc_pdn) {
        state = uninitialized;
        initialize_pins();
        power_cycle();
        configure_pulse_timings();
        if (state != error) { state = idle; }
    }

    int32_t getLED1Data() {
        int register_value = readData(LED1_ALED1VAL);
        if (register_value & 0x00200000){
            register_value |= 0xFFD00000;
        }
        return register_value;
    }

    int32_t getLED2Data() {
        int register_value = readData(LED2_ALED2VAL);
        if (register_value & 0x00200000){
            register_value |= 0xFFD00000;
        }
        return register_value;
    }

    /*
     * TIA_AMB_GAIN: RF_LED[2:0] set to 110
     * (see p.64 in docs for all Rf options)
     */
    bool setCancellationFilters(uint16_t value) {
        uint32_t tia_settings = readData(TIA_AMB_GAIN);
        tia_settings &= ~(0x07);
        /* tia_settings &= ~(0xFF); */
        tia_settings |= (value | 0x4400);
        writeData(TIA_AMB_GAIN, tia_settings);
        uint16_t new_value = readData(TIA_AMB_GAIN);
        if (new_value != value) {
            logger(l_error, "Failed to set ambient gain. Attempted to set %d, got %d.\n", value, new_value);
        }
        return new_value == value;
    }

    /*
     * LEDCNTRL (0x22)
     *    LED1[15:8], LED2[7:0]
     *  Formula:
     *       LED_Register_value
     *       ------------------  *  50 mA = current
     *            256
     */
    uint32_t setLEDCurrent(uint8_t value) {
        uint32_t both_leds = (((uint32_t)value) << 8) + value;
        writeData(LEDCNTRL, both_leds + 0x010000);
        uint16_t new_value = readData(LEDCNTRL);
        if (new_value != value) {
            logger(l_error, "Failed to set LED current. Attempted to set %d, got %d.\n", value, new_value);
        }
        return new_value == value;
    }

    device_state get_device_state() {
        return state;
    }
};



#endif
