#pragma once

#define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <array>

class Texture;

static constexpr int MAX_SKIRMISHMAP_PLAYERS = 5;

struct s_PreviewMap {
    SDL_Surface *terrain = nullptr;      // terrain bitmap (for showing preview at top right)
    int width, height;              // width & height of map
    std::vector<int> terrainType;   // terrainType per cell (for map creation)
    // int iStartCell[MAX_SKIRMISHMAP_PLAYERS];      // starting locations
    std::array<int, MAX_SKIRMISHMAP_PLAYERS> iStartCell; // starting locations
    bool validMap = true;           // is this a valid map? (is false, when map can be loaded but has invalid data)
    std::string name;               // name of map
    std::string author;             // author of map
    std::string description;        // description of map
    Texture *previewTex = nullptr; // texture for the map preview
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

    std::string getMapSize(int i) const;
    int getMapCount() const {
        return numberOfMaps;
    }

private:
    void loadSkirmish(const std::string &filename);
    void initRandomMap();
    void initPreviews();
    int numberOfMaps = 0; // number of maps loaded

    s_PreviewMap PreviewMap[MAX_SKIRMISHMAPS];

    bool m_debugMode;
};