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
    bool left_button_no_more_held() { return _left_button_no_more_held; }

    int x() { return _x; }
    int y() { return _y; }

    int getRectX() { return rect_x; }
    int getRectY() { return rect_y; }

  private:
    bool _left_button_pressed, _right_button_pressed;
    bool _left_button_was_pressed;
    bool _left_button_held, _left_button_no_more_held, _left_button_was_held;

    bool started_dragging() { return !_left_button_was_pressed && _left_button_pressed; }

    int _x, _y;
    int rect_x, rect_y;
};

#endif
