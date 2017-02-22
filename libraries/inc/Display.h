/*
 * Display.h
 *
 * Provide compatibility functions to create interoperability
 * between the STM32 peripheral libraries and the Adafruit 
 * libraries (written for Arduino).
 *
 */
#ifndef __Display_h__
#define __Display_h__ 

#include "SPI.h"
#include "Utils.h"
#include "Adafruit_RA8875.h"

#define RA8875_GREENYELLOW 0xAFE5      
#define RA8875_LIGHTGREY   0xC618     

static const int LONG_DELAY = 10000;

/* 
 * Large Font Bitmap Declarations
 *
 * Each digit is an 8x8 grid (zero padded by one on right and left).
 */
const uint8_t bigZero[8][8] =  { { 0, 0, 0, 1, 1, 0, 0, 0},
								 { 0, 0, 1, 0, 0, 1, 0, 0},
								 { 0, 1, 0, 0, 0, 0, 1, 0},
								 { 0, 1, 0, 0, 0, 0, 1, 0},
								 { 0, 1, 0, 0, 0, 0, 1, 0},
								 { 0, 1, 0, 0, 0, 0, 1, 0},
								 { 0, 0, 1, 0, 0, 1, 0, 0},
								 { 0, 0, 0, 1, 1, 0, 0, 0}
							   };

const uint8_t bigOne[8][8] =  { { 0, 0, 0, 1, 0, 0, 0, 0},
								{ 0, 0, 1, 1, 0, 0, 0, 0},
								{ 0, 1, 0, 1, 0, 0, 0, 0},
								{ 0, 0, 0, 1, 0, 0, 0, 0},
								{ 0, 0, 0, 1, 0, 0, 0, 0},
								{ 0, 0, 0, 1, 0, 0, 0, 0},
								{ 0, 0, 0, 1, 0, 0, 0, 0},
								{ 0, 1, 1, 1, 1, 1, 0, 0}
							};

const uint8_t bigTwo[8][8] =  { { 0, 0, 1, 1, 1, 1, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 0, 0, 1, 1, 0, 0},
								{ 0, 0, 0, 1, 0, 0, 0, 0},
								{ 0, 0, 1, 0, 0, 0, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 0, 0},
								{ 0, 1, 1, 1, 1, 1, 1, 0}
							};

const uint8_t bigThree[8][8] ={ { 0, 0, 1, 1, 1, 1, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 0, 1, 1, 1, 0, 0},
								{ 0, 0, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 1, 1, 1, 1, 0, 0}
							};

const uint8_t bigFour[8][8] = { { 0, 0, 0, 0, 1, 0, 0, 0},
								{ 0, 0, 0, 1, 1, 0, 0, 0},
								{ 0, 0, 1, 0, 1, 0, 0, 0},
								{ 0, 1, 0, 0, 1, 0, 0, 0},
								{ 0, 1, 0, 0, 1, 0, 0, 0},
								{ 0, 1, 1, 1, 1, 1, 1, 0},
								{ 0, 0, 0, 0, 1, 0, 0, 0},
								{ 0, 0, 0, 0, 1, 0, 0, 0}
							};

const uint8_t bigFive[8][8] = { { 0, 1, 1, 1, 1, 1, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 0, 0},
								{ 0, 1, 1, 1, 1, 1, 0, 0},
								{ 0, 0, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 1, 1, 1, 1, 0, 0}
							};

const uint8_t bigSix[8][8] =  { { 0, 0, 0, 1, 1, 1, 0, 0},
								{ 0, 0, 1, 0, 0, 0, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 0, 0},
								{ 0, 1, 1, 1, 1, 1, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 1, 1, 1, 1, 0, 0}
							};

const uint8_t bigSeven[8][8] = {{ 0, 1, 1, 1, 1, 1, 1, 0},
								{ 0, 0, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 0, 0, 0, 1, 0, 0},
								{ 0, 0, 0, 0, 0, 1, 0, 0},
								{ 0, 0, 0, 0, 0, 1, 0, 0},
								{ 0, 0, 0, 0, 1, 0, 0, 0},
								{ 0, 0, 0, 0, 1, 0, 0, 0},
								{ 0, 0, 0, 0, 1, 0, 0, 0}
							};

const uint8_t bigEight[8][8] = {{ 0, 0, 1, 1, 1, 1, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 1, 1, 1, 1, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 1, 1, 1, 1, 0, 0}
							};

const uint8_t bigNine[8][8] = { { 0, 0, 1, 1, 1, 1, 0, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 1, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 1, 1, 1, 1, 1, 0},
								{ 0, 0, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 0, 0, 0, 0, 1, 0},
								{ 0, 0, 0, 0, 0, 1, 0, 0},
								{ 0, 0, 1, 1, 1, 0, 0, 0}
							};

