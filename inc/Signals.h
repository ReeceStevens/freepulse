#include "interface.h"
#include "ecg.h"
#include "nibp.h"
#include "AFE4400.h"

SPI_Interface afe4400_spi(spi_c4);

ECG ecg = ECG(1,1,4,9,PB0,4096,RA8875_BLUE,RA8875_BLACK,1000,tim3,&tft);
PulseOx spo2 = PulseOx(5,1,4,9,&afe4400_spi,PA8,PB5,PA14,PA13,&tft);
NIBPReadout nibp = NIBPReadout(9,1,4,4,PB1,PA0,PA4,RA8875_GREEN,RA8875_BLACK,500,tim4,&tft);

void enableSignalAcquisition(void) {
    spo2.enable();
    ecg.enable();
    nibp.enable();
}

void connectSignalsToScreen(Screen& s) {
    s.add(ecg.signalTrace);
    s.add((ScreenElement*) &spo2);
    s.add((ScreenElement*) &nibp);
}
