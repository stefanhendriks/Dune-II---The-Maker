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
  terrain=NULL;
  map_camera=NULL;
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

  terrain = Surface::load("graphics/terrain.png");

  if (terrain == NULL) {
    cout << "Failed to read graphics/terrain.png data" << endl;
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

  map_camera = new MapCamera(0, 0, screen, &map);
  map.load("maps/4PL_Mountains.ini");
  unitRepository = new UnitRepository(&map);

  Unit* frigate = unitRepository->create(UNIT_FRIGATE, HOUSE_SARDAUKAR, 64, 64, 10);
  units.push_back(frigate);

  Unit* trike1 = unitRepository->create(UNIT_TRIKE, HOUSE_ATREIDES, 256, 256, 3);
  units.push_back(trike1);

  Unit* trike2 = unitRepository->create(UNIT_TRIKE, HOUSE_ATREIDES, 448, 448, 3);
  units.push_back(trike2);

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
        Unit* selected_unit = NULL;
        for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
          if ((*it)->is_point_within(p)) {
            selected_unit = *it;
            break;
          }
        }

        if (selected_unit != NULL) {
          deselectAllUnits();
          selected_unit->select();
          mouse.state_order_move();
        }
      }

      delete s;
    } else if (event->user.code == D2TM_DESELECT) {
      mouse.state_pointing();
      deselectAllUnits();
    } else if (event->user.code == D2TM_BOX_SELECT) {
      D2TMBoxSelectStruct *s = static_cast<D2TMBoxSelectStruct*>(event->user.data1);

      Rectangle rectangle = map_camera->toWorldCoordinates(s->rectangle);

      if (mouse.is_pointing()) {
        mouse.state_pointing();

        deselectAllUnits();
        for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
          if ((*it)->is_within(rectangle)) {
            (*it)->select();
            mouse.state_order_move();
          }
        }
      }

      delete s;
    } else if (event->user.code == D2TM_MOVE_UNIT) {
      D2TMMoveUnitStruct *s = static_cast<D2TMMoveUnitStruct*>(event->user.data1);
      Point p = map_camera->toWorldCoordinates(s->screen_position);

      for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
        if ((*it)->is_selected()) {
          (*it)->order_move(p);
        }
      }

      delete s;
    }

  }

}

void Game::render() {
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

  map_camera->draw(&map, terrain, screen);

  for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
    if ((*it)->is_on_air_layer()) map_camera->draw(*it, screen);
  }

  for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
    if ((*it)->is_on_ground_layer()) map_camera->draw(*it, screen);
  }

  map_camera->drawShroud(&map, shroud_edges, shroud_edges_shadow, screen);

  mouse.draw(screen);

  SDL_Flip(screen);
}

void Game::updateState() {
  keyboard.updateState();
  mouse.updateState();
  map_camera->updateState();

  for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
    (*it)->updateState();
  }
}

int Game::cleanup() {
  delete map_camera;
  delete unitRepository;
  for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
    delete *it;
  }
  units.clear();
  SDL_FreeSurface(screen);
  SDL_FreeSurface(terrain);
  SDL_FreeSurface(shroud_edges);
  SDL_FreeSurface(shroud_edges_shadow);
  SDL_Quit();
  return 0;
}

void Game::deselectAllUnits() {
  for (vector<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
    (*it)->unselect();
  }
}

