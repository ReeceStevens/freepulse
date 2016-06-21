#include "System.h"
#include "Display.h"

#define DISPLAY_CS PC4
#define DISPLAY_RESET PC5
#define DISPLAY_SPI spi_c1

Display tft(DISPLAY_CS, DISPLAY_RESET, DISPLAY_SPI);

const int s_width = 800;
const int s_height = 480;

const float xScale = 1024.0F/s_width;
const float yScale = 1024.0F/s_height;

// Defining screen proportions
const int rows = 10;
const int columns = 10;

const int vertical_scale = s_height / rows;
const int horizontal_scale = s_width / columns;

#include "interface.h"
#include "ecg_revised.h"

int currentMode = 0; // Change mode

/* Build UI Buttons */
Button settings = Button(9,9,2,2,RA8875_RED,"Alarm Settings",true,&tft);
Button confirm_button = Button(9,1,2,2,RA8875_GREEN,"Confirm",true,&tft);
Button cancel_button = Button(9,9,2,2,RA8875_RED,"Cancel",true,&tft);
Button default_button = Button(6,7,2,2,RA8875_LIGHTGREY,"Default Settings",true,&tft);
TextBox title = TextBox(1,3,1,3,RA8875_BLACK,RA8875_WHITE,3,true,"FreePulse Patient Monitor v0.9", &tft);
ECGReadout ecg = ECGReadout(2,1,3,8,15,RA8875_BLUE,RA8875_LIGHTGREY,&tft);
TextBox heartrate = TextBox(2,9,3,2,RA8875_BLACK,RA8875_WHITE,4,true,"60", &tft);

/*
 * showGrid() - 
 * DEVELOPMENT FUNCTION ONLY.
 * Draw gridlines for interface.
 */
void showGrid(void){
    for (int i = 1; i < rows; i += 1) {
        tft.drawLine(1,i*vertical_scale,s_width-1,i*vertical_scale,RA8875_LIGHTGREY);
    }
    for (int i = 1; i < columns; i += 1) {
        tft.drawLine(i*horizontal_scale,1,i*horizontal_scale,s_height-1,RA8875_LIGHTGREY);
    }
}

void MainScreenInit(void){
  tft.clearScreen(RA8875_BLACK);
  showGrid();
  title.draw();
  settings.draw();
  ecg.draw();
  heartrate.draw();
  //ecg2.draw();
  //ecg3.draw();
}

void SettingsScreenInit(void){
  tft.clearScreen(RA8875_BLACK);
  confirm_button.draw();
  default_button.draw();
  cancel_button.draw();
  showGrid();
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

int main(void)
{
	uint16_t* tp = new uint16_t[2];
	uint16_t* entry_tp = new uint16_t[2];
	char* analog_data = new char[50];
  	tft.startup();
	ecg.configure();
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
			tft.fillRect(290, 200, 330, 230, RA8875_BLACK);
			tft.textMode();
			tft.textSetCursor(300,200);
			tft.textColor(RA8875_WHITE, RA8875_BLACK);
			tft.textEnlarge(1);
			uint16_t analog_in_data = ECGReadout::analogRead();
			sprintf(analog_data, "%d", analog_in_data);
			tft.textWrite(analog_data);
			tft.graphicsMode();
		}
			tft.read_touch();	
			if (settings.isTapped(tp[0],tp[1])){
				tft.clearScreen(RA8875_BLACK);
				currentMode = alarms;
				entry_tp[0] = tp[0];
				entry_tp[1] = tp[1];
			}
			tft.drawPixel(tp[0],tp[1], RA8875_WHITE);
	}
	if (currentMode == alarms) {
        SettingsScreenInit();
		delay(PAUSE_SHORT);
		tft.read_touch();
		tft.reset_touch();
		while (currentMode == alarms) {
			while (digitalRead(tft.interrupt)){}
			tft.read_touch();
			tft.drawPixel(tp[0],tp[1], RA8875_WHITE);
            if (cancel_button.isTapped(tp[0],tp[1])) {
                tft.clearScreen(RA8875_BLACK);
                currentMode = home;
            }
		}
    }
    }
    return 0; 
}
