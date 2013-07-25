#include <iostream>

#include "SDL/SDL.h"
#include "SDL_image.h"
#include "map.h"
#include "game.h"
#include "surface.h"

using namespace std;

Game::Game() {
  playing=true;
  screen=NULL;
  tileset=NULL;
  map_camera=NULL;
  unit=NULL;
  devastator=NULL;
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


  screen = SDL_SetVideoMode(800, 600, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  //screen = SDL_SetVideoMode(800, 600, 32, SDL_FULLSCREEN | SDL_DOUBLEBUF);
  if(screen == NULL) {
     return false;
  }

  int flags = IMG_INIT_JPG | IMG_INIT_PNG;
  int initted=IMG_Init(flags);
  if( initted & flags != flags) {
    cout << "could not init SDL_Image" << endl;
    cout << "Reason: " << IMG_GetError() << endl;
    return false;
  }

  tileset = Surface::load("tileset.png");

  if (tileset == NULL) {
    cout << "Failed to read tileset data" << endl;
    return false;
  }

  map.setBoundaries(128,128);
  map_camera = new MapCamera(0, 0, screen, &map);


  unit = new Unit(Surface::load("quad.bmp", 255, 0, 255));
  devastator = new Unit(Surface::load("devastator.bmp", 255, 0, 255), 128, 128);

  return true;
}

void Game::onEvent(SDL_Event* event) {
  if(event->type == SDL_QUIT) {
    playing = false;
  }

  keyboard.onEvent(event);

}

void Game::render() {
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
  map_camera->draw(&map, tileset, screen);

 // todo draw with camera
  map_camera->draw(unit, screen);
  map_camera->draw(devastator, screen);

  SDL_Flip(screen);
}

void Game::updateState() {
  if (keyboard.isUpPressed()) map_camera->moveUp();
  if (keyboard.isDownPressed()) map_camera->moveDown();
  if (keyboard.isLeftPressed()) map_camera->moveLeft();
  if (keyboard.isRightPressed()) map_camera->moveRight();

  if (keyboard.isQPressed()) playing = false;

  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);

  if (mouse_x <= 1) map_camera->moveLeft();
  if (mouse_x >= (screen->w - 1)) map_camera->moveRight();
  if (mouse_y <= 1) map_camera->moveUp();
  if (mouse_y >= (screen->h - 1)) map_camera->moveDown();
}

int Game::cleanup() {
  delete map_camera;
  SDL_FreeSurface(screen);
  SDL_FreeSurface(tileset);
  SDL_Quit();
  return 0;
}


