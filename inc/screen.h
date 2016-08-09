#ifndef __screen_h__
#define __screen_h__

#include "Vector.h"
#include "interface.h"

class Screen {
private:
    Vector<ScreenElement> elements;
    uint16_t background_color = RA8875_BLACK;

public:

    Screen() {}

    Screen(uint16_t background_color) : background_color(background_color){}

    void add(const ScreenElement& e) {
        this->elements.push_back(e);
    }

    void initialDraw() {
        for (int i = 0; i < elements.size(); i ++) {
            elements[i].draw();    
        }
    }

    void update() {
        for (int i = 0; i < elements.size(); i ++) {
            elements[i].update();    
        }
    }
};


#endif
