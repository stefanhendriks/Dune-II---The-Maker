#include <iostream>

#include "mouse.h"
#include "surface.h"

using namespace std;

Mouse::Mouse() {
  _left_button_clicked = false;
  _right_button_clicked = false;
  _x = 0;
  _y = 0;
  rect_x = -1;
  rect_y = -1;
  state = MOUSE_POINTING;
  pointer = NULL;
  pointer_move = NULL;
}

void Mouse::init() {
  pointer = Surface::load("graphics/MS_Pointer.bmp");
  pointer_move = Surface::load("graphics/MS_Move.bmp");
}

void Mouse::onEvent(SDL_Event* event) {
  if (event->type == SDL_MOUSEMOTION) {
    _x = event->motion.x;
    _y = event->motion.y;
  } else {

    if (event->button.button == SDL_BUTTON_LEFT) {
      if (event->type == SDL_MOUSEBUTTONDOWN) {

        rect_x = event->button.x;
        rect_y = event->button.y;

      } else if (event->type == SDL_MOUSEBUTTONUP) {
        SDL_Event clickEvent;
        MouseClickedStruct *s;

        s = new MouseClickedStruct;
        s->x = event->button.x;
        s->y = event->button.y;

        clickEvent.type = SDL_USEREVENT;
        clickEvent.user.code = D2TM_MOUSE_CLICKED;
        clickEvent.user.data1 = s;
        clickEvent.user.data2 = NULL;

        SDL_PushEvent(&clickEvent);

        rect_x = -1;
        rect_y = -1;
      }
    }

    if (event->button.button == SDL_BUTTON_RIGHT) {
      if (event->type == SDL_MOUSEBUTTONDOWN) {

      } else if (event->type == SDL_MOUSEBUTTONUP) {
        SDL_Event clickEvent;

        clickEvent.type = SDL_USEREVENT;
        clickEvent.user.code = D2TM_DESELECT;
        clickEvent.user.data1 = NULL;
        clickEvent.user.data2 = NULL;

        SDL_PushEvent(&clickEvent);
      }

    }

  }

}

void Mouse::update_state() {
}

void Mouse::draw(SDL_Surface* screen) {
  if (state == MOUSE_POINTING) {
    //rectangleRGBA(screen, rect_x, rect_y, _x, _y, 255, 255, 255, 255);

    Surface::draw(pointer, screen, _x, _y);
  } else if (state == MOUSE_ORDER_MOVE) {
    // substract 16 , because the 'hot spot (click spot) of the mouse is always up left.' The move bitmap
    // has a 'center' where you move to, so we have to reposition the image to reflect the hot spot
    Surface::draw(pointer_move, screen, _x - 16, _y - 16);
  }
}
