#include "SDL/SDL.h"
#include "surface.h"
#include "map.h"
#include "random.h"
#include "eventfactory.h"

#include <math.h>       /* ceil */

Map::Map() {
  max_width = MAP_MAX_WIDTH;
  max_height = MAP_MAX_HEIGHT;

  for (int i = 0; i < MAP_MAX_SIZE; i++) {
    cells[i].tile = (flipCoin() ? 0 : 64);
  }

}

void Map::setBoundaries(int max_width, int max_height) {
  this->max_width = max_width;
  this->max_height = max_height;
}

//=============================================================================
//
MapCamera::MapCamera(int x, int y, SDL_Surface* screen, Map* map) {
  this->x = x; // pixel size, relative to map (starts at 0,0)
  this->y = y;
  this->max_cells_width_on_screen = ceil(screen->w / 32);
  this->max_cells_height_on_screen = ceil(screen->h / 32);
  this->map_y_boundary = map->getMaxHeight();
  this->map_x_boundary = map->getMaxWidth();
  this->scroll_speed = 8;
  this->move_x_velocity = 0.0F;
  this->move_y_velocity = 0.0F;
}

void MapCamera::updateState() {
  if (move_x_velocity > CAMERA_MAX_VELOCITY) move_x_velocity = CAMERA_MAX_VELOCITY;
  if (move_y_velocity > CAMERA_MAX_VELOCITY) move_y_velocity = CAMERA_MAX_VELOCITY;

  x += (move_x_velocity * scroll_speed);
  y += (move_y_velocity * scroll_speed);

  makeSureCoordinatesDoNotExceedMapLimits();
}

void MapCamera::makeSureCoordinatesDoNotExceedMapLimits() {
  if (y < 0) y = 0;
  if (y > max_y()) y = max_y();
  if (x < 0) x = 0;
  if (x > max_x()) x = max_x();
}

void MapCamera::draw(Unit* unit, SDL_Surface* screen) {

  // translate x , y into screen coordinates
  int draw_x = unit->getDrawX() - this->x;
  int draw_y = unit->getDrawY() - this->y;

  // TODO: if not visible on camera , do not draw
  unit->draw(screen, draw_x, draw_y);
}

void MapCamera::onEvent(SDL_Event* event) {
  if (event->type == SDL_USEREVENT) {

    if (event->user.code == D2TM_MOVE_CAMERA) {
      D2TMMoveCameraStruct *s = static_cast<D2TMMoveCameraStruct*>(event->user.data1);
      move_x_velocity = s->vec_x;
      move_y_velocity = s->vec_y;
    }

  }
}

void MapCamera::draw(Map* map, SDL_Surface* tileset, SDL_Surface* screen) {
  // determine x and y from map data.
  int startX = (this->x / TILE_SIZE);
  int startY = (this->y / TILE_SIZE);

  int offsetX = (this->x % TILE_SIZE);
  int offsetY = (this->y % TILE_SIZE);

  int endX = startX + (getWidth() + 1);
  int endY = startY + (getHeight() + 1);

  for (int dx = startX; dx < endX; dx++) {
    for (int dy = startY; dy < endY; dy++) {
      Cell c = map->getCell(dx, dy);
      // weird: have to compensate for the coordinates above. Drawing should be done separately
      // from coordinates of map.
      int drawX = (dx - startX) * TILE_SIZE;
      int drawY = (dy - startY) * TILE_SIZE;

      drawX -= offsetX;
      drawY -= offsetY;

      Surface::drawTile(tileset, screen, c.tile, drawX, drawY);
    }
  }
}

