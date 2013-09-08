#include <sstream>
#include <iomanip>

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

  for (int i = 0; i < height; i++) {
    string key = makeIntKeyWithLeadingZero(i);
    string value = reader.Get("DATA", key, "!!!UNKNOWN DATA!!!");
    cout << "Line with key " << key << " has value [" << value << "]" << endl;


  }

}


string MapLoader::makeIntKeyWithLeadingZero(int n) {
  stringstream ss;
  ss << setw(3) << setfill('0') << n;
  return ss.str();
}
