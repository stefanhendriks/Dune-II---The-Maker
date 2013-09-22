#include <iostream>
#include "keyboard.h"

using namespace std;

Keyboard::Keyboard():
    qPressed(false),
    emit_event(false),
    up(false), down(false), left(false), right(false)
{

}

void Keyboard::onEvent(SDL_Event* event) {
  if (event->type != SDL_KEYUP && event->type != SDL_KEYDOWN) return;

  switch( event->key.keysym.sym ) {
     case SDLK_UP:
       up = event->type == SDL_KEYDOWN;
       break;
     case SDLK_DOWN:
       down = event->type == SDL_KEYDOWN;
       break;
     case SDLK_LEFT:
       left = event->type == SDL_KEYDOWN;
       break;
     case SDLK_RIGHT:
       right = event->type == SDL_KEYDOWN;
       break;
     case SDLK_q: eventFactory.pushQuitEvent();
     default: break;
  }

  emit_event=true;
}

void Keyboard::updateState() {
  if (!emit_event) return;

  float vec_x = 0, vec_y = 0;

  if (up) vec_y -= 1;
  if (down) vec_y += 1;
  if (left) vec_x -= 1;
  if (right) vec_x += 1;

  eventFactory.pushMoveCameraEvent(vec_x, vec_y);

  emit_event = false;
}

