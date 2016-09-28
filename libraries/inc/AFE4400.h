/*
 * Registers and library interface for pulse oximetry
 * analog front end chip AFE4400
 */

#include <stdio.h>
#include <math.h>

#include "stm32f4xx_exti.h"
#include "SPI.h"
#include "Display.h"
#include "ScreenElement.h"
#include "Vector.h"

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

extern Console c;

enum pulseox_state {
    off, idle, calibrating, measuring
};

static const int DC_WINDOW_SIZE = 50;
static const int MAX_CALIBRATION_ITERATIONS = 10;
static const int MEASUREMENT_WINDOW_SIZE = 100;

class PulseOx: public ScreenElement {
private:
    CircleBuffer<int> red_signal;
    CircleBuffer<int> ir_signal;
    LargeNumberView* numview;
    SignalTrace* signalTrace;
    SPI_Interface* SPI;
    Pin_Num cs, rst, adc_rdy, adc_pdn;
    pulseox_state state = off;
    float dc_goal = 0.36; // 30% of ADC full-scale voltage (1.2V)
    double map_ratio = 1.2 / ((double) 0x001FFFFF);
    uint32_t current_rf_value = 0x06;
    uint32_t current_led_i_value = 0x1A;
    uint32_t confirmed_current_value;

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

    uint32_t getLED1Data() {
        while (!digitalRead(adc_rdy)) {}
        return readData(LED1_ALED1VAL);
        /* return readData(LED1VAL); */
    }

    uint32_t getLED2Data() {
        while (!digitalRead(adc_rdy)) {}
        return readData(LED2_ALED2VAL);
        /* return readData(LED2VAL); */
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
    uint32_t setLEDCurrent(uint8_t value) {
        uint32_t both_leds = (((uint32_t)value) << 8) + value;
        writeData(LEDCNTRL, both_leds);
        current_led_i_value = value;
        return readData(LEDCNTRL);
    }

    /*
     * From datasheet, p.31
     * Sets all pulse timing edges
     * Assumes a pulse repetition frequency of 500 Hz
     */
    void configurePulseTimings(void) {
        writeData(CONTROL0, 0x00000000);
        setRfValue(0x06);
        confirmed_current_value = setLEDCurrent(0x1A);
        writeData(CONTROL2, 0x002000);
        /* writeData(CONTROL1, 0x0102); // Enable timers */
        writeData(CONTROL1, 0x010707); // Enable timers
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
    }

public:
    PulseOx(int row, int column, int len, int width, SPI_Interface* SPI, Pin_Num cs, Pin_Num rst, Pin_Num adc_rdy, Pin_Num adc_pdn, Display* tft): ScreenElement(row,column,len,width,tft), SPI(SPI), cs(cs), rst(rst), adc_rdy(adc_rdy), adc_pdn(adc_pdn) {
        // Must perform software reset (SW_RST)
        this->cs = cs;
        this->rst = rst;
        this->adc_rdy = PA8; // Fixed pin for now, will generalize later.
        this->adc_pdn = adc_pdn;
		configure_GPIO(cs, NO_PU_PD, OUTPUT);
		configure_GPIO(rst, NO_PU_PD, OUTPUT);
		configure_GPIO(adc_pdn, NO_PU_PD, OUTPUT);
        digitalWrite(cs, HIGH);
        digitalWrite(adc_pdn, LOW);
    }

    void enable() {
        digitalWrite(adc_pdn, HIGH);
        delay(100000);
        digitalWrite(rst, HIGH);
        delay(100000);
        digitalWrite(rst, LOW);
        delay(100000);
        digitalWrite(rst, HIGH);
		SPI->begin();
        writeData(CONTROL0, SW_RST);
        configurePulseTimings();
        red_signal.resize(MEASUREMENT_WINDOW_SIZE);
        ir_signal.resize(MEASUREMENT_WINDOW_SIZE);
        display_signal.resize(MEASUREMENT_WINDOW_SIZE);
        init_sampler();
        signalTrace = new SignalTrace(row,column,len,width,RA8875_BLACK,RA8875_GREEN,0,700,&display_signal,tft);
        numview = new LargeNumberView(row + 1,column + width,len,3,RA8875_BLACK,RA8875_GREEN,true,0, tft);
        state = idle;
    }

    void draw() {
        numview->draw();
        signalTrace->draw();
    }

    void init_sampler(void) {
        configure_GPIO(adc_rdy, UP, INPUT);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);

        EXTI_InitTypeDef EXTI_InitStruct;

        EXTI_InitStruct.EXTI_Line = EXTI_Line8;
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_Init(&EXTI_InitStruct);

        NVIC_InitTypeDef NVIC_InitStruct;

        NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
        // TODO: come back to these priority levels.
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
    }

    bool can_sample(void) {
        return this->ready_to_sample;
    }

    void sample(void) {
        int red_val = getLED2Data();
        red_signal.add(red_val);
        c.print(red_val);
        c.print("\n");
        ir_signal.add(getLED1Data());
    }

    void update() {
        numview->update();
        signalTrace->update();
        switch(state) {
            case off:
                break;
            case idle:
            {
                if (ready_for_measurement) { state = measuring; }
                break;
            }
            case calibrating:
            {
                c.print("Beginning calibration");
                bool success = calibrate();
                c.print("Calibration complete!");
                if (success) {
                    state = measuring;
                } else {
                    // TODO: error handling for calibration
                    c.print("Calibration failed :(");
                    state = idle;
                }
                break;
            }
            case measuring:
            {
                c.print("Calibration succeeded! :D");
                /* int new_measurement = get_measurement(); */
                /* numview->changeNumber(new_measurement); */
                /* c.print("Pulseox value is: "); */
                /* c.print(new_measurement); */
                break;
            }
        }
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

    double mean(Vector<uint32_t> vals) {
        int running_sum = 0;
        int len = vals.size();
        for (int i = 0; i < len; i++) {
            running_sum += vals[i];
        }
        return ((double) running_sum) / ((double) len);
    }

    double ac_rms(Vector<int> vals, uint32_t mean) {
        int len = vals.size();
        int rms_sum = 0;
        for (int i = 0; i < len; i ++) {
            int ac_value = vals[i] - mean;
            rms_sum += (ac_value*ac_value);
        }
        // TODO: See if this is too slow of an operation!
        return sqrt(rms_sum / len);
    }

    /*
     * get_measurement() -- Trigger an SpO2 calculation
     *
     * TODO: measurements should be done on a timer interval, not in a loop.
     *
     * TODO: The measurement process can't hang the whole system. Needs to run
     *       in the background and play nice.
     */
    /* int get_measurement() { */
    /*     red_signal.empty(); */
    /*     ir_signal.empty(); */
    /*     /1* begin sampling *1/ */
    /*     for (int i = 0; i < MEASUREMENT_WINDOW_SIZE; i ++) { */
    /*         red_signal.push_back(getLED1Data()); */
    /*         ir_signal.push_back(getLED2Data()); */
    /*     } */
    /*     /1* perform calculation *1/ */
    /*     double dc_red = mean(red_signal); */
    /*     double dc_ir = mean(ir_signal); */
    /*     double ac_rms_red = ac_rms(red_signal, dc_red); */
    /*     double ac_rms_ir = ac_rms(ir_signal, dc_ir); */
    /*     double lambda = (ac_rms_red * dc_ir) / (ac_rms_ir * dc_red); */
    /*     /1* return lambda; *1/ */
    /*     return 110 - 25*(lambda); */
    /* } */


};
