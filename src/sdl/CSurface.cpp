#include "SDL.h"
#include "CSurface.h"

const int TILE_SIZE = 32; // squared

CSurface::CSurface() {
}

SDL_Surface* CSurface::loadBMP(char* File) {
    SDL_Surface* temp = NULL;
    SDL_Surface* result = NULL;

    if((temp = SDL_LoadBMP(File)) == NULL) {
            return NULL;
        }

    result = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    return result;
}

void CSurface::draw(SDL_Surface* src, SDL_Surface* dest, int dest_x, int dest_y) {
  if (src == NULL || dest == NULL) return;

  SDL_Rect destRect;

  destRect.x = dest_x;
  destRect.y = dest_y;

  SDL_BlitSurface(src, NULL, dest, &destRect);
}

void CSurface::draw(SDL_Surface* src, SDL_Surface* dest, int src_x, int src_y, int width, int height, int dest_x, int dest_y) {
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

void CSurface::drawTile(SDL_Surface* tileset, SDL_Surface* dest, int src_x, int src_y, int dest_x, int dest_y) {
  if (tileset == NULL || dest == NULL) return;
  CSurface::draw(tileset, dest, src_x * TILE_SIZE, src_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, dest_x, dest_y);
}
