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
  up=down=right=left=false;
  emit_event=false;
}

void Mouse::init(SDL_Surface* screen) {
  this->pointer = Surface::load("graphics/MS_Pointer.bmp");
  this->pointer_move = Surface::load("graphics/MS_Move.bmp");
  this->screen = screen;
}

bool Mouse::dragging_rectangle() {
  if (rect_x == -1 || rect_y == -1) return false;
  return (abs(_x - rect_x) > 3) && (abs(_y - rect_y) > 3);
}


void Mouse::onEvent(SDL_Event* event) {
  if (event->type != SDL_MOUSEMOTION && event->type != SDL_MOUSEBUTTONDOWN && event->type != SDL_MOUSEBUTTONUP) return;

  if (event->type == SDL_MOUSEMOTION) {
    emit_event = true;

    _x = event->motion.x;
    _y = event->motion.y;

    left = (_x <= 1);
    right = (_x >= (screen->w - 1));
    up = (_y <= 1);
    down = (_y >= (screen->h - 1));

  } else {

    if (event->button.button == SDL_BUTTON_LEFT) {
      if (event->type == SDL_MOUSEBUTTONDOWN) {

        rect_x = event->button.x;
        rect_y = event->button.y;

      } else if (event->type == SDL_MOUSEBUTTONUP) {

        if (dragging_rectangle()) {
          eventFactory.pushBoxSelectEvent(rect_x, rect_y, event->button.x, event->button.y);
        } else {
          eventFactory.pushSelectEvent(event->button.x, event->button.y);
        }
        rect_x = -1;
        rect_y = -1;
      }
    }

    if (event->button.button == SDL_BUTTON_RIGHT) {
      if (event->type == SDL_MOUSEBUTTONDOWN) {

      } else if (event->type == SDL_MOUSEBUTTONUP) {
        eventFactory.pushDeselectEvent();
      }

    }

  }

}

void Mouse::updateState() {
  if (!emit_event) return;

  float vec_x = 0, vec_y = 0;

  if (up) vec_y -= 1;
  if (down) vec_y += 1;
  if (left) vec_x -= 1;
  if (right) vec_x += 1;

  eventFactory.pushMoveCameraEvent(vec_x, vec_y);

  emit_event = false;
}

void Mouse::draw(SDL_Surface* screen) {
  if (state == MOUSE_POINTING) {
    if (dragging_rectangle()) rectangleRGBA(screen, rect_x, rect_y, _x, _y, 255, 255, 255, 255);

    Surface::draw(pointer, screen, _x, _y);
  } else if (state == MOUSE_ORDER_MOVE) {
    // substract 16 , because the 'hot spot (click spot) of the mouse is always up left.' The move bitmap
    // has a 'center' where you move to, so we have to reposition the image to reflect the hot spot
    Surface::draw(pointer_move, screen, _x - 16, _y - 16);
  }
}
