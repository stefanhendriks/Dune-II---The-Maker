#ifndef MAP_LOADER
#define MAP_LOADER

#include <string>

#include "map.h"

const int SUCCESS = 0;
const int ERROR_FILE_NOT_FOUND = 1;
const int ERROR_DIMENSIONS_INVALID = 2;
const int ERROR_FILE_MISSING_DATA = 3;
const int SUCCESS_WITH_WARNINGS = 4;

class MapLoader {

  public:
    static int load(std::string file, Map* map);

  private:
    static Terrain charToTerrainType(char c);

};

#endif
