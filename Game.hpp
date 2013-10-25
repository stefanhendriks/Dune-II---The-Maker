#ifndef GAME_H
#define GAME_H

#include "Map.hpp"
#include "Unit.hpp"
#include <memory>

#include <SFML/Graphics.hpp>

#include "SelectionBox.hpp"
#include "Mouse.hpp"
#include "FpsCounter.hpp"
#include "ActionManager.hpp"
#include "Player.hpp"
#include "UnitRepository.hpp"
#include "Console.hpp"


struct CreateUnitMessage : public Message{
    CreateUnitMessage(Unit::Type type, const Player& player, const sf::Vector2f& position);

    const Unit::Type type;
    const Player& player;
    const sf::Vector2f position;
};

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
    void selectUnit(Unit &unit);

    bool playing;
    sf::RenderWindow screen;
    sf::Texture terrain;
    sf::Texture shroudEdges;
    sf::View camera;
    SelectionBox box;
    std::unique_ptr<Map> map;
    Mouse mouse;
    FPS_Counter fpsCounter;    

    std::vector<Player> players;
    std::vector<Unit> units;

    Console console;
    ActionManager actions;
    MessageSystem messages;


    UnitRepository unitRepository;

    sf::Time dt;

};

#endif
