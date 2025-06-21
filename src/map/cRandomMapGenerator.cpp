#include "cRandomMapGenerator.h"

#include "d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "map/cMap.h"
#include "map/cMapEditor.h"
#include "drawers/SDLDrawer.hpp"

cRandomMapGenerator::cRandomMapGenerator()
{
}

void cRandomMapGenerator::generateRandomMap(int startingPoints, s_PreviewMap &randomMapEntry)
{
    // create random map
    map.init(128, 128);
    auto mapEditor = cMapEditor(map);

    int a_spice = rnd((startingPoints * 8)) + (startingPoints * 12);
    int a_rock = 32 + rnd(startingPoints * 3);
    int a_hill = 2 + rnd(12);

    // rock terrain is placed not near centre, also, we want 4 plateaus be
    // placed not too near to each other
    int maxRockSpots = 18;
    int iSpotRock[18]; // first 4
    memset(iSpotRock, -1, sizeof(iSpotRock));

    int totalSpots = a_spice + a_rock + a_hill;
    float piece = 1.0f / totalSpots;

    int iSpot = 0;
    int iFails = 0;

    //s_PreviewMap &randomMapEntry = PreviewMap[0];
    randomMapEntry.width = 128;
    randomMapEntry.height = 128;
    memset(randomMapEntry.iStartCell, -1, sizeof(randomMapEntry.iStartCell));

    int iDistance = 16;

    if (startingPoints == 2) iDistance = 32;
    if (startingPoints == 3) iDistance = 26;
    if (startingPoints == 4) iDistance = 22;

    float progress = 0;

    // x = 160
    // y = 180
    renderDrawer->drawSprite(bmp_screen, gfxinter->getSurface(BMP_GENERATING), 160, 180);

    // draw
    while (a_rock > 0) {
        int iCll = map.getRandomCellWithinMapWithSafeDistanceFromBorder(4);
        if (iCll < 0) continue;

        bool bOk = true;
        if (iSpot < maxRockSpots) {
            for (int s = 0; s < maxRockSpots; s++) {
                if (iSpotRock[s] > -1) {
                    if (ABS_length(map.getCellX(iCll), map.getCellY(iCll), map.getCellX(iSpotRock[s]),
                                   map.getCellY(iSpotRock[s])) < iDistance) {
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
            progress += piece;
            mapEditor.createRandomField(iCll, TERRAIN_ROCK, 5500 + rnd(3500));

            if (iSpot < startingPoints) {
                randomMapEntry.iStartCell[iSpot] = iCll;
            }
            else {
                mapEditor.createRandomField(iCll, TERRAIN_MOUNTAIN, 5 + rnd(15));
            }

            if (iSpot < maxRockSpots) {
                iSpotRock[iSpot] = iCll;
            }

            iSpot++;
            a_rock--;

            // blit on screen
            drawProgress(progress);
            renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);
        }

    }

    // soft out rock a bit
    mapEditor.removeSingleRockSpots();

    // blit on screen
    drawProgress(progress);
    renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);

    mapEditor.removeSingleRockSpots();


    // blit on screen
    drawProgress(progress);
    renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);

    mapEditor.removeSingleRockSpots();

    // blit on screen
    drawProgress(progress);
    renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);

    while (a_spice > 0) {
        int iCll = map.getRandomCellWithinMapWithSafeDistanceFromBorder(0);
        mapEditor.createRandomField(iCll, TERRAIN_SPICE, 2500);
        progress += piece;
        a_spice--;
        // blit on screen
        drawProgress(progress);
        renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);
    }

    while (a_hill > 0) {
        int cell = map.getRandomCellWithinMapWithSafeDistanceFromBorder(0);
        mapEditor.createRandomField(cell, TERRAIN_HILL, 500 + rnd(500));
        a_hill--;
        progress += piece;
        // blit on screen
        drawProgress(progress);
        renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);
    }


    for (int s = 0; s < 4; s++) {
        // make sure starting location has rock to have CONSTYARD placed, which makes sure we don't need to
        // spawn an MCV (fixing https://github.com/stefanhendriks/Dune-II---The-Maker/issues/312)
        mapEditor.createSquaredField(randomMapEntry.iStartCell[s], TERRAIN_ROCK, 2);
    }

    // end of map creation
    mapEditor.smoothMap();

    // blit on screen
    progress += 25;
    drawProgress(progress);
    renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);
    renderDrawer->FillWithColor(randomMapEntry.terrain, SDL_Color{0,0,0,255});

    // now put in previewmap 0
    for (int x = 0; x < map.getWidth(); x++)
        for (int y = 0; y < map.getHeight(); y++) {

            drawProgress(progress);

            int cll = map.getCellWithMapDimensions(x, y);
            if (cll < 0) continue;

            SDL_Color iColor = SDL_Color{194, 125, 60,255};

            // rock
            int cellType = map.getCellType(cll);
            if (cellType == TERRAIN_ROCK) iColor = SDL_Color{80, 80, 60,255};
            if (cellType == TERRAIN_MOUNTAIN) iColor = SDL_Color{48, 48, 36,255};
            if (cellType == TERRAIN_SPICEHILL) iColor = SDL_Color{180, 90, 25,255}; // a bit darker
            if (cellType == TERRAIN_SPICE) iColor = SDL_Color{186, 93, 32,255};
            if (cellType == TERRAIN_HILL) iColor = SDL_Color{188, 115, 50,255};

            randomMapEntry.terrainType[cll] = cellType;

            for (int s = 0; s < 4; s++) {
                if (randomMapEntry.iStartCell[s] > -1) {
                    int sx = map.getCellX(randomMapEntry.iStartCell[s]);
                    int sy = map.getCellY(randomMapEntry.iStartCell[s]);

                    if (sx == x && sy == y)
                        iColor = SDL_Color{255, 255, 255,255};
                }
            }

            renderDrawer->set_pixel(randomMapEntry.terrain, x, y, iColor);
        }

    // blit on screen
    drawProgress(progress);
    renderDrawer->blit(bmp_screen, screenSurface, 0, 0, 0, 0, game.m_screenW, game.m_screenH);
}

void cRandomMapGenerator::drawProgress(float progress) const
{
    int iProgress = progress * 211;
    //_rectfill(bmp_screen, 216, 225, 216 + iProgress, 257, SDL_Color{255, 0, 0));
    renderDrawer->drawRectFilled(bmp_screen, 216, 225, iProgress, 257-225, SDL_Color{255, 0, 0,255});
}
