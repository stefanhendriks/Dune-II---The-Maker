#ifndef CSURFACE_H
#define CSURFACE_H

class CSurface {

    public:
      CSurface();

      static SDL_Surface* loadBMP(char *file);

      static void draw(SDL_Surface* src, SDL_Surface* dest, int dest_x, int dest_y);

      static void draw(SDL_Surface* src, SDL_Surface* dest, int src_x, int src_y, int width, int height, int dest_x, int dest_y);
};

#endif
