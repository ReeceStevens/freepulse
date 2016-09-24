#include "System.h"
#include "Display.h"

#include "interface.h"
#include "Signals.h"

const int SHORT_DELAY = 1000;

extern Display tft;

Console c(USART2, 115200);
Screen mainScreen = Screen(&tft);
Screen settingsScreen = Screen(&tft);

extern "C" void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
		int val = ecg.read();
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

extern "C" void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        /* GPIO_SetBits(GPIOG, GPIO_Pin_13 | GPIO_Pin_14); */
        spo2.sample();
        /* GPIO_ResetBits(GPIOG, GPIO_Pin_13 | GPIO_Pin_14); */
        /* c.print("Interrupt"); */
        EXTI_ClearITPendingBit(EXTI_Line8);
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

void initDevelopmentHeartbeat(void) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    GPIO_InitTypeDef GPIO_InitDef;
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
    GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitDef);
}

void systemInit() {
    /* initDevelopmentHeartbeat(); */
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
            mainScreen.readTouch();
			tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
		}
		if (currentMode == settings) {
			SettingsScreenInit();
			delay(SHORT_DELAY);
            tft.clearTouchEvents();
			while (currentMode == settings) {
                settingsScreen.update(SHORT_DELAY);
                settingsScreen.readTouch();
				tft.drawPixel(tft.touch_points[0],tft.touch_points[1], RA8875_WHITE);
			}
		}
    }
    return 0;
}
