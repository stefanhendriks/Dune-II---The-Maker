#ifndef UNIT_H
#define UNIT_H

#include "SDL/SDL.h"
#include <SDL/SDL_gfxPrimitives.h>

#include "random.h"
#include "rectangle.h"
#include "surface.h"
#include "map.h"
#include "unit_move_behavior.h"
#include <memory>

const int FACING_RIGHT = 0;
const int FACING_UP_RIGHT = 1;
const int FACING_UP = 2;
const int FACING_LEFT_UP = 3;
const int FACING_LEFT = 4;
const int FACING_DOWN_LEFT = 5;
const int FACING_DOWN = 6;
const int FACING_RIGHT_DOWN = 7;
const int FACINGS = 8;          // used to calculate width of each 'tile' for a unit given a tilset

// TEMPORARILY HOUSES ARE DEFINED HERE, BY A LACK OF BETTER PLACE (TODO: REFACTOR)
// note, these house numbers are based on the palette indices. As index 144 is harkonnen
// colors and the formula to actually copy the correct colors is based from 144 + house nr
// we just use this as a convenience.
enum class House
{
    Harkonnen = 0,
    Atreides = 1,
    Ordos = 2,
    Fremen = 3,
    Sardaukar = 4,
    Mercenary = 5
};

class Unit {

  public:
    Unit(SDL_Surface* tileset, SDL_Surface* shadowset, Map* map, UnitMoveBehavior* move_behavior);
    Unit(SDL_Surface* tileset, SDL_Surface* shadowset, Map* map, UnitMoveBehavior* move_behavior, int x, int y, int viewRange);
    ~Unit();

    void draw(SDL_Surface* screen, MapCamera* map_camera);
    void updateState();

    void order_move(Point target);

    void select();
    void unselect();
    bool is_selected();
    bool is_within(const Rectangle& rectangle);
    bool is_on_air_layer();
    bool is_on_ground_layer();
    bool is_point_within(const Point& point);

  private:
    SDL_Surface* tileset;
    SDL_Surface* shadowset;
    SDL_Surface* selected_bitmap;

    Point target;           // target of interest (move/attack, etc)
    Point position;         // coordinates relative to top/left of map (in pixels)
    Point next_move_position;
    Point prev_position;
    Point size;

    int shadow_alpha;       // how transparant is the shadow being drawn (0 = invisible, 256 is solid)
    int anim_frame;         // animation frames are 'rows' in the tileset
    int offset_x, offset_y; // the offset from the tile respective up-left corner
    int view_range;
    int body_facing;        // facing, 8 directions. Clock-wise. ie: 0 (up), 1 (up-right), 2 (right), etc;
    int desired_body_facing;

    bool selected;

    Map* map;
    std::unique_ptr<UnitMoveBehavior> move_behavior;

    void init(SDL_Surface* tileset, SDL_Surface* shadowset, Map *map, int x, int y, int viewRange, UnitMoveBehavior* move_behavior);

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void turn_body();
    void updateMovePosition(Point p);

    int  desired_facing();
    bool is_moving();
    bool has_target();
    bool should_turn_body();
    void stopMoving();

    int  getDrawX();
    int  getDrawY();

    bool isOnLayer(short layer);
};


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

    Unit* create(int unitType, House house, int x, int y, int viewRange);

   private:
      SDL_Surface* unit_animation[MAX_UNIT_TYPES];
      SDL_Surface* unit_shadow[MAX_UNIT_TYPES];
      Map* map;
};

#endif
