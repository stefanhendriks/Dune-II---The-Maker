#ifndef MAP_H
#define MAP_H

#include <iostream>

#include "SDL/SDL.h"

#include "surface.h"
#include "rectangle.h"
#include "eventfactory.h"

#include <math.h>

#define DEV_DRAWGRID true

class Unit;

using namespace std;

const int MAP_MAX_SIZE = 65536; // 256X256 map
const int MAP_MAX_WIDTH = 256;
const int MAP_MAX_HEIGHT = 256;
const int MAP_MIN_WIDTH = 32;
const int MAP_MIN_HEIGHT = 32;

const int TILE_SIZE = 32; // squared

const int TERRAIN_TYPE_SAND     =  0;
const int TERRAIN_TYPE_HILL     =  1;
const int TERRAIN_TYPE_ROCK     =  2;
const int TERRAIN_TYPE_SPICE    =  3;
const int TERRAIN_TYPE_MOUNTAIN =  4;
const int TERRAIN_TYPE_SPICEHILL = 5;
const int TERRAIN_TYPE_SLAB     =  6;

const int TILES_IN_ROW_ON_TERRAIN_SURFACE = 17;


const int MAP_LAYER_GROUND = 0;
const int MAP_LAYER_AIR = 1;
const int MAP_MAX_LAYERS = 2;

class Cell {
  public:
    int tile; // tile to draw (one-dimension array)
    int terrain_type; // terrain type (sand, rock, etc)
    bool occupied[MAP_MAX_LAYERS];
    bool shrouded;
    int x, y;

    // considers "this" as center opposed to other cell. (ego-centric)
    bool shouldSmoothWithTerrainType(Cell* other) {
      if (this->terrain_type == TERRAIN_TYPE_ROCK) {
        return other->terrain_type != TERRAIN_TYPE_MOUNTAIN &&
               other->terrain_type != TERRAIN_TYPE_ROCK &&
               other->terrain_type != TERRAIN_TYPE_SLAB;
      }
      if (this->terrain_type == TERRAIN_TYPE_MOUNTAIN) {
        return other->terrain_type != TERRAIN_TYPE_MOUNTAIN;
      }
      if (this->terrain_type == TERRAIN_TYPE_SLAB) {
        return other->terrain_type != TERRAIN_TYPE_MOUNTAIN &&
               other->terrain_type != TERRAIN_TYPE_ROCK;
      }
      if (this->terrain_type == TERRAIN_TYPE_SPICE) {
        return other->terrain_type != TERRAIN_TYPE_SPICE &&
               other->terrain_type != TERRAIN_TYPE_SPICEHILL;
      }
      if (this->terrain_type == TERRAIN_TYPE_SPICEHILL) {
        return other->terrain_type != TERRAIN_TYPE_SPICEHILL;
      }
      return other->terrain_type != this->terrain_type;
    }
};

class Map {

  public:
    Map();

    void setBoundaries(int max_width, int max_height);

    void load(string file);

    Cell* getCell(Point map_point) {
      return getCell(map_point.x, map_point.y);
    }

    Cell* getCell(int x, int y) {
      x = min(max(x, 0), (MAP_MAX_WIDTH-1));
      y = min(max(y, 0), (MAP_MAX_HEIGHT-1));
      int cell = (y * MAP_MAX_WIDTH) + x;
      return &cells[cell];
    }

    void occupyCell(const Point& world_point, short layer) {
      getCell(toMapPoint(world_point))->occupied[layer] = true;
    }

    void unOccupyCell(const Point& world_point, short layer) {
      getCell(toMapPoint(world_point))->occupied[layer] = false;
    }

    void removeShroud(Point world_point, int range) {
      Point mapPoint = toMapPoint(world_point);
      int x = mapPoint.x;
      int y = mapPoint.y;
      for (int cell_x = max(x - range, 0); cell_x <= min(x + range, getMaxWidth() -1); cell_x++) {
        for (int cell_y = max(y - range, 0); cell_y <= min(y + range, getMaxHeight() -1); cell_y++) {
          if (pow(cell_x - x, 2) + pow(cell_y - y, 2) <= pow(range, 2) + 1) {
            getCell(cell_x, cell_y)->shrouded = false;
          }
        }
      }
    }

    int getMaxWidth() { return max_width; }
    int getMaxHeight() { return max_height; }

    void setMaxBoundaries(Point boundaries) {
      max_width = boundaries.x + 2;
      max_height = boundaries.y + 2;
      eventFactory.pushMapBoundariesChanged();
    }

    bool is_occupied(Point p, short layer);

    Point toMapPoint(const Point& world_point) {
      Point result(world_point.x / TILE_SIZE, world_point.y / TILE_SIZE);
      return result;
    }

  private:
    Cell cells[MAP_MAX_SIZE];
    EventFactory eventFactory;
    int max_width;
    int max_height;

    void init();
    void determineCellTile(Cell* c);
    void determineCellTileForMap();
    int determineTerrainTile(bool cell_up, bool cell_down, bool cell_left, bool cell_right);
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
    void drawShroud(Map* map, SDL_Surface* shroud_edges, SDL_Surface* shroud_edges_shadow, SDL_Surface* screen);

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

    Map* map;

		int getWidth() { return max_cells_width_on_screen; }
		int getHeight() { return max_cells_height_on_screen; }

    void makeSureCoordinatesDoNotExceedMapLimits();

    void moveUp() { move_y_velocity -= CAMERA_VELOCITY_ACCELERATION; }
    void moveDown() { move_y_velocity += CAMERA_VELOCITY_ACCELERATION; }
    void moveLeft() { move_x_velocity -= CAMERA_VELOCITY_ACCELERATION; }
    void moveRight() { move_x_velocity += CAMERA_VELOCITY_ACCELERATION; }
    void stopMoving() { move_x_velocity = move_y_velocity = 0.0F; }

    int min_x() { return TILE_SIZE; }
    int min_y() { return TILE_SIZE; }
    int max_y() { return (((map_y_boundary - max_cells_height_on_screen) - (1 + 2)) * TILE_SIZE); }
    int max_x() { return (((map_x_boundary - max_cells_width_on_screen) - (1 + 2)) * TILE_SIZE); }

    int determineShroudEdge(Map* map, Cell* c);
};
#endif
