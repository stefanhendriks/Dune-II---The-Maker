#include "cPreviewMaps.h"
#include "data/gfxdata.h"
#include "map/cMap.h"

#include <filesystem>
namespace fs=std::filesystem;

#include <iostream>
#include <charconv>
#include <algorithm>
#include "utils/cIniFile.h"

cPreviewMaps::cPreviewMaps()
{}

cPreviewMaps::~cPreviewMaps()
{}

void cPreviewMaps::loadSkirmish(const std::string& filename) {
    // std::cout << filename << std::endl;

    int iNew = -1;
    for (int i = 0; i < 100; i++) {
        if (PreviewMap[i].name[0] == '\0') {
            iNew = i;
            break;
        }
    }

    if (iNew < 0) {
        return;
    }

    cIniFile conf = cIniFile(filename);
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
    int maxCells = (maxWidth+1) * (maxHeight+1);

    //ugly code to transform "1254,5421,4523" to 1254 , 5421 , 4523
    for(int i=0;i<5;i++) {
        previewMap.iStartCell[i] = -1;
        previewMap.terrain = nullptr;
    }

    std::stringstream test(section.getStringValue("StartCell"));
    std::string segment;
    std::vector<std::string> seglist;
    while(std::getline(test, segment, ',')) {
        seglist.push_back(segment);
    }

    int nbrPlayer = std::min( 5, (int) seglist.size());

    for(int i=0;i< nbrPlayer; i++) {
        int startCell = std::stoi(seglist[i]);
        if (startCell < 0 || startCell >= maxCells) {
            // not good
            int foo = 1;
            foo = foo + 1;
        } else {
            previewMap.iStartCell[i] = startCell;
        }
    }

    previewMap.width = maxWidth + 1;
    previewMap.height = maxHeight + 1;

    cMap map;
    map.init( maxWidth+1, maxHeight+1);

    previewMap.mapdata = std::vector<int>(maxCells, -1);
    previewMap.terrain = create_bitmap(128, 128); // need to check if already exists and if so skip this step?
    clear_bitmap(previewMap.terrain);

    for (int iY = 0; iY < maxHeight; iY++) {
        const char *mapLine = vecmap[iY].c_str();
        for (int iX = 0; iX < maxWidth; iX++) {
            char letter = mapLine[iX];
            int iCll = map.makeCell((iX + 1), (iY + 1));

            int iColor = makecol(194, 125, 60);
            // rock
            if (letter == '%') iColor = makecol(80, 80, 60);
            if (letter == '^') iColor = makecol(80, 80, 60);
            if (letter == '&') iColor = makecol(80, 80, 60);
            if (letter == '(') iColor = makecol(80, 80, 60);
            // mountain
            if (letter == 'R') iColor = makecol(48, 48, 36);
            if (letter == 'r') iColor = makecol(48, 48, 36);
            // spicehill
            if (letter == '+') iColor = makecol(180, 90, 25); // bit darker
            // spice
            if (letter == '-') iColor = makecol(186, 93, 32);
            // HILLS (NEW)
            if (letter == 'H') iColor = makecol(188, 115, 50);
            if (letter == 'h') iColor = makecol(188, 115, 50);

            if (iCll > -1) {
                if (iColor == makecol(194, 125, 60)) {
                    previewMap.mapdata[iCll] = TERRAIN_SAND;
                } else if (iColor == makecol(80, 80, 60)) {
                    previewMap.mapdata[iCll] = TERRAIN_ROCK;
                } else if (iColor == makecol(48, 48, 36)) {
                    previewMap.mapdata[iCll] = TERRAIN_MOUNTAIN;
                } else if (iColor == makecol(180, 90, 25)) {
                    previewMap.mapdata[iCll] = TERRAIN_SPICEHILL;
                } else if (iColor == makecol(186, 93, 32)) {
                    previewMap.mapdata[iCll] = TERRAIN_SPICE;
                } else if (iColor == makecol(188, 115, 50)) {
                    previewMap.mapdata[iCll] = TERRAIN_HILL;
                } else {
                    logbook(fmt::format("iniLoader::skirmish() - Could not determine terrain type for char \"{}\", falling back to SAND",
                            letter));
                    previewMap.mapdata[iCll] = TERRAIN_SAND;
                    iColor = makecol(255, 255, 255);
                }
            }
            putpixel(previewMap.terrain, 1 + (iX * 2), 1 + (iY * 2), iColor);
            putpixel(previewMap.terrain, 1 + (iX * 2) + 1, 1 + (iY * 2), iColor);
            putpixel(previewMap.terrain, 1 + (iX * 2) + 1, 1 + (iY * 2) + 1, iColor);
            putpixel(previewMap.terrain, 1 + (iX * 2), 1 + (iY * 2) + 1, iColor);
        }
    }

    // starting points
    for (int i = 0; i < 5; i++) {
        if (previewMap.iStartCell[i] > -1) {
            int x = map.getCellX(previewMap.iStartCell[i]);
            int y = map.getCellY(previewMap.iStartCell[i]);
            putpixel(previewMap.terrain, 1 + (x * 2), 1 + (y * 2), makecol(255, 255, 255));
            putpixel(previewMap.terrain, 1 + (x * 2) + 1, 1 + (y * 2), makecol(255, 255, 255));
            putpixel(previewMap.terrain, 1 + (x * 2) + 1, 1 + (y * 2) + 1, makecol(255, 255, 255));
            putpixel(previewMap.terrain, 1 + (x * 2), 1 + (y * 2) + 1, makecol(255, 255, 255));
        }
    }
}

/*
Pre-scanning of skirmish maps:

- open file
- read [SKIRMISH] data (name of map, startcells, etc)
- create preview of map in BITMAP (minimap preview)
*/
void cPreviewMaps::prescanSkirmish()
{
    // scans for all ini files
    initPreviews(); // clear all of them

    const std::filesystem::path pathfile{"skirmish"};
    for (auto const& file : std::filesystem::directory_iterator{pathfile}) 
    {
        auto fullname = file.path().string();
        if (file.path().extension()==".ini") {
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
        previewMap.mapdata.clear();

        previewMap.iPlayers = 0;

        previewMap.width = 0;
        previewMap.height = 0;

        previewMap.iStartCell[0] = -1;
        previewMap.iStartCell[1] = -1;
        previewMap.iStartCell[2] = -1;
        previewMap.iStartCell[3] = -1;
        previewMap.iStartCell[4] = -1;
    }

    int maxCells = 128*128;
    s_PreviewMap &firstSkirmishMap = PreviewMap[0];

    firstSkirmishMap.mapdata = std::vector<int>(maxCells, -1);
    firstSkirmishMap.name = "Random map";
    //PreviewMap[0].terrain = (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat;
    firstSkirmishMap.terrain = create_bitmap(128, 128);
}
