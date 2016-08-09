#ifndef _nibp_h_
#define _nibp_h_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "System.h"
#include "CircleBuffer.h"
#include "interface.h"
#include "misc.h"
#include "Vector.h"

enum NIBPState {
    inflate, sensing, done, off
};

class NIBPReadout : public ScreenElement {
private:
	int fifo_size;
	int avg_size;
	int avg_cursor;
	int display_cursor;
	Pin_Num pulse_pn;
	Pin_Num pressure_pn;
	CircleBuffer<int> pulse_fifo;
	CircleBuffer<int> pressure_fifo;
	int scaling_factor;
	Vector<int> avg_queue;
	int trace_color;
	int background_color;
	int sampling_rate;
    const char* title = "";
    const char* body_text = "";
	TimerChannel timx;
    NIBPState state = off;



    /*
     * IIR Butterworth filter
     * Fs = 500 Hz
     * Fc = 10 Hz
     */
	double pulse_sos_filter[3][6] = {
		{1, 2, 1, 1, -1.9102, 0.9254},
		{1, 2, 1, 1, -1.8016, 0.8159},
		{1, 1, 0, 1, -0.8816, 0.0}
	};

    /*
     * IIR Butterworth filter
     * Fs = 500 Hz
     * Fc = 30 Hz
     */
	double pressure_sos_filter[3][6] = {
		{1, 2, 1, 1, -1.6696, 0.7957},
		{1, 2, 1, 1, -1.4328, 0.5410},
		{1, 1, 0, 1, -0.6796, 0.0}
	};

	double pulse_gain[3] = {0.0038, 0.0036, 0.0592};
	double pressure_gain[3] = {0.0315, 0.0271, 0.1602};


	double xs1[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};
	double ws1[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};

	double filter_pulse(int x, double sos_filter[3][6], double gain[3]) {
		xs1[0][0] = (double) x;
		double y;
		for (int i = 0; i < 3; i ++){
			xs1[i][0] *= gain[i];
			// Apply SOS
			ws1[i][0] = xs1[i][0] - sos_filter[i][4]*ws1[i][1] - sos_filter[i][5]*ws1[i][2];
			y = sos_filter[i][0]*ws1[i][0] + sos_filter[i][1]*ws1[i][1] + sos_filter[i][2]*ws1[i][2]; 
			// Shift coefficients
			xs1[i][2] = xs1[i][1];
			xs1[i][1] = xs1[i][0];
			ws1[i][2] = ws1[i][1];
			ws1[i][1] = ws1[i][0];
			// Carry over to next section
			if (i != 2) {
				xs1[i+1][0] = y;	
			}
		}
		return y; 
	}

	double xs2[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};
	double ws2[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};

	double filter_pressure(int x, double sos_filter[3][6], double gain[3]) {
		xs2[0][0] = (double) x;
		double y;
		for (int i = 0; i < 3; i ++){
			xs2[i][0] *= gain[i];
			// Apply SOS
			ws2[i][0] = xs2[i][0] - sos_filter[i][4]*ws2[i][1] - sos_filter[i][5]*ws2[i][2];
			y = sos_filter[i][0]*ws2[i][0] + sos_filter[i][1]*ws2[i][1] + sos_filter[i][2]*ws2[i][2]; 
			// Shift coefficients
			xs2[i][2] = xs2[i][1];
			xs2[i][1] = xs2[i][0];
			ws2[i][2] = ws2[i][1];
			ws2[i][1] = ws2[i][0];
			// Carry over to next section
			if (i != 2) {
				xs2[i+1][0] = y;	
			}
		}
		return y; 
	};

    /*
     * Calibration curve for pressure sensor.
     * Output is pressure in mmHg.
     */
    int calibrate(double input_value) {
        double m = 0.4838;
        double b = -795.9;
        return (int) (m*input_value + b);
    };

public:	
	int last_val = 0;

    NIBPReadout(int row, int column, int len, int width, Pin_Num pulse_pn, Pin_Num pressure_pn,
				int trace_color, int background_color, int sampling_rate, TimerChannel timx, Display* tft):
				ScreenElement(row,column,len,width,tft), pulse_pn(pulse_pn), pressure_pn(pressure_pn), 
                trace_color(trace_color), background_color(background_color), sampling_rate(sampling_rate), timx(timx) {
		fifo_size = real_width;
        pressure_fifo.resize(fifo_size);
        pulse_fifo.resize(fifo_size);
		avg_size = 10;
		avg_cursor = 0;
		display_cursor = 0;
		/* avg_queue = Vector<int>(avg_size); */
        avg_queue.resize(avg_size);
		scaling_factor = real_len;
		configure_GPIO(pulse_pn, NO_PU_PD, ANALOG);
		configure_GPIO(pressure_pn, NO_PU_PD, ANALOG);
	}

	void enable() {
		Sampler sampler(sampling_rate, timx);
		sampler.enable();
	}

	void draw_border(void){
	    tft->drawRect(coord_x,coord_y,real_width,real_len,trace_color);
    }

    NIBPState getState(void) {
        return this->state;
    }

    void setState(NIBPState state) {
        this->state = state;
    }

    void draw(void){
        tft->fillRect(coord_x,coord_y,real_width,real_len,background_color);
		draw_border();
    }

	int read(void) {
		int nibp_pulse_data = filter_pulse(analogRead(pulse_pn), pulse_sos_filter, pulse_gain);
		pulse_fifo.add(nibp_pulse_data);
		int nibp_pressure_data = filter_pressure(analogRead(pressure_pn), pressure_sos_filter, pressure_gain);
		pressure_fifo.add(nibp_pressure_data);
		return nibp_pulse_data; // Return is only for console logging purposes
		/* return calibrate(nibp_pressure_data); // Return is only for console logging purposes */
	}

    int scale(int raw_signal) {
        raw_signal -= 1040;
        double scale_factor = ((double) real_len) / 300.0;
        return (int) (raw_signal*scale_factor);
    }
    
    int getRecentAvg(int length) {
		uint32_t prim = __get_PRIMASK();
        int sum = 0;
		__disable_irq();
        for (int i = 0; i < length; i ++) {
            sum += pressure_fifo[i]; 
        }
		if (!prim) { 
			__enable_irq();
		}
        return sum / length;
    }

    void changeTitle(const char* new_title) {
        this->title = new_title;
    }

    // TODO: Figure out the compiler error here.
    void updateInstructions(void) {
        switch(this->state) {
            case off:
               break;
            case done:
                break;
            case sensing:
                break;
            case inflate:
                int avg_pressure = getRecentAvg(5);
                if (avg_pressure < 150) {
                    //TODO: add NIBP logic
                    changeTitle("Start inflating the cuff."); 
                } else {
                    /* changeTitle("Stop!", RA8875_RED); */
                    delay(1);
                }
                break;
            default:
                break;
        }
    }

    /*
     * display_signal() -
     * For the purposes of UX, we will only display the pulsing input.
     */
	void display_signal(void) {
		uint32_t prim = __get_PRIMASK();
		__disable_irq();
		int new_val = pulse_fifo.newest();
		if (!prim) { 
			__enable_irq();
		}
		int threshold = 30;
        /* int display = scale(new_val); */
        int display = new_val * real_len;
        display /= 3000;
		if (display > real_len) { display = real_len; }
		else if (display < 0) { display = 0; }
		int old_display = last_val * real_len;
		old_display /= 3000;
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
