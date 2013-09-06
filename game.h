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
      SDL_Surface* screen;
      SDL_Surface* tileset;
      SDL_Surface* shroud_edges;
      MapCamera* map_camera;
      Map map;
      Keyboard keyboard;
      Mouse mouse;

      // THIS WILL BE A LIST/ARRAY/VECTOR OF SOME SORT SOME DAY
      Unit* unit;
      Unit* devastator;
      // END

      UnitRepository *unitRepository;
};

#endif
