#define NULL 0

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"

#include "Adafruit_RA8875.h"

#define PAUSE_LONG  4000000L
#define PAUSE_SHORT 1000000L

GPIO_InitTypeDef GPIO_InitStructure;
SPI_InitTypeDef SPI_InitStruct;

/* BEGIN DISPLAY CODE */
#define RA8875_PINK        0xF81F
#define RA8875_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define RA8875_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define RA8875_INT 2
#define RA8875_WAIT 3
#define RA8875_CS 4
#define RA8875_RESET 5
#define HOMESCREEN 0
#define ALARMSCREEN 1

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// These are for default alarm values and UI
#define DEFAULT_ECG_MAX 98
#define DEFAULT_ECG_MIN 90
#define DEFAULT_SP02_MAX 100
#define DEFAULT_SP02_MIN 95
#define BOXSIZE 60

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS,RA8875_RESET);

const int s_height = 480;
const int s_width = 800;
const float xScale = 1024.0F/s_width;
const float yScale = 1024.0F/s_height;

// Defining screen proportions
const int rows = 10;
const int columns = 10;

const int vertical_scale = s_height / rows;
const int horizontal_scale = s_width / columns;

#include "interface.h"
#include "ecg_revised.h"

int currentMode = 0; // Change mode

// Keep track of screen inversion and alarm state
volatile int inverted = 0;
volatile int activeAlarm = 0;

bool success;

/* Build UI Buttons */

Button settings = Button(9,9,2,2,RA8875_RED,"Alarm Settings",true,&tft);
Button confirm_button = Button(9,1,2,2,RA8875_GREEN,"Confirm",true,&tft);
Button cancel_button = Button(9,9,2,2,RA8875_RED,"Cancel",true,&tft);
Button default_button = Button(6,7,2,2,RA8875_LIGHTGREY,"Default Settings",true,&tft);
TextBox title = TextBox(1,3,1,3,RA8875_BLACK,RA8875_WHITE,3,true,"FreePulse Patient Monitor v0.9", &tft);

//ECGReadout ecg = ECGReadout(2,1,3,8,15,RA8875_BLUE,RA8875_LIGHTGREY,&tft);
//ECGReadout ecg2 = ECGReadout(5,1,3,8,16,RA8875_RED,RA8875_LIGHTGREY,&tft);

/*
 * showGrid() - 
 * DEVELOPMENT FUNCTION ONLY.
 * Draw gridlines for interface.
 */
void showGrid(void){
    for (int i = 1; i < rows; i += 1) {
        tft.drawLine(1,i*vertical_scale,s_width-1,i*vertical_scale,RA8875_LIGHTGREY);
    }
    for (int i = 1; i < columns; i += 1) {
        tft.drawLine(i*horizontal_scale,1,i*horizontal_scale,s_height-1,RA8875_LIGHTGREY);
    }
}

/*
 * clearScreen(int color) - clear screen with specified color
 */
void clearScreen(int color){
    tft.fillScreen(color);
}

void MainScreenInit(void){
  clearScreen(RA8875_BLACK);
  showGrid();
  title.draw();
  settings.draw();
  //ecg.draw();
  //ecg2.draw();
}


void SettingsScreenInit(void){
  clearScreen(RA8875_BLACK);
  confirm_button.draw();
  default_button.draw();
  cancel_button.draw();
}


// A very rough delay function. Not highly accurate, but
// we are only using it for stalling until a peripheral is 
// initialized.
static void delay(__IO uint32_t nCount)
{
	//nCount *= 100; // clock speed (100 MHz) * (1 second / 1e6 microseconds)  = 100 cycles / microsecond
    while(nCount--)
        __asm("nop"); // do nothing
}

