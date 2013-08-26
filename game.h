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

      // THIS WILL BE A LIST/ARRAY/VECTOR OF SOME SORT SOME DAY
      Unit *unit;
      Unit *devastator;
      // END

      UnitRepository *unitRepository;

      bool isInRect(int x, int y, int width, int height);
      bool isInRect(int mouse_x, int mouse_y, int x, int y, int width, int height);
      bool isUnitInRect(Unit* unit, int x, int y, int width, int height);
};

#endif
