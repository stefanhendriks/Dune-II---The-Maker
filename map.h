#ifndef MAP_H
#define MAP_H

struct Cell {
  int tile; // tile to draw (one-dimension array)
};

class Map {

   public:
     Map();

     // TODO: camera coordinates
     void draw(SDL_Surface* tileset, SDL_Surface* screen);

   private:
    Cell cells[100]; // 100 = some arbitrary 10x10 map

};

#endif
