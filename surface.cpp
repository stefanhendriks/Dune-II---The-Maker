#include <iostream>
#include "SDL/SDL.h"
#include <SDL/SDL_image.h>
#include "surface.h"

#include "random.h"

using namespace std;

const int TILE_SIZE = 32; // OH DANG another one

Surface::Surface() {
}

SDL_Surface* Surface::load(std::string file) {
  SDL_Surface* temp = NULL;
  SDL_Surface* result = NULL;

  if((temp = IMG_Load(file.c_str())) == NULL) {
    cerr << "Unable to load file " << file.c_str() << endl;
    return NULL;
  }

  result = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  SDL_SetColorKey(result, SDL_SRCCOLORKEY, SDL_MapRGB(result->format, 255, 0, 255) );

  return result;
}

// creates a new sdl surface, based on the source
SDL_Surface* Surface::copy(SDL_Surface *source) {
  int width = source->w;
  int height = source->h;
  int bits = source->format->BitsPerPixel;

  SDL_Surface* copy = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_HWPALETTE, width, height, bits, NULL, NULL, NULL, NULL);
  if(copy == NULL) {
    cerr << "Failed to create a copy of surface; " << SDL_GetError() << endl;
    return NULL;
  }

  if (bits == 8) {
    SDL_SetColors(copy, source->format->palette->colors, 0, 256);
  } else {
    SDL_FillRect(copy, NULL, SDL_MapRGB(copy->format, 255, 0, 255));
  }

  SDL_SetColorKey(copy, SDL_SRCCOLORKEY, source->format->colorkey);

  Surface::draw(source, copy, 0, 0);

  return copy;
}

SDL_Surface* Surface::load8bit(std::string file) {
  SDL_Surface* result = NULL;

  if((result = IMG_Load(file.c_str())) == NULL) {
    cerr << "(8 bit) Unable to load file " << file.c_str() << endl;
    return NULL;
  }

  SDL_SetColorKey(result, SDL_SRCCOLORKEY, SDL_MapRGB(result->format, 0, 0, 0) );


  // downside is here, no palette conversion is done and thus we have not converted it yet
  // to the display format, causing SDL to do this for us on every blit. Which is slow.
  // TODO: do palette conversion on load

  return result;
}


SDL_Surface* Surface::load(std::string file, int r, int g, int b) {
  SDL_Surface* surf = load(file);
  SDL_SetColorKey(surf, SDL_SRCCOLORKEY, SDL_MapRGB(surf->format, r, g, b) );
  return surf;
}


void Surface::draw(SDL_Surface* src, SDL_Surface* dest, int dest_x, int dest_y) {
  if (src == NULL || dest == NULL) return;

  SDL_Rect destRect;

  destRect.x = dest_x;
  destRect.y = dest_y;

  SDL_BlitSurface(src, NULL, dest, &destRect);
}

// draw transparent surface (note: this is not the same as using a color key)
void Surface::draw(SDL_Surface* src, SDL_Surface* dest, int src_x, int src_y, int width, int height, int dest_x, int dest_y, Uint32 alpha) {
  if (src == NULL || dest == NULL) return;

  SDL_Rect destRect;

  destRect.x = dest_x;
  destRect.y = dest_y;

  SDL_Rect srcRect;

  srcRect.x = src_x;
  srcRect.y = src_y;
  srcRect.h = height;
  srcRect.w = width;

  Uint32 alphaFlag;

  if (alpha < SDL_ALPHA_OPAQUE) {
    alphaFlag = SDL_SRCALPHA;
  } else {
    // this will let SDL ignore alpha drawing
    alphaFlag = 0;
  }

  // Set alpha
  SDL_SetAlpha(src, alphaFlag, alpha);

  SDL_BlitSurface(src, &srcRect, dest, &destRect);

  // reset Alpha
  SDL_SetAlpha(src, 0, alpha);
}

void Surface::draw(SDL_Surface* src, SDL_Surface* dest, int src_x, int src_y, int width, int height, int dest_x, int dest_y) {
  if (src == NULL || dest == NULL) return;

  SDL_Rect destRect;

  destRect.x = dest_x;
  destRect.y = dest_y;

  SDL_Rect srcRect;

  srcRect.x = src_x;
  srcRect.y = src_y;
  srcRect.h = height;
  srcRect.w = width;

  SDL_BlitSurface(src, &srcRect, dest, &destRect);
}

void Surface::drawTile(SDL_Surface* tileset, SDL_Surface* dest, int src_x, int src_y, int dest_x, int dest_y, Uint32 alpha) {
  if (tileset == NULL || dest == NULL) return;
  Surface::draw(tileset, dest, src_x * TILE_SIZE, src_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, dest_x, dest_y, alpha);
}

void Surface::drawTile(SDL_Surface* tileset, SDL_Surface* dest, int src_x, int src_y, int dest_x, int dest_y) {
  if (tileset == NULL || dest == NULL) return;
  Surface::draw(tileset, dest, src_x * TILE_SIZE, src_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, dest_x, dest_y);
}

void Surface::drawIndexedTile(SDL_Surface* tileset, SDL_Surface* dest, int tileIndex, int dest_x, int dest_y, Uint32 alpha) {
  if (tileset == NULL || dest == NULL) return;
  int tiles_width = tileset->w / TILE_SIZE;

  int tileset_y = (tileIndex / tiles_width);
  int tileset_x = (tileIndex - (tileset_y * tiles_width));

  if (alpha < 255) {
    Surface::drawTile(tileset, dest, tileset_x, tileset_y, dest_x, dest_y, alpha);
  } else {
    Surface::drawTile(tileset, dest, tileset_x, tileset_y, dest_x, dest_y);
  }
}

void Surface::drawIndexedTile(SDL_Surface* tileset, SDL_Surface* dest, int tileIndex, int dest_x, int dest_y) {
  drawIndexedTile(tileset, dest, tileIndex, dest_x, dest_y, 255);
}
