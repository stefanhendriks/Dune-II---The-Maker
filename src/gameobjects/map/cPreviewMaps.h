#pragma once

// #define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <array>
#include <memory>

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
    cPreviewMaps();

    void setRenderDrawer(SDLDrawer *renderDrawer);

    ~cPreviewMaps() = default;

    void loadSkirmishMaps();
    void destroy();

    s_PreviewMap *getMap(int i);

    std::string getMapSize(int i) const;
    int getMapCount() const;

    void createEmptyMap(const std::string &name, const std::string &author,
            const std::string &desciption, int width, int height);

    void loadSkirmish(const std::string &filename);
private:
    void initRandomMap();
    void initPreviews();
    std::unique_ptr<s_PreviewMap> m_emptyMap;

    std::vector<std::unique_ptr<s_PreviewMap>> m_PreviewMap;
    SDLDrawer * m_renderDrawer = nullptr;
};