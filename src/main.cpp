#include "System.h"
#include "Display.h"

#include "screen.h"
#include "interface.h"
#include "ecg.h"

#define DISPLAY_CS PC4
#define DISPLAY_RESET PC5
#define DISPLAY_SPI spi_c1

#define SCREEN_ROWS 10
#define SCREEN_COLUMNS 10

SPI_Interface display_spi(DISPLAY_SPI);
Display tft(DISPLAY_CS, DISPLAY_RESET, &display_spi, SCREEN_ROWS, SCREEN_COLUMNS);

int currentMode = 0; // Change mode

Console c(USART2, 115200);

/* Build UI Buttons */
Screen mainScreen = Screen();
Button settings = Button(9,9,2,2,RA8875_RED,"Alarm Settings",true,&tft);
Button record = Button(5,9,2,2,RA8875_BLUE,"Data to Serial",true,&tft);
Button confirm_button = Button(9,1,2,2,RA8875_GREEN,"Confirm",true,&tft);
Button cancel_button = Button(9,9,2,2,RA8875_RED,"Cancel",true,&tft);
Button default_button = Button(6,7,2,2,RA8875_LIGHTGREY,"Default Settings",true,&tft);
TextBox title = TextBox(1,3,1,3,RA8875_BLACK,RA8875_WHITE,3,true,false,"FreePulse Patient Monitor v0.9", &tft);
TextBox hr_label = TextBox(2,8,3,3,RA8875_BLACK,RA8875_BLUE,2,true,true,"BPM", &tft);
LargeNumberView heartrate = LargeNumberView(3,8,2,3,RA8875_BLACK,RA8875_BLUE,true,60,&tft);

ECGReadout ecg = ECGReadout(2,1,3,7,PB0,4096,RA8875_BLUE,RA8875_LIGHTGREY,1000,tim3,&tft);

extern "C" void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		int val = ecg.read();
		c.print(val);
		c.print("\n");
	}
}

void composeInterface(void) {
    mainScreen.add(&settings);
    mainScreen.add(&title);
    mainScreen.add(&hr_label);
    mainScreen.add(&heartrate);
    mainScreen.add(ecg.signalTrace);
}

void MainScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
  tft.showGrid();
  mainScreen.initialDraw();
  /* title.draw(); */
  /* settings.draw(); */
  /* ecg.draw(); */
  /* heartrate.draw(); */
  /* hr_label.draw(); */
}

void SettingsScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
  tft.showGrid();
  confirm_button.draw();
  default_button.draw();
  cancel_button.draw();
}

enum layout{
	home, alarms	
};

void systemInit() {
	adcInit();
  	tft.startup();
    composeInterface();
	ecg.enable();
}

int main(void)
{
	c.configure();
	c.print("\n");
	c.print("Starting FreePulse...\n");
  	layout currentMode = home;
	systemInit();
	c.print("Welcome!\n");
	while (1) {
		MainScreenInit();
		delay(1000);
		tft.read_touch();
		tft.reset_touch();
		while (currentMode == home) {
			while (digitalRead(tft.interrupt)){
				/* ecg.display_signal(); */
                mainScreen.update();
				delay(100);
			}
			tft.read_touch();	
			if (settings.isTapped()){
				tft.fillScreen(RA8875_BLACK);
				currentMode = alarms;
			}
			tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
		}
		if (currentMode == alarms) {
			SettingsScreenInit();
			delay(1000);
			tft.read_touch();
			tft.reset_touch();
			while (currentMode == alarms) {
				while (digitalRead(tft.interrupt)){}
				tft.read_touch();
				tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
				if (cancel_button.isTapped()) {
					tft.fillScreen(RA8875_BLACK);
					currentMode = home;
				}
			}
		}
    }
    return 0; 
}
