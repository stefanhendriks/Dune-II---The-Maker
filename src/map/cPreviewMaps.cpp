#include "cPreviewMaps.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "map/MapGeometry.hpp"
#include "include/d2tmc.h"
#include "include/Texture.hpp"
#include <filesystem>

namespace fs = std::filesystem;

#include <algorithm>
#include "utils/cIniFile.h"

cPreviewMaps::cPreviewMaps(bool debugMode) : m_debugMode(debugMode)
{

}

void cPreviewMaps::destroy()
{
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        s_PreviewMap &previewMap = PreviewMap[i];
        if (previewMap.terrain) {
            SDL_FreeSurface(previewMap.terrain);
            previewMap.terrain = nullptr;
        }
        if (previewMap.previewTex) {
            delete previewMap.previewTex;
            previewMap.previewTex = nullptr;
        }
    }
}

void cPreviewMaps::loadSkirmish(const std::string &filename)
{
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
        logbook(std::format("Could not load file : {} ", filename));
        return; // skip this map loading
    }

    const cSection &section = conf.getSection("SKIRMISH");
    s_PreviewMap &previewMap = PreviewMap[iNew];
    previewMap.name = section.getStringValue("Title");
    previewMap.author = section.getStringValue("Author");
    previewMap.description = section.getStringValue("Description");

    const cSection &mapSection = conf.getSection("MAP");
    std::vector<std::string> vecmap = mapSection.getData();

    int maxWidth = vecmap[0].size();
    int maxHeight = vecmap.size();
    int maxCells = (maxWidth + 2) * (maxHeight + 2);

    previewMap.terrain = nullptr;
    previewMap.iStartCell.fill(-1); // set all starting cells to -1

    std::vector<int> numbers;
    std::stringstream ss(section.getStringValue("StartCell"));
    std::string segment;
    while(std::getline(ss, segment, ',')) {
        try {
            numbers.push_back(std::stoi(segment));
        } catch (const std::invalid_argument& e) {
            logbook(std::format("Could not convert (invalid argument): {}",segment));
        } catch (const std::out_of_range& e) {
            logbook(std::format("Could not convert (out of bounds): {}",segment));
        }
    }

    for (int i = 0; i < std::min(static_cast<int>(numbers.size()), MAX_SKIRMISHMAP_PLAYERS); i++) {
        try {
            int startCell = numbers[i];
            if (startCell < 0 || startCell >= maxCells) {
                previewMap.validMap = false;
                if (m_debugMode) {
                    logbook(std::format("StartCell [{}] invalid. (value must be between range [0-{}]), Map {}- is invalid" , startCell , maxCells, filename));
                }
            }
            else {
                previewMap.iStartCell[i] = startCell;
            }
        }
        catch (std::invalid_argument const &e) {
            // could not perform conversion
            if (m_debugMode) {
                logbook(std::format("Could not convert startCell [{}] to an int. Reason: {}", numbers[i], e.what()));
            }
        }
    }

    previewMap.width = maxWidth + 2;
    previewMap.height = maxHeight + 2;

    MapGeometry mapGeom(previewMap.width , previewMap.height);
    previewMap.terrainType = std::vector<int>(maxCells, -1);

    if (previewMap.terrain == nullptr) {
        previewMap.terrain = SDL_CreateRGBSurface(0,previewMap.width, previewMap.height,32,0,0,0,255);
    }
    renderDrawer->FillWithColor(previewMap.terrain, Color::black());

    for (int iY = 0; iY < maxHeight; iY++) {
        const char *mapLine = vecmap[iY].c_str();
        for (int iX = 0; iX < maxWidth; iX++) {
            int iCll = mapGeom.makeCell((iX + 1), (iY + 1));
            if (iCll < 0) continue; // skip invalid cells

            Color iColor = Color::white();
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
            int x = mapGeom.getCellX(startCell);
            int y = mapGeom.getCellY(startCell);
            renderDrawer->setPixel(previewMap.terrain, 1 + x, 1 + y, Color::white());
        }
    }
    if (previewMap.terrain!= nullptr){
        SDL_Texture* out = SDL_CreateTextureFromSurface(renderDrawer->getRenderer(), previewMap.terrain);
        if (out == nullptr) {
            logbook(std::format("Error creating texture from surface: {}", SDL_GetError()));
            return;
        }
        previewMap.previewTex = new Texture(out, previewMap.terrain->w, previewMap.terrain->h);
    }
    numberOfMaps++;
    if (m_debugMode) {
        logbook(std::format("Loaded skirmish map: {}, width: {}, height: {}",
                            previewMap.name, previewMap.width, previewMap.height));
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
    numberOfMaps = 0; // reset number of maps
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
    //reset all ressources
    destroy();
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        s_PreviewMap &previewMap = PreviewMap[i];
        // clear out name
        previewMap.name.clear();

        // clear out map data
        previewMap.terrainType.clear();

        // by default we assume this is a valid map
        previewMap.validMap = true;

        previewMap.width = 0;
        previewMap.height = 0;

        previewMap.iStartCell.fill(-1); // set all starting cells to -1
    }
    initRandomMap();
}


void cPreviewMaps::initRandomMap()
{
    s_PreviewMap &firstSkirmishMap = PreviewMap[0];
    firstSkirmishMap.name = "Random map";
    firstSkirmishMap.validMap = false;

    firstSkirmishMap.terrainType = std::vector<int>(1, -1);
    if (firstSkirmishMap.terrain != nullptr) {
        SDL_FreeSurface(firstSkirmishMap.terrain);
    }
    firstSkirmishMap.terrain = nullptr;
    if (firstSkirmishMap.previewTex != nullptr) {
        delete firstSkirmishMap.previewTex;
    }
    firstSkirmishMap.previewTex = nullptr;
}

std::string cPreviewMaps::getMapSize(int i) const {
    if (i > MAX_SKIRMISHMAPS) {
        return "Invalid";
    }
    int playableCells = (PreviewMap[i].width-2) * (PreviewMap[i].height-2);
    if (playableCells < 32 *32) {
        return "SMALL";
    }
    if (playableCells < 64 * 64) {
        return "MEDIUM";
    }
    if (playableCells < 128 * 128) {
        return "LARGE";
    }
    return "HUGE";
}

s_PreviewMap cPreviewMaps::createEmptyMap(const std::string &name, const std::string &author, 
            const std::string &desciption, int width, int height)
{
    s_PreviewMap emptyMap;
    emptyMap.name = name;
    emptyMap.author = author;
    emptyMap.description = desciption;
    emptyMap.width = width;
    emptyMap.height = height;
    emptyMap.validMap = true;
    emptyMap.terrainType = std::vector<int>(width * height, TERRAIN_SAND);
    return emptyMap;
}