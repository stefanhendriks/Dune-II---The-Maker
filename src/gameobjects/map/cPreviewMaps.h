#pragma once

// #define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <array>

class Texture;
class SDLDrawer;

static constexpr int MAX_SKIRMISHMAP_PLAYERS = 5;

struct s_PreviewMap {
    SDL_Surface *terrain = nullptr;      // terrain bitmap (for showing preview at top right)
    int width, height;              // width & height of map
    std::vector<int> terrainType;   // terrainType per cell (for map creation)
    std::array<int, MAX_SKIRMISHMAP_PLAYERS> iStartCell; // starting locations
    bool validMap = true;           // is this a valid map? (is false, when map can be loaded but has invalid data)
    std::string name;               // name of map
    std::string author;             // author of map
    std::string description;        // description of map
    Texture *previewTex = nullptr;  // texture for the map preview
};

class cPreviewMaps {
public:
    explicit cPreviewMaps(SDLDrawer *renderDrawer);

    ~cPreviewMaps() = default;

    void loadSkirmishMaps();
    void destroy();

    s_PreviewMap &getMap(int i) {
        if (i > MAX_SKIRMISHMAPS_CAPACITY) {
            return m_PreviewMap[0];
        }
        return m_PreviewMap[i];
    }

    std::string getMapSize(int i) const;
    int getMapCount() const {
        return m_numberOfMaps+1;
    }

    static s_PreviewMap createEmptyMap(const std::string &name, const std::string &author,
            const std::string &desciption, int width, int height);

private:
    void loadSkirmish(const std::string &filename);
    void initRandomMap();
    void initPreviews();
    int m_numberOfMaps = 0; // Review Stefan 25/10/2025 -> Replace with size of array m_PreviewMap?
    static constexpr int MAX_SKIRMISHMAPS_CAPACITY = 300;

    std::array<s_PreviewMap, MAX_SKIRMISHMAPS_CAPACITY> m_PreviewMap;
    SDLDrawer * m_renderDrawer;
};