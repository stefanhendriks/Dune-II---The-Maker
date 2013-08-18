#include "mouse.h"

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

