#ifndef __interface_h__
#define __interface_h__

#include "screen.h"
#include "ScreenElement.h"

#define DISPLAY_CS PC4
#define DISPLAY_RESET PC5
#define DISPLAY_WAIT PB6
#define DISPLAY_SPI spi_c1

#define SCREEN_ROWS 12
#define SCREEN_COLUMNS 12

enum layout{
	home, settings	
};

SPI_Interface display_spi(DISPLAY_SPI);
Display tft(DISPLAY_CS, DISPLAY_RESET, DISPLAY_WAIT, &display_spi, SCREEN_ROWS, SCREEN_COLUMNS);

layout currentMode = home;

void openSettings(void) {
    currentMode = settings;
};

void saveAlarmChanges(void) {
    currentMode = home;
};

void cancelAlarmChanges(void) {
    currentMode = home;
};

void restoreDefaultAlarms(void) {};

Button settings_button = Button(11,11,2,2,RA8875_RED,"Alarm Settings",
                         true,&tft,&openSettings);
Button confirm_button = Button(11,9,2,2,RA8875_GREEN,"Confirm",
                        true,&tft,&saveAlarmChanges);
Button cancel_button = Button(5,9,2,2,RA8875_RED,"Cancel",
                        true,&tft,&cancelAlarmChanges);
Button default_button = Button(6,7,2,2,RA8875_LIGHTGREY,"Default Settings",
                        true,&tft,&restoreDefaultAlarms);
TextBox title = TextBox(9,10,3,4,RA8875_BLACK,RA8875_WHITE,3,true,false,
                        "  FreePulse", &tft);
TextBox version = TextBox(10,11,3,4,RA8875_BLACK,RA8875_WHITE,3,true,false,
                        "   v1.0", &tft);

TextBox hr_label = TextBox(1,10,4,3,RA8875_BLACK,RA8875_GREEN,2,true,
                            true,"BPM",&tft);
TextBox spo2_label = TextBox(5,10,4,3,RA8875_BLACK,RA8875_BLUE,2,true,
                            true,"SpO2",&tft);
LargeNumberView heartrate = LargeNumberView(3,10,2,3,RA8875_BLACK,
                            RA8875_GREEN,true,60,&tft);

void composeMainScreen(Screen& s) {
    /* s.add(&settings_button); */
    s.add(&title);
    s.add(&version);
    s.add(&hr_label);
    s.add(&spo2_label);
    s.add(&heartrate);
    /* mainScreen.add(ecg.signalTrace); */
}

void composeSettingsScreen(Screen& s) {
    /* s.add(&confirm_button); */
    /* s.add(&default_button); */
    /* s.add(&cancel_button); */
}

#endif
