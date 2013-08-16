#ifndef UNIT_H
#define UNIT_H

#include "SDL/SDL.h"

const int FACING_UP = 0;
const int FACING_UP_RIGHT = 1;
const int FACING_RIGHT = 2;
const int FACING_RIGHT_DOWN = 3;
const int FACING_DOWN = 4;
const int FACING_DOWN_LEFT = 5;
const int FACING_LEFT = 6;
const int FACING_LEFT_UP = 7;

const int FACINGS = 8;          // used to calculate width of each 'tile' for a unit given a tilset

class Unit {

  public:
    Unit(SDL_Surface* tileset, SDL_Surface* shadowset);
    Unit(SDL_Surface* tileset, SDL_Surface* shadowset, int x, int y);
    ~Unit();

    void draw(SDL_Surface* screen, int x, int y);

    int getDrawX() { return x + offset_x; }
    int getDrawY() { return y + offset_y; }

    int width() { return tile_height; }
    int height() { return tile_width; }

    void select() { selected = true; }

  private:
    SDL_Surface* tileset;
    SDL_Surface* shadowset;
    SDL_Surface* selected_bitmap;
    int body_facing;  // facing, 8 directions. Clock-wise. ie: 0 (up), 1 (up-right), 2 (right), etc;

    int tile_width;   // the width is tileset width / 8
    int tile_height;

    int shadow_alpha; // how transparant is the shadow being drawn (0 = invisible, 256 is solid)

    int x, y;         // coordinates relative to top/left of map (in pixels)

    int anim_frame;   // animation frames are 'rows' in the tileset

    int offset_x, offset_y; // the offset from the tile respective up-left corner

    void init(SDL_Surface* tileset, SDL_Surface* shadowset, int x, int y);

    bool selected;
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
    UnitRepository();
    ~UnitRepository();

    void destroy();

    Unit* create(int unitType, int house, int x, int y);

   private:
      SDL_Surface* unit_animation[MAX_UNIT_TYPES];
      SDL_Surface* unit_shadow[MAX_UNIT_TYPES];
};

#endif
