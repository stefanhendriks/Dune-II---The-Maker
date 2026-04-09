#include "cMiniMapDrawer.h"
#include "game/cGameSettings.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/structures/cStructures.h"
#include "map/cMapCamera.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "player/cPlayer.h"
#include "player/cPlayers.h"
#include "sidebar/cSideBar.h"
#include "utils/cSoundPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>
#include "map/MapGeometry.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include <cassert>
#include <iostream>

#include "data/gfxaudio.h"

cMiniMapDrawer::cMiniMapDrawer(GameContext *ctx, cMap *map, cPlayer *player, cMapCamera *mapCamera) :
    m_isMouseOver(false),
    m_map(map),
    m_player(player),
    m_mapCamera(mapCamera),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get()),
    m_renderDrawer(ctx->getSDLDrawer()),
    m_status(eMinimapStatus::NOTAVAILABLE),
    m_iStaticFrame(STAT14),
    m_iTrans(0)
{
    assert(map!=nullptr);
    assert(player!=nullptr);
    assert(mapCamera!=nullptr);

    int reportX = cSideBar::WidthOfMinimap / getMapWidthInPixels();
    int reportY = cSideBar::HeightOfMinimap / getMapHeightInPixels();
    m_factorZoom = std::min(reportX, reportY);

    int halfWidthOfMinimap = cSideBar::WidthOfMinimap / 2;
    int halfWidthOfMap = getMapWidthInPixels() / 2;
    int topLeftX = game.m_gameSettings->getScreenW() - cSideBar::WidthOfMinimap;
    m_drawX = topLeftX + (halfWidthOfMinimap - m_factorZoom*halfWidthOfMap);

    int halfHeightOfMinimap = cSideBar::HeightOfMinimap / 2;
    int halfHeightOfMap = getMapHeightInPixels() / 2;
    int topLeftY = cSideBar::TopBarHeight;
    m_drawY = topLeftY + (halfHeightOfMinimap - m_factorZoom*halfHeightOfMap);

    m_centerX = topLeftX + (halfWidthOfMinimap - halfWidthOfMap);
    m_centerY = topLeftY + (halfHeightOfMinimap - halfHeightOfMap);

    m_RectMinimap = cRectangle(m_drawX, m_drawY, m_factorZoom*getMapWidthInPixels(), m_factorZoom * getMapHeightInPixels());
    m_RectFullMinimap = cRectangle(topLeftX, topLeftY, cSideBar::WidthOfMinimap, cSideBar::HeightOfMinimap);
    m_mipMapTex = m_renderDrawer->createRenderTargetTexture(getMapWidthInPixels(), getMapHeightInPixels());
}

cMiniMapDrawer::~cMiniMapDrawer()
{
    m_map = nullptr;
    m_mapCamera = nullptr;
    m_iStaticFrame = STAT14;
    m_status = eMinimapStatus::NOTAVAILABLE;
}

void cMiniMapDrawer::draw()
{
    if (!m_map) {
        return;
    }

    if (m_status == eMinimapStatus::NOTAVAILABLE) {
        return;
    }

    m_renderDrawer->renderRectFillColor(m_RectFullMinimap, Color::black());

    if (m_status == eMinimapStatus::POWERUP || m_status == eMinimapStatus::RENDERMAP || m_status == eMinimapStatus::POWERDOWN) {
        drawTerrain();
        drawUnitsAndStructures(false);
    }

    if (m_status == eMinimapStatus::LOWPOWER) {
        cleanDrawTerrain();
        drawUnitsAndStructures(true);
    }
    m_renderDrawer->renderStrechFullSprite(m_mipMapTex, m_RectMinimap);

    drawStaticFrame();
    drawViewPortRectangle();
}

void cMiniMapDrawer::drawStaticFrame()
{
    if (m_status == eMinimapStatus::NOTAVAILABLE) return;
    if (m_status == eMinimapStatus::RENDERMAP) return;
    if (m_status == eMinimapStatus::LOWPOWER) return;

    if (m_status == eMinimapStatus::POWERDOWN) {
        m_renderDrawer->renderStrechFullSprite(m_gfxinter->getTexture(m_iStaticFrame), m_RectFullMinimap);
        return;
    }

    // Draw static info
    // < STAT01 frames are going from very transparent to opaque
    if (m_iStaticFrame < STAT10) {
        m_iTrans = 255 - healthBar(192, (STAT12 - m_iStaticFrame), 12);
    } else {
        m_iTrans = 255;
    }

    // non-stat01 frames are drawn transparent
    if (m_iStaticFrame != STAT01) {
        m_renderDrawer->renderStrechFullSprite(m_gfxinter->getTexture(m_iStaticFrame), m_RectFullMinimap,m_iTrans);
    }
}

