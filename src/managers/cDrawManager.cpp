#include "cDrawManager.h"

#include "controls/cGameControlsContext.h"
#include "d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "player/cPlayer.h"
#include "utils/Graphics.hpp"

#include <SDL2/SDL.h>
#include <cassert>

cDrawManager::cDrawManager(cPlayer *thePlayer)
    : m_sidebarDrawer(thePlayer)
    , m_creditsDrawer(thePlayer)
    , m_orderDrawer(thePlayer)
    , m_mapDrawer(&map, thePlayer, mapCamera)
    , miniMapDrawer(&map, thePlayer, mapCamera)
    , m_particleDrawer()
    , m_messageDrawer()
    , m_placeitDrawer(thePlayer)
    , m_structureDrawer()
    , m_mouseDrawer(thePlayer)
    , m_sidebarColor(Color{214, 149, 20,255})
    , m_player(thePlayer)
    , m_textDrawer(game_font)
{
    assert(thePlayer);
    btnOptions = thePlayer->createTextureFromIndexedSurfaceWithPalette(
            gfxinter->getSurface(BTN_OPTIONS), TransparentColorIndex);
}

cDrawManager::~cDrawManager()
{
    if (btnOptions) {
        delete btnOptions;
    }
}

void cDrawManager::drawCombatState()
{
    // MAP
    renderDrawer->setClippingFor(0, cSideBar::TopBarHeight, mapCamera->getWindowWidth(), game.m_screenH);
    m_mapDrawer.drawTerrain();

    m_structureDrawer.drawStructuresFirstLayer();

    // draw layer 1 (beneath units, on top of terrain)
    m_particleDrawer.determineParticlesToDraw(*game.m_mapViewport);
    m_particleDrawer.drawLowerLayer();

    map.draw_units();

    map.draw_bullets();

    m_structureDrawer.drawStructuresSecondLayer();

    map.draw_units_2nd();

    m_particleDrawer.drawTopLayer();
    m_structureDrawer.drawStructuresHealthBars();

    m_mapDrawer.drawShroud();

    drawRallyPoint();

    renderDrawer->resetClippingFor();

    // GUI
    drawSidebar();

    drawOptionBar();

    renderDrawer->setClippingFor(0, cSideBar::TopBarHeight, mapCamera->getWindowWidth(), mapCamera->getWindowHeight() + cSideBar::TopBarHeight);
    drawStructurePlacing();
    renderDrawer->resetClippingFor();

    drawTopBarBackground();
    drawCredits();

    // THE MESSAGE
    drawMessage();

    drawNotifications();

    renderDrawer->resetClippingFor();

    if (game.m_drawUsages) {
        drawDebugInfoUsages();
        m_particleDrawer.drawDebugInfo();
    }
}

void cDrawManager::drawDebugInfoUsages() const
{
    int unitsUsed = 0;
    for (int i = 0; i < MAX_UNITS; i++) {
        if (unit[i].isValid()) {
            unitsUsed++;
        }
    }

    int structuresUsed = 0;
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = structure[i];
        if (pStructure) {
            structuresUsed++;
        }
    }

    int bulletsUsed = 0;
    for (int i = 0; i < MAX_BULLETS; i++) {
        cBullet &pBullet = bullet[i];
        if (pBullet.bAlive) {
            bulletsUsed++;
        }
    }

    int particlesUsed = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        cParticle &pParticle = particle[i];
        if (pParticle.isValid()) {
            particlesUsed++;
        }
    }

    int startY = 74;
    int height = 14;
    m_textDrawer.drawText(0, startY, std::format("Units {}/{}", unitsUsed, MAX_UNITS));
    m_textDrawer.drawText(0, startY + 1*height, std::format("Structures %d/%d", structuresUsed, MAX_STRUCTURES));
    m_textDrawer.drawText(0, startY + 2*height, std::format("Bullets %d/%d", bulletsUsed, MAX_BULLETS));
    m_textDrawer.drawText(0, startY + 3*height, std::format("Particles %d/%d", particlesUsed, MAX_PARTICLES));
}

