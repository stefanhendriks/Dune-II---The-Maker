#ifndef CSURFACE_H
#define CSURFACE_H

class CSurface {

    public:
      CSurface();

      static SDL_Surface* loadBMP(char *file);

      static void draw(SDL_Surface* src, SDL_Surface* dest, int dest_x, int dest_y);


};

#endif
