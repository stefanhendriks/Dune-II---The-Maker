#ifndef MAP_LOADER
#define MAP_LOADER

#include <string>

#include "rectangle.h"
#include "map.h"

const int SUCCESS = 0;
const int ERROR_FILE_NOT_FOUND = 1;
const int ERROR_DIMENSIONS_INVALID = 2;
const int SUCCESS_WITH_WARNINGS = 3;

class MapLoader {

  public:
    static int load(std::string file, Map* map);

  private:
    static string makeIntKeyWithLeadingZero(int n);
    static int charToTerrainType(char c);

};

#endif
