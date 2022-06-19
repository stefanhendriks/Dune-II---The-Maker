#include "cPreviewMaps.h"
#include "data/gfxdata.h"
#include "map/cMap.h"

#include <filesystem>

namespace fs = std::filesystem;

#include <iostream>
#include <algorithm>
#include "utils/cIniFile.h"

cPreviewMaps::cPreviewMaps(bool debugMode) : m_debugMode(debugMode) {

}

/**
 * Make sure to destroy all Allegro bitmaps
 */
void cPreviewMaps::destroy() {
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        s_PreviewMap &previewMap = PreviewMap[i];
        if (previewMap.terrain) {
            destroy_bitmap(previewMap.terrain);
            previewMap.terrain = nullptr;
        }
    }
}

void cPreviewMaps::loadSkirmish(const std::string &filename) {
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
    int maxCells = (maxWidth + 1) * (maxHeight + 1);

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
            } else {
                previewMap.iStartCell[i] = startCell;
            }
        } catch (std::invalid_argument const &e) {
            // could not perform conversion
            if (m_debugMode) {
                std::cerr << "Could not convert startCell [" << string << "] to an int. Reason:" << e.what() << "\n";
            }
        }
    }

    previewMap.width = maxWidth + 1;
    previewMap.height = maxHeight + 1;

    cMap map;
    map.init(maxWidth + 1, maxHeight + 1);

    previewMap.terrainType = std::vector<int>(maxCells, -1);

    if (previewMap.terrain == nullptr) {
        previewMap.terrain = create_bitmap(128, 128);
    }

    clear_bitmap(previewMap.terrain);

    for (int iY = 0; iY < maxHeight; iY++) {
        const char *mapLine = vecmap[iY].c_str();
        for (int iX = 0; iX < maxWidth; iX++) {
            int iCll = map.makeCell((iX + 1), (iY + 1));
            if (iCll < 0) continue; // skip invalid cells

            int iColor = -1;
            int terrainType = -1; // unknown

            char letter = mapLine[iX];
            if (letter == ')') {
                terrainType = TERRAIN_SAND;
                iColor = makecol(194, 125, 60);
            }
            // rock
            if (letter == '%' || letter == '^' || letter == '&' || letter == '(') {
                terrainType = TERRAIN_ROCK;
                iColor = makecol(80, 80, 60);
            }
            // mountain
            if (letter == 'R' || letter == 'r') {
                terrainType = TERRAIN_MOUNTAIN;
                iColor = makecol(48, 48, 36);
            }
            // spice hill
            if (letter == '+') {
                terrainType = TERRAIN_SPICEHILL;
                iColor = makecol(180, 90, 25); // bit darker
            }
            // spice
            if (letter == '-') {
                terrainType = TERRAIN_SPICE;
                iColor = makecol(186, 93, 32);
            }
            // sand hill
            if (letter == 'H' || letter == 'h') {
                terrainType = TERRAIN_HILL;
                iColor = makecol(188, 115, 50);
            }

            if (terrainType < 0 || iColor < 0) {
                logbook(fmt::format(
                    "iniLoader::skirmish() - Could not determine terrain type for char \"{}\", falling back to SAND",
                    letter));
                terrainType = TERRAIN_SAND;
                iColor = makecol(255, 255, 255); // show as purple to indicate wrong char
            }

            previewMap.terrainType[iCll] = terrainType;
            putpixel(previewMap.terrain, 1 + (iX * 2), 1 + (iY * 2), iColor);
            putpixel(previewMap.terrain, 1 + (iX * 2) + 1, 1 + (iY * 2), iColor);
            putpixel(previewMap.terrain, 1 + (iX * 2) + 1, 1 + (iY * 2) + 1, iColor);
            putpixel(previewMap.terrain, 1 + (iX * 2), 1 + (iY * 2) + 1, iColor);
        }
    }

    // starting points
    for (int i = 0; i < 5; i++) {
        int startCell = previewMap.iStartCell[i];
        if (startCell > -1) {
            int x = map.getCellX(startCell);
            int y = map.getCellY(startCell);
            putpixel(previewMap.terrain, 1 + (x * 2), 1 + (y * 2), makecol(255, 255, 255));
            putpixel(previewMap.terrain, 1 + (x * 2) + 1, 1 + (y * 2), makecol(255, 255, 255));
            putpixel(previewMap.terrain, 1 + (x * 2) + 1, 1 + (y * 2) + 1, makecol(255, 255, 255));
            putpixel(previewMap.terrain, 1 + (x * 2), 1 + (y * 2) + 1, makecol(255, 255, 255));
        }
    }
}

/*
Scanning of skirmish maps:

- for each file in the folder "skirmish"
- if extension is .ini, open file (else skip)
- load the file, into a sPreviewMap struct
  - also creates a preview of map in BITMAP (minimap preview)
*/
void cPreviewMaps::prescanSkirmish() {
    // scans for all ini files
    initPreviews(); // clear all of them

    const std::filesystem::path pathfile{"skirmish"};
    for (auto const &file: std::filesystem::directory_iterator{pathfile}) {
        auto fullname = file.path().string();
        if (file.path().extension() == ".ini") {
            loadSkirmish(fullname);
            logbook(fmt::format("Loading skirmish map: {}", fullname));
        }
    }
}

// Skirmish map initialization
void cPreviewMaps::initPreviews() {
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        s_PreviewMap &previewMap = PreviewMap[i];

        if (previewMap.terrain != nullptr) {
            destroy_bitmap(previewMap.terrain);
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
    //PreviewMap[0].terrain = (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat;
    firstSkirmishMap.terrain = create_bitmap(128, 128);
}
