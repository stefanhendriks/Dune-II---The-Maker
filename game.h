#ifndef GAME_H
#define GAME_H

//#include <vector>
#include "map.h"
//#include "keyboard.h"
//#include "mouse.h"
#include "unit.h"
#include <memory>
//#include "player.h"
#include <SFML/Graphics.hpp>
#include "selectionbox.h"
#include "mouse.h"
#include "fps_counter.h"
#include <Thor/Input.hpp>

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
      //void deselectAllUnits();
      bool playing;
      sf::RenderWindow screen;
      sf::Texture terrain;
      sf::Texture shroud_edges;
      sf::View camera;
      SelectionBox box;
      //SDL_Surface shroud_edges_shadow;
      //std::unique_ptr<MapCamera> map_camera;
      std::unique_ptr<Map> map;
      //Keyboard keyboard;
      Mouse mouse;
      FPS_Counter fpsCounter;

      thor::ActionMap<std::string> actionMap;
      thor::ActionMap<std::string>::CallbackSystem system;

      //std::vector<Player> players;
      std::vector<std::unique_ptr<Unit> > units; //possibly should be shared_ptr

      void selectUnit(Unit& unit);

      //std::unique_ptr<UnitRepository> unitRepository;

      //bool playerHasUnits(const Player& player) const;
};

#endif
