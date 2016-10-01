#ifndef _screen_element_h_
#define _screen_element_h_

#include "System.h"
#include "Display.h"
#include "CircleBuffer.h"
#include "Vector.h"


class ScreenElement {
protected:
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
    bool changed = false;

public:
    bool visible;
    bool lastTapped;
    void (*reducer) (void);

    Button(int row, int column, int len, int width, int color, const char* button_str, bool visible, Display* tft, void (*reducer) (void)):
		ScreenElement(row,column,len,width,tft), color(color), button_str(button_str),visible(visible),reducer(reducer){ 
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

    void update(){
        if (changed) { draw(); changed = false; }
		uint16_t x = tft->touch_points[0];
		uint16_t y = tft->touch_points[1];
	    if ((x >= coord_x) && (x <= (coord_x + real_width))){
		    if ((y >= coord_y) && (y <= (coord_y + real_len))){
                    reducer();
		    }
	    }
    }

    bool isTapped() {
		uint16_t x = tft->touch_points[0];
		uint16_t y = tft->touch_points[1];
	    if ((x >= coord_x) && (x <= (coord_x + real_width))){
		    if ((y >= coord_y) && (y <= (coord_y + real_len))){
                    return true;
		    }
	    }
        return false;
    }

    void changeColor(int new_background_color) {
        color = new_background_color;
        changed = true;
    }

    void changeText(const char* new_text) {
        button_str = new_text;
        changed = true;
    }
};

class TextBox : public ScreenElement {
private:
    int background_color;
    int text_color;
    int text_size;
    const char* str;
    bool changed = false;
    bool should_deallocate_before_reassignment = false;

public:
    bool visible;
	bool outline;

    TextBox(int row, int column, int len, int width, int background_color, int text_color, int text_size, bool visible, bool outline, const char* str, Display* tft):ScreenElement(row,column,len,width,tft), background_color(background_color), text_color(text_color), text_size(text_size), str(str), visible(visible), outline(outline) { };

    void draw(void){
        tft->textMode();
        tft->textSetCursor(coord_x,coord_y);
	    tft->textColor(text_color,background_color);
        tft->textEnlarge(1);
        tft->textWrite(" ");
        tft->textWrite(str);
        tft->graphicsMode();
		if (outline) {
			tft->drawRect(coord_x, coord_y, real_width, real_len, text_color);
		}
    }

    void update(void) {
        if (changed) { draw(); changed = false; }
    }

    void changeText(const char* new_text) {
        if (should_deallocate_before_reassignment) { delete [] str; }
        should_deallocate_before_reassignment = false;
        str = new_text;
        changed = true;
    }

    void changeText(int numerical_val) {
        if (should_deallocate_before_reassignment) { delete [] str; }
        should_deallocate_before_reassignment = true;
        char* result = new char[10]; 
        sprintf(result, "%d", numerical_val);
        const char* new_str = result;
        str = new_str;
        changed = true;
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
    bool has_updated = false;

public:
	bool visible;
    LargeNumberView(int row, int column, int len, int width, int background_color, int text_color, bool visible, int value, Display* tft):ScreenElement(row,column,len,width,tft), background_color(background_color), text_color(text_color), value(value), visible(visible) { };

	void draw(void) {
		int first_digit = value / 100;
		if (first_digit != 0) {
			tft->printLarge(first_digit, coord_x, coord_y, text_color, background_color);
		} else {
            tft->fillRect(coord_x, coord_y, tft->horizontal_scale, tft->vertical_scale*2, background_color);
        }
		int second_digit = value / 10 - first_digit * 10;
        if ((first_digit != 0) || (second_digit != 0)) {
			tft->printLarge(second_digit, coord_x+tft->horizontal_scale, coord_y, text_color, background_color);
        } else {
            tft->fillRect(coord_x+tft->horizontal_scale, coord_y, tft->horizontal_scale, tft->vertical_scale*2, background_color);
        }
		int third_digit = value - first_digit * 100 - second_digit * 10;
		tft->printLarge(third_digit, coord_x+(tft->horizontal_scale * 2), coord_y, text_color, background_color);
	}

    void update(void) {
        if (has_updated) {
            draw();
            has_updated = false;
        }
    }

    void changeNumber(int new_value) {
        value = new_value;
        has_updated = true;
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

class Sandbox : public ScreenElement {
private:
    Vector<ScreenElement*> elements;
    int last_size = 0;
    int background_color = RA8875_BLACK;

public:
    Sandbox(int row, int column, int len, int width, int background_color, Display* tft):
      ScreenElement(row,column,len,width,tft), background_color(background_color) {}

    void add(ScreenElement* a) {
        this->elements.push_back(a); 
    }

    void empty() {
        int size = this->elements.size();
        for (int i = 0; i < size; i ++) {
            delete this->elements[i];
        }
    }
   
    void draw() {
        int size = this->elements.size();
        for (int i = 0; i < size; i ++) {
            this->elements[i]->draw();
        }
    }

    void update() {
        int size = this->elements.size();
        for (int i = 0; i < size; i ++) {
            this->elements[i]->update();
        }
    }

};

#endif
