#include "cMiniMapDrawer.h"

#include "drawers/SDLDrawer.hpp"
#include "d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "player/cPlayer.h"
#include "sidebar/cSideBar.h"
#include "utils/cSoundPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>
#include "map/MapGeometry.hpp"
#include <cassert>
#include <iostream>

cMiniMapDrawer::cMiniMapDrawer(cMap *theMap, cPlayer *thePlayer, cMapCamera *theMapCamera) :
    m_isMouseOver(false),
    map(theMap),
    player(thePlayer),
    mapCamera(theMapCamera),
    status(eMinimapStatus::NOTAVAILABLE),
    iStaticFrame(STAT14),
    iTrans(0)
{
    assert(theMap);
    assert(thePlayer);
    assert(theMapCamera);

    int reportX = cSideBar::WidthOfMinimap / getMapWidthInPixels();
    int reportY = cSideBar::HeightOfMinimap / getMapHeightInPixels();
    factorZoom = std::min(reportX, reportY);

    int halfWidthOfMinimap = cSideBar::WidthOfMinimap / 2;
    int halfWidthOfMap = getMapWidthInPixels() / 2;
    int topLeftX = game.m_screenW - cSideBar::WidthOfMinimap;
    drawX = topLeftX + (halfWidthOfMinimap - factorZoom*halfWidthOfMap);

    int halfHeightOfMinimap = cSideBar::HeightOfMinimap / 2;
    int halfHeightOfMap = getMapHeightInPixels() / 2;
    int topLeftY = cSideBar::TopBarHeight;
    drawY = topLeftY + (halfHeightOfMinimap - factorZoom*halfHeightOfMap);

    centerX = topLeftX + (halfWidthOfMinimap - halfWidthOfMap);
    centerY = topLeftY + (halfHeightOfMinimap - halfHeightOfMap);

    m_RectMinimap = cRectangle(drawX, drawY, factorZoom*getMapWidthInPixels(), factorZoom * getMapHeightInPixels());
    m_RectFullMinimap = cRectangle(topLeftX, topLeftY, cSideBar::WidthOfMinimap, cSideBar::HeightOfMinimap);
    mipMapTex = renderDrawer->createRenderTargetTexture(getMapWidthInPixels(), getMapHeightInPixels());
}

cMiniMapDrawer::~cMiniMapDrawer()
{
    map = nullptr;
    mapCamera = nullptr;
    iStaticFrame = STAT14;
    status = eMinimapStatus::NOTAVAILABLE;
}

void cMiniMapDrawer::init()
{
    status = eMinimapStatus::NOTAVAILABLE;
    iStaticFrame = STAT14;
    iTrans = 0;
    m_isMouseOver = false;
}

void cMiniMapDrawer::drawViewPortRectangle()
{
    // Draw the magic rectangle (viewport)
    int iWidth = (mapCamera->getViewportWidth()) / TILESIZE_WIDTH_PIXELS;
    int iHeight = (mapCamera->getViewportHeight()) / TILESIZE_HEIGHT_PIXELS;
    iWidth--;
    iHeight--;

    int pixelSize = factorZoom;

    int startX = drawX + ((mapCamera->getViewportStartX() / TILESIZE_WIDTH_PIXELS) * pixelSize);
    int startY = drawY + ((mapCamera->getViewportStartY() / TILESIZE_HEIGHT_PIXELS) * pixelSize);

    int minimapWidth = iWidth * (pixelSize) + 1;
    int minimapHeight = iHeight * (pixelSize) + 1;

    //cap the rectangle to the minimap size
    if (startX < m_RectFullMinimap.getX()) {
        minimapWidth += startX - m_RectFullMinimap.getX();
        startX = m_RectFullMinimap.getX();
    }
    if (startY < m_RectFullMinimap.getY()) {
        minimapHeight += startY - m_RectFullMinimap.getY();
        startY = m_RectFullMinimap.getY();
    }
    if (startX + minimapWidth > m_RectFullMinimap.getEndX()) {
        minimapWidth = m_RectFullMinimap.getEndX() - startX;
    }
    if (startY + minimapHeight > m_RectFullMinimap.getEndY()) {
        minimapHeight = m_RectFullMinimap.getEndY() - startY;
    }
    renderDrawer->renderRectColor(startX, startY, minimapWidth, minimapHeight, Color{255, 255, 255,255});
}

int cMiniMapDrawer::getMapWidthInPixels() const {
    return map->getWidth();
}

int cMiniMapDrawer::getMapHeightInPixels() const {
    return map->getHeight();
}

void cMiniMapDrawer::drawTerrain()
{
    renderDrawer->beginDrawingToTexture(mipMapTex);
    Color iColor = Color{0, 0, 0,255};

    for (int x = 0; x < (map->getWidth()); x++) {
        for (int y = 0; y < (map->getHeight()); y++) {
            iColor = Color{0, 0, 0,255};
            //@mira where is map ?
            int iCll = map->getGeometry()->makeCell(x, y);

            if (map->isVisible(iCll, player->getId())) {
                iColor = getRGBColorForTerrainType(map->getCellType(iCll));
            }

            // TODO: make flexible map borders
            // do not show the helper border
            if (!map->isWithinBoundaries(x, y)) {
                iColor = Color{0, 0, 0,255};
            }

            renderDrawer->renderDot(x, y, iColor, 1);
        }
    }
    renderDrawer->endDrawingToTexture();
}

