#include "../include/d2tmh.h"
#include "cMiniMapDrawer.h"


cMiniMapDrawer::cMiniMapDrawer(cMap *theMap, cPlayer * thePlayer, cMapCamera * theMapCamera) : m_Player(thePlayer) {
	assert(theMap);
	assert(thePlayer);
	assert(theMapCamera);
	map = theMap;
	mapCamera = theMapCamera;
	iStaticFrame = STAT14;
	status = eMinimapStatus::NOTAVAILABLE;
	iTrans = 0;

    int halfWidthOfMinimap = cSideBar::WidthOfMinimap / 2;
    int halfWidthOfMap = getMapWidthInPixels() / 2;
    int topLeftX = game.screen_x - cSideBar::WidthOfMinimap;
    drawX = topLeftX + (halfWidthOfMinimap - halfWidthOfMap);

    int halfHeightOfMinimap = cSideBar::HeightOfMinimap / 2;
    int halfHeightOfMap = getMapHeightInPixels() / 2;
    int topLeftY = cSideBar::TopBarHeight;
    drawY = topLeftY + (halfHeightOfMinimap - halfHeightOfMap);

    m_RectMinimap = new cRectangle(drawX, drawY, getMapWidthInPixels(), getMapHeightInPixels());
    m_RectFullMinimap = new cRectangle(topLeftX, topLeftY, cSideBar::WidthOfMinimap, cSideBar::HeightOfMinimap);
}

cMiniMapDrawer::~cMiniMapDrawer() {
	map = NULL;
	mapCamera = NULL;
	iStaticFrame = STAT14;
    status = eMinimapStatus::NOTAVAILABLE;
	delete m_RectMinimap;
	delete m_RectFullMinimap;
}

void cMiniMapDrawer::drawViewPortRectangle() {
    // Draw the magic rectangle (viewport)
    int iWidth = (mapCamera->getViewportWidth()) / TILESIZE_WIDTH_PIXELS;
    int iHeight = (mapCamera->getViewportHeight()) / TILESIZE_HEIGHT_PIXELS;
    iWidth--;
    iHeight--;

    int pixelSize = 2;

    if (map->getWidth() > 64 || map->getHeight() > 64) {
        pixelSize = 1;
    }

    int startX = drawX + ((mapCamera->getViewportStartX() / TILESIZE_WIDTH_PIXELS) * pixelSize);
    int startY = drawY + ((mapCamera->getViewportStartY() / TILESIZE_HEIGHT_PIXELS) * pixelSize);

    int minimapWidth = (iWidth * pixelSize) + 1;
    int minimapHeight = (iHeight * pixelSize) + 1;

    rect(bmp_screen, startX, startY, startX + minimapWidth, startY + minimapHeight, makecol(255, 255, 255));
}

int cMiniMapDrawer::getMapWidthInPixels() {
    // for now, it always uses double pixels. But it could be 1 tile = 1 pixel later when map dimensions can be bigger.
    return map->getWidth() * 2; // double pixel size
}

int cMiniMapDrawer::getMapHeightInPixels() {
    // for now, it always uses double pixels. But it could be 1 tile = 1 pixel later when map dimensions can be bigger.
    return map->getHeight() * 2;
}

void cMiniMapDrawer::drawTerrain() {
	// startX = MAX_SCREEN_X - 129
    bool isBigMap = map->getWidth() > 64 || map->getHeight() > 64;

    int iColor = 0;

	for (int x = 0; x < (map->getWidth()); x++) {
        for (int y = 0; y < (map->getHeight()); y++) {
            iColor = makecol(0, 0, 0);
            int iCll = map->makeCell(x, y);

			if (map->isVisible(iCll, m_Player->getId())) {
				iColor = getRGBColorForTerrainType(map->getCellType(iCll));
			}

			// TODO: make flexible map borders
			// do not show the helper border
			if (!map->isWithinBoundaries(x, y)) {
				iColor = makecol(0, 0, 0);
			}

			int iDrawX = drawX + x;
			int iDrawY = drawY + y;
            if (isBigMap) {
                drawSingleDot(iDrawX, iDrawY, iColor);
            } else {
                // double sized 'pixels'.
                iDrawX += x;
                iDrawY += y;
                drawDoubleDot(iDrawX, iDrawY, iColor);
            }
		}
	}
}

/**
 * Draws minimap units and structures.
 *
 * @param playerOnly (if false, draws all other players than m_Player)
 */
