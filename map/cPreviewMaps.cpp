#include "cPreviewMaps.h"
#include "data/gfxdata.h"
#include "map/cMap.h"

#include <filesystem>
namespace fs=std::filesystem;

#include <iostream>
#include "utils/cIniFile.h"

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

    //  BITMAP *terrain;        // terrain
    //  int width, height;      // width & height of map
    //  std::vector<int> mapdata;
    //  int iPlayers;           // players
    //  int iStartCell[5];      // starting locations
    //  std::string name;
    PreviewMap[iNew].name = conf.getStr("SKIRMISH","Title");
    std::string vecStarCell= conf.getStr("SKIRMISH","StartCell");
    std::vector<std::string> vecmap = conf.getData("MAP");

    PreviewMap[iNew].width = vecmap[0].size() +1;
    PreviewMap[iNew].height = vecmap.size() +1;

    cMap map;
    int maxCells = PreviewMap[iNew].width * PreviewMap[iNew].height;
    map.init(PreviewMap[iNew].width, PreviewMap[iNew].height);

    PreviewMap[iNew].mapdata = std::vector<int>(maxCells, -1);

    std::cout << "Title " << PreviewMap[iNew].name <<  PreviewMap[iNew].width << "x" << PreviewMap[iNew].height << " StarCell " << vecStarCell << std::endl;
    // for (const auto& x : vecmap)
    //     std::cout << x << std::endl;
/*

    previewMap.mapdata = std::vector<int>(maxCells, -1);

    // Load file

    FILE *stream;                    // file stream
    int section = INI_NONE;            // section
    int wordtype = WORD_NONE;            // word

    int maxWidth = -1;
    int maxHeight = -1;

    int iYLine = 0;
    int iStart = 0;

    std::vector<std::string> mapLines = std::vector<std::string>();

    if ((stream = fopen(filename, "r+t")) != nullptr) {
        char linefeed[MAX_LINE_LENGTH];
        char lineword[30];
        char linesection[30];

        memset(lineword, '\0', sizeof(lineword));
        memset(linesection, '\0', sizeof(linesection));
        memset(linefeed, '\0', sizeof(linefeed));

        // infinite loop baby
        while (!feof(stream)) {
            INI_Sentence(stream, linefeed);

            // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
            // character (which is "//", ";" or "#"), or an empty line, then skip it
            if (isCommentLine(linefeed)) {
                continue;   // Skip
            }

            // Every line is checked for a new section.
            INI_Section(linefeed, linesection);

            if (linesection[0] != '\0' && strlen(linesection) > 1) {
                int iOld = section;
                section = INI_SectionType(linesection, section);

                // section found
                if (iOld != section) {
                    if (section == INI_MAP) {
                        if (previewMap.terrain == nullptr) {
                            previewMap.terrain = create_bitmap(128, 128);
                            clear_bitmap(previewMap.terrain);
                            //clear_to_color(PreviewMap[iNew].terrain, makecol(255,255,255));
                        }
                        continue; // skip
                    }
                }
            }

            // Okay, we found a new section; if its NOT [GAME] then we remember this one!

            if (section != INI_NONE) {
                INI_Word(linefeed, lineword);
                wordtype = INI_WordType(lineword, section);
            } else {
                continue;
            }

            if (section == INI_SKIRMISH) {
                if (wordtype == WORD_MAPNAME) {
                    char mes[256];
                    INI_WordValueSENTENCE(linefeed, mes);
                    previewMap.name = std::string(mes);
                    //logbook(PreviewMap[iNew].name);
                }

                if (wordtype == WORD_STARTCELL) {
                    if (iStart == 0) {
                        for (int i = 0; i < 5; i++)
                            previewMap.iStartCell[i] = -1;
                    }

                    // start locations
                    if (iStart < 5) {
                        previewMap.iStartCell[iStart] = INI_WordValueINT(linefeed);
                        iStart++;
                    }
                }
            }

            if (section == INI_MAP) {
                int iLength = strlen(linefeed);

                if (iLength > maxWidth) maxWidth = iLength;
                // END!
                if (iLength < 2) {
                    break; // done
                }

                std::string str(linefeed);
                mapLines.push_back(str);

                iYLine++;
                maxHeight = iYLine;
            }

        } // end of file reading
        

        previewMap.width = maxWidth + 1;
        previewMap.height = maxHeight + 1;
        map.resize(maxWidth+1, maxHeight+1);

        for (int iY = 0; iY < maxHeight; iY++) {
            const char *mapLine = mapLines[iY].c_str();
            for (int iX = 0; iX < maxWidth; iX++) {
                char letter[1];
                letter[0] = mapLine[iX];

                int iCll = map.makeCell((iX + 1), (iY + 1));

                int iColor = makecol(194, 125, 60);

                // rock
                if (letter[0] == '%') iColor = makecol(80, 80, 60);
                if (letter[0] == '^') iColor = makecol(80, 80, 60);
                if (letter[0] == '&') iColor = makecol(80, 80, 60);
                if (letter[0] == '(') iColor = makecol(80, 80, 60);

                // mountain
                if (letter[0] == 'R') iColor = makecol(48, 48, 36);
                if (letter[0] == 'r') iColor = makecol(48, 48, 36);

                // spicehill
                if (letter[0] == '+') iColor = makecol(180, 90, 25); // bit darker

                // spice
                if (letter[0] == '-') iColor = makecol(186, 93, 32);

                // HILLS (NEW)
                if (letter[0] == 'H') iColor = makecol(188, 115, 50);
                if (letter[0] == 'h') iColor = makecol(188, 115, 50);

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
                                letter[0]));
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
        fclose(stream);

    } // file exists
*/
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
