#ifndef GAME_H
#define GAME_H

#include <vector>
#include "map.h"
#include "keyboard.h"
#include "mouse.h"
#include "unit.h"
#include <memory>

class Game {

  public:
     Game();

     int execute();

   private:
      int init();
      void onEvent(SDL_Event* event);
      void updateState();
      void render();
      int cleanup();
      void deselectAllUnits();
      bool playing;
      SDL_Surface* screen;
      SDL_Surface* terrain;
      SDL_Surface* shroud_edges;
      SDL_Surface* shroud_edges_shadow;
      std::unique_ptr<MapCamera> map_camera;
      Map map;
      Keyboard keyboard;
      Mouse mouse;

      std::vector<std::unique_ptr<Unit> > units; //possibly should be shared_ptr

      std::unique_ptr<UnitRepository> unitRepository;
};

#endif
