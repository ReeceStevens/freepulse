#ifndef __interface_h__
#define __interface_h__

#include "Screen.h"
#include "ScreenElement.h"

#define DISPLAY_CS PC4
#define DISPLAY_RESET PC5
#define DISPLAY_SPI spi_c1

#define SCREEN_ROWS 10
#define SCREEN_COLUMNS 10

SPI_Interface display_spi(DISPLAY_SPI);
Display tft(DISPLAY_CS, DISPLAY_RESET, &display_spi, SCREEN_ROWS, SCREEN_COLUMNS);

Screen mainScreen = Screen();
Button settings = Button(9,9,2,2,RA8875_RED,"Alarm Settings",true,&tft);
Button record = Button(5,9,2,2,RA8875_BLUE,"Data to Serial",true,&tft);
Button confirm_button = Button(9,1,2,2,RA8875_GREEN,"Confirm",true,&tft);
Button cancel_button = Button(9,9,2,2,RA8875_RED,"Cancel",true,&tft);
Button default_button = Button(6,7,2,2,RA8875_LIGHTGREY,"Default Settings",true,&tft);
TextBox title = TextBox(1,3,1,3,RA8875_BLACK,RA8875_WHITE,3,true,false,"FreePulse Patient Monitor v0.9", &tft);
TextBox hr_label = TextBox(2,8,3,3,RA8875_BLACK,RA8875_BLUE,2,true,true,"BPM", &tft);
LargeNumberView heartrate = LargeNumberView(3,8,2,3,RA8875_BLACK,RA8875_BLUE,true,60,&tft);

void composeInterface(void) {
    mainScreen.add(&settings);
    mainScreen.add(&title);
    mainScreen.add(&hr_label);
    mainScreen.add(&heartrate);
    /* mainScreen.add(ecg.signalTrace); */
}

#endif
