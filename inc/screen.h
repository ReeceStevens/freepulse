#ifndef __Screen_h__
#define __Screen_h__

#include "Utils.h"
#include "Vector.h"
#include "ScreenElement.h"

class Screen {
private:
    Vector<ScreenElement*> elements;
    Vector<Button*> buttons;
    uint16_t background_color = RA8875_BLACK;
    Display* tft;

public:

    Screen(Display* tft): tft(tft) { }

    Screen(uint16_t background_color, Display* tft) : background_color(background_color), tft(tft) {}

    void add(ScreenElement* e) {
        this->elements.push_back(e);
    }

    void initialDraw() {
        for (int i = 0; i < elements.size(); i++) {
            elements[i]->draw();
        }
    }

    void update() {
        for (int i = 0; i < elements.size(); i++) {
            elements[i]->update();
        }
    }

    void update(int delay_time) {
        update();
        delay(delay_time);
    }

    void readTouch() {
        if (!digitalRead(tft->interrupt)) {
            tft->read_touch();
        }
    }

    void initialize() {
        tft->fillScreen(RA8875_BLACK);
        tft->clearTouchEvents();
        this->initialDraw();
    }

};


#endif
