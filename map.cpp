#include "map.h"
#include "maploader.h"
#include "random.h"
//#include "eventfactory.h"
//#include "unit.h"

#include <memory> //std::unique_ptr
#include <cmath>       /* ceil */

Map::Map(sf::Texture &terrain, sf::Texture &shroud_edges) :
  terrain(terrain),
  shroud_edges(shroud_edges)
{
  max_width = MAX_WIDTH - 1;
  max_height = MAX_HEIGHT - 1;

  for (int x = 0; x < MAX_WIDTH; x++) {
    for (int y = 0; y < MAX_HEIGHT; y++) {
      int i = (y * MAX_WIDTH) + x;
      cells[i].init(terrain, shroud_edges, x, y);
    }
  }
}

void Map::load(std::string file) {
  MapLoader::load(file, this);
  for (auto& cell : cells) {
      cell.setIndex(determineCellTile(&cell));      
  }
  updateShroud();
}

void Map::updateShroud()
{
    for (auto& cell : cells)
        cell.setShroudIndex(determineShroudEdge(&cell));
}

void Map::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (auto& cell : cells)
        target.draw(cell);
}

void Map::drawShrouded(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (auto& cell : cells)
        cell.drawShrouded(target, states);
}

int Map::determineCellTile(Cell* c) {
  bool cell_up = !c->shouldSmoothWithTerrainType(getCell(c->x, c->y-1));
  bool cell_down = !c->shouldSmoothWithTerrainType(getCell(c->x, c->y+1));
  bool cell_left = !c->shouldSmoothWithTerrainType(getCell(c->x-1, c->y));
  bool cell_right = !c->shouldSmoothWithTerrainType(getCell(c->x+1, c->y));

  return determineTerrainTile(cell_up, cell_down, cell_left, cell_right);
}

int Map::determineTerrainTile(bool cell_up, bool cell_down, bool cell_left, bool cell_right) {
  if ( cell_up &&  cell_down &&  cell_left &&  cell_right) return 0;
  if ( cell_up &&  cell_down && !cell_left &&  cell_right) return 1;
  if ( cell_up &&  cell_down &&  cell_left && !cell_right) return 2;
  if (!cell_up &&  cell_down &&  cell_left &&  cell_right) return 3;
  if ( cell_up && !cell_down &&  cell_left &&  cell_right) return 4;
  if (!cell_up &&  cell_down && !cell_left &&  cell_right) return 5;
  if ( cell_up && !cell_down &&  cell_left && !cell_right) return 6;
  if (!cell_up &&  cell_down &&  cell_left && !cell_right) return 7;
  if ( cell_up && !cell_down && !cell_left &&  cell_right) return 8;
  if (!cell_up && !cell_down && !cell_left && !cell_right) return 9;
  if (!cell_up && !cell_down &&  cell_left &&  cell_right) return 10;
  if (!cell_up && !cell_down &&  cell_left && !cell_right) return 11;
  if (!cell_up && !cell_down && !cell_left &&  cell_right) return 12;
  if ( cell_up && !cell_down && !cell_left && !cell_right) return 13;
  if (!cell_up &&  cell_down && !cell_left && !cell_right) return 14;
  if ( cell_up &&  cell_down && !cell_left && !cell_right) return 15;
  return -1;
}

int Map::determineShroudEdge(Cell* c) {
  if (c->shrouded) return 0;

  bool cell_up = getCell(c->x, c->y-1)->shrouded;
  bool cell_down = getCell(c->x, c->y+1)->shrouded;
  bool cell_left = getCell(c->x-1, c->y)->shrouded;
  bool cell_right = getCell(c->x+1, c->y)->shrouded;

  // Its harder then you think to make static consts for these 'magic values'.
  if (!cell_up && !cell_down && !cell_left && !cell_right)  return -1;
  if ( cell_up && !cell_down &&  cell_left && !cell_right)  return 1;
  if ( cell_up && !cell_down && !cell_left && !cell_right)  return 2;
  if ( cell_up && !cell_down && !cell_left &&  cell_right)  return 3;
  if (!cell_up && !cell_down && !cell_left &&  cell_right)  return 4;
  if (!cell_up &&  cell_down && !cell_left &&  cell_right)  return 5;
  if (!cell_up &&  cell_down && !cell_left && !cell_right)  return 6;
  if (!cell_up &&  cell_down &&  cell_left && !cell_right)  return 7;
  if (!cell_up && !cell_down &&  cell_left && !cell_right)  return 8;
  if ( cell_up &&  cell_down &&  cell_left && !cell_right)  return 9;
  if ( cell_up && !cell_down &&  cell_left &&  cell_right)  return 10;
  if ( cell_up &&  cell_down && !cell_left &&  cell_right)  return 11;
  if (!cell_up &&  cell_down &&  cell_left &&  cell_right)  return 12;

  return -1;
}


void Map::setBoundaries(int max_width, int max_height) {
  this->max_width = max_width;
  this->max_height = max_height;
}

bool Map::is_occupied(sf::Vector2i p, short layer) {
  int map_x = p.x / Cell::TILE_SIZE;
  int map_y = p.y / Cell::TILE_SIZE;
  Cell* c = getCell(map_x, map_y);
  return c->occupied[layer];
}


//=============================================================================
//
//MapCamera::MapCamera(int x, int y, SDL_Surface* screen, Map* map) {
  //this->x = x; // pixel size, relative to map (starts at 0,0)
  //this->y = y;
  //this->map = map;
  //this->max_cells_width_on_screen = ceil(screen->w / 32);
  //this->max_cells_height_on_screen = ceil(screen->h / 32);
  //this->map_y_boundary = map->getMaxHeight();
  //this->map_x_boundary = map->getMaxWidth();
  //this->scroll_speed = 8;
  //this->move_x_velocity = 0.0F;
  //this->move_y_velocity = 0.0F;
//}

//void MapCamera::updateState() {
  //if (move_x_velocity > CAMERA_MAX_VELOCITY) move_x_velocity = CAMERA_MAX_VELOCITY;
  //if (move_y_velocity > CAMERA_MAX_VELOCITY) move_y_velocity = CAMERA_MAX_VELOCITY;

  //x += (move_x_velocity * scroll_speed);
  //y += (move_y_velocity * scroll_speed);

  //makeSureCoordinatesDoNotExceedMapLimits();
//}

//void MapCamera::makeSureCoordinatesDoNotExceedMapLimits() {
  //if (y < min_y()) y = min_y();
  //if (y > max_y()) y = max_y();
  //if (x < min_x()) x = min_x();
  //if (x > max_x()) x = max_x();
//}

//void MapCamera::draw(Unit* unit, SDL_Surface* screen) {
  //// TODO: if not visible on camera , do not draw
  //unit->draw(screen, this);
//}

//void MapCamera::onEvent(SDL_Event* event) {
  //if (event->type == SDL_USEREVENT) {

    //if (event->user.code == D2TM_MOVE_CAMERA) {
      //std::unique_ptr<D2TMMoveCameraStruct> s(static_cast<D2TMMoveCameraStruct*>(event->user.data1));
      //move_x_velocity = s->vector.x;
      //move_y_velocity = s->vector.y;
    //} else if (event->user.code == D2TM_MAP_BOUNDARIES_CHANGED) {
      //this->map_y_boundary = map->getMaxHeight();
      //this->map_x_boundary = map->getMaxWidth();
    //}

  //}
//}
