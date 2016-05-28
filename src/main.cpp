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
#define RA8875_INT 5
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

uint16_t tx, ty;
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


/* Build UI Buttons */
/*
Button settings = Button(9,9,2,2,RA8875_RED,"Alarm Settings",true,&tft);
Button confirm_button = Button(9,1,2,2,RA8875_GREEN,"Confirm",true,&tft);
Button cancel_button = Button(9,9,2,2,RA8875_RED,"Cancel",true,&tft);
Button default_button = Button(6,7,2,2,RA8875_LIGHTGREY,"Default Settings",true,&tft);

TextBox title = TextBox(1,3,1,3,RA8875_BLACK,RA8875_WHITE,3,true,"FreePulse Patient Monitor v0.9", &tft);

ECGReadout ecg = ECGReadout(2,1,3,8,15,RA8875_BLUE,RA8875_LIGHTGREY,&tft);
ECGReadout ecg2 = ECGReadout(5,1,3,8,16,RA8875_RED,RA8875_LIGHTGREY,&tft);
*/
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

void gui_init() {
	tft.begin();

	tft.displayOn(true);
	tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	tft.PWM1out(255);

	// With hardware accelleration this is instant
	tft.fillScreen(RA8875_BLACK);
	tft.touchEnable(true);
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
  //title.draw();
  //settings.draw();
  //ecg.draw();
  //ecg2.draw();
}


void SettingsScreenInit(void){
  clearScreen(RA8875_BLACK);
  //confirm_button.draw();
  //default_button.draw();
  //cancel_button.draw();
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

int main(void)
{
	external_IO_setup();
  	gui_init();
  	currentMode = HOMESCREEN;
    //MainScreenInit();
  	clearScreen(RA8875_GREENYELLOW);
	showGrid();
	while(true){
		external_LED();		
	}
    /*while (1)
    {
		external_LED();
		if (currentMode == HOMESCREEN) {
			//MainScreenInit();
			int delay_touch_detection = 10000;
			for (int i = 0; i < delay_touch_detection; i += 1) {
				tft.touchRead(&tx, &ty);
			}
			// Clear the touch points to prevent double-presses
			tx = 0;
			ty = 0;
			int display_timer = 0;
			int max_time = 200;
			while(currentMode == HOMESCREEN) {
				if (display_timer == max_time) {
					display_timer = 0;
				} else { display_timer += 1; }
				//delay(10);
				if ((tft.touched())) {
					while (tft.touched()) {
						tft.touchRead(&tx, &ty);
					}
				}
				else {
					// If no touch events, clear the touch points.
					tx = 0;
					ty = 0;
				}
				if (settings.isTapped(tx,ty)){
					clearScreen(RA8875_BLACK);
					currentMode = ALARMSCREEN;
				}
			}
		}
		if (currentMode == ALARMSCREEN) {
			SettingsScreenInit();
			int delay_touch_detection = 10000;
			for (int i = 0; i < delay_touch_detection; i += 1) {
				tft.touchRead(&tx, &ty);
			}
			// Clear the touch points to prevent double-presses
			tx = 0;
			ty = 0;
			while (currentMode == ALARMSCREEN) {
				if ((tft.touched())) {
					while (tft.touched()) {
						tft.touchRead(&tx, &ty);
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
    }*/
    return 0; 
}
