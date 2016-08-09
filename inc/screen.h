#ifndef __Screen.h__
#define __Screen.h__

#include "Vector.h"
#include "ScreenElement.h"

class Screen {
private:
    Vector<ScreenElement*> elements;
    uint16_t background_color = RA8875_BLACK;

public:

    Screen() {}

    Screen(uint16_t background_color) : background_color(background_color){}

    void add(ScreenElement* e) {
        this->elements.push_back(e);
    }

    void initialDraw() {
        for (int i = 0; i < elements.size(); i ++) {
            elements[i]->draw();
        }
    }

    void update() {
        for (int i = 0; i < elements.size(); i ++) {
            elements[i]->update();
        }
    }

    void performOnClick() {
        for (int i = 0; i < elements.size(); i ++) {
            if (isButton(elements[i])) {
                Button* b = dynamic_cast<Button*> (elements[i]);
                if (b->isTapped()) {
                    b->onClick(); //TODO: Implement this.
                }
            }
        }
    }
};


#endif
