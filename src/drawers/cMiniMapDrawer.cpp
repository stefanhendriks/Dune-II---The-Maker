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

#include <cassert>

cMiniMapDrawer::cMiniMapDrawer(cMap *theMap, cPlayer *thePlayer, cMapCamera *theMapCamera) :
    m_isMouseOver(false),
    map(theMap),
    player(thePlayer),
    mapCamera(theMapCamera),
    status(eMinimapStatus::NOTAVAILABLE),
    iStaticFrame(STAT14),
    iTrans(0),
    isBigMap(theMap->isBigMap())
{
    assert(theMap);
    assert(thePlayer);
    assert(theMapCamera);

    int halfWidthOfMinimap = cSideBar::WidthOfMinimap / 2;
    int halfWidthOfMap = getMapWidthInPixels() / 2;
    int topLeftX = game.m_screenW - cSideBar::WidthOfMinimap;
    drawX = topLeftX + (halfWidthOfMinimap - halfWidthOfMap);

    int halfHeightOfMinimap = cSideBar::HeightOfMinimap / 2;
    int halfHeightOfMap = getMapHeightInPixels() / 2;
    int topLeftY = cSideBar::TopBarHeight;
    drawY = topLeftY + (halfHeightOfMinimap - halfHeightOfMap);

    m_RectMinimap = cRectangle(drawX, drawY, getMapWidthInPixels(), getMapHeightInPixels());
    m_RectFullMinimap = cRectangle(topLeftX, topLeftY, cSideBar::WidthOfMinimap, cSideBar::HeightOfMinimap);
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
    isBigMap = map->isBigMap();
    m_isMouseOver = false;
}

void cMiniMapDrawer::drawViewPortRectangle()
{
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

    //_rect(bmp_screen, startX, startY, startX + minimapWidth, startY + minimapHeight, SDL_Color{255, 255, 255));
    renderDrawer->renderRectColor(startX, startY, minimapWidth, minimapHeight, SDL_Color{255, 255, 255,255});
}

int cMiniMapDrawer::getMapWidthInPixels()
{
    // for now, it always uses double pixels. But it could be 1 tile = 1 pixel later when map dimensions can be bigger.
    if (map->isBigMap()) {
        return map->getWidth();
    }
    return map->getWidth() * 2; // double pixel size
}

int cMiniMapDrawer::getMapHeightInPixels()
{
    // for now, it always uses double pixels. But it could be 1 tile = 1 pixel later when map dimensions can be bigger.
    if (map->isBigMap()) {
        return map->getHeight();
    }
    return map->getHeight() * 2;
}

void cMiniMapDrawer::drawTerrain()
{
    SDL_Color iColor = SDL_Color{0, 0, 0,255};

    for (int x = 0; x < (map->getWidth()); x++) {
        for (int y = 0; y < (map->getHeight()); y++) {
            iColor = SDL_Color{0, 0, 0,255};
            int iCll = map->makeCell(x, y);

            if (map->isVisible(iCll, player->getId())) {
                iColor = getRGBColorForTerrainType(map->getCellType(iCll));
            }

            // TODO: make flexible map borders
            // do not show the helper border
            if (!map->isWithinBoundaries(x, y)) {
                iColor = SDL_Color{0, 0, 0,255};
            }

            int iDrawX = drawX + x;
            int iDrawY = drawY + y;

            if (!isBigMap) {
                // double sized 'pixels'.
                iDrawX += x;
                iDrawY += y;
            }
            renderDrawer->renderDot(iDrawX, iDrawY, iColor, isBigMap ? 1 : 2);
        }
    }
}

/**
 * Draws minimap units and structures.
 *
 * @param playerOnly (if false, draws all other players than player)
 */
void cMiniMapDrawer::drawUnitsAndStructures(bool playerOnly)
{

    SDL_Color iColor = renderDrawer->getColor_BLACK();

    for (int x = 0; x < map->getWidth(); x++) {
        for (int y = 0; y < map->getHeight(); y++) {
            // do not show the helper border
            if (!map->isWithinBoundaries(x, y)) continue;

            int iCll = map->makeCell(x, y);

            if (!map->isVisible(iCll, player->getId())) {
                // invisible cell
                continue;
            }

            iColor = renderDrawer->getColor_BLACK();

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
            auto black = renderDrawer->getColor_BLACK();
            if (iColor.r != black.r && iColor.g != black.g && iColor.b != black.b) {
                int iDrawX = drawX + x;
                int iDrawY = drawY + y;

                if (!isBigMap) {
                    iDrawX += x;
                    iDrawY += y;
                }
                renderDrawer->renderDot(iDrawX, iDrawY, iColor, isBigMap ? 1 : 2);
            }
        }
    }
}


