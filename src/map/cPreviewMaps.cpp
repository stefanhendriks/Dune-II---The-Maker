#include "cPreviewMaps.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "map/cMap.h"
#include "include/d2tmc.h"
#include "include/Texture.hpp"
#include <filesystem>

namespace fs = std::filesystem;

#include <iostream>
#include <algorithm>
#include "utils/cIniFile.h"

cPreviewMaps::cPreviewMaps(bool debugMode) : m_debugMode(debugMode)
{

}

/**
 * Make sure to destroy all Allegro bitmaps
 */
void cPreviewMaps::destroy()
{
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        s_PreviewMap &previewMap = PreviewMap[i];
        if (previewMap.terrain) {
            SDL_FreeSurface(previewMap.terrain);
            previewMap.terrain = nullptr;
        }
    }
}

void cPreviewMaps::loadSkirmish(const std::string &filename)
{
    // std::cout << filename << std::endl;

    int iNew = -1;
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        if (PreviewMap[i].name[0] == '\0') {
            iNew = i;
            break;
        }
    }

    if (iNew < 0) {
        return;
    }

    cIniFile conf = cIniFile(filename, m_debugMode);
    if (!conf.isLoadSuccess()) {
        return; // skip this map loading
    }

    const cSection &section = conf.getSection("SKIRMISH");
    s_PreviewMap &previewMap = PreviewMap[iNew];
    previewMap.name = section.getStringValue("Title");

    const cSection &mapSection = conf.getSection("MAP");
    std::vector<std::string> vecmap = mapSection.getData();

    int maxWidth = vecmap[0].size();
    int maxHeight = vecmap.size();
    int maxCells = (maxWidth + 2) * (maxHeight + 2);

    //ugly code to transform "1254,5421,4523" to 1254 , 5421 , 4523
    for (int i = 0; i < 5; i++) {
        previewMap.iStartCell[i] = -1;
        previewMap.terrain = nullptr;
    }

    for (int i = 0; i < MAX_SKIRMISHMAP_PLAYERS; i++) {
        if (!section.hasValue("StartCell", i)) continue;

        const std::string &string = section.getStringValue("StartCell", i);
        try {
            int startCell = std::stoi(string);
            if (startCell < 0 || startCell >= maxCells) {
                previewMap.validMap = false;
                if (m_debugMode) {
                    std::cerr << "StartCell [" << startCell << "] invalid. (value must be between range [0-" << maxCells << ")]. For map " << filename << " - marking map as invalid.\n";
                }
            }
            else {
                previewMap.iStartCell[i] = startCell;
            }
        }
        catch (std::invalid_argument const &e) {
            // could not perform conversion
            if (m_debugMode) {
                std::cerr << "Could not convert startCell [" << string << "] to an int. Reason:" << e.what() << "\n";
            }
        }
    }

    previewMap.width = maxWidth + 2;
    previewMap.height = maxHeight + 2;

    cMap map;
    map.init(previewMap.width , previewMap.height);

    previewMap.terrainType = std::vector<int>(maxCells, -1);

    if (previewMap.terrain == nullptr) {
        previewMap.terrain = SDL_CreateRGBSurface(0,previewMap.width, previewMap.height,32,0,0,0,255);
    }
    renderDrawer->FillWithColor(previewMap.terrain, Color::black());

    for (int iY = 0; iY < maxHeight; iY++) {
        const char *mapLine = vecmap[iY].c_str();
        for (int iX = 0; iX < maxWidth; iX++) {
            int iCll = map.makeCell((iX + 1), (iY + 1));
            if (iCll < 0) continue; // skip invalid cells

            Color iColor = Color::white(); //Color{255,255,255,255};
            int terrainType = -1; // unknown

            char letter = mapLine[iX];
            if (letter == ')') {
                terrainType = TERRAIN_SAND;
                iColor = Color{194, 125, 60,255};
            }
            // rock
            if (letter == '%' || letter == '^' || letter == '&' || letter == '(') {
                terrainType = TERRAIN_ROCK;
                iColor = Color{80, 80, 60,255};
            }
            // mountain
            if (letter == 'R' || letter == 'r') {
                terrainType = TERRAIN_MOUNTAIN;
                iColor = Color{48, 48, 36,255};
            }
            // spice hill
            if (letter == '+') {
                terrainType = TERRAIN_SPICEHILL;
                iColor = Color{180, 90, 25,255}; // bit darker
            }
            // spice
            if (letter == '-') {
                terrainType = TERRAIN_SPICE;
                iColor = Color{186, 93, 32,255};
            }
            // sand hill
            if (letter == 'H' || letter == 'h') {
                terrainType = TERRAIN_HILL;
                iColor = Color{188, 115, 50,255};
            }

            if (terrainType < 0) {
                logbook(std::format(
                            "iniLoader::skirmish() - Could not determine terrain type for char \"{}\", falling back to SAND",
                            letter));
                terrainType = TERRAIN_SAND;
                iColor = Color{160, 32, 240, 255}; // show as purple to indicate wrong char
            }

            previewMap.terrainType[iCll] = terrainType;
            renderDrawer->setPixel(previewMap.terrain, 1 + iX, 1 + iY, iColor);
        }
    }

    // starting points
    for (int i = 0; i < 5; i++) {
        int startCell = previewMap.iStartCell[i];
        if (startCell > -1) {
            int x = map.getCellX(startCell);
            int y = map.getCellY(startCell);
            renderDrawer->setPixel(previewMap.terrain, 1 + x, 1 + y, Color::white());
        }
    }
    if (previewMap.terrain!= nullptr){
        SDL_Texture* out = SDL_CreateTextureFromSurface(renderDrawer->getRenderer(), previewMap.terrain);
        if (out == nullptr) {
            std::cerr << "Error creating texture from surface: " << SDL_GetError() << std::endl;
            return;
        }
        previewMap.previewTex = new Texture(out, previewMap.terrain->w, previewMap.terrain->h);
    }
}

