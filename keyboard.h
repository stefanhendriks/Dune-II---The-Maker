#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "SDL/SDL.h"
#include "eventfactory.h"

class Keyboard {

  public:
    Keyboard();

    void onEvent(SDL_Event* event);

    bool isQPressed() { return qPressed; }

  private:
    bool qPressed;
    EventFactory eventFactory;
};

#endif
