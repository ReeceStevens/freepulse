#include "System.h"
#include "Display.h"

#include "interface.h"
#include "ecg_revised.h"

#define DISPLAY_CS PC4
#define DISPLAY_RESET PC5
#define DISPLAY_SPI spi_c1

#define SCREEN_ROWS 10
#define SCREEN_COLUMNS 10
Display tft(DISPLAY_CS, DISPLAY_RESET, DISPLAY_SPI, SCREEN_ROWS, SCREEN_COLUMNS);


int currentMode = 0; // Change mode

/* Build UI Buttons */
Button settings = Button(9,9,2,2,RA8875_RED,"Alarm Settings",true,&tft);
Button confirm_button = Button(9,1,2,2,RA8875_GREEN,"Confirm",true,&tft);
Button cancel_button = Button(9,9,2,2,RA8875_RED,"Cancel",true,&tft);
Button default_button = Button(6,7,2,2,RA8875_LIGHTGREY,"Default Settings",true,&tft);
TextBox title = TextBox(1,3,1,3,RA8875_BLACK,RA8875_WHITE,3,true,"FreePulse Patient Monitor v0.9", &tft);
ECGReadout ecg = ECGReadout(2,1,3,8,PB0,RA8875_BLUE,RA8875_LIGHTGREY,1000,tim3,&tft);
TextBox heartrate = TextBox(2,9,3,2,RA8875_BLACK,RA8875_WHITE,4,true,"60", &tft);


void MainScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
  tft.showGrid();
  title.draw();
  settings.draw();
  ecg.draw();
  heartrate.draw();
  //ecg2.draw();
  //ecg3.draw();
}

void SettingsScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
  confirm_button.draw();
  default_button.draw();
  cancel_button.draw();
  tft.showGrid();
}

extern "C" void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		ecg.read();
	}
}

enum layout{
	home, alarms	
};

void systemInit(void) {
	adcInit();
}

int main(void)
{
	/* uint16_t* tp = new uint16_t[2]; */
	/* uint16_t* entry_tp = new uint16_t[2]; */
	/* char* analog_data = new char[50]; */
	systemInit();
  	tft.startup();
	delay(PAUSE_LONG);
  	layout currentMode = home;
	while (1) {
    MainScreenInit();
	delay(PAUSE_SHORT);
	tft.read_touch();
	tft.reset_touch();
	while (currentMode == home) {
		while (digitalRead(tft.interrupt)){
			ecg.display_signal();
		}
			tft.read_touch();	
			if (settings.isTapped(tft.touch_points[0],tft.touch_points[1])){
				tft.fillScreen(RA8875_BLACK);
				currentMode = alarms;
				/* entry_tp[0] = tft.touch_points[0]; */
				/* entry_tp[1] = tft.touch_points[1]; */
			}
			tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
	}
	if (currentMode == alarms) {
        SettingsScreenInit();
		delay(PAUSE_SHORT);
		tft.read_touch();
		tft.reset_touch();
		while (currentMode == alarms) {
			while (digitalRead(tft.interrupt)){}
			tft.read_touch();
			tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
            if (cancel_button.isTapped(tft.touch_points[0],tft.touch_points[1])) {
                tft.fillScreen(RA8875_BLACK);
                currentMode = home;
            }
		}
    }
    }
    return 0; 
}