void cDrawManager::drawCredits()
{
    m_creditsDrawer.draw();
}

void cDrawManager::drawRallyPoint()
{
    cPlayer &humanPlayer = players[HUMAN];
    if (humanPlayer.selected_structure < 0) return;
    cAbstractStructure *theStructure = structure[humanPlayer.selected_structure];
    if (!theStructure) return;
    int rallyPointCell = theStructure->getRallyPoint();
    if (rallyPointCell < 0) return;

    int drawX = mapCamera->getWindowXPositionFromCell(rallyPointCell);
    int drawY = mapCamera->getWindowYPositionFromCell(rallyPointCell);

    SDL_Surface *mouseMoveBitmap = gfxdata->getSurface(MOUSE_MOVE);

    int rallyPointWidthScaled = mapCamera->factorZoomLevel(mouseMoveBitmap->w);
    int rallyPointHeightScaled = mapCamera->factorZoomLevel(mouseMoveBitmap->h);
    cRectangle src = {0,0,mouseMoveBitmap->w,mouseMoveBitmap->h};
    cRectangle dest = {drawX, drawY, rallyPointWidthScaled, rallyPointHeightScaled};
    renderDrawer->renderStrechSprite(gfxdata->getTexture(MOUSE_MOVE), src, dest);

    int startX = theStructure->iDrawX() + mapCamera->factorZoomLevel(theStructure->getWidthInPixels() / 2);
    int startY = theStructure->iDrawY() + mapCamera->factorZoomLevel(theStructure->getHeightInPixels() / 2);

    int offset = (mouseMoveBitmap->w/2);
    drawX = mapCamera->getWindowXPositionFromCellWithOffset(rallyPointCell, offset);
    drawY = mapCamera->getWindowYPositionFromCellWithOffset(rallyPointCell, offset);

    int endX = drawX;
    int endY = drawY;

    renderDrawer->renderLine( startX, startY, endX, endY, players[HUMAN].getMinimapColor());
}

void cDrawManager::drawSidebar()
{
    renderDrawer->setClippingFor(game.m_screenW - cSideBar::SidebarWidth, 0, game.m_screenW, game.m_screenH);
    m_sidebarDrawer.draw();
    miniMapDrawer.draw();
    renderDrawer->resetClippingFor();
}

/**
 * When placing a structure, draw a transparent version of the structure we want to place.
 */
void cDrawManager::drawStructurePlacing()
{
    // TODO: move drawing this into mouse state draw function ??
    if (m_player->isContextMouseState(eMouseState::MOUSESTATE_DEPLOY)) return;
    if (!m_player->isContextMouseState(eMouseState::MOUSESTATE_PLACE)) return;

    cBuildingListItem *itemToPlace = m_player->getSideBar()->getList(eListType::LIST_CONSTYARD)->getItemToPlace();
    if (itemToPlace == nullptr) return;

    m_placeitDrawer.draw(itemToPlace, m_player->getGameControlsContext()->getMouseCell());
}

void cDrawManager::drawMessage()
{
    m_messageDrawer.draw();

    // TODO: replace messageDrawer with drawMessageBar?
    // messageBarDrawer->drawMessageBar();
}

void cDrawManager::drawCombatMouse()
{
    drawMouse();

    cGameControlsContext *context = m_player->getGameControlsContext();
    //context->isMouseRightButtonPressed()
    if (context->shouldDrawToolTip()) {
        m_mouseDrawer.drawToolTip();
    }
}

void cDrawManager::drawMouse()
{
    m_mouseDrawer.draw();
}

void cDrawManager::drawTopBarBackground()
{
    Texture *topbarPiece = gfxinter->getTexture(BMP_TOPBAR_BACKGROUND);
    for (int x = 0; x < game.m_screenW; x+= topbarPiece->w) {
        renderDrawer->renderSprite(topbarPiece, x, 0);
    }

    renderDrawer->renderSprite(btnOptions, 1, 0);

    //HACK HACK: for now do it like this, instead of using an actual GUI object here
    cRectangle optionsRect = cRectangle(0,0, 162, 30);
    if (game.getMouse()->isLeftButtonClicked() && game.getMouse()->isOverRectangle(&optionsRect)) {
        game.setNextStateToTransitionTo(GAME_OPTIONS);
    }
}

