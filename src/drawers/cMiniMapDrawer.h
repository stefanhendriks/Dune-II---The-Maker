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
class GameContext;
class Graphics;

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
    explicit cMiniMapDrawer(GameContext *ctx, cMap *map, cPlayer *player, cMapCamera *mapCamera);
    cMiniMapDrawer() = delete;
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
    void drawUnitsAndStructures(bool playerOnly) const;

    Color getRGBColorForTerrainType(int terrainType);

    int getMapHeightInPixels() const;
    int getMapWidthInPixels() const;

private:
    void onMouseAt(const s_MouseEvent &event);
    void onMousePressedLeft(const s_MouseEvent &event);
    void cleanDrawTerrain() const;
    int getMouseCell(int mouseX, int mouseY);

    bool m_isMouseOver;

    //@mira : where is map ? Where is this initialisation ?
    cMap *m_map;	// the minimap drawer reads data from here
    cPlayer *m_player;	// the player used as 'context' (ie, for drawing the rectangle / viewport on the minimap)
    cMapCamera *m_mapCamera;
    cRectangle m_RectMinimap; // the minimap (map) itself
    cRectangle m_RectFullMinimap; // the total space it could take
    Texture *mipMapTex;
    Graphics *m_gfxinter;

    eMinimapStatus m_status;

    int m_iStaticFrame;
    int m_iTrans;							// transparancy

    // the top left coordinates for the minimap
    int drawX, drawY;
    int centerX, centerY; // the center of the minimap

    // bool isBigMap;
    int factorZoom = 1; // factor to zoom the minimap, based on the map size and the minimap size
};
