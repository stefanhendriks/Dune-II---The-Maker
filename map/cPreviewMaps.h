#pragma once

#define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps

#include <allegro.h>
#include <vector>
#include <string>

struct s_PreviewMap {
     BITMAP *terrain = nullptr;        // terrain
     int width, height;      // width & height of map
     std::vector<int> mapdata;
     int iPlayers;           // players
     int iStartCell[5];      // starting locations
     std::string name;       // name
};

class cPreviewMaps
{
public:
    cPreviewMaps();
    ~cPreviewMaps();
    void prescanSkirmish();
    s_PreviewMap& getMap(int i)  { if (i > MAX_SKIRMISHMAPS) return PreviewMap[0]; return PreviewMap[i]; }
private:
    void loadSkirmish(const std::string& filename);
    void initPreviews();
    s_PreviewMap PreviewMap[MAX_SKIRMISHMAPS];
};