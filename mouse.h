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

    void onEvent(SDL_Event* event, SDL_Surface* screen);

    bool left_button_clicked() { return _left_button_clicked; }
    bool right_button_clicked() { return _right_button_clicked; }

    void state_pointing() { state = MOUSE_POINTING; }
    void state_order_move() { state = MOUSE_ORDER_MOVE; }

    bool is_pointing() { return state == MOUSE_POINTING; }
    bool is_ordering_to_move() { return state == MOUSE_ORDER_MOVE; }

    int x() { return _x; }
    int y() { return _y; }

    //TODO: refactor, that it returns a rect class with world coordinates
    int getRectX() { return rect_x; }
    int getRectY() { return rect_y; }

    void draw(SDL_Surface* screen);

  private:
    SDL_Surface* pointer;
    SDL_Surface* pointer_move;
    bool _left_button_clicked, _right_button_clicked;

    bool dragging_rectangle();

    int state;

    int _x, _y;
    int rect_x, rect_y;

    bool is_mouse_on_screen(SDL_Surface* screen) { return (_x > 1 && _x < (screen->w - 1)) && (_y > 1 && _y < (screen->h -1)); }

};

#endif