class Display: public Adafruit_RA8875 {
private:
	SPI_Interface* SPI;	

	/* Compatibility function overriding the Adafruit_RA8875::begin() call */
	bool begin(__attribute__((unused))enum RA8875sizes s) {
		_size = RA8875_800x480;
		digitalWrite(_cs, HIGH);
		SPI->begin();
		soft_reset();
		initialize();
		return true;
	}
	
	void printBigNumber(const uint8_t bitmap[8][8], int x, int y, uint16_t foreground, uint16_t background){
		int cursor_x = x;
		int cursor_y = y;
		int xscale = horizontal_scale/8;
		int yscale = vertical_scale/4;
		for (int row = 0; row < 8; row ++) {
			cursor_x = x;
			for (int col = 0; col < 8; col ++) {
				if (bitmap[row][col] == 1) {
					fillRect(cursor_x, cursor_y, xscale, yscale, foreground);
				} else {
					fillRect(cursor_x, cursor_y, xscale, yscale, background);
				}
				cursor_x += xscale;
			}
			cursor_y += yscale;
		}
	}

public:
	uint16_t* touch_points;
	Pin_Num interrupt, wait, cs, rst;
	int width = 800;
	int height = 480;
	int rows;
	int columns;
	int vertical_scale;
	int horizontal_scale;
	
	Display(Pin_Num cs, Pin_Num rst, Pin_Num wait, SPI_Interface* SPIx, int rows, int columns):
		Adafruit_RA8875(cs,rst,wait,SPIx), SPI(SPIx), wait(wait), cs(cs), rst(rst),  rows(rows), columns(columns){
		configure_GPIO(_cs, NO_PU_PD, OUTPUT);
		configure_GPIO(_rst, UP, OUTPUT);
		this->interrupt = PA12;
		configure_GPIO(this->interrupt, UP, INPUT); // INT pin
		configure_GPIO(this->wait, UP, INPUT); // WAIT pin
		this->vertical_scale = height / rows;
		this->horizontal_scale = width / columns;
		this->_width = width;
		this->_height = height;
		this->touch_points = new uint16_t[2];
	}
	
	void startup() {
		begin(RA8875_800x480);
        delay(LONG_DELAY);
		displayOn(true);
		GPIOX(true);      // Enable TFT - display enable tied to GPIOX
		PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
		PWM1out(255);
		fillScreen(RA8875_BLACK);
        delay(LONG_DELAY);
		touchEnable(true);
	}

	void soft_reset() {
		digitalWrite(_rst, LOW);
		delay(100); // Hold reset low for at least 40.96 usec 
		digitalWrite(_rst, HIGH);
		delay(1500); // At least 1ms for system stabilization
	}

	void read_touch() {
		uint16_t tx,ty;
		this->touchRead(&tx, &ty);
		uint32_t tempx = (uint32_t) tx;
		uint32_t tempy = (uint32_t) ty;
		tempx *= _width;
		tempy *= _height;
		tempx /= 1023;
		tempy /= 1023;
		this->touch_points[0] = (uint16_t) tempx;
		this->touch_points[1] = (uint16_t) tempy;
	}

	void reset_touch() {
		this->touch_points[0] = 0;
		this->touch_points[1] = 0;
	}

    void clearTouchEvents() {
        read_touch();
        reset_touch();
    }

	void showGrid(void){
		for (int i = 1; i < rows; i += 1) {
			drawLine(1,i*vertical_scale,width-1,i*vertical_scale,RA8875_LIGHTGREY);
		}
		for (int i = 1; i < columns; i += 1) {
			drawLine(i*horizontal_scale,1,i*horizontal_scale,height-1,RA8875_LIGHTGREY);
		}
	}

	void printLarge(int num, int x, int y, uint16_t foreground, uint16_t background) {
		switch(num) {
			case 0:
				printBigNumber(bigZero, x, y, foreground, background);
				break;
			case 1:
				printBigNumber(bigOne, x, y, foreground, background);
				break;
			case 2:
				printBigNumber(bigTwo, x, y, foreground, background);
				break;
			case 3:
				printBigNumber(bigThree, x, y, foreground, background);
				break;
			case 4:
				printBigNumber(bigFour, x, y, foreground, background);
				break;
			case 5:
				printBigNumber(bigFive, x, y, foreground, background);
				break;
			case 6:
				printBigNumber(bigSix, x, y, foreground, background);
				break;
			case 7:
				printBigNumber(bigSeven, x, y, foreground, background);
				break;
			case 8:
				printBigNumber(bigEight, x, y, foreground, background);
				break;
			case 9:
				printBigNumber(bigNine, x, y, foreground, background);
				break;
			default:
				return;
		}
	}

};

#endif
