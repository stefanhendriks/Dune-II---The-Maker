#include "cRandomMapGenerator.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "data/gfxdata.h"
//#include "data/gfxinter.h"
#include "gameobjects/map/cMap.h"
#include "gameobjects/map/cMapEditor.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "utils/RNG.hpp"
#include "utils/d2tm_math.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

cRandomMapGenerator::cRandomMapGenerator()
{
}

void cRandomMapGenerator::generateRandomMap(int width, int height, int startingPoints, s_PreviewMap &randomMapEntry)
{
    // create random map
    game.m_gameObjectsContext->getMap().init(width, height);
    auto mapEditor = cMapEditor(game.m_gameObjectsContext->getMap());

    int a_spice = RNG::rnd((startingPoints * 8)) + (startingPoints * 12);
    int a_rock = 32 + RNG::rnd(startingPoints * 3);
    int a_hill = 2 + RNG::rnd(12);

    // rock terrain is placed not near centre, also, we want 4 plateaus be
    // placed not too near to each other
    int maxRockSpots = 18;
    int iSpotRock[18]; // first 4
    memset(iSpotRock, -1, sizeof(iSpotRock));

    int iSpot = 0;
    int iFails = 0;

    randomMapEntry.width = width;
    randomMapEntry.height = height;
    // memset(randomMapEntry.iStartCell, -1, sizeof(randomMapEntry.iStartCell));
    randomMapEntry.iStartCell.fill(-1); // set all starting cells to -1

    int iDistance = 16;

    if (startingPoints == 2) iDistance = 32;
    if (startingPoints == 3) iDistance = 26;
    if (startingPoints == 4) iDistance = 22;

    // draw
    while (a_rock > 0) {
        int iCll = game.m_gameObjectsContext->getMapGeometry()->getRandomCellWithinMapWithSafeDistanceFromBorder(4);
        if (iCll < 0) continue;

        bool bOk = true;
        if (iSpot < maxRockSpots) {
            for (int s = 0; s < maxRockSpots; s++) {
                if (iSpotRock[s] > -1) {
                    if (ABS_length(game.m_gameObjectsContext->getMapGeometry()->getCellX(iCll), game.m_gameObjectsContext->getMapGeometry()->getCellY(iCll), game.m_gameObjectsContext->getMapGeometry()->getCellX(iSpotRock[s]),
                                   game.m_gameObjectsContext->getMapGeometry()->getCellY(iSpotRock[s])) < iDistance) {
                        bOk = false;
                    }
                    else {
                        iFails++;
                    }
                }
            }
        }

        if (iFails > 10) {
            iFails = 0;
            bOk = true;
        }

        if (bOk) {
            // progress += piece;
            mapEditor.createRandomField(iCll, TERRAIN_ROCK, 5500 + RNG::rnd(3500));

            if (iSpot < startingPoints) {
                randomMapEntry.iStartCell[iSpot] = iCll;
            }
            else {
                mapEditor.createRandomField(iCll, TERRAIN_MOUNTAIN, 5 + RNG::rnd(15));
            }

            if (iSpot < maxRockSpots) {
                iSpotRock[iSpot] = iCll;
            }

            iSpot++;
            a_rock--;
        }

    }

    // soft out rock a bit
    mapEditor.removeSingleRockSpots();
    mapEditor.removeSingleRockSpots();
    mapEditor.removeSingleRockSpots();

    while (a_spice > 0) {
        int iCll = game.m_gameObjectsContext->getMapGeometry()->getRandomCellWithinMapWithSafeDistanceFromBorder(0);
        mapEditor.createRandomField(iCll, TERRAIN_SPICE, 2500);
        a_spice--;
    }

    while (a_hill > 0) {
        int cell = game.m_gameObjectsContext->getMapGeometry()->getRandomCellWithinMapWithSafeDistanceFromBorder(0);
        mapEditor.createRandomField(cell, TERRAIN_HILL, 500 + RNG::rnd(500));
        a_hill--;
    }


    for (int s = 0; s < 4; s++) {
        // make sure starting location has rock to have CONSTYARD placed, which makes sure we don't need to
        // spawn an MCV (fixing https://github.com/stefanhendriks/Dune-II---The-Maker/issues/312)
        mapEditor.createSquaredField(randomMapEntry.iStartCell[s], TERRAIN_ROCK, 2);
    }

    // end of map creation
    mapEditor.smoothMap();

    global_renderDrawer->FillWithColor(randomMapEntry.terrain, Color::Black);

    // now put in previewmap 0
    for (int x = 0; x < game.m_gameObjectsContext->getMap().getWidth(); x++) {
        for (int y = 0; y < game.m_gameObjectsContext->getMap().getHeight(); y++) {

            int cll = game.m_gameObjectsContext->getMapGeometry()->getCellWithMapDimensions(x, y);
            if (cll < 0) continue;

            Color iColor = Color{194, 125, 60,255};

            // rock
            int cellType = game.m_gameObjectsContext->getMap().getCellType(cll);
            if (cellType == TERRAIN_ROCK) iColor = Color{80, 80, 60,255};
            if (cellType == TERRAIN_MOUNTAIN) iColor = Color{48, 48, 36,255};
            if (cellType == TERRAIN_SPICEHILL) iColor = Color{180, 90, 25,255}; // a bit darker
            if (cellType == TERRAIN_SPICE) iColor = Color{186, 93, 32,255};
            if (cellType == TERRAIN_HILL) iColor = Color{188, 115, 50,255};

            randomMapEntry.terrainType[cll] = cellType;

            for (int s = 0; s < 4; s++) {
                if (randomMapEntry.iStartCell[s] > -1) {
                    int sx = game.m_gameObjectsContext->getMapGeometry()->getCellX(randomMapEntry.iStartCell[s]);
                    int sy = game.m_gameObjectsContext->getMapGeometry()->getCellY(randomMapEntry.iStartCell[s]);

                    if (sx == x && sy == y)
                        iColor = Color::White;
                }
            }

            global_renderDrawer->setPixel(randomMapEntry.terrain, x, y, iColor);
        }
    }
}