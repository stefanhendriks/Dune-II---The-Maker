#include <iostream>
#include "keyboard.h"

using namespace std;

Keyboard::Keyboard() {
  qPressed = false;
}

void Keyboard::onEvent(SDL_Event* event) {
  if (event->type == SDL_KEYDOWN) {
    switch( event->key.keysym.sym ) {
       case SDLK_UP: eventFactory.pushMoveCameraEvent(D2TM_CAMERA_MOVE_UP); break;
       case SDLK_DOWN: eventFactory.pushMoveCameraEvent(D2TM_CAMERA_MOVE_DOWN); break;
       case SDLK_LEFT: eventFactory.pushMoveCameraEvent(D2TM_CAMERA_MOVE_LEFT); break;
       case SDLK_RIGHT: eventFactory.pushMoveCameraEvent(D2TM_CAMERA_MOVE_RIGHT); break;
       case SDLK_q: eventFactory.pushQuitEvent();
       default: break;
    }
  }
}

