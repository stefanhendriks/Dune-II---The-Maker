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
    PreviewMap[iNew].name = conf.getStr("SKIRMISH","Title");
    std::vector<std::string> vecmap = conf.getData("MAP");


    //ugly code to transform "1254,5421,4523" to 1254 , 5421 , 4523
    for(int i=0;i<5;i++)
        PreviewMap[iNew].iStartCell[i]=-1;
    std::stringstream test(conf.getStr("SKIRMISH","StartCell"));
    std::string segment;
    std::vector<std::string> seglist;
    while(std::getline(test, segment, ',')) {
        seglist.push_back(segment);
    }
    int nbrPlayer = std::min( 5, (int) seglist.size());
    for(int i=0;i< nbrPlayer; i++)
        PreviewMap[iNew].iStartCell[i]=std::stoi(seglist[i]);

    int maxWidth = vecmap[0].size() ;
    int maxHeight = vecmap.size() ;
    int maxCells = (maxWidth+1) * (maxHeight+1);

    PreviewMap[iNew].width = maxWidth+1;
    PreviewMap[iNew].height = maxHeight+1;

    cMap map;
    map.init( maxWidth+1, maxHeight+1);

    PreviewMap[iNew].mapdata = std::vector<int>(maxCells, -1);
    PreviewMap[iNew].terrain = create_bitmap(128, 128);
    clear_bitmap(PreviewMap[iNew].terrain);

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
                    PreviewMap[iNew].mapdata[iCll] = TERRAIN_SAND;
                } else if (iColor == makecol(80, 80, 60)) {
                    PreviewMap[iNew].mapdata[iCll] = TERRAIN_ROCK;
                } else if (iColor == makecol(48, 48, 36)) {
                    PreviewMap[iNew].mapdata[iCll] = TERRAIN_MOUNTAIN;
                } else if (iColor == makecol(180, 90, 25)) {
                    PreviewMap[iNew].mapdata[iCll] = TERRAIN_SPICEHILL;
                } else if (iColor == makecol(186, 93, 32)) {
                    PreviewMap[iNew].mapdata[iCll] = TERRAIN_SPICE;
                } else if (iColor == makecol(188, 115, 50)) {
                    PreviewMap[iNew].mapdata[iCll] = TERRAIN_HILL;
                } else {
                    logbook(fmt::format("iniLoader::skirmish() - Could not determine terrain type for char \"{}\", falling back to SAND",
                            letter));
                    PreviewMap[iNew].mapdata[iCll] = TERRAIN_SAND;
                    iColor = makecol(255, 255, 255);
                }
            }
            putpixel(PreviewMap[iNew].terrain, 1 + (iX * 2), 1 + (iY * 2), iColor);
            putpixel(PreviewMap[iNew].terrain, 1 + (iX * 2) + 1, 1 + (iY * 2), iColor);
            putpixel(PreviewMap[iNew].terrain, 1 + (iX * 2) + 1, 1 + (iY * 2) + 1, iColor);
            putpixel(PreviewMap[iNew].terrain, 1 + (iX * 2), 1 + (iY * 2) + 1, iColor);
        }
    }

    // starting points
    for (int i = 0; i < 5; i++) {
        if (PreviewMap[iNew].iStartCell[i] > -1) {
            int x = map.getCellX(PreviewMap[iNew].iStartCell[i]);
            int y = map.getCellY(PreviewMap[iNew].iStartCell[i]);
            putpixel(PreviewMap[iNew].terrain, 1 + (x * 2), 1 + (y * 2), makecol(255, 255, 255));
            putpixel(PreviewMap[iNew].terrain, 1 + (x * 2) + 1, 1 + (y * 2), makecol(255, 255, 255));
            putpixel(PreviewMap[iNew].terrain, 1 + (x * 2) + 1, 1 + (y * 2) + 1, makecol(255, 255, 255));
            putpixel(PreviewMap[iNew].terrain, 1 + (x * 2), 1 + (y * 2) + 1, makecol(255, 255, 255));
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
    PreviewMap[0].mapdata = std::vector<int>(maxCells, -1);
    PreviewMap[0].name = "Random map";
    //PreviewMap[0].terrain = (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat;
    PreviewMap[0].terrain = create_bitmap(128, 128);
}
