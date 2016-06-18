/*
 * Adafruit_Compat.h
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


class Display: public Adafruit_RA8875 {
private:
	SPI_Interface* SPI;
public:
	Display(Pin_Num cs, Pin_Num rst, SPI_Channel SPIx):Adafruit_RA8875(cs,rst){
		configure_GPIO(PC2, UP, OUTPUT); // INT pin
		configure_GPIO(PC3, UP, OUTPUT); // WAIT pin
		this->SPI = new SPI_Interface(SPIx);

	}

	/* Compatibility function overriding the Adafruit_RA8875::begin() call */
	bool begin(__attribute__((unused))enum RA8875sizes s){
		_size = RA8875_800x480;
		_width = 800;
		_height = 480;
		SPI->begin();
		this->initialize();
		return true;
	}

};

#endif
