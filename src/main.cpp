#include "System.h"
#include "Display.h"

#include "Interface.h"
#include "Signals.h"

const int SHORT_DELAY = 1000;

extern Display tft;

Console c(USART2, 115200);
Screen mainScreen = Screen(&tft);
Screen settingsScreen = Screen(&tft);

extern "C" void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		int val = ecg.read();
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
		delay(SHORT_DELAY);
        tft.clearTouchEvents();
		while (currentMode == home) {
            mainScreen.update(SHORT_DELAY);
            mainScreen.propogateTouch();
			tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
		}
		if (currentMode == settings) {
			SettingsScreenInit();
			delay(SHORT_DELAY);
            tft.clearTouchEvents();
			while (currentMode == settings) {
                settingsScreen.update(SHORT_DELAY);
                settingsScreen.propogateTouch();
				tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
			}
		}
    }
    return 0; 
}
