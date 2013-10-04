#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "unit.h"
#include <memory>

#include <SFML/Graphics.hpp>

#include "selectionbox.h"
#include "mouse.h"
#include "fps_counter.h"
#include "actionmanager.h"
#include "player.h"

class Game {

  const int IDEAL_FPS = 60;

  public:
     Game();

     int execute();

   private:
      bool init();
      void updateState(sf::Time dt);
      void render();
      int cleanup();

      bool playing;
      sf::RenderWindow screen;
      sf::Texture terrain;
      sf::Texture shroud_edges;
      sf::View camera;
      SelectionBox box;
      std::unique_ptr<Map> map;
      Mouse mouse;
      FPS_Counter fpsCounter;

      std::vector<Player> players;
      std::vector<Unit> units;

      ActionManager actions;

      friend class ActionManager;
};

#endif
