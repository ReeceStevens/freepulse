#include "interface.h"
#include "ecg.h"
#include "nibp.h"
#include "AFE4400.h"

SPI_Interface afe4400_spi(spi_c4);

PulseOx spo2 = PulseOx(1,1,4,9,&afe4400_spi,PA8,PB5,PA14,PA13,&tft);
NIBPReadout nibp = NIBPReadout(5,1,4,4,PB1,PA0,PA4,RA8875_BLUE,RA8875_BLACK,500,tim4,&tft);

void enableSignalAcquisition(void) {
    spo2.enable();
    nibp.enable();
}

void connectSignalsToScreen(Screen& s) {
    s.add((ScreenElement*) &spo2);
    s.add((ScreenElement*) &nibp);
}
