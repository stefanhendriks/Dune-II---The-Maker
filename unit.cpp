#include "unit.h"
#include "surface.h"
////
Unit::Unit(SDL_Surface* tileset) {
  this->tileset = tileset;
}

void Unit::draw(SDL_Surface* screen) {
  //Surface::draw(tileset, screen, 0, 0, 32, 32, 0, 0, SDL_ALPHA_OPAQUE);
  //
  Surface::drawTrans(tileset, screen, 0, 0, 32, 32, 0, 0);
}
