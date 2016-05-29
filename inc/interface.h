#ifndef _interface_h_
#define _interface_h_
#include "Adafruit_RA8875.h"

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
    Adafruit_RA8875* tft_interface;

    ScreenElement(int row, int column, int len, int width, Adafruit_RA8875* tft_interface):row(row),column(column),len(len),width(width),tft_interface(tft_interface){
        coord_x = horizontal_scale*(column-1);
        coord_y = vertical_scale*(row-1);
        real_len = vertical_scale*len;
        real_width = horizontal_scale*width;
    }
};



/*
 * TODO: Write a function that uses the button_str argument
 * and creates a button label. Center the label based on the size
 * of the button.
 */
class Button : public ScreenElement{
private:
    int color;
    char* button_str;

public:
    bool visible;
    bool lastTapped;

    Button(int row, int column, int len, int width, int color, char* button_str, bool visible, Adafruit_RA8875* tft):ScreenElement(row,column,len,width,tft), color(color), button_str(button_str),visible(visible){ 
        lastTapped = 0;
    };

    void draw(void){
	    tft_interface->fillRect(coord_x,coord_y,real_width,real_len,color);
        tft_interface->textMode();
        //tft_interface->textSetCursor(coord_x, coord_y);
        tft_interface->textSetCursor(coord_x + (real_width/5), coord_y + (real_len/2)-5);
        tft_interface->textColor(RA8875_BLACK, color);
        tft_interface->textEnlarge(1);
        tft_interface->textWrite(button_str);
        tft_interface->graphicsMode();
    }


    bool isTapped(uint16_t raw_x, uint16_t raw_y){
	    // Weirdness is happening right now where x and y axes are switched
	    // Once this is fixed in the main script, fix this function too!
        uint16_t x = (uint16_t) raw_x / xScale; 
        uint16_t y = (uint16_t) raw_y / yScale; 
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
    char* str;

public:
    bool visible;

    TextBox(int row, int column, int len, int width, int background_color, int text_color, int text_size, bool visible, char* str, Adafruit_RA8875* tft):ScreenElement(row,column,len,width,tft), background_color(background_color), text_color(text_color), text_size(text_size), str(str), visible(visible) { };

    void draw(void){
        tft_interface->textMode();
        //tft_interface->textSetCursor(coord_x+5,coord_y+5);
        tft_interface->textSetCursor(coord_x,coord_y);
	    tft_interface->textColor(text_color,background_color);
        //tft_interface->textEnlarge(text_size);
        tft_interface->textEnlarge(1);
        //tft_interface->setCursor(coord_x+5,coord_y+5);
        tft_interface->textWrite(str);
        tft_interface->graphicsMode();
    }

};
#endif
