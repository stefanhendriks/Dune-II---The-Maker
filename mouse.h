#ifndef MOUSE_H
#define MOUSE_H

#include "SDL/SDL.h"
#include "SDL_gfxPrimitives.h"

const int MOUSE_POINTING = 0;
const int MOUSE_ORDER_MOVE = 1;

class Mouse {

  public:
    Mouse();
    void init();

    void update_state();

    bool left_button_pressed() { return _left_button_pressed; };
    bool right_button_pressed() { return _right_button_pressed; };

    bool left_button_held() { return _left_button_held; };
    bool left_button_no_more_held() { return _left_button_no_more_held; }

    bool dragged_rectangle() { return _dragged_rectangle; }
    bool dragging_rectangle() { return (abs(_x - rect_x) > 3) && (abs(_y - rect_y) > 3); }

    void state_pointing() { state = MOUSE_POINTING; }
    void state_order_move() { state = MOUSE_ORDER_MOVE; }

    bool is_pointing() { return state == MOUSE_POINTING; }

    int x() { return _x; }
    int y() { return _y; }

    //TODO: refactor, that it returns a rect class with world coordinates
    int getRectX() { return rect_x; }
    int getRectY() { return rect_y; }

    void draw(SDL_Surface* screen);

  private:
    SDL_Surface* pointer;
    SDL_Surface* pointer_move;
    bool _left_button_pressed, _right_button_pressed;
    bool _left_button_was_pressed;
    bool _left_button_held, _left_button_no_more_held, _left_button_was_held;

    bool _dragged_rectangle;

    bool started_dragging() { return !_left_button_was_pressed && _left_button_pressed; }

    int state;

    int _x, _y;
    int rect_x, rect_y;
};

#endif
