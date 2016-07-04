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
#include "Adafruit_RA8875.h"

#define RA8875_GREENYELLOW 0xAFE5      
#define RA8875_LIGHTGREY   0xC618     

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

public:
	uint16_t* touch_points;
	Pin_Num interrupt, wait, cs, rst;
	int width = 800;
	int height = 480;
	int rows;
	int columns;
	int vertical_scale;
	int horizontal_scale;
	
	Display(Pin_Num cs, Pin_Num rst, SPI_Interface* SPIx, int rows, int columns):
		Adafruit_RA8875(cs,rst,SPIx), SPI(SPIx), cs(cs), rst(rst), rows(rows), columns(columns){
		configure_GPIO(_cs, NO_PU_PD, OUTPUT);
		configure_GPIO(_rst, NO_PU_PD, OUTPUT);
		this->interrupt = PC2;
		configure_GPIO(this->interrupt, UP, INPUT); // INT pin
		this->wait = PC3;
		configure_GPIO(this->wait, UP, INPUT); // WAIT pin
		this->vertical_scale = height / rows;
		this->horizontal_scale = width / columns;
		this->_width = width;
		this->_height = height;
		this->touch_points = new uint16_t[2];
	}
	
	void startup() {
		begin(RA8875_800x480);
		displayOn(true);
		GPIOX(true);      // Enable TFT - display enable tied to GPIOX
		PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
		PWM1out(255);
		fillScreen(RA8875_BLACK);
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

	void showGrid(void){
		for (int i = 1; i < rows; i += 1) {
			drawLine(1,i*vertical_scale,width-1,i*vertical_scale,RA8875_LIGHTGREY);
		}
		for (int i = 1; i < columns; i += 1) {
			drawLine(i*horizontal_scale,1,i*horizontal_scale,height-1,RA8875_LIGHTGREY);
		}
	}

};

#endif
