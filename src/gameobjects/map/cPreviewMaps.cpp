/**
 * @file cPreviewMaps.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cPreviewMaps.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/map/MapGeometry.hpp"
#include "include/Texture.hpp"
#include "utils/common.h"
#include "utils/Log.h"
#include "utils/cIniFile.h"
#include <filesystem>
#include <fstream>
#include <cctype>
#include <string>

namespace fs = std::filesystem;

#include <algorithm>
#include "include/cAssert.h"
#include <format>
// #include <iostream>

cPreviewMaps::cPreviewMaps() :
    m_emptyMap(std::make_unique<PreviewMapView>()),
    m_PreviewMap()
{
    //initPreviews();
}

s_PreviewMap *cPreviewMaps::getMap(int i)
{
    return &getView(i)->map;
}

PreviewMapView *cPreviewMaps::getView(int i)
{
    if (i == -1) {
        return m_emptyMap.get();
    }
    if (i < 0 || i >= static_cast<int>(m_PreviewMap.size()) || m_PreviewMap[i] == nullptr) {
        if (m_PreviewMap.empty() || m_PreviewMap[0] == nullptr) {
            return m_emptyMap.get();
        }
        return m_PreviewMap[0].get();
    }
    return m_PreviewMap[i].get();
}

bool cPreviewMaps::isValidMap(int i) const
{
    if (i < 0 || i >= static_cast<int>(m_PreviewMap.size()) || m_PreviewMap[i] == nullptr) {
        return false;
    }
    return m_PreviewMap[i]->map.validMap;
}

int cPreviewMaps::getMapCount() const
{
    return static_cast<int>(m_PreviewMap.size()-1); // -1 because the first map is reserved for the random map, which is not a real skirmish map
}

void cPreviewMaps::setSDLDrawer(SDLDrawer *sdlDrawer)
{
    m_sdlDrawer = sdlDrawer;
}

void cPreviewMaps::drawPreview(const s_PreviewMap &previewMap, SDL_Surface *surface) const
{
    if (surface == nullptr || surface->w != previewMap.width || surface->h != previewMap.height) return;
    if (static_cast<int>(previewMap.terrainType.size()) < previewMap.width * previewMap.height) return;

    m_sdlDrawer->FillWithColor(surface, Color::Black);

    for (int y = 0; y < previewMap.height; y++) {
        for (int x = 0; x < previewMap.width; x++) {
            int cellType = previewMap.terrainType[(y * previewMap.width) + x];
            if (cellType < 0) continue; // not part of the map (border), stays black

            Color color = Color{194, 125, 60,255};
            if (cellType == TERRAIN_ROCK) color = Color{80, 80, 60,255};
            if (cellType == TERRAIN_MOUNTAIN) color = Color{48, 48, 36,255};
            if (cellType == TERRAIN_SPICEHILL) color = Color{180, 90, 25,255}; // a bit darker
            if (cellType == TERRAIN_SPICE) color = Color{186, 93, 32,255};
            if (cellType == TERRAIN_HILL) color = Color{188, 115, 50,255};
            m_sdlDrawer->setPixel(surface, x, y, color);
        }
    }

    // starting points
    for (int startCell : previewMap.iStartCell) {
        if (startCell > -1) {
            m_sdlDrawer->setPixel(surface, startCell % previewMap.width, startCell / previewMap.width, Color::White);
        }
    }
}

void cPreviewMaps::destroy()
{
    // std::cout << "Destroying preview maps" << std::endl;
    for (auto& previewMap : m_PreviewMap) {
        if (previewMap == nullptr) {
            continue;
        }
        if (previewMap->terrain) {
            SDL_DestroySurface(previewMap->terrain);
            previewMap->terrain = nullptr;
        }
        if (previewMap->previewTex) {
            delete previewMap->previewTex;
            previewMap->previewTex = nullptr;
        }
    }
}

void cPreviewMaps::loadSkirmish(const std::string &filename)
{
    cIniFile conf = cIniFile(filename);
    if (!conf.isLoadSuccess()) {
        Logger::error(COMP_MAP, "cPreviewMaps", "Could not load file : {}", filename);
        return; // skip this map loading
    }

    auto view = std::make_unique<PreviewMapView>();
    s_PreviewMap *previewMap = &view->map;
    const cSection &section = conf.getSection("SKIRMISH");
    previewMap->name = section.getStringValue("Title");
    previewMap->author = section.getStringValue("Author");
    previewMap->description = section.getStringValue("Description");

    const cSection &mapSection = conf.getSection("MAP");
    std::vector<std::string> vecmap = mapSection.getData();

    int maxWidth = vecmap[0].size();
    int maxHeight = vecmap.size();
    int maxCells = (maxWidth + 2) * (maxHeight + 2);

    view->terrain = nullptr;
    previewMap->iStartCell.fill(-1); // set all starting cells to -1

    std::vector<int> numbers;
    std::stringstream ss(section.getStringValue("StartCell"));
    std::string segment;
    while(std::getline(ss, segment, ',')) {
        try {
            numbers.push_back(std::stoi(segment));
        } catch (const std::invalid_argument& e) {
            Logger::warn(COMP_MAP, "cPreviewMaps", "Could not convert (invalid argument): {}", segment);
        } catch (const std::out_of_range& e) {
            Logger::warn(COMP_MAP, "cPreviewMaps", "Could not convert (out of bounds): {}", segment);
        }
    }

    int validStartCells = 0;
    for (int i = 0; i < std::min(static_cast<int>(numbers.size()), MAX_SKIRMISHMAP_PLAYERS); i++) {
        try {
            int startCell = numbers[i];
            if (startCell < 0 || startCell >= maxCells) {
                previewMap->validMap = false;
                Logger::warn(COMP_MAP, "cPreviewMaps", "StartCell [{}] invalid. (value must be between range [0-{}]), Map {} - is invalid", startCell, maxCells, filename);
            }
            else {
                previewMap->iStartCell[i] = startCell;
                validStartCells++;
            }
        }
        catch (std::invalid_argument const &e) {
            // could not perform conversion
            Logger::warn(COMP_MAP, "cPreviewMaps", "Could not convert startCell [{}] to an int. Reason: {}", numbers[i], e.what());
        }
    }

    auto readCells = [&](const std::string &key, std::vector<int> &cells) {
        std::stringstream cellStream(section.getStringValue(key));
        while (std::getline(cellStream, segment, ',')) {
            try {
                int cell = std::stoi(segment);
                if (cell == RANDOM_CELL) {
                    cells.push_back(cell);
                } else if (cell < 0 || cell >= maxCells) {
                    Logger::warn(COMP_MAP, "cPreviewMaps", "{} [{}] invalid. (value must be between range [0-{}]), Map {} - ignoring this value", key, cell, maxCells - 1, filename);
                } else {
                    cells.push_back(cell);
                }
            } catch (const std::invalid_argument&) {
                Logger::warn(COMP_MAP, "cPreviewMaps", "Could not convert {} (invalid argument): {}", key, segment);
            } catch (const std::out_of_range&) {
                Logger::warn(COMP_MAP, "cPreviewMaps", "Could not convert {} (out of bounds): {}", key, segment);
            }
        }
    };
    readCells("Bloom", previewMap->blooms);
    readCells("Field", previewMap->fields);

    if (validStartCells < 2) {
        previewMap->validMap = false;
        Logger::warn(COMP_MAP, "cPreviewMaps", "Map {} has only {} valid start cell(s); at least 2 are required - map is invalid", filename, validStartCells);
    }

    previewMap->width = maxWidth + 2;
    previewMap->height = maxHeight + 2;

    MapGeometry mapGeom(previewMap->width , previewMap->height);
    previewMap->terrainType = std::vector<int>(maxCells, -1);

    if (view->terrain == nullptr) {
        view->terrain = SDL_CreateSurface(previewMap->width, previewMap->height, SDL_PIXELFORMAT_RGBA32);
    }

    std::vector<int> unknownCells; // cells with a character we could not translate
    for (int iY = 0; iY < maxHeight; iY++) {
        const char *mapLine = vecmap[iY].c_str();
        for (int iX = 0; iX < maxWidth; iX++) {
            int iCll = mapGeom.makeCell((iX + 1), (iY + 1));
            if (iCll < 0) continue; // skip invalid cells

            int terrainType = -1; // unknown

            char letter = mapLine[iX];
            if (letter == ')') {
                terrainType = TERRAIN_SAND;
            }
            // rock
            if (letter == '%' || letter == '^' || letter == '&' || letter == '(') {
                terrainType = TERRAIN_ROCK;
            }
            // mountain
            if (letter == 'R' || letter == 'r') {
                terrainType = TERRAIN_MOUNTAIN;
            }
            // spice hill
            if (letter == '+') {
                terrainType = TERRAIN_SPICEHILL;
            }
            // spice
            if (letter == '-') {
                terrainType = TERRAIN_SPICE;
            }
            // sand hill
            if (letter == 'H' || letter == 'h') {
                terrainType = TERRAIN_HILL;
            }

            if (terrainType < 0) {
                Logger::error(COMP_MAP, "cPreviewMaps", "Could not determine terrain type for char \"{}\", falling back to SAND", letter);
                terrainType = TERRAIN_SAND;
                unknownCells.push_back(iCll);
            }

            previewMap->terrainType[iCll] = terrainType;
        }
    }

    drawPreview(*previewMap, view->terrain);

    // show cells with a wrong char as purple
    for (int cell : unknownCells) {
        m_sdlDrawer->setPixel(view->terrain, mapGeom.getCellX(cell), mapGeom.getCellY(cell), Color{160, 32, 240, 255});
    }

    if (view->terrain!= nullptr){
        SDL_Texture* out = SDL_CreateTextureFromSurface(m_sdlDrawer->getRenderer(), view->terrain);
        if (out == nullptr) {
            Logger::error(COMP_SDL2, "cPreviewMaps", "Error creating texture from surface: {}", SDL_GetError());
            return;
        }
        SDL_SetTextureScaleMode(out, SDL_SCALEMODE_NEAREST);
        view->previewTex = new Texture(out, view->terrain->w, view->terrain->h);
    }
    Logger::info(COMP_MAP, "cPreviewMaps", "Loaded skirmish map: {}, width: {}, height: {}", previewMap->name, previewMap->width, previewMap->height);
    m_PreviewMap.push_back(std::move(view));
}

/*
Scanning of skirmish maps:

- for each file in the folder "skirmish"
- if extension is .ini, open file (else skip)
- load the file, into a sm_PreviewMap struct
  - also creates a preview of map in BITMAP (minimap preview)
*/
void cPreviewMaps::loadSkirmishMaps()
{
    initPreviews(); // clear all of them

    const std::filesystem::path pathfile{"skirmish"};
    for (auto const &file: std::filesystem::directory_iterator{pathfile}) {
        auto fullname = file.path().string();
        // std::cout << "Found file: " << fullname << std::endl;
        if (file.path().extension() == ".ini") {
            loadSkirmish(fullname);
            Logger::info(COMP_MAP, "cPreviewMaps", "Loading skirmish map: {}", fullname);
        }
    }
}

