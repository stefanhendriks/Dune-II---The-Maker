#include <sstream>
#include <iomanip>
#include <string>
#include <vector>

#include "MapLoader.hpp"

#include "INIReader.hpp"

using namespace std;

// Borrowed from: http://stackoverflow.com/questions/890164/how-can-i-split-a-string-by-a-delimiter-into-an-array
std::vector<string> explode(const string& str, const char& ch) {
  string next;
  vector<string> result;

  // For each character in the string
  for (string::const_iterator it = str.begin(); it != str.end(); it++) {
    // If we've hit the terminal character
    if (*it == ch) {
      // If we have some characters accumulated
      if (!next.empty()) {
        // Add them to the result vector
        result.push_back(next);
        next.clear();
      }
    } else {
      // Accumulate the next character into the sequence
      next += *it;
    }
  }
  if (!next.empty())
    result.push_back(next);
  return result;
}

int MapLoader::load(std::string file, Map* map) {
  INIReader reader(file);

  if (reader.ParseError() < 0) {
    cerr << "[MAPLOADER] ERROR: Failed to load INI file: " << file << endl;
    return ERROR_FILE_NOT_FOUND;
  }

  string title = reader.Get("SKIRMISH", "title", "Unknown map title.");
  cout << "[MAPLOADER] INFO: Loading map with title " << title << endl;

  int width = reader.GetInteger("SKIRMISH", "width", -1);
  int height = reader.GetInteger("SKIRMISH", "height", -1);

  if (width < Map::MIN_WIDTH || height < Map::MIN_HEIGHT) {
    cerr << "[MAPLOADER] ERROR: Given dimensions " << width << "x" << height << " are below minimum of " << Map::MIN_WIDTH << "x" << Map::MAX_WIDTH << "." << endl;
    return ERROR_DIMENSIONS_INVALID;
  }

  map->setBoundaries(width, height);

  string data = reader.Get("SKIRMISH", "data", "ERROR");
  if (data.at(0) == 'E') {
    cerr << "[MAPLOADER] ERROR: Expected to have a multi-line value for key 'data', but no data found." << endl;
    return ERROR_FILE_MISSING_DATA;
  }

  std::vector<std::string> result = explode(data, '\n');

  int warnings = 0;

  for (int y = 0; y < height; y++) {
    if (y > (int)result.size()) {
      cerr << "[MAPLOADER] WARNING ("<< warnings++ << "): Wanted to read data at line " << y << ", but there is no data. Map incomplete!?" << endl;
      break;
    }

    string value = result[y];

    for (int x = 0; x < width; x++) {

      if (x >= (int)value.length()) {
        cerr << "[MAPLOADER] WARNING (" << warnings << "): While parsing line " << value << ":" << endl;
        cerr << "[MAPLOADER] WARNING (" << warnings << "): Line was expected to have width " << width << " but is actually " << value.length() << ". Either width/height property is wrong in [SKIRMISH] section or [DATA] has wrong amount of data in this line." << endl;
        warnings++;
        continue;
      }

      Terrain terrain_type = charToTerrainType(value.at(x));
      Cell& cell = map->getCell(x + 1, y + 1); // compensate for invisible border
      cell.setType(terrain_type);

    }
  }

  std::cout << "[MAPLOADER] Done reading file with " << warnings << " warnings." << std::endl;

  if (warnings > 0) return SUCCESS_WITH_WARNINGS;

  return SUCCESS;
}

Terrain MapLoader::charToTerrainType(char c) {
  if (c == 'R') return Terrain::Mountain;
  if (c == '^') return Terrain::Rock;
  if (c == ')') return Terrain::Sand;
  if (c == '-') return Terrain::Spice;
  if (c == '+') return Terrain::Spicehill;
  if (c == 'h') return Terrain::Hill;
  std::cerr << "[MAPLOADER] WARNING: Unable to map character " << c << " to a terrain type. Known chars are R^)-+h" << std::endl;
  return Terrain::Sand; //returns Sand as default - alternative error handling possible
}
