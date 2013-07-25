#include <iostream>
#include "SDL/SDL.h"
#include "SDL_image.h"
#include "surface.h"

using namespace std;

const int TILE_SIZE = 32; // squared

Surface::Surface() {
}

SDL_Surface* Surface::load(std::string File) {
  SDL_Surface* temp = NULL;
  SDL_Surface* result = NULL;

  if((temp = IMG_Load(File.c_str())) == NULL) {
    return NULL;
  }

  result = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  return result;
}

void Surface::draw(SDL_Surface* src, SDL_Surface* dest, int dest_x, int dest_y) {
  if (src == NULL || dest == NULL) return;

  SDL_Rect destRect;

  destRect.x = dest_x;
  destRect.y = dest_y;

  SDL_BlitSurface(src, NULL, dest, &destRect);
}


void Surface::drawTrans(SDL_Surface* src, SDL_Surface* dest, int src_x, int src_y, int width, int height, int dest_x, int dest_y) {
  if (src == NULL || dest == NULL) return;

  SDL_Rect destRect;

  destRect.x = dest_x;
  destRect.y = dest_y;

  SDL_Rect srcRect;

  srcRect.x = src_x;
  srcRect.y = src_y;
  srcRect.h = height;
  srcRect.w = width;

  int r = 255, g = 0, b = 255;
  int result = SDL_SetColorKey(src, SDL_SRCCOLORKEY, SDL_MapRGB(src->format, r, g, b) );

  if (result != 0) {
    cout << "Error setting SDL Color key" << endl;
  }

  SDL_BlitSurface(src, &srcRect, dest, &destRect);
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

void Surface::drawTile(SDL_Surface* tileset, SDL_Surface* dest, int src_x, int src_y, int dest_x, int dest_y) {
  if (tileset == NULL || dest == NULL) return;
  Surface::draw(tileset, dest, src_x * TILE_SIZE, src_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, dest_x, dest_y);
}

void Surface::drawTile(SDL_Surface* tileset, SDL_Surface* dest, int tileIndex, int dest_x, int dest_y) {
  if (tileset == NULL || dest == NULL) return;
  int tiles_width = tileset->w / TILE_SIZE;
  int tiles_height = tileset->h / TILE_SIZE;

  int tileset_y = (tileIndex / tiles_width);
  int tileset_x = tileIndex - (tileset_y * tiles_height);

  Surface::drawTile(tileset, dest, tileset_x, tileset_y, dest_x, dest_y);
}
