/*
 * Display.h
 *
 * Provide compatibility functions to create interoperability
 * between the STM32 peripheral libraries and the Adafruit 
 * libraries (written for Arduino).
 *
 */
#ifndef __Adafruit_Compat_h__
#define __Adafruit_Compat_h__

#include <stdint.h>

#include "System.h"
#include "SPI.h"
#include "Adafruit_RA8875.h"

#define RA8875_GREENYELLOW 0xAFE5      
#define RA8875_LIGHTGREY   0xC618     

#define RA8875_INT PC2
#define RA8875_WAIT PC3

class Display: public Adafruit_RA8875 {
private:
	SPI_Interface* SPI;	

	/* Compatibility function overriding the Adafruit_RA8875::begin() call */
	bool begin(__attribute__((unused))enum RA8875sizes s){
		_size = RA8875_800x480;
		_width = 800;
		_height = 480;
		SPI->begin();
		this->initialize();
		return true;
	}

public:
	uint16_t* touch_points;
	Pin_Num interrupt, wait, cs, rst;

	Display(Pin_Num cs, Pin_Num rst, SPI_Channel SPIx):Adafruit_RA8875(cs,rst){
		this->interrupt = PC2;
		configure_GPIO(this->interrupt, UP, OUTPUT); // INT pin
		this->wait = PC3;
		configure_GPIO(this->wait, UP, OUTPUT); // WAIT pin
		this->SPI = new SPI_Interface(SPIx);
		this->cs = cs;
		this->rst = rst;
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

	void clearScreen(int color){
		this->fillScreen(color);
	}
	
	void read_touch(){
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
};
#endif
