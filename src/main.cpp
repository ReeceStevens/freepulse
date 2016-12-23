#include "System.h"
#include "Display.h"

Console c(USART1, 115200);
#include "interface.h"
#include "Signals.h"

const int SHORT_DELAY = 1000;

extern Display tft;

Screen mainScreen = Screen(&tft);
Screen settingsScreen = Screen(&tft);

extern "C" void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
		int val = ecg.read();
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

extern "C" void TIM4_IRQHandler(void) {
	if (TIM_GetITStatus (TIM4, TIM_IT_Update) != RESET) {
		if(nibp.can_sample()) {nibp.read();}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

extern "C" void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        /* spo2.self_check(); */
        if (spo2.can_sample()) {
            /* spo2.sample(); */
            c.print(spo2.sample());
            c.print("\n");
        }
        EXTI_ClearITPendingBit(EXTI_Line14);
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
	/* c.print("Starting FreePulse...\n"); */
	systemInit();
	/* c.print("Welcome!\n"); */
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
