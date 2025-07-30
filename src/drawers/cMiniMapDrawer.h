/*
 * cMiniMapDrawer.h
 *
 *  Created on: 20-okt-2010
 *      Author: Stefan
 */

#pragma once

#include "map/cMap.h"
#include "map/cMapCamera.h"
#include "utils/cRectangle.h"
#include "utils/Color.hpp"

class cPlayer;
class Texture;

// the BuildingListItemState
enum eMinimapStatus {
    NOTAVAILABLE, // show logo / no radar available
    POWERUP,      // powering up (radar available & enough power)
    RENDERMAP,    // map is rendering (radar available & enough power)
    POWERDOWN,    // powering down (radar available & not enough power)
    LOWPOWER,     // map is not rendered (radar available & not enough power)
};

class cMiniMapDrawer {
public:
    cMiniMapDrawer(cMap *theMap, cPlayer *thePlayer, cMapCamera *theMapCamera);
    ~cMiniMapDrawer();

    void draw();

    void drawStaticFrame();

    // in-between solution until we have a proper combat state object
    void init();

    void think();

    bool isMouseOver();

    void setPlayer(cPlayer *thePlayer);

    void onNotifyMouseEvent(const s_MouseEvent &event);

protected:

    void drawTerrain();
    void drawViewPortRectangle();
    void drawUnitsAndStructures(bool playerOnly);

    Color getRGBColorForTerrainType(int terrainType);

    int getMapHeightInPixels();
    int getMapWidthInPixels();

private:
    void onMouseAt(const s_MouseEvent &event);
    void onMousePressedLeft(const s_MouseEvent &event);

    int getMouseCell(int mouseX, int mouseY);

    bool m_isMouseOver;

    cMap *map;	// the minimap drawer reads data from here
    cPlayer *player;	// the player used as 'context' (ie, for drawing the rectangle / viewport on the minimap)
    cMapCamera *mapCamera;
    cRectangle m_RectMinimap; // the minimap (map) itself
    cRectangle m_RectFullMinimap; // the total space it could take
    Texture *mipMapTex;

    eMinimapStatus status;

    int iStaticFrame;
    int iTrans;							// transparancy

    // the top left coordinates for the minimap
    int drawX, drawY;

    // bool isBigMap;
    int factorZoom = 1; // factor to zoom the minimap, based on the map size and the minimap size
};
