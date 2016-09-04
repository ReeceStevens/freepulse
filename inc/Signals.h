#include "Interface.h"
#include "ecg.h"

ECG ecg = ECG(1,1,4,9,PB0,4096,RA8875_BLUE,RA8875_BLACK,1000,tim3,&tft);

void enableSignalAcquisition(void) {
    ecg.enable();
}

void connectSignalsToScreen(Screen& s) {
    s.add(ecg.signalTrace);
}
