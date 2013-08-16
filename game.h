#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "keyboard.h"
#include "mouse.h"
#include "unit.h"

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

      bool playing;
      SDL_Surface *screen;
      SDL_Surface *tileset;
      MapCamera *map_camera;
      Map map;
      Keyboard keyboard;
      Mouse mouse;

      Unit *unit;
      Unit *devastator;
      UnitRepository *unitRepository;
};

#endif
