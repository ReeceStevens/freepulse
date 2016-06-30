#ifndef _interface_h_
#define _interface_h_

#include "System.h"
#include "Display.h"

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

    ScreenElement(int row, int column, int len, int width, Display* tft):
		row(row),column(column),len(len),width(width),tft(tft){
        coord_x = tft->horizontal_scale*(column-1);
        coord_y = tft->vertical_scale*(row-1);
        real_len = tft->vertical_scale*len;
        real_width = tft->horizontal_scale*width;
    }
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

    TextBox(int row, int column, int len, int width, int background_color, int text_color, int text_size, bool visible, const char* str, Display* tft):ScreenElement(row,column,len,width,tft), background_color(background_color), text_color(text_color), text_size(text_size), str(str), visible(visible) { };

    void draw(void){
        tft->textMode();
        tft->textSetCursor(coord_x,coord_y);
	    tft->textColor(text_color,background_color);
        tft->textEnlarge(1);
        tft->textWrite(str);
        tft->graphicsMode();
    }

};
#endif
