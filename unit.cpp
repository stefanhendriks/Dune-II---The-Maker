#include "unit.h"
#include "surface.h"
#include "random.h"

Unit::Unit(SDL_Surface* tileset) {
  this->tileset = tileset;
  this->body_facing = rnd(FACINGS);
  this->tile_width = tileset->w / FACINGS;
  this->tile_height = 32; // for now assumption. Need to think how to deal with shadow + top facings
}

void Unit::draw(SDL_Surface* screen) {
  int src_x = this->body_facing * this->tile_width;

  Surface::drawTrans(tileset, screen, src_x, 0, this->tile_width, this->tile_height, 0, 0);
}
