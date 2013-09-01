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
  init();

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

  tileset = Surface::load("tileset.png");

  if (tileset == NULL) {
    cout << "Failed to read tileset data" << endl;
    return false;
  }

  SDL_ShowCursor(0);
  mouse.init(screen);

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

  mouse.onEvent(event);
  keyboard.onEvent(event);

  map_camera->onEvent(event);

  if (event->type == SDL_USEREVENT) {
    if (event->user.code == D2TM_SELECT) {
      D2TMSelectStruct *s = static_cast<D2TMSelectStruct*>(event->user.data1);

      Point p = map_camera->toWorldCoordinates(s->screen_position);
      int mx = p.x;
      int my = p.y;

      delete s;

      if (mouse.is_pointing()) {

        if (isInRect(mx, my, devastator->getDrawX(), devastator->getDrawY(), devastator->width(), devastator->height())) {
          mouse.state_order_move();
          unit->unselect();
          devastator->unselect();
          devastator->select();
        }

        if (isInRect(mx, my, unit->getDrawX(), unit->getDrawY(), unit->width(), unit->height())) {
          mouse.state_order_move();
          unit->unselect();
          devastator->unselect();
          unit->select();
        }
      }

    } else if (event->user.code == D2TM_DESELECT) {
      mouse.state_pointing();
      devastator->unselect();
      unit->unselect();
    } else if (event->user.code == D2TM_BOX_SELECT) {
      D2TMBoxSelectStruct *s = static_cast<D2TMBoxSelectStruct*>(event->user.data1);

      Point start = map_camera->toWorldCoordinates(s->start);
      Point end = map_camera->toWorldCoordinates(s->end);
      int rectX = start.x;
      int rectY = start.y;
      int endX = end.x;
      int endY = end.y;

      delete s;

      if (mouse.is_pointing()) {
        mouse.state_pointing();

        unit->unselect();
        devastator->unselect();

        if (isUnitInRect(devastator, rectX, rectY, endX, endY)) {
          mouse.state_order_move();
          devastator->select();
        }

        if (isUnitInRect(unit, rectX, rectY, endX, endY)) {
          mouse.state_order_move();
          unit->select();
        }
      }
    } else if (event->user.code == D2TM_MOVE_UNIT) {
      D2TMMoveUnitStruct *s = static_cast<D2TMMoveUnitStruct*>(event->user.data1);
      Point p = map_camera->toWorldCoordinates(s->screen_position);

      if (unit->is_selected()) unit->move_to(p);
      if (devastator->is_selected()) devastator->move_to(p);

      delete s;
    }

  }

}

void Game::render() {
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

  map_camera->draw(&map, tileset, screen);
  map_camera->draw(unit, screen);
  map_camera->draw(devastator, screen);

  mouse.draw(screen);

  SDL_Flip(screen);
}

void Game::updateState() {
  keyboard.updateState();
  mouse.updateState();
  map_camera->updateState();
  //} else if (mouse.is_ordering_to_move()) {

    //if (mouse.left_button_pressed()) {
      //if (unit->is_selected()) {
        //unit->move_to(mouse.x(), mouse.y());
      //}
    //}
  //}
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

bool Game::isInRect(int mouse_x, int mouse_y, int x, int y, int width, int height) {
  return (mouse_x >= x && mouse_x < (x + width)) && (mouse_y >= y && mouse_y < (y + height));
}
