#ifndef MOUSE_H
#define MOUSE_H

#include "SDL/SDL.h"

class Mouse {

  public:
    Mouse();

    void update_state();

    bool left_button_pressed() { return _left_button_pressed; };
    bool right_button_pressed() { return _right_button_pressed; };

    bool left_button_held() { return _left_button_held; };

    int x() { return _x; }
    int y() { return _y; }

  private:
    bool _left_button_pressed, _right_button_pressed, _left_button_held;

    int _x, _y;
};

#endif
