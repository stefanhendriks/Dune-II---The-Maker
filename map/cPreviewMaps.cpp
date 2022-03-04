#include "cPreviewMaps.h"
#include "data/gfxdata.h"
#include "map/cMap.h"

#include <filesystem>
namespace fs=std::filesystem;

#include <iostream>

cPreviewMaps::cPreviewMaps()
{}

cPreviewMaps::~cPreviewMaps()
{}

void cPreviewMaps::prescanSkirmish()
{
    INI_PRESCAN_SKIRMISH();
}

void cPreviewMaps::INI_LOAD_SKIRMISH(const char filename[80]) {
    std::cout << filename << std::endl;
}

/*
Pre-scanning of skirmish maps:

- open file
- read [SKIRMISH] data (name of map, startcells, etc)
- create preview of map in BITMAP (minimap preview)
*/
void cPreviewMaps::INI_PRESCAN_SKIRMISH() {
    // scans for all ini files
    INIT_PREVIEWS(); // clear all of them

    const std::filesystem::path pathfile{"skirmish"};
    for (auto const& file : std::filesystem::directory_iterator{pathfile}) 
    {
        auto fullname = file.path().string();
        if (file.path().extension()==".ini") {
            INI_LOAD_SKIRMISH(fullname.c_str());
            logbook(fmt::format("Loading skirmish map: {}", fullname));
        }
    }
}

// Skirmish map initialization
void cPreviewMaps::INIT_PREVIEWS() {
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
