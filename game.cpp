#include <iostream>

#include "SDL/SDL.h"
#include "SDL_image.h"
#include "SDL_gfxPrimitives.h"
#include "point.h"
#include "map.h"
#include "game.h"
#include "surface.h"
#include "eventfactory.h"

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
  if (!init()) {
    cerr << "Errors occured when starting the game. Stopping." << endl;
    return 1;
  }

  SDL_Event event;

  while(playing) {
    SDL_PumpEvents();
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

  tileset = Surface::load("graphics/tileset.png");

  if (tileset == NULL) {
    cout << "Failed to read graphics/tileset.png data" << endl;
    return false;
  }

  shroud_edges = Surface::load("graphics/shroud_edges.bmp");

  if (shroud_edges == NULL) {
    cout << "Failed to read graphics/shroud_edges.bmp data" << endl;
    return false;
  }

  shroud_edges_shadow = Surface::load("graphics/shroud_edges_shadow.bmp");

  if (shroud_edges_shadow == NULL) {
    cout << "Failed to read graphics/shroud_edges_shadow.bmp data" << endl;
    return false;
  }

  SDL_ShowCursor(0);
  mouse.init(screen);

  map.setBoundaries(128,128);
  map_camera = new MapCamera(0, 0, screen, &map);

  unitRepository = new UnitRepository(&map);

  unit = unitRepository->create(UNIT_FRIGATE, HOUSE_SARDAUKAR, 64, 64, 5);
  devastator = unitRepository->create(UNIT_TRIKE, HOUSE_ATREIDES, 128, 128, 3);

  return true;
}

void Game::onEvent(SDL_Event* event) {
  if(event->type == SDL_QUIT) {
    playing = false;
  }

  mouse.onEvent(event);
  keyboard.onEvent(event);

  map_camera->onEvent(event);

  if (event->type == SDL_USEREVENT) {
    if (event->user.code == D2TM_SELECT) {
      D2TMSelectStruct *s = static_cast<D2TMSelectStruct*>(event->user.data1);

      Point p = map_camera->toWorldCoordinates(s->screen_position);

      if (mouse.is_pointing()) {

        if (devastator->is_point_within(p)) {
          mouse.state_order_move();
          unit->unselect();
          devastator->unselect();
          devastator->select();
        }

        if (unit->is_point_within(p)) {
          mouse.state_order_move();
          unit->unselect();
          devastator->unselect();
          unit->select();
        }
      }

      delete s;
    } else if (event->user.code == D2TM_DESELECT) {
      mouse.state_pointing();
      devastator->unselect();
      unit->unselect();
    } else if (event->user.code == D2TM_BOX_SELECT) {
      D2TMBoxSelectStruct *s = static_cast<D2TMBoxSelectStruct*>(event->user.data1);

      Rectangle rectangle = map_camera->toWorldCoordinates(s->rectangle);

      if (mouse.is_pointing()) {
        mouse.state_pointing();

        unit->unselect();
        devastator->unselect();

        if (devastator->is_within(rectangle)) {
          mouse.state_order_move();
          devastator->select();
        }

        if (unit->is_within(rectangle)) {
          mouse.state_order_move();
          unit->select();
        }
      }

      delete s;
    } else if (event->user.code == D2TM_MOVE_UNIT) {
      D2TMMoveUnitStruct *s = static_cast<D2TMMoveUnitStruct*>(event->user.data1);
      Point p = map_camera->toWorldCoordinates(s->screen_position);

      if (unit->is_selected()) unit->order_move(p);
      if (devastator->is_selected()) devastator->order_move(p);

      delete s;
    }

  }

}

void Game::render() {
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

  map_camera->draw(&map, tileset, screen);
  map_camera->draw(unit, screen);
  map_camera->draw(devastator, screen);
  map_camera->drawShroud(&map, shroud_edges, shroud_edges_shadow, screen);

  mouse.draw(screen);

  SDL_Flip(screen);
}

void Game::updateState() {
  keyboard.updateState();
  mouse.updateState();
  map_camera->updateState();

  unit->updateState();
  devastator->updateState();
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

