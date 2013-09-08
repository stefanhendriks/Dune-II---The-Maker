#include <sstream>
#include <iomanip>
#include <string>

#include "maploader.h"

#include "INIReader.h"

using namespace std;

void MapLoader::load(std::string file, Map* map) {
  INIReader reader(file);

  if (reader.ParseError() < 0) {
    cerr << "Failed to load INI file: " << file << endl;
    return;
  }

  string title = reader.Get("SKIRMISH", "title", "Unknown map title.");
  cout << "Loading map with title " << title << endl;

  int width = reader.GetInteger("SKIRMISH", "width", map->getMaxWidth());
  int height = reader.GetInteger("SKIRMISH", "height", map->getMaxHeight());

  cout << "Dimensions are " << width << "x" << height << "." << endl;

  for (int y = 0; y < height; y++) {
    string key = makeIntKeyWithLeadingZero((y + 1));
    string value = reader.Get("DATA", key, "!!!UNKNOWN DATA!!!");

    if (value.at(0) == '!') {
      cerr << "ERROR: Expected to have data for key " << key << " but none was found." << endl;
      continue;
    }

    for (int x = 0; x < width; x++) {

      if (x >= value.length()) {
        cerr << "ERROR: While parsing line (key=" << key << ") " << value << ":" << endl;
        cerr << "Line was expected to have width " << width << " but is actually " << value.length() << ". Either width/height property is wrong in [SKIRMISH] section or [DATA] has wrong amount of data in this line." << endl;
        continue;
      }

      char c = value.at(x);
      if (c == 'R') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_MOUNTAIN;
      if (c == '^') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_ROCK;
      if (c == ')') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_SAND;
      if (c == '-') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_SPICE;
      if (c == '+') map->getCell(x, y)->terrain_type = TERRAIN_TYPE_SPICEHILL;
    }
  }

}


string MapLoader::makeIntKeyWithLeadingZero(int n) {
  stringstream ss;
  ss << setw(3) << setfill('0') << n;
  return ss.str();
}
