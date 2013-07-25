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
    Unit(SDL_Surface* tileset);

    void draw(SDL_Surface* screen);

  private:
    SDL_Surface* tileset;
    int body_facing; // facing, 8 directions. Clock-wise. ie: 0 (up), 1 (up-right), 2 (right), etc;

    int tile_width; // the width is tileset width / 8
    int tile_height; // the height is tileset height / 2 (expecting shadow as second row)
    

};


#endif
