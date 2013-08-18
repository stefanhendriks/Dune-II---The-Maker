#include "mouse.h"
#include "surface.h"

Mouse::Mouse() {
  _left_button_pressed = false;
  _right_button_pressed = false;
  _left_button_held = false;
  _left_button_no_more_held = false;
  _left_button_was_held = false;
  _left_button_was_pressed = false;
  _dragged_rectangle = false;
  _x = 0;
  _y = 0;
  state = MOUSE_POINTING;
  pointer = NULL;
  pointer_move = NULL;
}

void Mouse::init() {
  pointer = Surface::load("graphics/MS_Pointer.bmp");
  pointer_move = Surface::load("graphics/MS_Move.bmp");
}

void Mouse::update_state() {
  SDL_GetMouseState(&_x, &_y);
  _left_button_was_pressed = _left_button_pressed;
  _left_button_was_held = _left_button_held;

  _left_button_pressed = SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1);
  _right_button_pressed = SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3);

  if (started_dragging()) {
   rect_x = _x;
   rect_y = _y;
  }

  _left_button_held = (_left_button_was_pressed && _left_button_pressed);
  _left_button_no_more_held = (_left_button_was_held && !_left_button_pressed);

  _dragged_rectangle = left_button_no_more_held() && dragging_rectangle();
}

void Mouse::draw(SDL_Surface* screen) {
  if (left_button_held()) {
    rectangleRGBA(screen, rect_x, rect_y, _x, _y, 255, 255, 255, 255);
  }

  if (state == MOUSE_POINTING) {
    Surface::draw(pointer, screen, _x, _y);
  } else if (state == MOUSE_ORDER_MOVE) {
    // substract 16 , because the 'hot spot (click spot) of the mouse is always up left.' The move bitmap
    // has a 'center' where you move to, so we have to reposition the image to reflect the hot spot
    Surface::draw(pointer_move, screen, _x - 16, _y - 16);
  }
}
