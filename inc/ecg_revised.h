#ifndef _ecg_h_
#define _ecg_h_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "System.h"
#include "CircleBuffer.h"
#include "interface.h"
#include "misc.h"
#include "Vector.h"

class ECGReadout : public ScreenElement {
private:
	int fifo_size;
	int avg_size;
	int avg_cursor;
	int display_cursor;
	Pin_Num pn;
	CircleBuffer<int> fifo;
	int scaling_factor;
	Vector<int> avg_queue;
	int trace_color;
	int background_color;
	int sampling_rate;
	TimerChannel timx;
	double sos_filter[5][6] = {
		{1, 2, 1, 1, -1.4818, 0.8316},
		{1, 2, 1, 1, -1.2772, 0.5787},
		{1, 2, 1, 1, -1.1430, 0.4128},
		{1, 2, 1, 1, -1.0619, 0.3126},
		{1, 2, 1, 1, -1.0237, 0.2654}
	};

	double gain[5] = {0.0875, 0.754, 0.0675, 0.0627, 0.0604};

	double xs[5][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};
	double ws[5][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};

	double filter(int x) {
		xs[0][0] = (double) x;
		double y;
		for (int i = 0; i < 5; i ++){
			xs[i][0] *= gain[i];
			// Apply SOS
			ws[i][0] = xs[i][0] - sos_filter[i][4]*ws[i][1] - sos_filter[i][5]*ws[i][2];
			y = sos_filter[i][0]*ws[i][0] + sos_filter[i][1]*ws[i][1] + sos_filter[i][2]*ws[i][2]; 
			// Shift coefficients
			xs[i][2] = xs[i][1];
			xs[i][1] = xs[i][0];
			ws[i][2] = ws[i][1];
			ws[i][1] = ws[i][0];
			// Carry over to next section
			if (i != 4) {
				xs[i+1][0] = y;	
			}
		}
		return y/10 - 1500; 
	}


public:	
	int last_val = 0;

    ECGReadout(int row, int column, int len, int width, Pin_Num pn, 
				int trace_color, int background_color, int sampling_rate, TimerChannel timx, Display* tft):
				ScreenElement(row,column,len,width,tft), pn(pn), trace_color(trace_color),
			   	background_color(background_color), sampling_rate(sampling_rate), timx(timx) {
		fifo_size = real_width;
		fifo = CircleBuffer<int>(fifo_size);
		avg_size = 10;
		avg_cursor = 0;
		display_cursor = 0;
		avg_queue = Vector<int>(avg_size);
		scaling_factor = real_len;
		configure_GPIO(pn, NO_PU_PD, ANALOG);
	}

	void enable() {
		Sampler sampler(sampling_rate, timx);
		sampler.enable();
	}

	void draw_border(void){
	    tft->drawRect(coord_x,coord_y,real_width,real_len,trace_color);
    }

    void draw(void){
        tft->fillRect(coord_x,coord_y,real_width,real_len,background_color);
		draw_border();
    }

	int read(void) {
		int ecg_data = filter(analogRead(pn));
		fifo.add(ecg_data);
		return ecg_data;
	}

	void display_signal(void) {
		uint32_t prim = __get_PRIMASK();
		__disable_irq();
		int new_val = fifo.newest();
		if (!prim) { 
			__enable_irq();
		}
		int threshold = 5;
		int display = new_val * real_len;
		display /= 4096;
		if (display > real_len) { display = real_len; }
		else if (display < 0) { display = 0; }
		int old_display = last_val * real_len;
		old_display /= 4096;
		if (old_display > real_len) { old_display = real_len; }
		else if (old_display < 0) { old_display = 0; }
		tft->drawFastVLine(coord_x + display_cursor, coord_y, real_len, background_color);
		if ((display - old_display > threshold) || (display - old_display < -threshold)) {
			tft->drawLine(coord_x + display_cursor, coord_y + real_len - old_display, coord_x + display_cursor, coord_y + real_len - display, trace_color);
		} else {
			tft->drawPixel(coord_x + display_cursor, coord_y + real_len - display, trace_color);
		}
		display_cursor = CircleBuffer<int>::mod(display_cursor+1, real_width); // Advance display cursor
		tft->drawFastVLine(coord_x + display_cursor, coord_y, real_len, RA8875_WHITE); // Draw display cursor
		last_val = new_val;
	}

};
#endif
