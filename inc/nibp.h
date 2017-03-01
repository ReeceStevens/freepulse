#ifndef _nibp_h_
#define _nibp_h_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GPIO.h"
#include "Console.h"
#include "Utils.h"
#include "Timer.h"
#include "CircleBuffer.h"
#include "interface.h"
#include "misc.h"
#include "Vector.h"

const int BIG_DELAY = 400;

enum NIBPState {
    inflate, measure, calculate, done, start, error, na
};

void noop_onclick(void) {}

// TODO: Does this need to be more efficient?
double rms(int* vals, int window) {
    int running_sum = 0;
    for (int i = 0; i < window; i ++) {
        int x = vals[i];
        running_sum += x*x;
    }
    running_sum = ((double) running_sum) / ((double) window);
    return sqrt(running_sum);
}

class NIBPReadout : public ScreenElement {
private:
	int fifo_size;
	int avg_size;
	int avg_cursor;
	int display_cursor;
    int delay_counter = 0;
	Pin_Num pulse_pn;
	Pin_Num pressure_pn;
    Pin_Num valve_out;
	CircleBuffer<int> pulse_fifo;
	CircleBuffer<int> pressure_fifo;
	int scaling_factor;
	Vector<int> avg_queue;
	int trace_color;
	int background_color;
	int sampling_rate;
    int goal_pressure;
    double offset = -605;
    bool sampling = true;
    int rms_window_size = 1000;
    Vector<double> pulse_rms_measurements;
    Vector<int> pressure_measurements;
    TextBox* title;
    TextBox* value;
    Button* button;
    LargeNumberView* systolic;
    LargeNumberView* diastolic;
	TimerChannel timx;

    void open_valve() {
        digitalWrite(valve_out, HIGH);
    }

    void close_valve() {
        digitalWrite(valve_out, LOW);
    }

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
     * Fc = 5 Hz
     */
	double pressure_sos_filter[3][6] = {
		{1, 2, 1, 1, -1.9581, 0.9619},
		{1, 2, 1, 1, -1.8996, 0.9033},
		{1, 1, 0, 1, -0.9391, 0.0}
	};

	double pulse_gain[3] = {0.0038, 0.0036, 0.0592};
	double pressure_gain[3] = {0.00097, 0.00094, 0.0305};


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
        double m = 0.3998;
        int calibrated_value = (int) (m*input_value + offset);
        return calibrated_value < 0 ? 0 : calibrated_value;
    };

    int max_idx(Vector<double> a) {
        int max = 0;
        double max_val = a[0];
        for (int i = 1; i < a.size(); i++) {
            if (a[i] > max_val) {
                max = i;
                max_val = a[i];
            }
        }
        return max;
    }

    double max(Vector<double> a) {
        int max = a[0];
        for (int i = 1; i < a.size(); i++) {
            if (a[i] > max) {
                max = a[i];
            }
        }
        return max;
    }

    double min(Vector<double> a) {
        int min = a[0];
        for (int i = 1; i < a.size(); i++) {
            if (a[i] < min) {
                min = a[i];
            }
        }
        return min;

    }

