#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "SDL/SDL.h"
#include "eventfactory.h"

class Keyboard {

  public:
    Keyboard();

    void onEvent(SDL_Event* event);

    bool isQPressed() { return qPressed; }

    void updateState();

  private:
    bool qPressed;
    bool up, down, left, right;
    bool emit_event;

    EventFactory eventFactory;

};

#endif
