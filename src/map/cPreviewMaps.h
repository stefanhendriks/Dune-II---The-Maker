#pragma once

#define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps

#include <allegro.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>

static constexpr int MAX_SKIRMISHMAP_PLAYERS = 5;

struct s_PreviewMap {
    BITMAP *terrain = nullptr;      // terrain bitmap (for showing preview at top right)
    int width, height;              // width & height of map
    std::vector<int> terrainType;   // terrainType per cell (for map creation)
    int iStartCell[MAX_SKIRMISHMAP_PLAYERS];      // starting locations
    bool validMap = true;           // is this a valid map? (is false, when map can be loaded but has invalid data)
    std::string name;               // name of map
};

class cPreviewMaps {
public:
    cPreviewMaps(bool debugMode);

    ~cPreviewMaps() = default;

    void loadSkirmishMaps();
    void destroy();

    s_PreviewMap &getMap(int i) {
        if (i > MAX_SKIRMISHMAPS) {
            return PreviewMap[0];
        }
        return PreviewMap[i];
    }

private:
    void loadSkirmish(const std::string &filename);

    void initPreviews();

    s_PreviewMap PreviewMap[MAX_SKIRMISHMAPS];

    bool m_debugMode;
};