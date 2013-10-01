#include <iostream>

#include <SFML/Graphics.hpp>
#include "game.h"



Game::Game():
    playing(true),
    screen(),
    map(nullptr)
    //map_camera(nullptr),
    //unitRepository(nullptr)
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
    sf::Event event;
    actionMap.clearEvents();
    sf::Time dt = clock.restart();
    while(screen.pollEvent(event)) {
      actionMap.pushEvent(event);
    }

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

  //init a trike
  sf::Image trikeImage;
  trikeImage.loadFromFile("graphics/Unit_Trike.bmp");
  trikeImage.createMaskFromColor(sf::Color(0,0,0));
  sf::Texture* trikeTexture = new sf::Texture; //yes we are leaking! Player should own this
  trikeTexture->loadFromImage(trikeImage);

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
  units.emplace_back(new Unit(*trikeTexture, *trikeShadowTexture, *selectedTexture, 256, 256, 0));

  //remove shroud here
  map->removeShroud(static_cast<sf::Vector2i>(units[0]->getPosition()), 10);

  ////init two players
  //int idCount = 0;
  //players.emplace_back(House::Sardaukar, idCount++);
  //players.emplace_back(House::Harkonnen, idCount++);

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

  //thor Actions here

  actionMap["boxStart"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
  actionMap["orderMove"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
  actionMap["boxDrag"] = thor::Action(sf::Mouse::Left, thor::Action::Hold);
  actionMap["boxRelease"] = thor::Action(sf::Mouse::Left, thor::Action::ReleaseOnce);
  actionMap["deselectAll"] = thor::Action(sf::Mouse::Right, thor::Action::PressOnce);
  actionMap["close"] = thor::Action(sf::Event::Closed) || thor::Action(sf::Keyboard::Q, thor::Action::PressOnce);
  actionMap["cameraLeft"] = thor::Action(sf::Keyboard::Left, thor::Action::Hold);
  actionMap["cameraRight"] = thor::Action(sf::Keyboard::Right, thor::Action::Hold);
  actionMap["cameraUp"] = thor::Action(sf::Keyboard::Up, thor::Action::Hold);
  actionMap["cameraDown"] = thor::Action(sf::Keyboard::Down, thor::Action::Hold);

  typedef thor::ActionContext<std::string> actionContext;

  system.connect("close", [this](actionContext){playing = false;});

  system.connect("boxRelease", [this](actionContext){
    for (auto& unit : units){
      if (box.intersects(unit->getBounds())){
        unit->select();
        system.connect("orderMove", [this, &unit](actionContext context){
          unit->order_move(screen.mapPixelToCoords(sf::Vector2i(context.event->mouseButton.x, context.event->mouseButton.y)));
        });
        mouse.setType(Mouse::Type::Move); //at least one unit selected...
      }
    }
    box.clear();
  });

  system.connect("boxStart", [this](actionContext context){
    sf::Vector2f toSet = screen.mapPixelToCoords(sf::Vector2i(context.event->mouseButton.x, context.event->mouseButton.y));
    box.setTopLeft(toSet);
  });

  system.connect("deselectAll", [this](actionContext){
//    system.clearConnections("orderMove"); // -> This is bugged in Thor
    mouse.setType(Mouse::Type::Default);
    for (auto& unit : units)
      unit->unselect();
  });

  system.connect("boxDrag", [this](actionContext){
    box.setBottomRight(screen.mapPixelToCoords(sf::Mouse::getPosition(screen)));
  });

  const float cameraSpeed = 15.f;

  moveVector = sf::Vector2f();

  system.connect("cameraLeft", [this, cameraSpeed](actionContext) {moveVector.x -= cameraSpeed;});
  system.connect("cameraRight", [this, cameraSpeed](actionContext){moveVector.x = cameraSpeed; });
  system.connect("cameraUp", [this, cameraSpeed](actionContext)   {moveVector.y -= cameraSpeed;});
  system.connect("cameraDown", [this, cameraSpeed](actionContext) {moveVector.y = cameraSpeed; });

  return true;
}

void Game::render() {
  screen.clear();

  screen.setView(camera);

  screen.draw(*map);

  for (const auto& unit : units)
      screen.draw(*unit);

//  map->drawShrouded(screen, sf::RenderStates::Default);

  screen.draw(box);

  screen.draw(fpsCounter);

  screen.setView(camera);
  screen.draw(mouse);

  screen.display();
}

void Game::updateState(sf::Time dt) {
  actionMap.invokeCallbacks(system, &screen);  

  mouse.setPosition(screen.mapPixelToCoords(sf::Mouse::getPosition(screen)));

  sf::Vector2f topLeft = camera.getCenter() - (camera.getSize() / 2.f);
  sf::Vector2f downRight = camera.getCenter() + (camera.getSize() / 2.f);

  if (moveVector.x < 0 && topLeft.x <= 0) moveVector.x = 0;
  if (moveVector.y < 0 && topLeft.y <= 0) moveVector.y = 0;
  if (moveVector.x > 0 && downRight.x >= (map->getMaxWidth() + 3) * Cell::TILE_SIZE) moveVector.x = 0;
  if (moveVector.y > 0 && downRight.y >= (map->getMaxHeight() + 3) * Cell::TILE_SIZE) moveVector.y = 0;
  camera.move(moveVector);
  moveVector = sf::Vector2f();

  for (auto& unit: units){
      unit->updateState();
  }

  fpsCounter.update(dt);
  map->prepare(screen.mapPixelToCoords(sf::Vector2i(0,0)));
}
