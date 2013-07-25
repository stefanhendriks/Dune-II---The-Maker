#include <iostream>

#include "unit.h"
#include "surface.h"
#include "random.h"

using namespace std;

Unit::Unit(SDL_Surface* tileset) {
  init(tileset, 128 + rnd(256), 128 + rnd(256));
}

Unit::Unit(SDL_Surface* tileset, int x, int y) {
  init(tileset, x, y);
}

void Unit::draw(SDL_Surface* screen, int x, int y) {
  int src_x = this->body_facing * this->tile_width;
  int src_y = this->tile_height * 1;
  Surface::draw(tileset, screen, src_x, 0, this->tile_width, this->tile_height, x, y);
  Surface::draw(tileset, screen, src_x, src_y, this->tile_width, this->tile_height, x, y, this->shadow_alpha);
}

void Unit::init(SDL_Surface* tileset, int x, int y) {
  this->tileset = tileset;
  this->body_facing = rnd(FACINGS);
  this->tile_width = tileset->w / FACINGS;
  // check if our assumption (width==height) is true for this tileset, and if not warn the user.
  if (tileset->h % this->tile_width > 0) {
    cerr << "WARNING: Tileset must have tiles with same height as width, assuming thats the case.";
    cerr << "(Tile width is " << this->tile_width << "(Will divide " << tileset->h << " with " << this->tile_width << " makes " << (tileset->h / this->tile_width) << ") which would be ok if height where " << (tileset->h / this->tile_width) * this->tile_width << ")";
    cerr << endl;
  }
  this->tile_height = this->tile_width;
  this->shadow_alpha = 128;
  this->x = x;
  this->y = y;
}