/*
Scanning of skirmish maps:

- for each file in the folder "skirmish"
- if extension is .ini, open file (else skip)
- load the file, into a sPreviewMap struct
  - also creates a preview of map in BITMAP (minimap preview)
*/
void cPreviewMaps::loadSkirmishMaps()
{
    // scans for all ini files
    initPreviews(); // clear all of them

    const std::filesystem::path pathfile{"skirmish"};
    for (auto const &file: std::filesystem::directory_iterator{pathfile}) {
        auto fullname = file.path().string();
        if (file.path().extension() == ".ini") {
            loadSkirmish(fullname);
            logbook(std::format("Loading skirmish map: {}", fullname));
        }
    }
}

// Skirmish map initialization
void cPreviewMaps::initPreviews()
{
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        s_PreviewMap &previewMap = PreviewMap[i];

        if (previewMap.terrain != nullptr) {
            SDL_FreeSurface(previewMap.terrain);
            previewMap.terrain = nullptr;
        }

        // clear out name
        previewMap.name.clear();

        // clear out map data
        previewMap.terrainType.clear();

        // by default we assume this is a valid map
        previewMap.validMap = true;

        previewMap.width = 0;
        previewMap.height = 0;

        previewMap.iStartCell[0] = -1;
        previewMap.iStartCell[1] = -1;
        previewMap.iStartCell[2] = -1;
        previewMap.iStartCell[3] = -1;
        previewMap.iStartCell[4] = -1;
    }

    int maxCells = 128 * 128;
    s_PreviewMap &firstSkirmishMap = PreviewMap[0];

    firstSkirmishMap.terrainType = std::vector<int>(maxCells, -1);
    firstSkirmishMap.name = "Random map";
    firstSkirmishMap.terrain = SDL_CreateRGBSurface(0,128, 128,32,0,0,0,255);
}
