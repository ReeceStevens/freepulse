#include "System.h"
#include "Display.h"

#include "interface.h"
#include "Signals.h"

Console c(USART2, 115200);
Screen mainScreen = Screen();
Screen settingsScreen = Screen();

extern "C" void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		int val = ecg.read();
		/* c.print(val); */
		/* c.print("\n"); */
	}
}

void MainScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
  tft.showGrid();
  mainScreen.initialDraw();
}

void SettingsScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
  tft.showGrid();
  settingsScreen.initialDraw();
}


void systemInit() {
	adcInit();
  	tft.startup();
    composeMainScreen(mainScreen);
    composeSettingsScreen(settingsScreen);
    connectSignalsToScreen(mainScreen);
    enableSignalAcquisition();
}

int main(void)
{
	c.configure();
	c.print("\n");
	c.print("Starting FreePulse...\n");
	systemInit();
	c.print("Welcome!\n");
	while (1) {
		MainScreenInit();
		delay(1000);
		tft.read_touch();
		tft.reset_touch();
		while (currentMode == home) {
			while (digitalRead(tft.interrupt)){
                mainScreen.update();
				delay(1000);
			}
            delay(1000);
			tft.read_touch();
            c.print("x: ");
            c.print(tft.touch_points[0]);
            c.print(", y: ");
            c.print(tft.touch_points[1]);
            c.print("\n");
            mainScreen.listenForTouch();
			tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
		}
		if (currentMode == settings) {
			SettingsScreenInit();
			delay(1000);
			tft.read_touch();
			tft.reset_touch();
			while (currentMode == settings) {
				while (digitalRead(tft.interrupt)){}
				tft.read_touch();
                settingsScreen.listenForTouch();
				tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
			}
		}
    }
    return 0; 
}
