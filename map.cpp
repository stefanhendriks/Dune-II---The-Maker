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

  // REFACTOR:
  if (isInRect(draw_x, draw_y, unit->width(), unit->height())) {
    SDL_Rect rect;
    rect.w = unit->width();
    rect.h = unit->height();
    rect.x = draw_x;
    rect.y = draw_y;
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format , 128 , 128 , 128 ) );

    if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)) {
      unit->select();
    }
  }

  // REFACTOR: (dafuq?)
  if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)) {
    // C&C way deselecting!
    unit->unselect();
  }

  // TODO: if not on screen, do not draw
  unit->draw(screen, draw_x, draw_y);
}

bool MapCamera::isInRect(int x, int y, int width, int height) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  return (mouse_x >= x && mouse_x < (x + width)) && (mouse_y >= y && mouse_y < (y + height));
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

      int mouse_x, mouse_y;
      SDL_GetMouseState(&mouse_x, &mouse_y);

      // weird: have to compensate for the coordinates above. Drawing should be done separately
      // from coordinates of map.
      int drawX = (dx - startX) * 32;
      int drawY = (dy - startY) * 32;

      drawX -= offsetX;
      drawY -= offsetY;

      Surface::drawTile(tileset, screen, c.tile, drawX, drawY);

      if ((mouse_x >= drawX && mouse_x <= (drawX + 32)) && (mouse_y >= drawY && mouse_y <= (drawY + 32))) {
	      SDL_Rect rect;
	      rect.w = 32;
	      rect.h = 32;
	      rect.x = drawX;
	      rect.y = drawY;
	      SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format , 0 , 0 , 20 ) );
      }
    }
  }
}

