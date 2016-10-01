#include "interface.h"
#include "ecg.h"
#include "nibp.h"

ECG ecg = ECG(1,1,4,9,PB0,4096,RA8875_BLUE,RA8875_BLACK,1000,tim3,&tft);
NIBPReadout nibp = NIBPReadout(5,1,4,4,PB1,PA0,RA8875_GREEN,RA8875_BLACK,500,tim4,&tft);

void enableSignalAcquisition(void) {
    ecg.enable();
    nibp.enable();
}

void connectSignalsToScreen(Screen& s) {
    s.add(ecg.signalTrace);
    s.add((ScreenElement*) &nibp);
}
