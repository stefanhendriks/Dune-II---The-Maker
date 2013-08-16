#include "mouse.h"

Mouse::Mouse() {
  _left_button_pressed = false;
  _right_button_pressed = false;
  _left_button_held = false;
  _x = 0;
  _y = 0;
}

void Mouse::update_state() {
  SDL_GetMouseState(&_x, &_y);
  _left_button_pressed = SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1);
  _right_button_pressed = SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3);
}

