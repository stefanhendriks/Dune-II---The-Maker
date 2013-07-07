#include "SDL.h"
#include "game.h"

Game::Game() {
  playing=true;
  screen=NULL;
}

int Game::execute() {
  init();
  
  SDL_Event event;
  
  while(playing) {
    while(SDL_PollEvent(&event)) {
      onEvent(&event);
    }

    updateState();
    render();
  }

  return cleanup();
}

int Game::init() {
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    return false;
  }

  if((screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
     return false;
  }

  return true;
}

void Game::onEvent(SDL_Event* event) {
  if(event->type == SDL_QUIT) {
    playing = false;
  }
}

void Game::render() {
}

void Game::updateState() {
}

int Game::cleanup() {
  SDL_Quit();
  return 0;
}


