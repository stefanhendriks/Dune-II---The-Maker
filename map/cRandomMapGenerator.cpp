#include "cRandomMapGenerator.h"

#include <allegro.h>

cRandomMapGenerator::cRandomMapGenerator(cMap& map, s_PreviewMap& preview) 
        : m_map(map)
        , m_preview(preview) {
}

void cRandomMapGenerator::generateRandomMap(int startingPoints, std::function<void(float)> onProgress) {
    // create random map
    m_map.init(128, 128);

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

    m_preview.width = 128;
    m_preview.height = 128;
    memset(m_preview.iStartCell, -1, sizeof(m_preview.iStartCell));

    int iDistance = 16;

    if (startingPoints == 2) iDistance = 32;
    if (startingPoints == 3) iDistance = 26;
    if (startingPoints == 4) iDistance = 22;

    float progress = 0;

    // draw
    while (a_rock > 0) {
        int iCll = m_map.getRandomCellWithinMapWithSafeDistanceFromBorder(4);
        if (iCll < 0) continue;

        bool bOk = true;
        if (iSpot < maxRockSpots) {
            for (int s = 0; s < maxRockSpots; s++) {
                if (iSpotRock[s] > -1) {
                    if (ABS_length(m_map.getCellX(iCll), m_map.getCellY(iCll), m_map.getCellX(iSpotRock[s]),
                                   m_map.getCellY(iSpotRock[s])) < iDistance) {
                        bOk = false;
                    } else {
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
                m_preview.iStartCell[iSpot] = iCll;
            } else {
                mapEditor.createRandomField(iCll, TERRAIN_MOUNTAIN, 5 + rnd(15));
            }

            if (iSpot < maxRockSpots) {
                iSpotRock[iSpot] = iCll;
            }

            iSpot++;
            a_rock--;

            onProgress(progress);
        }

    }

    // soft out rock a bit
    mapEditor.removeSingleRockSpots();

    onProgress(progress);

    mapEditor.removeSingleRockSpots();

    onProgress(progress);

    mapEditor.removeSingleRockSpots();

    onProgress(progress);

    while (a_spice > 0) {
        int iCll = m_map.getRandomCellWithinMapWithSafeDistanceFromBorder(0);
        mapEditor.createRandomField(iCll, TERRAIN_SPICE, 2500);
        progress += piece;
        a_spice--;
        onProgress(progress);
    }

    while (a_hill > 0) {
        int cell = m_map.getRandomCellWithinMapWithSafeDistanceFromBorder(0);
        mapEditor.createRandomField(cell, TERRAIN_HILL, 500 + rnd(500));
        a_hill--;
        progress += piece;
        onProgress(progress);
    }

    for (int s = 0; s < 4; s++) {
        // make sure starting location has rock to have CONSTYARD placed, which makes sure we don't need to
        // spawn an MCV (fixing https://github.com/stefanhendriks/Dune-II---The-Maker/issues/312)
        mapEditor.createSquaredField(m_preview.iStartCell[s], TERRAIN_ROCK, 2);
    }

    // end of map creation
    mapEditor.smoothMap();

    progress += 25;
    onProgress(progress);

    clear_to_color(m_preview.terrain, makecol(0, 0, 0));

    // now put in previewmap 0
    for (int x = 0; x < m_map.getWidth(); x++)
        for (int y = 0; y < m_map.getHeight(); y++) {

            onProgress(progress);

            int cll = m_map.getCellWithMapDimensions(x, y);
            if (cll < 0) continue;

            int iColor = makecol(194, 125, 60);

            // rock
            int cellType = m_map.getCellType(cll);
            if (cellType == TERRAIN_ROCK) iColor = makecol(80, 80, 60);
            if (cellType == TERRAIN_MOUNTAIN) iColor = makecol(48, 48, 36);
            if (cellType == TERRAIN_SPICEHILL) iColor = makecol(180, 90, 25); // bit darker
            if (cellType == TERRAIN_SPICE) iColor = makecol(186, 93, 32);
            if (cellType == TERRAIN_HILL) iColor = makecol(188, 115, 50);

            m_preview.mapdata[cll] = cellType;

            for (int s = 0; s < 4; s++) {
                if (m_preview.iStartCell[s] > -1) {
                    int sx = m_map.getCellX(m_preview.iStartCell[s]);
                    int sy = m_map.getCellY(m_preview.iStartCell[s]);

                    if (sx == x && sy == y)
                        iColor = makecol(255, 255, 255);
                }
            }

            putpixel(m_preview.terrain, x, y, iColor);
        }

    onProgress(progress);
}
