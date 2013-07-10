#include "SDL.h"
#include "CSurface.h"
#include "map.h"

Map::Map() {
  for (int i = 0; i < 100; i++) {
    cells[i].tile = 0;
  }

  // test if this really works
  cells[40].tile = 1;
}

void Map::draw(SDL_Surface* tileset, SDL_Surface* screen) {

  int max_map_width = 10;

  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 10; y++) {
      int cellIndex = (y * max_map_width) + x;

      Cell c = cells[cellIndex];

      CSurface::drawTile(tileset, screen, c.tile, x * 32, y * 32);
    }
  }

}
