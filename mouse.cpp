#include <iostream>

#include "mouse.h"
#include "surface.h"
#include "eventfactory.h"

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

bool Mouse::dragging_rectangle() {
  if (rect_x == -1 || rect_y == -1) return false;
  return (abs(_x - rect_x) > 3) && (abs(_y - rect_y) > 3);
}

void Mouse::onEvent(SDL_Event* event, SDL_Surface* screen) {
  EventFactory eventFactory;
  
  if (event->type == SDL_MOUSEMOTION) {
    _x = event->motion.x;
    _y = event->motion.y;

    //if (_x <= 1) map_camera->moveLeft();
    //if (_x >= (screen->w - 1)) map_camera->moveRight();
    //if (_y <= 1) map_camera->moveUp();
    //if (_y >= (screen->h - 1)) map_camera->moveDown();

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