void cMiniMapDrawer::init()
{
    m_status = eMinimapStatus::NOTAVAILABLE;
    m_iStaticFrame = STAT14;
    m_iTrans = 0;
    m_isMouseOver = false;
}

// TODO: Respond to game events instead of using the "think" function (tell, don't ask)
void cMiniMapDrawer::think()
{
    if (m_player->hasAtleastOneStructure(RADAR)) {
        if (m_status == eMinimapStatus::NOTAVAILABLE) {
            m_status = eMinimapStatus::POWERUP;
        }
    }
    else {
        m_status = eMinimapStatus::NOTAVAILABLE;
    }

    if (m_status == eMinimapStatus::NOTAVAILABLE) return;

    bool hasRadarAndEnoughPower = (m_player->getAmountOfStructuresForType(RADAR) > 0) && m_player->bEnoughPower();

    // minimap state is enough power
    if (m_status == eMinimapStatus::POWERUP || m_status == eMinimapStatus::RENDERMAP) {
        if (!hasRadarAndEnoughPower) {
            // go to state power down (not enough power)
            m_status = eMinimapStatus::POWERDOWN;
            // "Radar de-activated""
            game.playVoice(SOUND_VOICE_04_ATR, m_player->getId());
        }
    }

    // minimap state is not enough power
    if (m_status == eMinimapStatus::POWERDOWN || m_status == eMinimapStatus::LOWPOWER) {
        if (hasRadarAndEnoughPower) {
            // go to state power up (enough power)
            m_status = eMinimapStatus::POWERUP;
            game.playSound(SOUND_RADAR);
            // "Radar activated"
            game.playVoice(SOUND_VOICE_03_ATR, m_player->getId());
        }
    }

    // think about the animation

    if (m_status == eMinimapStatus::POWERDOWN) {
        if (m_iStaticFrame < STAT21) {
            m_iStaticFrame++;
        }
        else {
            m_status = eMinimapStatus::LOWPOWER;
        }
    }
    else if (m_status == eMinimapStatus::POWERUP) {
        if (m_iStaticFrame > STAT01) {
            m_iStaticFrame--;
        }
        else {
            m_status = eMinimapStatus::RENDERMAP;
        }
    }
}

bool cMiniMapDrawer::isMouseOver()
{
    return m_isMouseOver;
}

