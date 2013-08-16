#include "SDL/SDL.h"
#include "surface.h"
#include "map.h"
#include "random.h"
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
}

void MapCamera::draw(Unit* unit, SDL_Surface* screen) {

  // translate x , y into screen coordinates
  int draw_x = unit->getDrawX() - this->x;
  int draw_y = unit->getDrawY() - this->y;

  // TODO: if not on screen, do not draw
  unit->draw(screen, draw_x, draw_y);
}


void MapCamera::draw(Map* map, SDL_Surface* tileset, SDL_Surface* screen) {
  // determine x and y from map data.
  int startX = (this->x / 32);
  int startY = (this->y / 32);

  int offsetX = (this->x % 32);
  int offsetY = (this->y % 32);

  int endX = startX + (getWidth() + 1);
  int endY = startY + (getHeight() + 1);

  for (int dx = startX; dx < endX; dx++) {
    for (int dy = startY; dy < endY; dy++) {
      Cell c = map->getCell(dx, dy);
      // weird: have to compensate for the coordinates above. Drawing should be done separately
      // from coordinates of map.
      int drawX = (dx - startX) * 32;
      int drawY = (dy - startY) * 32;

      drawX -= offsetX;
      drawY -= offsetY;

      Surface::drawTile(tileset, screen, c.tile, drawX, drawY);
    }
  }
}

