#ifndef MAP_H
#define MAP_H

#include <iostream>

#include "rectangle.h"
#include <math.h>

class Unit;

using namespace std;

const int MAP_MAX_SIZE = 65536; // 256X256 map
const int MAP_MAX_WIDTH = 256;
const int MAP_MAX_HEIGHT = 256;

const int TILE_SIZE = 32; // squared

const int UNIT_VIEW_RANGE = 5;

class Cell {
  public:
    int tile; // tile to draw (one-dimension array)
    bool occupied;
    bool isShrouded;
};

class Map {

  public:
    Map();

    void setBoundaries(int max_width, int max_height);

    Cell* getCell(int x, int y) {
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
      return &cells[cell];
    }

    void occupyCell(int x, int y) {
      getCell(x, y)->occupied = true;
    }

    void unOccupyCell(int x, int y) {
      getCell(x, y)->occupied = false;
    }

    void removeShroud(int x, int y) {
      for (int cell_x = max(x - UNIT_VIEW_RANGE, 0); cell_x <= min(x + UNIT_VIEW_RANGE, getMaxWidth() -1); cell_x++) {
        for (int cell_y = max(y - UNIT_VIEW_RANGE, 0); cell_y <= min(y + UNIT_VIEW_RANGE, getMaxHeight() -1); cell_y++) {
          if ( pow(cell_x - x,2) + pow(cell_y - y,2) <= pow(UNIT_VIEW_RANGE,2) ) {
            getCell(cell_x, cell_y)->isShrouded = false;
          }
        }
      }
    }

    int getMaxWidth() { return max_width; }
    int getMaxHeight() { return max_height; }

    bool is_occupied(Point p);

  private:
    Cell cells[MAP_MAX_SIZE];
    int max_width;
    int max_height;

};

const float CAMERA_VELOCITY_ACCELERATION = 0.25f;
const float CAMERA_MAX_VELOCITY = 2.5f;

class MapCamera {

	public:
		MapCamera(int x, int y, SDL_Surface* screen, Map* map);

    void onEvent(SDL_Event* event);

    void updateState();

    void draw(Map* map, SDL_Surface* tileset, SDL_Surface* screen);
    void draw(Unit* unit, SDL_Surface* screen);
    void drawShroud(Map* map, SDL_Surface* screen);

    Point toScreenCoordindates(const Point& point_with_world_coords) {
      int screen_x = screenCoordinateX(point_with_world_coords.x);
      int screen_y = screenCoordinateY(point_with_world_coords.y);
      Point result(screen_x, screen_y);
      return result;
    }

    Point toWorldCoordinates(const Point& point_with_screen_coords) {
      int world_x = worldCoordinateX(point_with_screen_coords.x);
      int world_y = worldCoordinateY(point_with_screen_coords.y);
      Point result(world_x, world_y);
      return result;
    }

    Rectangle toWorldCoordinates(const Rectangle& rect_with_screen_points) {
      Point start = toWorldCoordinates(rect_with_screen_points.start);
      Point end = toWorldCoordinates(rect_with_screen_points.end);
      Rectangle result(start, end);
      return result;
    }

    // todo: REMOVE THESE FROM PUBLIC
    int screenCoordinateX(int world_x) { return world_x - this->x; }
    int screenCoordinateY(int world_y) { return world_y - this->y; }
    int worldCoordinateX(int x) { return this->x + x; };
    int worldCoordinateY(int y) { return this->y + y; };

	private:
		int x, y;
		int max_cells_width_on_screen;
		int max_cells_height_on_screen;
    int map_y_boundary;
    int map_x_boundary;
    int scroll_speed;

    float move_x_velocity;
    float move_y_velocity;


		int getWidth() { return max_cells_width_on_screen; }
		int getHeight() { return max_cells_height_on_screen; }

    void makeSureCoordinatesDoNotExceedMapLimits();

    void moveUp() { move_y_velocity -= CAMERA_VELOCITY_ACCELERATION; }
    void moveDown() { move_y_velocity += CAMERA_VELOCITY_ACCELERATION; }
    void moveLeft() { move_x_velocity -= CAMERA_VELOCITY_ACCELERATION; }
    void moveRight() { move_x_velocity += CAMERA_VELOCITY_ACCELERATION; }
    void stopMoving() { move_x_velocity = move_y_velocity = 0.0F; }

    int max_y() { return (map_y_boundary - max_cells_height_on_screen) * 32; }
    int max_x() { return (map_x_boundary - max_cells_width_on_screen) * 32; }
};
#endif