/**
 * Draws minimap units and structures.
 *
 * @param playerOnly (if false, draws all other players than player)
 */
void cMiniMapDrawer::drawUnitsAndStructures(bool playerOnly) const {
    renderDrawer->beginDrawingToTexture(mipMapTex);
    const Color black = Color::black();
    for (int x = 0; x < map->getWidth(); x++) {
        for (int y = 0; y < map->getHeight(); y++) {
            // do not show the helper border
            if (!map->isWithinBoundaries(x, y)) continue;

            int iCll = map->getGeometry()->makeCell(x, y);

            if (!map->isVisible(iCll, player->getId())) {
                // invisible cell
                continue;
            }

            // default : black
            Color iColor = Color{0,0,0,255};

            int idOfStructureAtCell = map->getCellIdStructuresLayer(iCll);
            if (idOfStructureAtCell > -1) {
                int iPlr = structure[idOfStructureAtCell]->getOwner();
                if (playerOnly) {
                    if (iPlr != player->getId()) continue; // skip non player units
                }
                iColor = players[iPlr].getMinimapColor();
            }

            int idOfUnitAtCell = map->getCellIdUnitLayer(iCll);
            if (idOfUnitAtCell > -1) {
                int iPlr = unit[idOfUnitAtCell].iPlayer;
                if (playerOnly) {
                    if (iPlr != player->getId()) continue; // skip non player units
                }
                iColor = players[iPlr].getMinimapColor();
            }

            int idOfAirUnitAtCell = map->getCellIdAirUnitLayer(iCll);
            if (idOfAirUnitAtCell > -1) {
                int iPlr = unit[idOfAirUnitAtCell].iPlayer;
                if (playerOnly) {
                    if (iPlr != player->getId()) continue; // skip non player units
                }
                iColor = players[iPlr].getMinimapColor();
            }

            int idOfWormAtCell = map->getCellIdWormsLayer(iCll);
            if (idOfWormAtCell > -1) {
                if (playerOnly) {
                    continue; // skip sandworms
                }
                iColor = player->getSelectFadingColor();
            }

            // no need to draw black on black background
            if (iColor.r == black.r && iColor.g == black.g && iColor.b == black.b) {
                continue;
            }
            renderDrawer->renderDot(x, y, iColor, 1 /*isBigMap ? 1 : 2*/);
        }
    }
    renderDrawer->endDrawingToTexture();
}


Color cMiniMapDrawer::getRGBColorForTerrainType(int terrainType)
{
    // get color for terrain type (for minimap)
    switch (terrainType) {
        case TERRAIN_ROCK:
            return Color{80, 80, 60,255};
        case TERRAIN_SPICE:
            return Color{186, 93, 32,255};
        case TERRAIN_SPICEHILL:
            return Color{180, 90, 25,255};
        case TERRAIN_HILL:
            return Color{188, 115, 50,255};
        case TERRAIN_MOUNTAIN:
            return Color{48, 48, 36,255};
        case TERRAIN_SAND:
            return Color{194, 125, 60,255};
        case TERRAIN_WALL:
            return Color{192, 192, 192,255};
        case TERRAIN_SLAB:
            return Color{80, 80, 80,255};
        case TERRAIN_BLOOM:
            return Color{214, 145, 100,255};
        case -1:
            return Color{255, 0, 255,255};
        default:
            return Color{255, 0, 255,255};
    }
}

void cMiniMapDrawer::draw()
{
    if (!map) {
        return;
    }

    if (status == eMinimapStatus::NOTAVAILABLE) {
        return;
    }

    renderDrawer->renderRectFillColor(m_RectFullMinimap, Color::black());

    if (status == eMinimapStatus::POWERUP || status == eMinimapStatus::RENDERMAP || status == eMinimapStatus::POWERDOWN) {
        drawTerrain();
        drawUnitsAndStructures(false);
    }

    if (status == eMinimapStatus::LOWPOWER) {
        cleanDrawTerrain();
        drawUnitsAndStructures(true);
    }
    cRectangle src = cRectangle(0, 0, mipMapTex->w, mipMapTex->h);
    renderDrawer->renderStrechSprite(mipMapTex, src , m_RectMinimap);

    drawStaticFrame();

    drawViewPortRectangle();
}

void cMiniMapDrawer::cleanDrawTerrain() const {
    renderDrawer->beginDrawingToTexture(mipMapTex);
    renderDrawer->renderClearToColor(Color::black());
    renderDrawer->endDrawingToTexture();
}

