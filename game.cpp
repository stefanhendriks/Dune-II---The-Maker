#include <iostream>

#include "SDL/SDL.h"
#include "SDL_image.h"
#include "SDL_gfxPrimitives.h"
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
    // SDL rest?
  }

  return cleanup();
}

int Game::init() {
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    return false;
  }

  screen = SDL_SetVideoMode(800, 600, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
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

  unitRepository = new UnitRepository();

  unit = unitRepository->create(UNIT_FRIGATE, HOUSE_SARDAUKAR, 64, 64);
  devastator = unitRepository->create(UNIT_TRIKE, HOUSE_ATREIDES, 128, 128);

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

  if (mouse.left_button_held()) {
    rectangleRGBA(screen, mouse.getRectX(), mouse.getRectY(), mouse.x(), mouse.y(), 255, 255, 255, 255);
  }


  SDL_Flip(screen);
}

void Game::updateState() {
  SDL_PumpEvents();
  if (keyboard.isUpPressed()) map_camera->moveUp();
  if (keyboard.isDownPressed()) map_camera->moveDown();
  if (keyboard.isLeftPressed()) map_camera->moveLeft();
  if (keyboard.isRightPressed()) map_camera->moveRight();

  if (keyboard.isQPressed()) playing = false;

  mouse.update_state();

  if (mouse.x() <= 1) map_camera->moveLeft();
  if (mouse.x() >= (screen->w - 1)) map_camera->moveRight();
  if (mouse.y() <= 1) map_camera->moveUp();
  if (mouse.y() >= (screen->h - 1)) map_camera->moveDown();

  if (isInRect(devastator->getDrawX(), devastator->getDrawY(), devastator->width(), devastator->height())) {
    if(mouse.left_button_pressed() && !mouse.left_button_held()) {
      devastator->select();
    }
  }

  if (isInRect(unit->getDrawX(), unit->getDrawY(), unit->width(), unit->height())) {
    if(mouse.left_button_pressed() && !mouse.left_button_held()) {
      unit->select();
    }
  }

  if(mouse.right_button_pressed()) {
    devastator->unselect();
    unit->unselect();
  }

  if (mouse.dragged_rectangle()) {
    int rectX = map_camera->worldCoordinateX(mouse.getRectX());
    int rectY = map_camera->worldCoordinateY(mouse.getRectY());
    int endX = map_camera->worldCoordinateX(mouse.x());
    int endY = map_camera->worldCoordinateY(mouse.y());

    if (endX < rectX) swap(endX, rectX);
    if (endY < rectY) swap(endY, rectY);

    unit->unselect();
    devastator->unselect();

    if (isUnitInRect(devastator, rectX, rectY, endX, endY)) {
        devastator->select();
    }

    if (isUnitInRect(unit, rectX, rectY, endX, endY)) {
        unit->select();
    }
  }

}

int Game::cleanup() {
  delete map_camera;
  delete unitRepository;
  delete unit;
  delete devastator;
  SDL_FreeSurface(screen);
  SDL_FreeSurface(tileset);
  SDL_Quit();
  return 0;
}

bool Game::isUnitInRect(Unit* unit, int x, int y, int end_x, int end_y) {
  // this checks the up-left position of a unit: TODO: do rectangle intersection
  int unit_x = unit->getDrawX();
  int unit_y = unit->getDrawY();
  return (unit_x >= x && unit_x < end_x) && (unit_y >= y && unit_y < end_y);
}

bool Game::isInRect(int x, int y, int width, int height) {
  int mouse_x = mouse.x();
  int mouse_y = mouse.y();
  return (mouse_x >= x && mouse_x < (x + width)) && (mouse_y >= y && mouse_y < (y + height));
}