static std::string normalizeStringForFileName(const std::string& input)
{
    std::string output = input;
    std::replace(output.begin(), output.end(), ' ', '_');
    output.erase(std::remove_if(output.begin(), output.end(),
        [](unsigned char c) { return !std::isalnum(c) && c != '_'; }), output.end());
    if (output.empty()) {
        output = "custom_map";
    }
    return output;
}

bool cPreviewMaps::saveSkirmishMap(const s_PreviewMap &map, bool backup)
{
    const std::string fileName = backup
        ? "skirmish/backup.ini"
        : "skirmish/" + normalizeStringForFileName(map.name) + ".ini";

    const size_t requiredTerrainCells = static_cast<size_t>(map.width) * static_cast<size_t>(map.height);
    if (map.width < 3 || map.height < 3 || map.terrainType.size() < requiredTerrainCells) {
        Logger::error(COMP_MAP, "cPreviewMaps", "Map terrain data is incomplete: {}", fileName);
        return false;
    }

    for (int y = 1; y < map.height - 1; y++) {
        for (int x = 1; x < map.width - 1; x++) {
            const size_t index = static_cast<size_t>(y) * static_cast<size_t>(map.width) + static_cast<size_t>(x);
            switch (map.terrainType[index]) {
                case TERRAIN_SAND:
                case TERRAIN_ROCK:
                case TERRAIN_MOUNTAIN:
                case TERRAIN_SPICEHILL:
                case TERRAIN_SPICE:
                case TERRAIN_HILL:
                    break;
                default:
                    Logger::error(COMP_MAP, "cPreviewMaps", "Unknown terrain value {} in map {}", map.terrainType[index], fileName);
                    return false;
            }
        }
    }

    std::ofstream saveFile(fileName);
    if (!saveFile.is_open()) {
        Logger::error(COMP_MAP, "cPreviewMaps", "Unable to open modified map for saving: {}", fileName);
        return false;
    }

    saveFile << "[SKIRMISH]\n";
    saveFile << "Title = " << map.name << "\n";
    saveFile << "Author = " << map.author << "\n";
    saveFile << "Description = " << map.description << "\n\n";
    saveFile << "Width = " << map.width - 2 << "\nHeight = " << map.height - 2 << "\n";
    saveFile << "StartCell=";
    bool firstStartCell = true;
    for (size_t i = 0; i < map.iStartCell.size(); i++) {
        if (map.iStartCell[i] != -1) {
            if (!firstStartCell) {
                saveFile << ",";
            }
            saveFile << map.iStartCell[i];
            firstStartCell = false;
        }
    }
    saveFile << "\n\n\n[MAP]\n";

    for (int y = 1; y < map.height - 1; y++) {
        for (int x = 1; x < map.width - 1; x++) {
            const size_t index = static_cast<size_t>(y) * static_cast<size_t>(map.width) + static_cast<size_t>(x);

            switch (map.terrainType[index]) {
                case TERRAIN_SAND:
                    saveFile << ')';
                    break;
                case TERRAIN_ROCK:
                    saveFile << '^';
                    break;
                case TERRAIN_MOUNTAIN:
                    saveFile << 'R';
                    break;
                case TERRAIN_SPICEHILL:
                    saveFile << '+';
                    break;
                case TERRAIN_SPICE:
                    saveFile << '-';
                    break;
                case TERRAIN_HILL:
                    saveFile << 'h';
                    break;
                default:
                    Logger::error(COMP_MAP, "cPreviewMaps", "Unknown terrain value {} in map {}", map.terrainType[index], fileName);
                    return false;
            }
        }
        saveFile << "\n";
    }

    saveFile.close();
    if (!saveFile) {
        Logger::error(COMP_MAP, "cPreviewMaps", "Failed while writing modified map: {}", fileName);
        return false;
    }

    loadSkirmish(fileName);
    return true;
}