void cMiniMapDrawer::drawStaticFrame()
{
    if (status == eMinimapStatus::NOTAVAILABLE) return;
    if (status == eMinimapStatus::RENDERMAP) return;
    if (status == eMinimapStatus::LOWPOWER) return;

    if (status == eMinimapStatus::POWERDOWN) {
        cRectangle src= cRectangle(0, 0, gfxinter->getTexture(iStaticFrame)->w, gfxinter->getTexture(iStaticFrame)->h);
        renderDrawer->renderStrechSprite(gfxinter->getTexture(iStaticFrame), src, m_RectFullMinimap);
        return;
    }

    // Draw static info
    // < STAT01 frames are going from very transparent to opaque
    if (iStaticFrame < STAT10) {
        iTrans = 255 - health_bar(192, (STAT12 - iStaticFrame), 12);
    } else {
        iTrans = 255;
    }

    // non-stat01 frames are drawn transparent
    if (iStaticFrame != STAT01) {
        cRectangle src= cRectangle(0, 0, gfxinter->getTexture(iStaticFrame)->w, gfxinter->getTexture(iStaticFrame)->h);
        renderDrawer->renderStrechSprite(gfxinter->getTexture(iStaticFrame), src, m_RectFullMinimap,iTrans);
    }
}

int cMiniMapDrawer::getMouseCell(int mouseX, int mouseY)
{
    int mouseMiniMapX = mouseX - drawX;
    int mouseMiniMapY = mouseY - drawY;
    mouseMiniMapX /= factorZoom;
    mouseMiniMapY /= factorZoom;
    auto mouseMiniMapPoint = map->fixCoordinatesToBeWithinPlayableMap(mouseMiniMapX, mouseMiniMapY);

    return map->getGeometry()->getCellWithMapBorders(mouseMiniMapPoint.x, mouseMiniMapPoint.y);
}

// TODO: Respond to game events instead of using the "think" function (tell, don't ask)
void cMiniMapDrawer::think()
{
    if (player->hasAtleastOneStructure(RADAR)) {
        if (status == eMinimapStatus::NOTAVAILABLE) {
            status = eMinimapStatus::POWERUP;
        }
    }
    else {
        status = eMinimapStatus::NOTAVAILABLE;
    }

    if (status == eMinimapStatus::NOTAVAILABLE) return;

    bool hasRadarAndEnoughPower = (player->getAmountOfStructuresForType(RADAR) > 0) && player->bEnoughPower();

    // minimap state is enough power
    if (status == eMinimapStatus::POWERUP || status == eMinimapStatus::RENDERMAP) {
        if (!hasRadarAndEnoughPower) {
            // go to state power down (not enough power)
            status = eMinimapStatus::POWERDOWN;
            // "Radar de-activated""
            game.playVoice(SOUND_VOICE_04_ATR, player->getId());
        }
    }

    // minimap state is not enough power
    if (status == eMinimapStatus::POWERDOWN || status == eMinimapStatus::LOWPOWER) {
        if (hasRadarAndEnoughPower) {
            // go to state power up (enough power)
            status = eMinimapStatus::POWERUP;
            game.playSound(SOUND_RADAR);
            // "Radar activated"
            game.playVoice(SOUND_VOICE_03_ATR, player->getId());
        }
    }

    // think about the animation

    if (status == eMinimapStatus::POWERDOWN) {
        if (iStaticFrame < STAT21) {
            iStaticFrame++;
        }
        else {
            status = eMinimapStatus::LOWPOWER;
        }
    }
    else if (status == eMinimapStatus::POWERUP) {
        if (iStaticFrame > STAT01) {
            iStaticFrame--;
        }
        else {
            status = eMinimapStatus::RENDERMAP;
        }
    }
}

void cMiniMapDrawer::onMouseAt(const s_MouseEvent &event)
{
    m_isMouseOver = m_RectMinimap.isPointWithin(event.coords.x, event.coords.y);
}

bool cMiniMapDrawer::isMouseOver()
{
    return m_isMouseOver;
}

void cMiniMapDrawer::setPlayer(cPlayer *thePlayer)
{
    this->player = thePlayer;
}

void cMiniMapDrawer::onMousePressedLeft(const s_MouseEvent &event)
{
    if (m_RectFullMinimap.isPointWithin(event.coords.x, event.coords.y) && // on minimap space
            !game.getMouse()->isBoxSelecting() // pressed the mouse and not boxing anything..
       ) {

        if (player->hasAtleastOneStructure(RADAR)) {
            auto m_mouse = game.getMouse();
            int mouseCellOnMinimap = getMouseCell(m_mouse->getX(), m_mouse->getY());
            mapCamera->centerAndJumpViewPortToCell(mouseCellOnMinimap);
        }
    }
}

void cMiniMapDrawer::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case eMouseEventType::MOUSE_MOVED_TO:
            onMouseAt(event);
            return;
        case eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED:
            onMousePressedLeft(event); // click has same behavior as 'press'
            return;
        case eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED:
            onMousePressedLeft(event);
            return;
        default:
            return;
    }

}