void cMiniMapDrawer::drawUnitsAndStructures(bool playerOnly) {

	int iColor=allegroDrawer->getColor_BLACK();

	for (int x = 0; x < map->getWidth(); x++) {
		for (int y = 0; y < map->getHeight(); y++) {
            // do not show the helper border
            if (!map->isWithinBoundaries(x, y)) continue;

            int iCll = map->makeCell(x, y);

			if (!map->isVisible(iCll, m_Player->getId())) {
			    // invisible cell
			    continue;
			}

            iColor = allegroDrawer->getColor_BLACK();

            int idOfStructureAtCell = map->getCellIdStructuresLayer(iCll);
            if (idOfStructureAtCell > -1) {
                int	iPlr = structure[idOfStructureAtCell]->getOwner();
                if (playerOnly) {
                    if (iPlr != m_Player->getId()) continue; // skip non m_Player units
                }
                iColor = player[iPlr].getMinimapColor();
            }

            int idOfUnitAtCell = map->getCellIdUnitLayer(iCll);
            if (idOfUnitAtCell > -1) {
                int iPlr = unit[idOfUnitAtCell].iPlayer;
                if (playerOnly) {
                    if (iPlr != m_Player->getId()) continue; // skip non m_Player units
                }
                iColor = player[iPlr].getMinimapColor();
            }

            int idOfAirUnitAtCell = map->getCellIdAirUnitLayer(iCll);
            if (idOfAirUnitAtCell > -1) {
                int iPlr = unit[idOfAirUnitAtCell].iPlayer;
                if (playerOnly) {
                    if (iPlr != m_Player->getId()) continue; // skip non m_Player units
                }
                iColor = player[iPlr].getMinimapColor();
            }

            int idOfWormAtCell = map->getCellIdWormsLayer(iCll);
            if (idOfWormAtCell > -1) {
                if (playerOnly) {
                    continue; // skip sandworms
                }
                iColor = m_Player->getSelectFadingColor();
            }

            // no need to draw black on black background
            if (iColor != allegroDrawer->getColor_BLACK()) {
                int iDrawX=drawX + x;
                int iDrawY=drawY + y;

                if (map->getWidth() > 64 || map->getHeight() > 64) {
                    drawSingleDot(iDrawX, iDrawY, iColor);
                } else {
                    iDrawX += x;
                    iDrawY += y;
                    drawDoubleDot(iDrawX, iDrawY, iColor);
                }
            }
		}
	}
}



int cMiniMapDrawer::getRGBColorForTerrainType(int terrainType) {
	// get color for terrain type (for minimap)
	switch (terrainType) {
		case TERRAIN_ROCK:
			return makecol(80,80,60);
		case TERRAIN_SPICE:
			return makecol(186,93,32);
		case TERRAIN_SPICEHILL:
			return makecol(180,90,25);
		case TERRAIN_HILL:
			return makecol(188, 115, 50);
		case TERRAIN_MOUNTAIN:
			return makecol(48, 48, 36);
		case TERRAIN_SAND:
			return makecol(194, 125, 60);
		case TERRAIN_WALL:
			return makecol(192, 192, 192);
		case TERRAIN_SLAB:
			return makecol(80,80,80);
		case TERRAIN_BLOOM:
			return makecol(214,145,100);
	    case -1:
            return makecol(255, 0, 255);
		default:
			return makecol(255, 0, 255);
	}
}

void cMiniMapDrawer::interact() {
	if (m_RectMinimap->isMouseOver() && // on minimap
	    cMouse::isLeftButtonPressed() && !cMouse::isBoxSelecting() // pressed the mouse and not boxing anything..
	    ) {

	    if (m_Player->hasAtleastOneStructure(RADAR)) {
            int mouseCellOnMinimap = getMouseCell(mouse_x, mouse_y);
            mapCamera->centerAndJumpViewPortToCell(mouseCellOnMinimap);
        }
	}
}

void cMiniMapDrawer::draw() {
    if (!map) return;

    if (status == eMinimapStatus::NOTAVAILABLE) return;

    allegroDrawer->drawRectangleFilled(bmp_screen, m_RectFullMinimap, makecol(0,0,0));
    set_clip_rect(bmp_screen, m_RectFullMinimap->getX(), m_RectFullMinimap->getY(), m_RectFullMinimap->getEndX(), m_RectFullMinimap->getEndY());

    if (status == eMinimapStatus::POWERUP ||
        status == eMinimapStatus::RENDERMAP ||
        status == eMinimapStatus::POWERDOWN) {
        drawTerrain();
        drawUnitsAndStructures(false);
    }

    if (status == eMinimapStatus::LOWPOWER) {
        drawUnitsAndStructures(true);
    }

    drawStaticFrame();

    drawViewPortRectangle();
    set_clip_rect(bmp_screen, 0, 0, game.screen_x, game.screen_y);
}

