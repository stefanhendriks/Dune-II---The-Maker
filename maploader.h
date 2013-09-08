#ifndef MAP_LOADER
#define MAP_LOADER

#include <string>

#include "rectangle.h"
#include "map.h"

class MapLoader {

  public:
    static void load(std::string file, Map* map);

  private:
    static string makeIntKeyWithLeadingZero(int n);

};

#endif
