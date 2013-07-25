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

    void draw(SDL_Surface* screen, int x, int y);

    int getX() { return x; }
    int getY() { return y; }

  private:
    SDL_Surface* tileset;
    SDL_Surface* shadowset;
    int body_facing;  // facing, 8 directions. Clock-wise. ie: 0 (up), 1 (up-right), 2 (right), etc;

    int tile_width;   // the width is tileset width / 8
    int tile_height;  // the height is tileset height / 2 (expecting shadow as second row)

    int shadow_alpha; // how transparant is the shadow being drawn (0 = invisible, 256 is solid)

    int x, y;         // coordinates relative to top/left of map (in pixels)

    int anim_frame;   // animation frames are 'rows' in the tileset

    void init(SDL_Surface* tileset, SDL_Surface* shadowset, int x, int y);

};

#endif