SDL_Color cMiniMapDrawer::getRGBColorForTerrainType(int terrainType)
{
    // get color for terrain type (for minimap)
    switch (terrainType) {
        case TERRAIN_ROCK:
            return SDL_Color{80, 80, 60,255};
        case TERRAIN_SPICE:
            return SDL_Color{186, 93, 32,255};
        case TERRAIN_SPICEHILL:
            return SDL_Color{180, 90, 25,255};
        case TERRAIN_HILL:
            return SDL_Color{188, 115, 50,255};
        case TERRAIN_MOUNTAIN:
            return SDL_Color{48, 48, 36,255};
        case TERRAIN_SAND:
            return SDL_Color{194, 125, 60,255};
        case TERRAIN_WALL:
            return SDL_Color{192, 192, 192,255};
        case TERRAIN_SLAB:
            return SDL_Color{80, 80, 80,255};
        case TERRAIN_BLOOM:
            return SDL_Color{214, 145, 100,255};
        case -1:
            return SDL_Color{255, 0, 255,255};
        default:
            return SDL_Color{255, 0, 255,255};
    }
}

void cMiniMapDrawer::draw()
{
    if (!map) return;

    if (status == eMinimapStatus::NOTAVAILABLE) return;

    renderDrawer->drawRectFilled(bmp_screen, m_RectFullMinimap, SDL_Color{0, 0, 0,255});
    auto tmp = SDL_Rect{m_RectFullMinimap.getX(), m_RectFullMinimap.getY(),
                        m_RectFullMinimap.getWidth(), m_RectFullMinimap.getHeight()};
    SDL_SetClipRect(bmp_screen, &tmp);

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
    SDL_SetClipRect(bmp_screen, nullptr);
}

void cMiniMapDrawer::drawStaticFrame()
{
    if (status == eMinimapStatus::NOTAVAILABLE) return;
    if (status == eMinimapStatus::RENDERMAP) return;
    if (status == eMinimapStatus::LOWPOWER) return;

    if (status == eMinimapStatus::POWERDOWN) {
        renderDrawer->renderSprite(gfxinter->getTexture(iStaticFrame), drawX, drawY);
        return;
    }

    // Draw static info
    // < STAT01 frames are going from very transparent to opaque
    if (iStaticFrame < STAT10) {
        iTrans = 255 - health_bar(192, (STAT12 - iStaticFrame), 12);
    }
    else {
        iTrans = 255;
    }

    // non-stat01 frames are drawn transparent
    if (iStaticFrame != STAT01) {
        // @Mira fix trasnparency set_trans_blender(0, 0, 0, iTrans);
        renderDrawer->renderSprite(gfxinter->getTexture(iStaticFrame), drawX, drawY);
        //renderDrawer->drawTransSprite(gfxinter->getSurface(iStaticFrame), gfxinter->getSurface(iStaticFrame), drawX, drawY, iTrans);
        // reset the trans blender
        // @Mira fix trasnparency set_trans_blender(0, 0, 0, 128);
    }
}

int cMiniMapDrawer::getMouseCell(int mouseX, int mouseY)
{
    // the minimap can be 128x128 pixels at the bottom right of the screen.
    int mouseMiniMapX = mouseX - drawX;
    int mouseMiniMapY = mouseY - drawY;

    // HACK HACK: Major assumption here - if map dimensions ever get > 64x64 this will BREAK!
    // However, every dot is (due the 64x64 map) 2 pixels wide...
    if (map->getHeight() > 64 || map->getWidth() > 64) {
        // do nothing (assume, 1 cell = 1 pixel)
    }
    else {
        // old behavior assumes 1 cell = 2x2 pixels
        mouseMiniMapX /= 2;
        mouseMiniMapY /= 2;
    }

    auto mouseMiniMapPoint = map->fixCoordinatesToBeWithinPlayableMap(mouseMiniMapX, mouseMiniMapY);

    return map->getCellWithMapBorders(mouseMiniMapPoint.x, mouseMiniMapPoint.y);
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
