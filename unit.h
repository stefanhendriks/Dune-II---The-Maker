#ifndef UNIT_H
#define UNIT_H

#include "SDL/SDL.h"

class Unit {

  public:
    Unit(SDL_Surface* tileset);

    void draw(SDL_Surface* screen);

  private:
    SDL_Surface* tileset;


};


#endif