// Skirmish map initialization
void cPreviewMaps::initPreviews()
{
    //reset all ressources
    destroy();
    m_PreviewMap.clear();
    //m_previewMap->push_back(std::make_unique<s_PreviewMap>());
    initRandomMap();
}


void cPreviewMaps::initRandomMap()
{
    // std::cout << "Initializing random map" << std::endl;
    auto firstSkirmishMap = std::make_unique<PreviewMapView>();
    firstSkirmishMap->map.name = "Random map";
    firstSkirmishMap->map.validMap = false;

    firstSkirmishMap->map.terrainType = std::vector<int>(1, -1);
    // if (firstSkirmishMap.terrain != nullptr) {
    //     SDL_DestroySurface(firstSkirmishMap.terrain);
    // }
    firstSkirmishMap->terrain = nullptr;
    // if (firstSkirmishMap.previewTex != nullptr) {
        // delete firstSkirmishMap.previewTex;
    // }
    firstSkirmishMap->previewTex = nullptr;
    if (m_PreviewMap.empty() || m_PreviewMap[0] == nullptr) {
        // std::cout << "Pushing random map as first map in the list" << std::endl;
        m_PreviewMap.push_back(std::move(firstSkirmishMap));
    }
    else {
        // std::cout << "Replacing first map in the list with random map" << std::endl;
        m_PreviewMap[0] = std::move(firstSkirmishMap);
    }
    //m_PreviewMap.push_back(std::move(firstSkirmishMap));
}

