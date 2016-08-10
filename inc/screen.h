#ifndef __Screen_h__
#define __Screen_h__

#include "Vector.h"
#include "ScreenElement.h"

class Screen {
private:
    Vector<ScreenElement*> elements;
    Vector<Button*> buttons;
    uint16_t background_color = RA8875_BLACK;

public:

    Screen() {}

    Screen(uint16_t background_color) : background_color(background_color){}

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

    /*
     * Note: Buttons all have noop update functions, so
     * we will skip them (for now).
     */
    void update() {
        for (int i = 0; i < elements.size(); i++) {
            elements[i]->update();
        }
    }

    void listenForTouch() {
        for (int i = 0; i < buttons.size(); i++) {
            buttons[i]->updateIfTapped();
        }
    }

};


#endif
