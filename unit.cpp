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
  Surface::draw(shadowset, screen, src_x, src_y, this->tile_width, this->tile_height, x, y, this->shadow_alpha);
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
//////////////////////////////////////////

UnitRepository::UnitRepository() {
  for (int i = 0; i < MAX_UNIT_TYPES; i++) {
    unit_animation[i] = NULL;
    unit_shadow[i] = NULL;
  }

  unit_animation[UNIT_QUAD] = Surface::load8bit("graphics/Unit_Quad.bmp");
  unit_shadow[UNIT_QUAD] = Surface::load("graphics/Unit_Quad_s.bmp", 255, 0, 255);

  unit_animation[UNIT_DEVASTATOR] = Surface::load8bit("graphics/Unit_Devastator.bmp");
  unit_shadow[UNIT_DEVASTATOR] = Surface::load("graphics/Unit_Devastator_s.bmp", 255, 0, 255);

}

UnitRepository::~UnitRepository() {
  for (int i = 0; i < MAX_UNIT_TYPES; i++) {
    SDL_FreeSurface(unit_animation[i]);
    SDL_FreeSurface(unit_shadow[i]);
  }
}

Unit* UnitRepository::create(int unitType, int house, int x, int y) {
  SDL_Surface* copy = Surface::copy(unit_animation[unitType]);
  int paletteIndexUsedForColoring = 144;
  int paletteIndex = paletteIndexUsedForColoring + (16 * house);
  SDL_SetColors(copy, &copy->format->palette->colors[paletteIndex], paletteIndexUsedForColoring, 8);

  SDL_Surface* shadow_copy = Surface::copy(unit_shadow[unitType]);
  Unit* unit = new Unit(copy, shadow_copy);
  return unit;
}
