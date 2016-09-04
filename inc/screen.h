#ifndef __Screen_h__
#define __Screen_h__

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

    void add(Button* e) {
        this->buttons.push_back(e);
    }

    void initialDraw() {
        for (int i = 0; i < elements.size(); i++) {
            elements[i]->draw();
        }
        for (int i = 0; i < buttons.size(); i++) {
            buttons[i]->draw();
        }
    }

    void update() {
        for (int i = 0; i < elements.size(); i++) {
            elements[i]->update();
        }
        for (int i = 0; i < buttons.size(); i++) {
            buttons[i]->update();
        }
    }

    void update(int delay_time) {
        update();
        delay(delay_time);
    }

    void propogateTouch() {
        if (!digitalRead(tft->interrupt)) {
            tft->read_touch();
            for (int i = 0; i < buttons.size(); i++) {
                buttons[i]->updateIfTapped();
            }
        }
    }

};


#endif
