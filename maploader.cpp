#include <sstream>
#include <iomanip>
#include <string>

#include "maploader.h"

#include "INIReader.h"

using namespace std;

int MapLoader::load(std::string file, Map* map) {
  INIReader reader(file);

  if (reader.ParseError() < 0) {
    cerr << "[MAPLOADER] ERROR: Failed to load INI file: " << file << endl;
    return ERROR_FILE_NOT_FOUND;
  }

  string title = reader.Get("SKIRMISH", "title", "Unknown map title.");
  cerr << "[MAPLOADER] ERROR: Loading map with title " << title << endl;

  int width = reader.GetInteger("SKIRMISH", "width", -1);
  int height = reader.GetInteger("SKIRMISH", "height", -1);

  if (width < MAP_MIN_WIDTH || height < MAP_MIN_HEIGHT) {
    cerr << "[MAPLOADER] ERROR: Given dimensions " << width << "x" << height << " are below minimum of " << MAP_MIN_WIDTH << "x" << MAP_MAX_WIDTH << "." << endl;
    return ERROR_DIMENSIONS_INVALID;
  }

  int warnings = 0;

  for (int y = 0; y < height; y++) {
    string key = makeIntKeyWithLeadingZero((y + 1));
    string value = reader.Get("DATA", key, "!!!UNKNOWN DATA!!!");

    if (value.at(0) == '!') {
      cerr << "[MAPLOADER] WARNING ("<< warnings << "): Expected to have data for key " << key << " but none was found." << endl;
      warnings++;
      continue;
    }

    for (int x = 0; x < width; x++) {

      if (x >= value.length()) {
        cerr << "[MAPLOADER] WARNING (" << warnings << "): While parsing line (key=" << key << ") " << value << ":" << endl;
        cerr << "[MAPLOADER] WARNING (" << warnings << "): Line was expected to have width " << width << " but is actually " << value.length() << ". Either width/height property is wrong in [SKIRMISH] section or [DATA] has wrong amount of data in this line." << endl;
        warnings++;
        continue;
      }

      char c = value.at(x);
      if (c == 'R') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_MOUNTAIN;
      if (c == '^') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_ROCK;
      if (c == ')') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_SAND;
      if (c == '-') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_SPICE;
      if (c == '+') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_SPICEHILL;
      if (c == 'h') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_HILL;
    }
  }

  cout << "[MAPLOADER] Done reading file with " << warnings << " warnings." << endl;

  if (warnings > 0) return SUCCESS_WITH_WARNINGS;

  return SUCCESS;
}


string MapLoader::makeIntKeyWithLeadingZero(int n) {
  stringstream ss;
  ss << setw(3) << setfill('0') << n;
  return ss.str();
}
