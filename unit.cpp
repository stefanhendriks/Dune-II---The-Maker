#include "unit.h"
#include "surface.h"
#include "random.h"

Unit::Unit(SDL_Surface* tileset) {
  this->tileset = tileset;
  this->body_facing = rnd(FACINGS);
  this->tile_width = tileset->w / FACINGS;
  this->tile_height = 32; // for now assumption. Need to think how to deal with shadow + top facings
  this->shadow_alpha = 128;
  this->x = rnd(256);
  this->y = rnd(256);
}

void Unit::draw(SDL_Surface* screen, int x, int y) {
  int src_x = this->body_facing * this->tile_width;
  int src_y = this->tile_height * 1;
  Surface::draw(tileset, screen, src_x, 0, this->tile_width, this->tile_height, x, y);
  Surface::draw(tileset, screen, src_x, src_y, this->tile_width, this->tile_height, x, y, this->shadow_alpha);
}
