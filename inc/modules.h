#include "interface.h"
#include "ecg.h"
#include "nibp.h"
#include "AFE4400.h"
#include "Sampler.h"


/* PulseOx spo2 = PulseOx(1,1,4,9,&afe4400_spi,PA8,PB5,PA14,PA13,&tft); */
/* NIBPReadout nibp = NIBPReadout(5,1,4,4,PB1,PA0,PA4,RA8875_BLUE,RA8875_BLACK,500,tim4,&tft); */

SPI_Interface afe4400_spi(spi_c4);
AFE4400* pulse_ox = new AFE4400(&afe4400_spi, PA8, PB5, PA14, PA13);

uint32_t sample_pulse_ox_ir(void) {
    return pulse_ox->getLED1Data();
}
Sampler* spo2_ir_sampler = new Sampler(&sample_pulse_ox_ir, PA14, 3000);

uint32_t sample_pulse_ox_red(void) {
    return pulse_ox->getLED2Data();
}
Sampler* spo2_red_sampler = new Sampler(&sample_pulse_ox_red, PA14, 3000);

extern "C" void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        spo2_ir_sampler->sample();
        spo2_red_sampler->sample();
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}

/* void enableSignalAcquisition(void) { */
/*     spo2.enable(); */
/*     nibp.enable(); */
/* } */

/* void connectSignalsToScreen(Screen& s) { */
/*     s.add((ScreenElement*) &spo2); */
    /* s.add((ScreenElement*) &nibp); */
/* } */
