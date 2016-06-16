/*
 * Adafruit_Compat.h
 *
 * Provide compatibility functions to create interoperability
 * between the STM32 peripheral libraries and the Adafruit 
 * libraries (written for Arduino).
 *
 */
#include <stdint.h>

#include "System.h"




class Adafruit_Compat {
public:
	void digitalWrite(Pin pin, int state) {
		uint16_t pin_addr = (1 << (pin.physical_pin));
		switch(state) {
			case 0:
				pin.GPIOx->BSRRH |= pin_addr;
				break;
			default:
				pin.GPIOx->BSRRL |= pin_addr;
		}
	};
	void analogWrite(int pin, int value);
	int digitalRead(Pin pin) {
		uint16_t pin_addr = (1 << (pin.physical_pin));
		if (pin.GPIOx->IDR & pin_addr) { return 1; }
		else { return 0; }
	};
	int analogRead(int pin);

	class SPI {
	public:	
		void begin();
		void setClockDivider(uint8_t clock_divider);
		void setDataMode();
		uint8_t transfer(uint8_t data);
	};

};

