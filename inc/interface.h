#ifndef _interface_h_
#define _interface_h_

#include "System.h"
#include "Display.h"
#include "CircleBuffer.h"

class ScreenElement {
private:
    int row;
    int column;
	int len;
	int width;

public:
	int coord_x;
	int coord_y;
    int real_len;
    int real_width;
    Display* tft;

    ScreenElement() {}

    ScreenElement(int row, int column, int len, int width, Display* tft):
		row(row),column(column),len(len),width(width),tft(tft){
        coord_x = tft->horizontal_scale*(column-1);
        coord_y = tft->vertical_scale*(row-1);
        real_len = tft->vertical_scale*len;
        real_width = tft->horizontal_scale*width;
    }

    virtual void draw(void){};
    virtual void update(void){};
};

class Button : public ScreenElement{
private:
    int color;
    const char* button_str;

public:
    bool visible;
    bool lastTapped;

    Button(int row, int column, int len, int width, int color, const char* button_str, bool visible, Display* tft):
		ScreenElement(row,column,len,width,tft), color(color), button_str(button_str),visible(visible){ 
        lastTapped = 0;
    };

    void draw(void){
	    tft->fillRect(coord_x,coord_y,real_width,real_len,color);
        tft->textMode();
        tft->textSetCursor(coord_x + (real_width/5), coord_y + (real_len/2)-5);
        tft->textColor(RA8875_BLACK, color);
        tft->textEnlarge(0);
        tft->textWrite(button_str);
        tft->graphicsMode();
    }


    bool isTapped(){
		uint16_t x = tft->touch_points[0];
		uint16_t y = tft->touch_points[1];
	    if ((x >= coord_x) && (x <= (coord_x + real_width))){
		    if ((y >= coord_y) && (y <= (coord_y + real_len))){
				    return true;
		    }
	    }
	    return false;	
    }
};

class TextBox : public ScreenElement {
private:
    int background_color;
    int text_color;
    int text_size;
    const char* str;

public:
    bool visible;
	bool outline;

    TextBox(int row, int column, int len, int width, int background_color, int text_color, int text_size, bool visible, bool outline, const char* str, Display* tft):ScreenElement(row,column,len,width,tft), background_color(background_color), text_color(text_color), text_size(text_size), str(str), visible(visible), outline(outline) { };

    void draw(void){
        tft->textMode();
        tft->textSetCursor(coord_x,coord_y);
	    tft->textColor(text_color,background_color);
        tft->textEnlarge(1);
        tft->textWrite(str);
        tft->graphicsMode();
		if (outline) {
			tft->drawRect(coord_x, coord_y, real_width, real_len, text_color);
		}
    }

};


/*
 * Special note for LargeNumberView:
 *
 * Requires 3 grid spaces of width to properly display three digit numbers.
 */
class LargeNumberView : public ScreenElement {
private:
    int background_color;
    int text_color;
	int value;

public:
	bool visible;
    LargeNumberView(int row, int column, int len, int width, int background_color, int text_color, bool visible, int value, Display* tft):ScreenElement(row,column,len,width,tft), background_color(background_color), text_color(text_color), value(value), visible(visible) { };

	void draw(void) {
		int first_digit = value / 100;
		if (first_digit != 0) {
			tft->printLarge(first_digit, coord_x, coord_y, text_color, background_color);
		}
		int second_digit = value / 10 - first_digit * 10;
		if (second_digit != 0) {
			tft->printLarge(second_digit, coord_x+tft->horizontal_scale, coord_y, text_color, background_color);
		}
		int third_digit = value - first_digit * 100 - second_digit * 10;
		tft->printLarge(third_digit, coord_x+(tft->horizontal_scale * 2), coord_y, text_color, background_color);
	}

};

class SignalTrace : public ScreenElement {
private:
    CircleBuffer<int>* displayData;
    int background_color;
    int trace_color;
    int display_cursor = 0;
    int last_val;
    int max_signal_value = 4096;

public:
    SignalTrace(int row, int column, int len, int width, int background_color, int trace_color, 
                int max_signal_value, CircleBuffer<int>* displayData, Display* tft):
                ScreenElement(row,column,len,width,tft), displayData(displayData), background_color(background_color), 
                trace_color(trace_color), max_signal_value(max_signal_value) {}
        
    void draw(void) {
        tft->fillRect(coord_x,coord_y,real_width,real_len,background_color);
	    tft->drawRect(coord_x,coord_y,real_width,real_len,trace_color);
    }

    void update(void) {
		uint32_t prim = __get_PRIMASK();
		__disable_irq();
		int new_val = displayData->newest();
		if (!prim) { 
			__enable_irq();
		}
		int threshold = 5;
		int display = new_val * real_len;
		display /= max_signal_value;
		if (display > real_len) { display = real_len; }
		else if (display < 0) { display = 0; }
		int old_display = last_val * real_len;
		old_display /= max_signal_value;
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
