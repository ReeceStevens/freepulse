#include "Logger.h"
#include "Timer.h"
#include "Display.h"
#include "Utils.h"
#include "interface.h"
#include "Signals.h"

volatile uint32_t pulse_clock = 0;
const int SHORT_DELAY = 1000;

extern Display tft;

Screen mainScreen = Screen(&tft);
Screen settingsScreen = Screen(&tft);

extern "C" void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
        /* int val = ecg.read(); */
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

extern "C" void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus (TIM4, TIM_IT_Update) != RESET) {
        if(nibp.can_sample()) {
            nibp.read();
        }
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

extern "C" void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        /* spo2.self_check(); */
        if (spo2.can_sample()) {
            spo2.sample();
            if (spo2.tally_samples) spo2.num_samples += 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}

extern "C" void SysTick_Handler(void) {
    pulse_clock += 1;
}

void MainScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
  mainScreen.initialDraw();
}

void SettingsScreenInit(void){
  tft.fillScreen(RA8875_BLACK);
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
    SysTick_Config(8400);
    initialize_ADCs();
    logger(l_info, "System timers initialized\n");
    tft.startup();
    logger(l_info, "Display initialized\n");
    composeMainScreen(mainScreen);
    composeSettingsScreen(settingsScreen);
    connectSignalsToScreen(mainScreen);
    logger(l_info, "Interface initialized\n");
    enableSignalAcquisition();
    logger(l_info, "Sampling modules started\n");
}

int main(void)
{
    logger(l_info, "Starting up...\n");
    systemInit();
    logger(l_info, "Welcome to FreePulse!\n");
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
