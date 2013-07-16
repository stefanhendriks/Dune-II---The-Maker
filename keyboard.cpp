#include "keyboard.h"

Keyboard::Keyboard() {
  upPressed = downPressed = leftPressed = rightPressed = qPressed = false;
}

void Keyboard::onEvent(SDL_Event* event) {
  bool keyDown = event->type == SDL_KEYDOWN;
  switch( event->key.keysym.sym ) {
     case SDLK_UP: upPressed = keyDown; break;
     case SDLK_DOWN: downPressed = keyDown; break;
     case SDLK_LEFT: leftPressed = keyDown; break;
     case SDLK_RIGHT: rightPressed = keyDown; break;
     case SDLK_q: qPressed = keyDown;
     default: break;
  }
}
