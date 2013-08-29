#ifndef MAP_H
#define MAP_H

#include <iostream>

#include "unit.h"

using namespace std;

const int MAP_MAX_SIZE = 65536; // 256X256 map
const int MAP_MAX_WIDTH = 256;
const int MAP_MAX_HEIGHT = 256;

class Cell {
  public:
    int tile; // tile to draw (one-dimension array)
};

class Map {

   public:
     Map();

	   void setBoundaries(int max_width, int max_height);

     Cell getCell(int x, int y) {
       if (x < 0) {
         cerr << "Map::getCell x[" << x << "] got out of bounds, fixing." << endl;
         x = 0;
       }
       if (x >= MAP_MAX_WIDTH) {
         cerr << "Map::getCell x[" << x << "] got out of bounds, fixing." << endl;
         x = (MAP_MAX_WIDTH - 1); // 0 based so substract! (0 till 255):
       }

       if (y < 0) {
         cerr << "Map::getCell y[" << y << "] got out of bounds, fixing." << endl;
         y = 0;
       }
       if (y >= MAP_MAX_HEIGHT) {
         cerr << "Map::getCell y[" << y << "] got out of bounds, fixing." << endl;
         y = (MAP_MAX_HEIGHT - 1); // 0 based so substract! (0 till 255):
       }

       int cell = (y * MAP_MAX_WIDTH) + x;
       return cells[cell];
     }

     int getMaxWidth() { return max_width; }
     int getMaxHeight() { return max_height; }

   private:
    Cell cells[MAP_MAX_SIZE];
	  int max_width;
	  int max_height;

};

class MapCamera {

	public:
		MapCamera(int x, int y, SDL_Surface* screen, Map* map);

    void onEvent(SDL_Event* event);

    void moveUp() { for (int i = 0; i < scroll_speed; i++) { if (y > 0) y -= 1; } }
    void moveDown() { for (int i = 0; i < scroll_speed; i++) { if (y < max_y()) y += 1; } }
    void moveLeft() { for (int i = 0; i < scroll_speed; i++) { if (x > 0) x-= 1; } }
    void moveRight() { for (int i = 0; i < scroll_speed; i++) { if (x < max_x()) x+= 1; } }

    void draw(Map* map, SDL_Surface* tileset, SDL_Surface* screen);
    void draw(Unit* unit, SDL_Surface* screen);

    int worldCoordinateX(int x) { return this->x + x; };
    int worldCoordinateY(int y) { return this->y + y; };

	private:
		int x, y;
		int max_cells_width_on_screen;
		int max_cells_height_on_screen;
    int map_y_boundary;
    int map_x_boundary;
    int scroll_speed;

		int getWidth() { return max_cells_width_on_screen; }
		int getHeight() { return max_cells_height_on_screen; }

    int max_y() { return (map_y_boundary - max_cells_height_on_screen) * 32; }
    int max_x() { return (map_x_boundary - max_cells_width_on_screen) * 32; }
};
#endif
