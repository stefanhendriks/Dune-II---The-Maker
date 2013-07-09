#include "SDL.h"
#include "CSurface.h"

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
