#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "SDL/SDL.h"

class Keyboard {

  public:
    Keyboard();

    void onEvent(SDL_Event* event);

    bool isUpPressed() { return upPressed; }
    bool isDownPressed() { return downPressed; }
    bool isLeftPressed() { return leftPressed; }
    bool isRightPressed() { return rightPressed; }
    bool isQPressed() { return qPressed; }

  private:
    bool upPressed, downPressed, leftPressed, rightPressed, qPressed;

};

#endif