public:	
	int last_val = 0;
    NIBPState state = start;
    NIBPState prev_state = na;
    Sandbox* sandbox;

    NIBPReadout(int row, int column, int len, int width, Pin_Num pulse_pn, Pin_Num pressure_pn, Pin_Num valve_out,
				int trace_color, int background_color, int sampling_rate, TimerChannel timx, Display* tft):
				ScreenElement(row,column,len,width,tft), pulse_pn(pulse_pn), pressure_pn(pressure_pn), valve_out(valve_out),
                trace_color(trace_color), background_color(background_color), sampling_rate(sampling_rate),
                timx(timx) {
		fifo_size = 1000;
        pressure_fifo.resize(fifo_size);
        pulse_fifo.resize(fifo_size);
        pulse_rms_measurements.resize(20);
        pressure_measurements.resize(20);
		avg_size = 10;
		avg_cursor = 0;
		display_cursor = 0;
        avg_queue.resize(avg_size);
		scaling_factor = real_len;
		configure_GPIO(pulse_pn, NO_PU_PD, ANALOG);
		configure_GPIO(pressure_pn, NO_PU_PD, ANALOG);
		configure_GPIO(valve_out, NO_PU_PD, OUTPUT);
        sandbox = new Sandbox(row,column,len,width,background_color,tft);
        title = new TextBox(row,column,1,width,background_color,RA8875_WHITE,3,true,false,"",tft);
        value = new TextBox(row+1,column,1,width,background_color,RA8875_WHITE,3,true,false,"",tft);
        button = new Button(row+2,column+1,2,width-2,background_color,"",true,tft,&noop_onclick);
        sandbox->add(title);
        sandbox->add(value);
        sandbox->add(button);
        systolic = new LargeNumberView(row, column + 4, len, width, RA8875_BLACK, RA8875_GREEN, true, 0, tft);
        diastolic = new LargeNumberView(row + 2, column + 4, len, width, RA8875_BLACK, RA8875_GREEN, true, 0, tft);
        open_valve();
	}

	void enable() {
		enable_timer(sampling_rate, timx);
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
        updateInstructions();
        sandbox->draw();
        systolic->draw();
        diastolic->draw();
    }

    void update(void) {
        updateInstructions();
        sandbox->update();
        systolic->update();
        diastolic->update();
    }

	int read(void) {
		int nibp_pulse_data = filter_pulse(analogRead(pulse_pn), pulse_sos_filter, pulse_gain);
		pulse_fifo.add(nibp_pulse_data);
		int nibp_pressure_data = filter_pressure(analogRead(pressure_pn), pressure_sos_filter, pressure_gain);
		pressure_fifo.add(nibp_pressure_data);
		/* return nibp_pulse_data; // Return is only for console logging purposes */
		return calibrate(nibp_pressure_data); // Return is only for console logging purposes
	}

    int scale(int raw_signal) {
        raw_signal -= 1040;
        double scale_factor = ((double) real_len) / 300.0;
        return (int) (raw_signal*scale_factor);
    }

    int getRecentAvg(CircleBuffer<int> buffer, int length) {
        int sum = 0;
        sampling = false;
        for (int i = 0; i < length; i ++) {
            sum += calibrate(buffer[i]); 
        }
        sampling = true;
        return sum / length;
    }

    double getPulsePressureRMS(int window) {
        sampling = false;
        int* pulse_data = new int[window];
        for (int i = 0; i < window; i ++) {
            pulse_data[i] = pulse_fifo[i];
        }
        sampling = true;
        double retval = rms(pulse_data, window);
        delete [] pulse_data;
        return retval;
    }

    void updateInstructions(void) {
        switch(this->state) {
            case start:
            {
                if (prev_state != state) {
                    open_valve();
                    prev_state = state;
                    title->changeText("NIBP Test");
                    value->changeText("");
                    button->changeText("Start");
                    button->changeColor(RA8875_GREEN);
                    draw();
                }
                if (prev_state == state){
                    if (button->isTapped()) {
                        state = inflate;
                        delay(100000);
                        tft->clearTouchEvents();
                    }
                }
                break;
            }
            case inflate:
            {
                if (prev_state != state) {
                    int avg_open_valve_pressure = getRecentAvg(pressure_fifo, 5);
                    offset -= avg_open_valve_pressure;
                    close_valve(); // Close the valve
                    prev_state = state;
                    goal_pressure = 150;
                    title->changeText("Inflating...");
                    button->changeText("Cancel");
                    button->changeColor(RA8875_RED);
                    int avg_pressure = getRecentAvg(pressure_fifo, 5);
                    value->changeText(avg_pressure);
                    draw();
                }
                else {
                    int avg_pressure = getRecentAvg(pressure_fifo, 5);
                    value->changeText(avg_pressure);
                    if (avg_pressure > goal_pressure) {
                        state = measure;
                    }
                    if (button->isTapped()) {
                        state = start;
                        delay(100000);
                        tft->clearTouchEvents();
                    }
                }
                break;
            }
            case measure:
            {
                if (button->isTapped()) {
                    state = start;
                    delay(100000);
                    tft->clearTouchEvents();
                }
                if (prev_state != state) {
                    prev_state = state;
                    title->changeText("Measuring...");
                    button->changeText("Cancel");
                    pulse_rms_measurements.empty();
                }
                else {
                    int avg_pressure = getRecentAvg(pressure_fifo, 8);
                    value->changeText(avg_pressure);
                    if (goal_pressure < 40) {
                        // Finished! Validate measurements
                        // and perform calculations.
                        // For now, log out the array.
                        logger(l_info, "rms_vals = [");
                        for (int i = 0; i < pulse_rms_measurements.size(); i ++) {
                            logger(l_info, "%f, ", pulse_rms_measurements[i]);
                        }
                        logger(l_info, "];\n");
                        state = calculate;
                    }
                    if (avg_pressure < goal_pressure + 7) {
                        close_valve(); // Close valve for measurement
                    }
                    if (avg_pressure < goal_pressure) {
                        if (delay_counter < BIG_DELAY) {
                             delay_counter++; 
                             break;
                        } else {
                            delay_counter = 0;
                        }
                        // 1. Get the RMS value
                        double rms_val = getPulsePressureRMS(rms_window_size);
                        logger(l_info, "Measurement taken, %f at pressure %d mmHg\n", rms_val, avg_pressure);
                        /* logger(l_info, rms_val); */
                        /* logger(l_info, " at pressure "); */
                        /* logger(l_info, avg_pressure); */
                        /* logger(l_info, "mmHg\n"); */
                        // 2. Store it in the pulse pressure array
                        pulse_rms_measurements.push_back(rms_val);
                        pressure_measurements.push_back(avg_pressure);
                        // 3. Decrement the goal pressure by 10 mmHg.
                        goal_pressure -= 10;
                        open_valve(); // Re-open valve
                        /* goal_pressure -= 5; */
                    }
                }
                break;
            }
            case calculate:
            {
                logger(l_info, "rms_vals = [");
                for (int i = 0; i < pulse_rms_measurements.size(); i ++) {
                    /* logger(l_info, pulse_rms_measurements[i]); */
                    logger(l_info, "%f, ", pulse_rms_measurements[i]);
                }
                logger(l_info, "];\n");
                int rms_max_idx = max_idx(pulse_rms_measurements);
                double rms_max = max(pulse_rms_measurements);
                double rms_min = min(pulse_rms_measurements);
                double goal_rms_dia = ((rms_max - rms_min) * 2 / 3) + rms_min;
                double goal_rms_sys = ((rms_max - rms_min) * 2 / 3) + rms_min;
                // Find diastolic
                // Start searching one after the MAP, and throw away the last measurement
                int closest_pressure = -1;
                int last_above = -1;
                int first_below = -1;
                for (int i = rms_max_idx + 1; i < pulse_rms_measurements.size() - 1; i ++) {
                    if (closest_pressure == -1) {
                        closest_pressure = i;
                        if (pulse_rms_measurements[closest_pressure] >= goal_rms_sys) { last_above = closest_pressure; }
                        else { last_above = i + 1; }
                    } else {
                        double new_error = pulse_rms_measurements[i] - goal_rms_dia;
                        if (new_error >= 0) { last_above = i; }
                        else if (last_above == i - 1) {
                            first_below = i;
                            break;
                        }
                    }
                }
                int diastolic = (double) (pressure_measurements[last_above] - pressure_measurements[first_below]) * (goal_rms_dia - pulse_rms_measurements[first_below]) \
                                    / (pulse_rms_measurements[last_above] - pulse_rms_measurements[first_below]) + pressure_measurements[first_below];
                /* int diastolic = closest_pressure; */
                // Find systolic
                // Start searching one before the MAP
                closest_pressure = -1;
                last_above = -1;
                first_below = -1;
                for (int i = rms_max_idx - 1; i >= 0; i --) {
                    if (closest_pressure == -1) {
                        closest_pressure = i;
                        if (pulse_rms_measurements[closest_pressure] >= goal_rms_sys) { last_above = closest_pressure; }
                        else { last_above = i + 1; }
                    } else {
                        double new_error = pulse_rms_measurements[i] - goal_rms_sys;
                        if (new_error >= 0) { last_above = i; }
                        else if (last_above == i + 1) {
                            first_below = i;
                            break;
                        }
                    }
                }
                /* int systolic = closest_pressure; */
                int systolic = (double) (pressure_measurements[last_above] - pressure_measurements[first_below]) * (goal_rms_sys - pulse_rms_measurements[first_below]) \
                                    / (pulse_rms_measurements[last_above] - pulse_rms_measurements[first_below]) + pressure_measurements[first_below];
                this->systolic->changeNumber(systolic);
                this->diastolic->changeNumber(diastolic);
                state = done;
                break;
            }
            case done:
            {
                if (prev_state != state) {
                    prev_state = state;
                    title->changeText("Complete!");
                    value->changeText("    ");
                    button->changeText("Restart");
                    button->changeColor(RA8875_GREEN);
                }
                else {
                    if (button->isTapped()) {
                        state = start;
                    }
                }
                break;
            }
            case error:
            {
                if (prev_state != state) {
                    prev_state = state;
                    title->changeText("NIBP Error.");
                    value->changeText("    ");
                    button->changeText("Retry");
                    button->changeColor(RA8875_RED);
                }
                else {
                    if (button->isTapped()) {
                        state = start;
                    }
                }
                prev_state = state;
                break;
            }
            default:
                break;
        }
    }

    bool can_sample(void) {
        return sampling;
    }
};

#endif
