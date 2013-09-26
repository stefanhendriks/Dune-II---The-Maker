#include <iostream>

#include <SFML/Graphics.hpp>
#include "game.h"

Game::Game():
    playing(true),
    screen(sf::VideoMode(800, 600), "Dune 2 - The Maker"),
    map(nullptr)
    //map_camera(nullptr),
    //unitRepository(nullptr)
{
    screen.setFramerateLimit(60); //comment out for fps benchmarks

    init();
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

int Game::init() {
  if (!terrain.loadFromFile("graphics/terrain.png")) {
    std::cout << "Failed to read graphics/terrain.png data" << std::endl;
    return false;
  }
  map.reset(new Map(terrain));
  map->load("maps/4PL_Mountains.ini");

  //shroud_edges = Surface::load("graphics/shroud_edges.bmp");

  //if (shroud_edges == NULL) {
    //cout << "Failed to read graphics/shroud_edges.bmp data" << endl;
    //return false;
  //}

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
  if (event.type == sf::Event::Closed) {
    playing = false;
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

  //map_camera->draw(&map, terrain, screen);

  //for (auto& unit : units){
      //if (unit->is_on_ground_layer()) map_camera->draw(unit.get(), screen);
  //}
  //for (auto& unit : units){
      //if (unit->is_on_air_layer()) map_camera->draw(unit.get(), screen);
  //}

  //map_camera->drawShroud(&map, shroud_edges, shroud_edges_shadow, screen);

  //mouse.draw(screen);

  screen.display();
}

void Game::updateState() {
  //keyboard.updateState();
  //mouse.updateState();
  //map_camera->updateState();

  //for (auto& unit: units){
      //unit->updateState();
  //}
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

