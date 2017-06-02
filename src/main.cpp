#include "Logger.h"
#include "Timer.h"
#include "Display.h"
#include "Utils.h"
#include "modules.h"
#include "interface.h"

const int SHORT_DELAY = 1000;

extern Display tft;

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


void initDevelopmentHeartbeat(void) {
#ifdef DEBUG
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    GPIO_InitTypeDef GPIO_InitDef;
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
    GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitDef);
#endif
}

void systemInit() {
    initDevelopmentHeartbeat();
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
        asm("wfi"); // TODO Make sure this is the right ASM instruction.
    }
    return 0;
}
