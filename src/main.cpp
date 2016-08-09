#include "System.h"
#include "Display.h"

#include "Interface.h"
#include "Signals.h"

Console c(USART2, 115200);

extern "C" void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		int val = ecg.read();
		c.print(val);
		c.print("\n");
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
    connectSignalsToInterface();
    enableSignalADCs();
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
