#include <iostream>

#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include "Houses.hpp"



Game::Game():
  playing(true),
  screen(),
  map(nullptr),
  actions(screen)
{
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  screen.create(sf::VideoMode(800, 600), "Dune 2 - The Maker", sf::Style::Close, settings);
  screen.setFramerateLimit(IDEAL_FPS);
  screen.setMouseCursorVisible(false);

  if (!init()){
    std::cerr << "Failed to initialized game.";
    playing = false;
  }
}

int Game::execute() {

  sf::Clock clock;

  while(playing) {
    sf::Time dt = clock.restart();

    updateState(dt);
    render();
  }

  return 0;
}

bool Game::init() {
  if (!terrain.loadFromFile("graphics/terrain.png")) {
    std::cout << "Failed to read graphics/terrain.png data" << std::endl;
    return false;
  }

  sf::Image temp;
  if (!temp.loadFromFile("graphics/shroud_edges.bmp")) {
    std::cout << "Failed to read graphics/shroud_edges.bmp data" << std::endl;
    return false;
  }

  temp.createMaskFromColor(sf::Color(255, 0, 255));
  shroudEdges.loadFromImage(temp);

  map.reset(new Map(terrain, shroudEdges));
  map->load("maps/4PL_Mountains.ini");

  camera.reset({0,0,800,600});
  screen.setView(camera);

  //init two players
  int idCount = 0;
  players.emplace_back(House::Sardaukar, idCount++);
  players.emplace_back(House::Harkonnen, idCount++);

  units.push_back(std::move(unitRepository.create(Unit::Type::Trike, players[0], sf::Vector2f(256, 256), *map)));
  units.push_back(std::move(unitRepository.create(Unit::Type::Quad, players[1], sf::Vector2f(300, 300), *map)));

  units.push_back(std::move(unitRepository.create(Unit::Type::Soldier, players[0], sf::Vector2f(400, 500), *map)));
  units.push_back(std::move(unitRepository.create(Unit::Type::Soldier, players[0], sf::Vector2f(410, 500), *map)));
  units.push_back(std::move(unitRepository.create(Unit::Type::Soldier, players[0], sf::Vector2f(220, 500), *map)));
  units.push_back(std::move(unitRepository.create(Unit::Type::Soldier, players[0], sf::Vector2f(430, 500), *map)));

  units.push_back(std::move(unitRepository.create(Unit::Type::Devastator, players[1], sf::Vector2f(500, 200), *map)));

  //register listeners
  typedef thor::ActionContext<std::string> actionContext;

  actions.connect("close", [this](actionContext){playing = false;});

  actions.connect("boxRelease", [this](actionContext){
    for (auto& unit : units){
      if (box.intersects(unit.getBounds())){
        selectUnit(unit);
      }
    }
    box.clear();
  });

  actions.connect("boxStart", [this](actionContext context) {
    if (mouse.getType() != Mouse::Type::Default) {
      return;
    }
    sf::Vector2f toSet = screen.mapPixelToCoords(mouse.getHotspot(*context.event), camera);
    box.setTopLeft(toSet);
  });

  actions.connect("singleSelect", [this](actionContext context){
    sf::Vector2f toCheck = screen.mapPixelToCoords(mouse.getHotspot(*context.event), camera);
    for (auto& unit : units){
      if (unit.getBounds().contains(toCheck))
        selectUnit(unit);
    }
  });

  actions.connect("deselectAll", [this](actionContext){
    actions.disconnect("orderMove");
    mouse.setType(Mouse::Type::Default);
    for (auto& unit : units)
      unit.unselect();
  });

  actions.connect("boxDrag", [this](actionContext){
    box.setBottomRight(screen.mapPixelToCoords(sf::Mouse::getPosition(screen), camera));
  });

  const float cameraSpeed = 15.f;

  actions.connect("cameraLeft", [this, cameraSpeed](actionContext) {camera.move(-cameraSpeed, 0.f);});
  actions.connect("cameraRight", [this, cameraSpeed](actionContext){camera.move(cameraSpeed, 0.f); });
  actions.connect("cameraUp", [this, cameraSpeed](actionContext)   {camera.move(0.f, -cameraSpeed);});
  actions.connect("cameraDown", [this, cameraSpeed](actionContext) {camera.move(0.f, cameraSpeed); });

  return true;
}

void Game::render() {
  screen.clear();

  screen.setView(camera);

  screen.draw(*map);

  for (const auto& unit : units)
    screen.draw(unit);

  map->drawShrouded(screen, sf::RenderStates::Default);

  screen.draw(box);

  screen.draw(fpsCounter);

  screen.setView(camera);

  screen.draw(mouse);

  screen.display();
}

void Game::updateState(sf::Time dt) {
  actions.update();

  sf::Vector2i mousePosition = sf::Mouse::getPosition(screen);

  if      (mousePosition.x < 50 ) actions.trigger("cameraLeft");
  else if (mousePosition.y < 50 ) actions.trigger("cameraUp");
  else if (mousePosition.x > 750) actions.trigger("cameraRight");
  else if (mousePosition.y > 550) actions.trigger("cameraDown");

  sf::Vector2f half_of_camera = camera.getSize() / 2.f;
  sf::Vector2f topLeft = camera.getCenter() - (half_of_camera);
  sf::Vector2f downRight = camera.getCenter() + (half_of_camera);

  if (topLeft.x <= Cell::TILE_SIZE) camera.setCenter(half_of_camera.x + Cell::TILE_SIZE, camera.getCenter().y);
  if (topLeft.y <= Cell::TILE_SIZE) camera.setCenter(camera.getCenter().x, half_of_camera.y + Cell::TILE_SIZE);

  int max_width = (map->getMaxWidth() + 1) * Cell::TILE_SIZE;
  int max_height = (map->getMaxHeight() + 1) * Cell::TILE_SIZE;

  if (downRight.x >= max_width) camera.setCenter(max_width - half_of_camera.x, camera.getCenter().y);
  if (downRight.y >= max_height) camera.setCenter(camera.getCenter().x, max_height - half_of_camera.y);

  mouse.setPosition(screen.mapPixelToCoords(mousePosition,camera));

  for (auto& unit: units){
    unit.updateState(units);
  }

  fpsCounter.update(dt);
  map->prepare(screen.mapPixelToCoords(sf::Vector2i(0,0)));
}


void Game::selectUnit(Unit &unit)
{
  unit.select();
  actions.connect("orderMove", [this, &unit](thor::ActionContext<std::string> context){
    unit.orderMove(screen.mapPixelToCoords(mouse.getHotspot(*context.event), camera));
  });
  mouse.setType(Mouse::Type::Move); //at least one unit selected...
}
