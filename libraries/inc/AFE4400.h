/*
 * Registers and library interface for pulse oximetry
 * analog front end chip AFE4400
 */

#include <stdio.h>

#include "SPI.h"

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
#define LED1VAl         0x2C
#define ALED1VAL        0x2D
#define LED2_ALED2VAL   0x2E
#define LED1_ALED1VAL   0x2F

#define DIAG            0x30

enum pulseox_state {
    off, idle, calibrate, sample, calculate
};

const int DC_WINDOW_SIZE = 50;
const int MAX_CALIBRATION_ITERATIONS = 10;

class PulseOx {
private:
    SPI_Interface* SPI;
    Pin_Num cs, adc_rdy;
    pulseox_state state;
    float dc_goal = 0.36; // 30% of ADC full-scale voltage (1.2V)
    double map_ratio = 1.2 / ((double) 0x001FFFFF);
    uint32_t current_rf_value = 0x06;
    uint32_t current_led_i_value = 0x1A;

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
        writeData(CONTROL0, 0x00000001);
        SPI->transfer(target_register);
        // Send dummy data to read the next 24 bits
        uint8_t first_transfer = SPI->transfer(0x00);
        uint8_t second_transfer = SPI->transfer(0x00);
        uint8_t third_transfer = SPI->transfer(0x00);
        uint32_t register_data = (first_transfer << 16) + (second_transfer << 8) + third_transfer;
        return register_data;
    }

    uint32_t getLED1Data() {
        while (!digitalRead(adc_rdy)) {}
        return readData(LED1_ALED1VAL);
    }

    uint32_t getLED2Data() {
        while (!digitalRead(adc_rdy)) {}
        return readData(LED2_ALED2VAL);
    }

    uint32_t getSignalDC() {
        uint32_t running_sum = 0;
        for (int i = 0; i < DC_WINDOW_SIZE; i ++) {
            running_sum += getLED1Data();
        }
        return running_sum / DC_WINDOW_SIZE;
    }

    /*
     * Register values are 22-bit 2s complement,
     * mapped to the range of -1.2 - 1.2 V
     */
    float mapValueToVoltage(uint32_t register_value) {
        if (register_value & 0x00200000){
            // Value is negative
            uint32_t abs_value = ~(register_value | 0xFFC00000);
            return (float) (((double) abs_value) * map_ratio) * -1;
        } else {
            // Value is positive
            return (float) (((double) register_value) * map_ratio);
        }
    }

    /*
     * TIA_AMB_GAIN: RF_LED[2:0] set to 110
     * (see p.64 in docs for all Rf options)
     */
    void setRfValue(uint8_t value) {
        uint32_t tia_settings = readData(TIA_AMB_GAIN);
        tia_settings &= ~(0x07);
        tia_settings |= value;
        writeData(TIA_AMB_GAIN, tia_settings);
        current_rf_value = value;
    }

    /*
     * LEDCNTRL (0x22)
     *    LED1[15:8], LED2[7:0]
     *  Formula:
     *       LED_Register_value
     *       ------------------  *  50 mA = current
     *            256
     */
    void setLEDCurrent(uint8_t value) {
        uint32_t both_leds = (value << 8) + value;
        writeData(LEDCNTRL, both_leds);
        current_led_i_value = value;
    }

public:
   
    PulseOx(SPI_Interface* SPI, Pin_Num cs, Pin_Num adc_rdy): SPI(SPI) {
        // Must perform software reset (SW_RST)
		configure_GPIO(cs, NO_PU_PD, OUTPUT);
        digitalWrite(cs, HIGH);
		SPI->begin();
        writeData(CONTROL0, SW_RST);
        delay(1000);
    }

    /*
     * calibrate() -- Calibrate the TIA gain and LED drive current
     * before taking a measurement. Required to compensate for differing
     * ambient light conditions, etc.
     */
    bool calibrate(){
        // 1. Set R_f to 1MOhm
        setRfValue(0x06);

        // 2. Set LED drive current to 5mA
        setLEDCurrent(0x1A);

        int iter_count = 0;
        while(1) {
            float dc = mapValueToVoltage(getSignalDC());
            if ((dc - dc_goal < 1e-3) || (dc_goal - dc < 1e-3)) {
                return true;
            }
            else if (dc < dc_goal) {
                // Increase LED current
                if (current_led_i_value == 0xFF) {
                    return false;
                }
                current_led_i_value += 2;
                setLEDCurrent(current_led_i_value);
            }
            else {
                // Reduce Rf (i.e. reduce TIA gain)
                if (current_rf_value == 0) {
                    return false;
                }
                else {
                    current_rf_value -= 1;
                    setRfValue(current_rf_value);
                }
            }
            // Timeout if calibration isn't done in time.
            if (iter_count > MAX_CALIBRATION_ITERATIONS) {
                return false;
            }
            iter_count++;
        }
    }


};
