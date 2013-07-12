#include "SDL/SDL.h"
#include "CSurface.h"
#include "map.h"

Map::Map() {
  max_width = MAP_MAX_WIDTH;
  max_height = MAP_MAX_HEIGHT;

  for (int i = 0; i < MAP_MAX_SIZE; i++) {
    cells[i].tile = 0;
  }

  // test if this really works
  cells[40].tile = 1;
}

void Map::setBoundaries(int max_width, int max_height) {
  this->max_width = max_width;
  this->max_height = max_height;
}

void Map::draw(SDL_Surface* tileset, SDL_Surface* screen) {

  int max_cells_width_on_screen = screen->w / 32;
  int max_cells_height_on_screen = screen->h / 32;

  for (int x = 0; x < max_cells_width_on_screen; x++) {
    for (int y = 0; y < max_cells_height_on_screen; y++) {
      int cellIndex = Cell::getCellIndex(x, y);
      
      Cell c = cells[cellIndex];

      CSurface::drawTile(tileset, screen, c.tile, x * 32, y * 32);
    }
  }

}
