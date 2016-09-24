#include "interface.h"
#include "ecg.h"
#include "AFE4400.h"

SPI_Interface afe4400_spi(spi_c2);

ECG ecg = ECG(1,1,4,9,PB0,4096,RA8875_BLUE,RA8875_BLACK,1000,tim3,&tft);
PulseOx spo2 = PulseOx(6,1,3,3,&afe4400_spi,PC8,PA9,PA8,PB7,&tft);

void enableSignalAcquisition(void) {
    spo2.enable();
    ecg.enable();
}

void connectSignalsToScreen(Screen& s) {
    s.add(ecg.signalTrace);
    s.add((ScreenElement*) &spo2);
}
