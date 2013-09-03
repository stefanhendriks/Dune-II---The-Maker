#ifndef UNIT_H
#define UNIT_H

#include "SDL/SDL.h"
#include "SDL_gfxPrimitives.h"

#include "random.h"
#include "rectangle.h"
#include "surface.h"
#include "map.h"

const int FACING_RIGHT = 0;
const int FACING_UP_RIGHT = 1;
const int FACING_UP = 2;
const int FACING_LEFT_UP = 3;
const int FACING_LEFT = 4;
const int FACING_DOWN_LEFT = 5;
const int FACING_DOWN = 6;
const int FACING_RIGHT_DOWN = 7;

const int FACINGS = 8;          // used to calculate width of each 'tile' for a unit given a tilset

class Unit {

  public:
    Unit(SDL_Surface* tileset, SDL_Surface* shadowset, Map* map);
    Unit(SDL_Surface* tileset, SDL_Surface* shadowset, Map* map, int x, int y);
    ~Unit();

    void draw(SDL_Surface* screen, MapCamera* map_camera);

    void updateState();

    void order_move(Point target) {
      // snap coordinates
      int y = (target.y / TILE_SIZE) * TILE_SIZE;
      int x = (target.x / TILE_SIZE) * TILE_SIZE;
      this->target = Point(x,y);
    }

    void select() { selected = true; }
    void unselect() { selected = false; }
    bool is_selected() { return selected; }

    bool is_point_within(const Point& point) {
      Rectangle current_area = Rectangle(position, (position + size));
      return current_area.is_point_within(point);
    }

    bool is_within(const Rectangle& rectangle) {
      return rectangle.is_point_within(position);
    }

  private:
    SDL_Surface* tileset;
    SDL_Surface* shadowset;
    SDL_Surface* selected_bitmap;
    int body_facing;  // facing, 8 directions. Clock-wise. ie: 0 (up), 1 (up-right), 2 (right), etc;
    int desired_body_facing;

    Point size;

    int shadow_alpha; // how transparant is the shadow being drawn (0 = invisible, 256 is solid)

    int anim_frame;   // animation frames are 'rows' in the tileset

    int offset_x, offset_y; // the offset from the tile respective up-left corner

    void init(SDL_Surface* tileset, SDL_Surface* shadowset, Map *map, int x, int y);

    bool selected;

    Point target;     // target of interest (move/attack, etc)
    Point position;   // coordinates relative to top/left of map (in pixels)
    Point next_move_position;
    Point prev_position;


    int desired_facing();

    void updateMovePosition(Point p) {
      this->next_move_position = this->next_move_position + p;
      this->desired_body_facing = desired_facing();
    }

    bool is_moving() {
      return position != next_move_position;
    }

    bool has_target() {
      return position != target;
    }

    bool should_turn_body() {
      return desired_body_facing != body_facing;
    }

    Map* map;

    void turn_body();

    void moveUp() { updateMovePosition(Point(0, -TILE_SIZE)); }
    void moveDown() { updateMovePosition(Point(0, TILE_SIZE)); }
    void moveLeft() { updateMovePosition(Point(-TILE_SIZE, 0)); }
    void moveRight() { updateMovePosition(Point(TILE_SIZE, 0)); }
    void stopMoving() {
      this->next_move_position = position;
    }

    int getDrawX() { return position.x + offset_x; }
    int getDrawY() { return position.y + offset_y; }
};


// TEMPORARILY HOUSES ARE DEFINED HERE, BY A LACK OF BETTER PLACE (TODO: REFACTOR)
// note, these house numbers are based on the palette indices. As index 144 is harkonnen
// colors and the formula to actually copy the correct colors is based from 144 + house nr
// we just use this as a convenience.
const int HOUSE_HARKONNEN = 0;
const int HOUSE_ATREIDES =  1;
const int HOUSE_ORDOS =  2;
const int HOUSE_SARDAUKAR =  4;
const int HOUSE_FREMEN =  3;
const int HOUSE_MERCENARY =  5;



const int UNIT_QUAD = 0;
const int UNIT_TRIKE = 1;
const int UNIT_DEVASTATOR = 2;
const int UNIT_CARRYALL = 3;
const int UNIT_FRIGATE = 4;
const int MAX_UNIT_TYPES = 16;


class UnitRepository {

  public:
    UnitRepository(Map *map);
    ~UnitRepository();

    void destroy();

    Unit* create(int unitType, int house, int x, int y);

   private:
      SDL_Surface* unit_animation[MAX_UNIT_TYPES];
      SDL_Surface* unit_shadow[MAX_UNIT_TYPES];
      Map* map;
};

#endif