void cDrawManager::setPlayerToDraw(cPlayer *playerToDraw)
{
    m_player = playerToDraw;
    m_creditsDrawer.setPlayer(playerToDraw);
    m_sidebarDrawer.setPlayer(playerToDraw);
    m_orderDrawer.setPlayer(playerToDraw);
    miniMapDrawer.setPlayer(playerToDraw);
    m_mapDrawer.setPlayer(playerToDraw);
}

void cDrawManager::drawOptionBar()
{
    // upper bar
    renderDrawer->renderRectFillColor(0, 0, game.m_screenW, cSideBar::TopBarHeight, Color{0, 0, 0,255});
    renderDrawer->renderRectFillColor(0,game.m_screenW, 40,32, Color{214,149,20,255});

    for (int w = 0; w < (game.m_screenW + 800); w += 789) {
        renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_TOP_BAR), w, 31);
    }
}

void cDrawManager::drawNotifications()
{
    std::vector<cPlayerNotification> &notifications = m_player->getNotifications();
//    int y = cSideBar::TopBarHeight + 14; // 12 pixels
    int y = game.m_screenH - 44;
    for (auto &notification : notifications) {
        m_textDrawer.drawText(4, y, notification.getColor(), notification.getMessage().c_str());
        y-=15;
    }
}

void cDrawManager::think()
{
    miniMapDrawer.think();
}

void cDrawManager::init()
{
    miniMapDrawer.init();
}

void cDrawManager::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_sidebarDrawer.onNotifyMouseEvent(event);
}

void cDrawManager::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    m_sidebarDrawer.onNotifyKeyboardEvent(event);

    switch (event.eventType) {
        case eKeyEventType::HOLD:
            onKeyDown(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressed(event);
            break;
        default:
            break;
    }
}

void cDrawManager::onKeyDown(const cKeyboardEvent &event)
{
    if (game.isDebugMode()) {
        if (event.hasKeys(SDL_SCANCODE_TAB, SDL_SCANCODE_D)) {
            m_mapDrawer.setDrawWithoutShroudTiles(true);
        }
        if (event.hasKey(SDL_SCANCODE_G)) {
            m_mapDrawer.setDrawGrid(true);
        }
    }
}

void cDrawManager::onKeyPressed(const cKeyboardEvent &event)
{
    if (game.isDebugMode()) {
        // one of these we're pressed, that's enough info to revert back as it breaks the
        // mandatory 'both keys must be pressed' state:
        if (event.hasEitherKey(SDL_SCANCODE_TAB, SDL_SCANCODE_D)) {
            m_mapDrawer.setDrawWithoutShroudTiles(false);
        }
        if (event.hasKey(SDL_SCANCODE_G)) {
            m_mapDrawer.setDrawGrid(false);
        }
    }
}

void cDrawManager::missionInit()
{
    m_creditsDrawer.setCredits();
    m_messageDrawer.initCombatPosition();
}

void cDrawManager::thinkFast_statePlaying()
{
    m_creditsDrawer.thinkFast();
}

void cDrawManager::thinkFast()
{
    m_messageDrawer.thinkFast();
}

void cDrawManager::drawMessageBar()
{
    m_messageDrawer.draw();
}

void cDrawManager::setMessage(std::string msg, bool keepMessage)
{
    m_messageDrawer.setMessage(msg, keepMessage);
}

void cDrawManager::setKeepMessage(bool value)
{
    m_messageDrawer.setKeepMessage(value);
}

void cDrawManager::regionInit(int offsetX, int offsetY)
{
    m_messageDrawer.initRegionPosition(offsetX, offsetY);
}

bool cDrawManager::hasMessage()
{
    return m_messageDrawer.hasMessage();
}


//int points[] =
//{
//    0,0,
//    100,100,
//    540,100,
//    640,0
//};
//
//spline(bmp_screen, points, Color{255,255,255));//will draw a nice loopy curve
