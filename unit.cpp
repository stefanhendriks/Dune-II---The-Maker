#include <iostream>

#include "unit.h"
#include "surface.h"
#include "random.h"

using namespace std;

Unit::Unit(SDL_Surface* tileset, SDL_Surface* shadowset) {
  init(tileset, shadowset, 128 + rnd(256), 128 + rnd(256));
}

Unit::Unit(SDL_Surface* tileset, SDL_Surface* shadowset, int x, int y) {
  init(tileset, shadowset, x, y);
}

Unit::~Unit() {
  SDL_FreeSurface(tileset);
  SDL_FreeSurface(shadowset);
}

void Unit::draw(SDL_Surface* screen, int x, int y) {
  int src_x = this->body_facing * this->tile_width;
  int src_y = this->tile_height * this->anim_frame;
  Surface::draw(tileset, screen, src_x, src_y, this->tile_width, this->tile_height, x, y);
//Surface::draw(shadowset, screen, src_x, src_y, this->tile_width, this->tile_height, x, y, this->shadow_alpha);
}

void Unit::init(SDL_Surface* tileset, SDL_Surface* shadowset, int x, int y) {
  this->tileset = tileset;
  this->shadowset = shadowset;
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
  this->anim_frame = 0;
}



//////////////////////////////////////////
// Unit Repository
//

UnitRepository::UnitRepository() {
  int house = rnd(6);
  int paletteIndex = 144 + (16 * house);
  quad = Surface::load8bit("graphics/Unit_Quad.bmp");
  SDL_SetColors(quad, &quad->format->palette->colors[paletteIndex], 144, 8);

  quad_shadow = Surface::load("graphics/Unit_Quad_s.bmp", 255, 0, 255);
}

UnitRepository::~UnitRepository() {
  SDL_FreeSurface(quad);
  SDL_FreeSurface(quad_shadow);
}

Unit* UnitRepository::create(int unitType, int house, int x, int y) {
  SDL_Surface* src = quad;
  //SDL_Surface* src = Surface::copy(quad);
  Unit* unit = new Unit(src, NULL);
  return unit;
}