void cMiniMapDrawer::drawStaticFrame() {
    if (status == eMinimapStatus::NOTAVAILABLE) return;
    if (status == eMinimapStatus::RENDERMAP) return;
    if (status == eMinimapStatus::LOWPOWER) return;

    if (status == eMinimapStatus::POWERDOWN) {
        draw_sprite(bmp_screen, (BITMAP *)gfxinter[iStaticFrame].dat, drawX, drawY);
        return;
    }

	// Draw static info
    // < STAT01 frames are going from very transparent to opaque
     if (iStaticFrame < STAT10) {
         iTrans = 255 - health_bar(192, (STAT12-iStaticFrame), 12);
     } else {
         iTrans = 255;
     }

     // non-stat01 frames are drawn transparent
     if (iStaticFrame != STAT01) {
         set_trans_blender(0,0,0, iTrans);

         draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[iStaticFrame].dat, drawX, drawY);
         // reset the trans blender
         set_trans_blender(0,0,0,128);
     }
}

void cMiniMapDrawer::drawDoubleDot(int x, int y, int color) {
	// draw a double sized 'pixel'
	putpixel(bmp_screen, x, y, color);
	putpixel(bmp_screen, x + 1, y, color);
	putpixel(bmp_screen, x + 1, y + 1, color);
	putpixel(bmp_screen, x, y + 1, color);
}

void cMiniMapDrawer::drawSingleDot(int x, int y, int color) {
	putpixel(bmp_screen, x, y, color);
}

int cMiniMapDrawer::getMouseCell(int mouseX, int mouseY) {
    // the minimap can be 128x128 pixels at the bottom right of the screen.
    int mouseMiniMapX = mouseX - drawX;
    int mouseMiniMapY = mouseY - drawY;

    // HACK HACK: Major assumption here - if map dimensions ever get > 64x64 this will BREAK!
    // However, every dot is (due the 64x64 map) 2 pixels wide...
    if (map->getHeight() > 64 || map->getWidth() > 64) {
        // do nothing
    } else {
        // old behavior assumes double sized pixels
        mouseMiniMapX /= 2;
        mouseMiniMapY /= 2;
    }

    // the mouse is the center of the screen, so substract half of the viewport coordinates
    int newX = mouseMiniMapX;
    int newY = mouseMiniMapY;

    return map->getCellWithMapBorders(newX, newY);
}

void cMiniMapDrawer::think() {
    if (m_Player->hasAtleastOneStructure(RADAR)) {
        if (status == eMinimapStatus::NOTAVAILABLE) {
            status = eMinimapStatus::POWERUP;
        }
    } else {
        status = eMinimapStatus::NOTAVAILABLE;
    }

    if (status == eMinimapStatus::NOTAVAILABLE) return;

    bool hasRadarAndEnoughPower = (m_Player->getAmountOfStructuresForType(RADAR) > 0) && m_Player->bEnoughPower();

    // minimap state is enough power
    if (status == eMinimapStatus::POWERUP || status == eMinimapStatus::RENDERMAP) {
        if (!hasRadarAndEnoughPower) {
            // go to state power down (not enough power)
            status = eMinimapStatus::POWERDOWN;
            play_voice(SOUND_VOICE_04_ATR); // radar de-activated!
        }
    }

    // minimap state is not enough power
    if (status == eMinimapStatus::POWERDOWN || status == eMinimapStatus::LOWPOWER) {
        if (hasRadarAndEnoughPower) {
            // go to state power up (enough power)
            status = eMinimapStatus::POWERUP;
            play_sound_id(SOUND_RADAR);
            play_voice(SOUND_VOICE_03_ATR); // radar activated!
        }
    }

    // think about the animation

    if (status == eMinimapStatus::POWERDOWN) {
        if (iStaticFrame < STAT21) {
            iStaticFrame++;
        } else {
            status = eMinimapStatus::LOWPOWER;
        }
    } else if (status == eMinimapStatus::POWERUP) {
        if (iStaticFrame > STAT01) {
            iStaticFrame--;
        } else {
            status = eMinimapStatus::RENDERMAP;
        }
    }
}

bool cMiniMapDrawer::isMouseOver() {
    return m_RectMinimap->isMouseOver();
}

void cMiniMapDrawer::setPlayer(cPlayer *thePlayer) {
    this->m_Player = thePlayer;
}
