#include <iostream>

#include <SFML/Graphics.hpp>
#include "game.h"
#include "houses.h"



Game::Game():
    playing(true),
    screen(),
    map(nullptr),
    actions(this)
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
  shroud_edges.loadFromImage(temp);

  map.reset(new Map(terrain, shroud_edges));
  map->load("maps/4PL_Mountains.ini");

  camera.reset({0,0,800,600});
  screen.setView(camera);

  sf::Image trikeShadowImage;
  trikeShadowImage.loadFromFile("graphics/Unit_Trike_s.bmp");
  trikeShadowImage.createMaskFromColor(sf::Color(255,0,255));
  sf::Texture* trikeShadowTexture = new sf::Texture;
  trikeShadowTexture->loadFromImage(trikeShadowImage);

  sf::Image selectedImage;
  selectedImage.loadFromFile("graphics/selected.bmp");
  selectedImage.createMaskFromColor(sf::Color(255, 0, 255));
  sf::Texture* selectedTexture = new sf::Texture; //more leaks!
  selectedTexture->loadFromImage(selectedImage);


   //init two players
  int idCount = 0;
  players.emplace_back(House::Sardaukar, idCount++);
  players.emplace_back(House::Harkonnen, idCount++);

  int unitIdCount = 0;
  units.emplace_back(players[0].getTexture(), *trikeShadowTexture, *selectedTexture, 256, 256, 0, *map, unitIdCount++);
  units.emplace_back(players[1].getTexture(), *trikeShadowTexture, *selectedTexture, 300, 300, 0, *map, unitIdCount++);

  //units.emplace_back(unitRepository->create(UNIT_FRIGATE, House::Sardaukar, 3, 3, 10, SUBCELL_CENTER, players[0]));
  //units.emplace_back(unitRepository->create(UNIT_TRIKE, House::Sardaukar, 8, 8, 3, SUBCELL_CENTER, players[0]));

  //// soldiers
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Sardaukar, 14, 14, 3, SUBCELL_CENTER, players[0]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Sardaukar, 14, 14, 3, SUBCELL_UPLEFT, players[0]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Sardaukar, 14, 14, 3, SUBCELL_UPRIGHT, players[0]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Sardaukar, 14, 14, 3, SUBCELL_DOWNLEFT, players[0]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Sardaukar, 14, 14, 3, SUBCELL_DOWNRIGHT, players[0]));

  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Harkonnen, 18, 8, 3, SUBCELL_CENTER, players[1]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Harkonnen, 18, 8, 3, SUBCELL_UPLEFT, players[1]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Harkonnen, 18, 8, 3, SUBCELL_UPRIGHT, players[1]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Harkonnen, 18, 8, 3, SUBCELL_DOWNLEFT, players[1]));
  //units.emplace_back(unitRepository->create(UNIT_SOLDIER, House::Harkonnen, 18, 8, 3, SUBCELL_DOWNRIGHT, players[1]));


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

  sf::Vector2f half_of_camera = camera.getSize() / 2.f;
  sf::Vector2f topLeft = camera.getCenter() - (half_of_camera);
  sf::Vector2f downRight = camera.getCenter() + (half_of_camera);

  if (topLeft.x <= Cell::TILE_SIZE) camera.setCenter(half_of_camera.x + Cell::TILE_SIZE, camera.getCenter().y);
  if (topLeft.y <= Cell::TILE_SIZE) camera.setCenter(camera.getCenter().x, half_of_camera.y + Cell::TILE_SIZE);

  int max_width = (map->getMaxWidth() + 1) * Cell::TILE_SIZE;
  int max_height = (map->getMaxHeight() + 1) * Cell::TILE_SIZE;

  if (downRight.x >= max_width) camera.setCenter(max_width - half_of_camera.x, camera.getCenter().y);
  if (downRight.y >= max_height) camera.setCenter(camera.getCenter().x, max_height - half_of_camera.y);

  mouse.setPosition(screen.mapPixelToCoords(sf::Mouse::getPosition(screen),camera));

  if (mouse.getPosition().x < 5) {
    // todo: trigger events
  }



  for (auto& unit: units){
    unit.updateState(units);
  }

  fpsCounter.update(dt);
  map->prepare(screen.mapPixelToCoords(sf::Vector2i(0,0)));
}
