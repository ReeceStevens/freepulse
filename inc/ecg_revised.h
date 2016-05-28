#ifndef _ecg_h_
#define _ecg_h_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CircleFifo.h"
#include "Vector.h"

class ECGReadout : public ScreenElement {
private:
	int fifo_size;
	int avg_size;
	int avg_cursor;
	int display_cursor;
	int pin;
	CircleFifo<int> fifo;
	int scaling_factor;
	Vector<int> avg_queue;
	int trace_color;
	int background_color;

public:	
    ECGReadout(int row, int column, int len, int width, int pin, 
				int trace_color, int background_color, Adafruit_RA8875* tft_interface):
				ScreenElement(row,column,len,width,tft_interface), pin(pin), trace_color(trace_color),
			   	background_color(background_color) {
		fifo_size = real_width;
		fifo = CircleFifo<int>(fifo_size);
		avg_size = 10;
		avg_cursor = 0;
		display_cursor = 0;
		avg_queue = Vector<int>(avg_size);
		scaling_factor = real_len;			
	}
    void draw_border(void){
	    tft_interface->drawRect(coord_x,coord_y,real_width,real_len,trace_color);
    };
    void draw(void){
        tft_interface->fillRect(coord_x,coord_y,real_width,real_len,background_color);
		draw_border();
    };

	void read(void) {
		if (avg_cursor < avg_size) {
			//int input = analogRead(pin);
			int input = 500;
			avg_queue[avg_cursor] = input;
			avg_cursor += 1;
		} else {
			avg_cursor = 0;
			int avg;
			for (int i = 0; i < avg_size; i ++ ) {
				avg += avg_queue[i];
			}
			avg /= avg_size;
			fifo.add(avg);
		}	
	}

	void display_signal(void) {
		// Get newest value from fifo
		//noInterrupts();
		int new_val = fifo[0];
		int last_val = fifo[1];
		//interrupts();
		int threshold = 1900;
		int display = new_val * real_len;
		display /= 2046;
		if (display > real_len) { display = real_len; }
		else if (display < 0) { display = 0; }
		int old_display = last_val * real_len;
		old_display /= 2046;
		if (old_display > real_len) { old_display = real_len; }
		else if (old_display < 0) { old_display = 0; }
		tft_interface->drawFastVLine(coord_x + display_cursor, coord_y, real_len, background_color);
		if ((display - old_display > threshold) || (display - old_display < -threshold)) {
			tft_interface->drawLine(coord_x + display_cursor, coord_y + real_len - old_display, coord_x + display_cursor, coord_y + real_len - display, trace_color);
		} else {
			tft_interface->drawPixel(coord_x + display_cursor, coord_y + real_len - display, trace_color);
		}
		display_cursor = CircleFifo<int>::mod(display_cursor+1, real_width); // Advance display cursor
		tft_interface->drawFastVLine(coord_x + display_cursor, coord_y, real_len, RA8875_WHITE); // Draw display cursor
	}

};
#endif
