#ifndef Surface_H
#define Surface_H

#include <string>

class Surface {

    public:
      Surface();

      static SDL_Surface* load(std::string file);
      static SDL_Surface* load(std::string file, int r, int g, int b);

      static void draw(SDL_Surface* src, SDL_Surface* dest, int dest_x, int dest_y);
      static void draw(SDL_Surface* src, SDL_Surface* dest, int src_x, int src_y, int width, int height, int dest_x, int dest_y);
      static void draw(SDL_Surface* src, SDL_Surface* dest, int src_x, int src_y, int width, int height, int dest_x, int dest_y, Uint32 alpha);

      static void drawTile(SDL_Surface* tileset, SDL_Surface* dest, int src_x, int src_y, int dest_x, int dest_y);

      // draw a tile , based on the input width/height (and fixed 32x32 tilesize) it is calculated which tile to draw
      static void drawTile(SDL_Surface* tileset, SDL_Surface* dest, int tileIndex, int dest_x, int dest_y);
};

#endif
