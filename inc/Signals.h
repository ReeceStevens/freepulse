#include "Interface.h"
#include "ecg.h"

ECG ecg = ECG(2,1,3,7,PB0,4096,RA8875_BLUE,RA8875_LIGHTGREY,1000,tim3,&tft);

void enableSignalAcquisition(void) {
    ecg.enable();
}

void connectSignalsToScreen(Screen& s) {
    s.add(ecg.signalTrace);
}