void gui_init() {
	success = tft.begin();
	tft.displayOn(true);
	tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	tft.PWM1out(255);

	// With hardware acceleration this is instant
	tft.fillScreen(RA8875_BLACK);

	// Touch screen options
	tft.touchEnable(true);
	tft.writeReg(0x71, 0x44); // manual mode + debounce
	tft.writeReg(0x71, tft.readReg(0x71) | 0x01); // wait for touch event
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

// Compatibility Functions
void digitalWrite_A(int pin, int state) {
	uint16_t pin_addr = (1 << (pin));
	switch (state) {
		case 0: 
			GPIOA->BSRRH |= pin_addr;
			break;
		default: // Anything other than zero is high
			GPIOA->BSRRL |= pin_addr;
	}
}

extern uint8_t digitalRead(int pin);

static void external_LED(__IO uint32_t pause) {
	digitalWrite_A(10, 1);
	delay(pause);
	digitalWrite_A(10,0);
	delay(pause);
}


int main(void)
{
	uint16_t tx, ty;
	external_IO_setup();
  	gui_init();
	delay(PAUSE_LONG);
  	currentMode = HOMESCREEN;
	char* coords = new char[15];
	while (1) {
    MainScreenInit();
	int delay_touch_detection = 10000;
	tx = 0;
	ty = 0;
	while (currentMode == HOMESCREEN) {
		tft.writeReg(0x71, (tft.readReg(0x71) & ~(0x03)) |  0x01); // wait for touch event
		while (digitalRead(RA8875_INT)){}
			tft.writeReg(0x71, (tft.readReg(0x71) & ~(0x03)) |  0x02); // latch x
			tft.writeReg(0x71, (tft.readReg(0x71) & ~(0x03)) |  0x03); // latch y
			
			  tx = tft.readReg(RA8875_TPXH);
			  ty = tft.readReg(RA8875_TPYH);
			  uint8_t temp = tft.readReg(RA8875_TPXYL);
			  tx <<= 2;
			  ty <<= 2;
			  tx |= temp & 0x03;        // get the bottom x bits
			  ty |= (temp >> 2) & 0x03; // get the bottom y bits

			tft.writeReg(0x71, (tft.readReg(0x71) & ~(0x03))); // set idle
			tft.writeReg(0xF1, 0x04); // clear interrupt bit
			
			uint32_t xtemp = (uint32_t) tx;
			uint32_t ytemp = (uint32_t) ty;
			xtemp *= s_width;
			ytemp *= s_height;
			xtemp /= 1023;
			ytemp /= 1023;
			tx = (uint16_t) xtemp;
			ty = (uint16_t) ytemp;
			
			if (settings.isTapped(tx,ty)){
				clearScreen(RA8875_BLACK);
				currentMode = ALARMSCREEN;
			}
			tft.drawPixel(tx,ty, RA8875_WHITE);
			tft.fillRect(290,200,390,300, RA8875_BLACK);
			tft.textMode();
			tft.textSetCursor(300,200);
			tft.textColor(RA8875_WHITE, RA8875_BLACK);
			tft.textEnlarge(1);
			sprintf(coords, "%d", tx);
			tft.textWrite(coords);
			tft.textSetCursor(300,250);
			sprintf(coords, "%d", ty);
			tft.textWrite(coords);
			tft.graphicsMode();
	}
	if (currentMode == ALARMSCREEN) {
        SettingsScreenInit();
		delay_touch_detection = 10000;
    	for (int i = 0; i < delay_touch_detection; i += 1) {
			tft.touchRead(&tx, &ty);
    	}
		// Clear the touch points to prevent double-presses
		tx = 0;
		ty = 0;
        while (currentMode == ALARMSCREEN) {
            if (digitalRead(RA8875_WAIT) && (tft.touched())) {
				while (digitalRead(RA8875_WAIT) && tft.touched()) {
					tft.touchRead(&tx, &ty);
					tft.drawPixel(tx,ty, RA8875_WHITE);
				}
            }
			else {
				// If no touch events, clear the touch points.
				tx = 0;
				ty = 0;
			}
            if (cancel_button.isTapped(tx,ty)) {
                clearScreen(RA8875_BLACK);
                currentMode = HOMESCREEN;
            }
        }
    }
	}
	/*if (success) {
		while(true){
			external_LED(PAUSE_SHORT);		
		}
	} else {
		while(true){
			external_LED(PAUSE_LONG);	
		}
	}*/
    return 0; 
}