std::string cPreviewMaps::getMapSize(int i) const
{
    if (i < 0 || i >= static_cast<int>(m_PreviewMap.size()) || m_PreviewMap[i] == nullptr) {
        return "Invalid";
    }
    int playableCells = (m_PreviewMap[i]->map.width-2) * (m_PreviewMap[i]->map.height-2);
    if (playableCells <= 32 *32) {
        return "SMALL";
    }
    if (playableCells <= 64 * 64) {
        return "MEDIUM";
    }
    if (playableCells <= 128 * 64) {
        return "LARGE";
    }
    return "HUGE";
}

void cPreviewMaps::createEmptyMap(const std::string &name, const std::string &author, 
            const std::string &desciption, int width, int height)
{
    if (m_emptyMap == nullptr) {
        m_emptyMap = std::make_unique<PreviewMapView>();
    }
    m_emptyMap->map.name = name;
    m_emptyMap->map.author = author;
    m_emptyMap->map.description = desciption;
    m_emptyMap->map.width = width;
    m_emptyMap->map.height = height;
    m_emptyMap->map.validMap = true;
    m_emptyMap->map.terrainType = std::vector<int>(width * height, TERRAIN_SAND);
    m_emptyMap->map.iStartCell.fill(-1);
}

void cPreviewMaps::setEditorMap(s_PreviewMap &&map)
{
    m_emptyMap = std::make_unique<PreviewMapView>();
    m_emptyMap->map = std::move(map);
}