void cMiniMapDrawer::setPlayer(cPlayer *thePlayer)
{
    this->m_player = thePlayer;
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

void cMiniMapDrawer::drawTerrain()
{
    m_renderDrawer->beginDrawingToTexture(m_mipMapTex);
    Color iColor = Color{0, 0, 0,255};

    for (int x = 0; x < (m_map->getWidth()); x++) {
        for (int y = 0; y < (m_map->getHeight()); y++) {
            iColor = Color{0, 0, 0,255};
            //@mira where is map ?
            int iCll = m_map->getGeometry().makeCell(x, y);

            if (m_map->isVisible(iCll, m_player->getId())) {
                iColor = getRGBColorForTerrainType(m_map->getCellType(iCll));
            }

            // TODO: make flexible map borders
            // do not show the helper border
            if (!m_map->isWithinBoundaries(x, y)) {
                iColor = Color{0, 0, 0,255};
            }

            m_renderDrawer->renderDot(x, y, iColor, 1);
        }
    }
    m_renderDrawer->endDrawingToTexture();
}

void cMiniMapDrawer::drawViewPortRectangle()
{
    // Draw the magic rectangle (viewport)
    int iWidth = (m_mapCamera->getViewportWidth()) / TILESIZE_WIDTH_PIXELS;
    int iHeight = (m_mapCamera->getViewportHeight()) / TILESIZE_HEIGHT_PIXELS;
    iWidth--;
    iHeight--;

    int pixelSize = m_factorZoom;

    int startX = m_drawX + ((m_mapCamera->getViewportStartX() / TILESIZE_WIDTH_PIXELS) * pixelSize);
    int startY = m_drawY + ((m_mapCamera->getViewportStartY() / TILESIZE_HEIGHT_PIXELS) * pixelSize);

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
    m_renderDrawer->renderRectColor(startX, startY, minimapWidth, minimapHeight, Color{255, 255, 255,255});
}

/**
 * Draws minimap units and structures.
 *
 * @param playerOnly (if false, draws all other players than player)
 */
void cMiniMapDrawer::drawUnitsAndStructures(bool playerOnly) const {
    m_renderDrawer->beginDrawingToTexture(m_mipMapTex);
    const Color black = Color::black();
    for (int x = 0; x < m_map->getWidth(); x++) {
        for (int y = 0; y < m_map->getHeight(); y++) {
            // do not show the helper border
            if (!m_map->isWithinBoundaries(x, y)) continue;

            int iCll = m_map->getGeometry().makeCell(x, y);

            if (!m_map->isVisible(iCll, m_player->getId())) {
                // invisible cell
                continue;
            }

            // default : black
            Color iColor = Color{0,0,0,255};

            int idOfStructureAtCell = m_map->getCellIdStructuresLayer(iCll);
            if (idOfStructureAtCell > -1) {
                int iPlr = game.m_gameObjectsContext->getStructures()[idOfStructureAtCell]->getOwner();
                if (playerOnly) {
                    if (iPlr != m_player->getId()) continue; // skip non player units
                }
                iColor = game.getPlayer(iPlr).getMinimapColor();
            }

            int idOfUnitAtCell = m_map->getCellIdUnitLayer(iCll);
            if (idOfUnitAtCell > -1) {
                int iPlr = game.getUnit(idOfUnitAtCell).iPlayer;
                if (playerOnly) {
                    if (iPlr != m_player->getId()) continue; // skip non player units
                }
                iColor = game.getPlayer(iPlr).getMinimapColor();
            }

            int idOfAirUnitAtCell = m_map->getCellIdAirUnitLayer(iCll);
            if (idOfAirUnitAtCell > -1) {
                int iPlr = game.getUnit(idOfAirUnitAtCell).iPlayer;
                if (playerOnly) {
                    if (iPlr != m_player->getId()) continue; // skip non player units
                }
                iColor = game.getPlayer(iPlr).getMinimapColor();
            }

            int idOfWormAtCell = m_map->getCellIdWormsLayer(iCll);
            if (idOfWormAtCell > -1) {
                if (playerOnly) {
                    continue; // skip sandworms
                }
                iColor = m_player->getSelectFadingColor();
            }

            // no need to draw black on black background
            if (iColor.r == black.r && iColor.g == black.g && iColor.b == black.b) {
                continue;
            }
            m_renderDrawer->renderDot(x, y, iColor, 1);
        }
    }
    m_renderDrawer->endDrawingToTexture();
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

int cMiniMapDrawer::getMapHeightInPixels() const {
    return m_map->getHeight();
}

int cMiniMapDrawer::getMapWidthInPixels() const {
    return m_map->getWidth();
}

void cMiniMapDrawer::onMouseAt(const s_MouseEvent &event)
{
    m_isMouseOver = m_RectMinimap.isPointWithin(event.coords.x, event.coords.y);
}

void cMiniMapDrawer::onMousePressedLeft(const s_MouseEvent &event)
{
    if (m_RectFullMinimap.isPointWithin(event.coords.x, event.coords.y) && // on minimap space
        !game.getMouse()->isBoxSelecting() // pressed the mouse and not boxing anything..
    ) {

        if (m_player->hasAtleastOneStructure(RADAR)) {
            auto m_mouse = game.getMouse();
            int mouseCellOnMinimap = getMouseCell(m_mouse->getX(), m_mouse->getY());
            m_mapCamera->centerAndJumpViewPortToCell(mouseCellOnMinimap);
        }
    }
}

void cMiniMapDrawer::cleanDrawTerrain() const {
    m_renderDrawer->beginDrawingToTexture(m_mipMapTex);
    m_renderDrawer->renderClearToColor(Color::black());
    m_renderDrawer->endDrawingToTexture();
}

int cMiniMapDrawer::getMouseCell(int mouseX, int mouseY)
{
    int mouseMiniMapX = mouseX - m_drawX;
    int mouseMiniMapY = mouseY - m_drawY;
    mouseMiniMapX /= m_factorZoom;
    mouseMiniMapY /= m_factorZoom;
    auto mouseMiniMapPoint = m_map->fixCoordinatesToBeWithinPlayableMap(mouseMiniMapX, mouseMiniMapY);

    return m_map->getGeometry().getCellWithMapBorders(mouseMiniMapPoint.x, mouseMiniMapPoint.y);
}
