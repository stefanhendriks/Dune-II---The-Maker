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
  cells[10].tile = 64;
}

void Map::setBoundaries(int max_width, int max_height) {
  this->max_width = max_width;
  this->max_height = max_height;
}

void Map::draw(SDL_Surface* tileset, SDL_Surface* screen, MapCamera * map_camera) {

  for (int x = map_camera->getX(); x < map_camera->getWidth(); x++) {
    for (int y = map_camera->getY(); y < map_camera->getHeight(); y++) {
      int cellIndex = Cell::getCellIndex(x, y);
      
      Cell c = cells[cellIndex];

		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);

		int drawX = x * 32;
		int drawY = y * 32;

      CSurface::drawTile(tileset, screen, c.tile, drawX, drawY);

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

//=============================================================================
//
MapCamera::MapCamera(int x, int y, SDL_Surface * screen) {
	this->x = x;
	this->y = y;
   this->max_cells_width_on_screen = screen->w / 32;
   this->max_cells_height_on_screen = screen->h / 32;
}

