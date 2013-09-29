#include <iostream>

#include <SFML/Graphics.hpp>
#include "game.h"



Game::Game():
    playing(true),
    screen(sf::VideoMode(800, 600), "Dune 2 - The Maker", sf::Style::Close),
    map(nullptr)
    //map_camera(nullptr),
    //unitRepository(nullptr)
{
    screen.setFramerateLimit(IDEAL_FPS);
    screen.setMouseCursorVisible(false);

    if (!init()){
        std::cerr << "Failed to initialized game.";
        playing = false;
    }
}

int Game::execute() {

  while(playing) {
    sf::Event event;
    while(screen.pollEvent(event)) {
      onEvent(event);
    }

    updateState();
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

  sf::Image selectedImage;
  selectedImage.loadFromFile("graphics/selected.bmp");
  selectedImage.createMaskFromColor(sf::Color(255, 0, 255));
  sf::Texture* selectedTexture = new sf::Texture; //more leaks!
  selectedTexture->loadFromImage(selectedImage);
  units.emplace_back(new Unit(*trikeTexture, *selectedTexture, 100, 100, 0));

  //remove shroud here
  map->removeShroud(static_cast<sf::Vector2i>(units[0]->getPosition()), 10);

  //shroud_edges_shadow = Surface::load("graphics/shroud_edges_shadow.bmp");

  //if (shroud_edges_shadow == NULL) {
    //cout << "Failed to read graphics/shroud_edges_shadow.bmp data" << endl;
    //return false;
  //}

  //mouse.init(screen);

  //map_camera.reset(new MapCamera(0, 0, screen, &map));
  //map.load("maps/4PL_Mountains.ini");
  //unitRepository.reset(new UnitRepository(&map));

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

  return true;
}

void Game::onEvent(sf::Event event) {

  switch (event.type){
    case sf::Event::Closed:
      playing = false; break;
    case sf::Event::KeyPressed:
      switch (event.key.code) {
        case sf::Keyboard::Q:
          playing = false; break;
        default:
          break;
      }
      break;
  case sf::Event::MouseButtonPressed:
      switch (event.mouseButton.button){
      case sf::Mouse::Left:{
          sf::Vector2f toSet = screen.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
          box.setTopLeft(toSet);
          break;
      }
      default:
          break;
      }
      break;
  case sf::Event::MouseButtonReleased:
      switch (event.mouseButton.button){
      case sf::Mouse::Left:
          for (auto& unit : units){
              if (box.intersects(unit->getBounds())){
                  unit->select();
                  mouse.setType(Mouse::Type::Move); //at least one unit selected...
              }              
          }
          for (auto& unit : units){
              if (unit->is_selected()) unit->order_move(screen.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
          }
          box.clear();
          break;
      case sf::Mouse::Right:
          //deselect all units
          for (auto& unit : units){
              unit->unselect();
              mouse.setType(Mouse::Type::Default);
          }
      default:
          break;
      }
    default:
      break;
  }

  //mouse.onEvent(event);
  //keyboard.onEvent(event);

  //map_camera->onEvent(event);

  //if (event->type == SDL_USEREVENT) {
    //if (event->user.code == D2TM_SELECT) {
      //std::unique_ptr<D2TMSelectStruct> s(static_cast<D2TMSelectStruct*>(event->user.data1));

      //Point p = map_camera->toWorldCoordinates(s->screen_position);

      //if (mouse.is_pointing()) {
        //Unit* selected_unit = NULL;
        //for (auto& unit : units){
            //if (unit->is_point_within(p)){
                //selected_unit = unit.get();
                //break;
            //}

        //}
        //if (selected_unit != NULL) {
          //deselectAllUnits();
          //selected_unit->select();
          //mouse.state_order_move();
        //}
      //}
    //} else if (event->user.code == D2TM_DESELECT) {
      //mouse.state_pointing();
      //deselectAllUnits();
    //} else if (event->user.code == D2TM_BOX_SELECT) {
      //std::unique_ptr<D2TMBoxSelectStruct> s(static_cast<D2TMBoxSelectStruct*>(event->user.data1));

      //Rectangle rectangle = map_camera->toWorldCoordinates(s->rectangle);

      //if (mouse.is_pointing()) {
        //mouse.state_pointing();

        //deselectAllUnits();
        //for (auto& unit : units){
            //if (unit->is_within(rectangle)){
                //unit->select();
                //mouse.state_order_move();
            //}
        //}
      //}
    //} else if (event->user.code == D2TM_MOVE_UNIT) {
      //std::unique_ptr<D2TMMoveUnitStruct> s(static_cast<D2TMMoveUnitStruct*>(event->user.data1));
      //Point p = map_camera->toWorldCoordinates(s->screen_position);

      //for (auto& unit : units){
          //if (unit->is_selected())
              //unit->order_move(p);
      //}
    //}

  //}

}

void Game::render() {
  screen.clear();

  screen.draw(*map);

  for (const auto& unit : units)
      screen.draw(*unit);

  map->drawShrouded(screen, sf::RenderStates::Default);

  screen.draw(box);

  screen.draw(mouse);

  screen.display();
}

void Game::updateState() {
  static const float cameraSpeed = 10.f;
  float vec_x = 0.f, vec_y = 0.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) vec_y -= cameraSpeed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) vec_y += cameraSpeed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) vec_x -= cameraSpeed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) vec_x += cameraSpeed;

  camera.move(vec_x, vec_y);
  screen.setView(camera);

  if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      box.setBottomRight(screen.mapPixelToCoords(sf::Mouse::getPosition(screen)));

  mouse.setPosition(screen.mapPixelToCoords(sf::Mouse::getPosition(screen)));

  //keyboard.updateState();
  //mouse.updateState();
  //map_camera->updateState();

  for (auto& unit: units){
      unit->updateState();
  }
}

//void Game::deselectAllUnits() {
    //for (auto& unit : units)
        //unit->unselect();
//}

//bool Game::playerHasUnits(const Player &player) const
//{
    //for (const auto& unit : units){
        //if (unit->getOwner()==player)
            //return true; //unit belonging to player found
    //}
    //return false;
//}

