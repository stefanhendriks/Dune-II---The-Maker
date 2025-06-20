#include "cDrawManager.h"

#include "controls/cGameControlsContext.h"
#include "d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "drawers/cAllegroDrawer.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "player/cPlayer.h"
#include "drawers/cAllegroDrawer.h"
#include <allegro.h>

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
    , m_optionsBar(nullptr)
    , m_sidebarColor(makecol(214, 149, 20))
    , m_player(thePlayer)
    , m_topBarBmp(nullptr)
    , m_textDrawer(game_font)
{
    assert(thePlayer);
}

cDrawManager::~cDrawManager()
{
    if (m_optionsBar) {
        destroy_bitmap(m_optionsBar);
    }
    if (m_topBarBmp) {
        destroy_bitmap(m_topBarBmp);
    }
}

void cDrawManager::drawCombatState()
{
    // MAP
    renderDrawer->setClippingFor(bmp_screen, 0, cSideBar::TopBarHeight, mapCamera->getWindowWidth(), game.m_screenH);
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

    renderDrawer->resetClippingFor(bmp_screen);

    // GUI
    drawSidebar();

    drawOptionBar();

    renderDrawer->setClippingFor(bmp_screen, 0, cSideBar::TopBarHeight, mapCamera->getWindowWidth(), mapCamera->getWindowHeight() + cSideBar::TopBarHeight);
    drawStructurePlacing();
    renderDrawer->resetClippingFor(bmp_screen);

    drawTopBarBackground();
    drawCredits();

    // THE MESSAGE
    drawMessage();

    drawNotifications();

    renderDrawer->resetClippingFor(bmp_screen);

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
    m_textDrawer.drawTextWithTwoIntegers(0, startY, "Units %d/%d", unitsUsed, MAX_UNITS);
    m_textDrawer.drawTextWithTwoIntegers(0, startY + 1*height, "Structures %d/%d", structuresUsed, MAX_STRUCTURES);
    m_textDrawer.drawTextWithTwoIntegers(0, startY + 2*height, "Bullets %d/%d", bulletsUsed, MAX_BULLETS);
    m_textDrawer.drawTextWithTwoIntegers(0, startY + 3*height, "Particles %d/%d", particlesUsed, MAX_PARTICLES);
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

    set_trans_blender(0,0,0,128);
    int drawX = mapCamera->getWindowXPositionFromCell(rallyPointCell);
    int drawY = mapCamera->getWindowYPositionFromCell(rallyPointCell);

    BITMAP *mouseMoveBitmap = (BITMAP *) gfxdata[MOUSE_MOVE].dat;

    int rallyPointWidthScaled = mapCamera->factorZoomLevel(mouseMoveBitmap->w);
    int rallyPointHeightScaled = mapCamera->factorZoomLevel(mouseMoveBitmap->h);
    renderDrawer->stretchSprite(mouseMoveBitmap, bmp_screen, drawX, drawY, rallyPointWidthScaled, rallyPointHeightScaled);

    int startX = theStructure->iDrawX() + mapCamera->factorZoomLevel(theStructure->getWidthInPixels() / 2);
    int startY = theStructure->iDrawY() + mapCamera->factorZoomLevel(theStructure->getHeightInPixels() / 2);

    int offset = (mouseMoveBitmap->w/2);
    drawX = mapCamera->getWindowXPositionFromCellWithOffset(rallyPointCell, offset);
    drawY = mapCamera->getWindowYPositionFromCellWithOffset(rallyPointCell, offset);

    int endX = drawX;
    int endY = drawY;

    renderDrawer->drawLine(bmp_screen, startX, startY, endX, endY, players[HUMAN].getMinimapColor());
}

void cDrawManager::drawSidebar()
{
    renderDrawer->setClippingFor(bmp_screen, game.m_screenW - cSideBar::SidebarWidth, 0, game.m_screenW, game.m_screenH);
    m_sidebarDrawer.draw();
    miniMapDrawer.draw();
    renderDrawer->resetClippingFor(bmp_screen);
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
    if (context->shouldDrawToolTip()) {
        m_mouseDrawer.drawToolTip();
    }
}

void cDrawManager::drawMouse()
{
    select_mouse_cursor(MOUSE_CURSOR_ALLEGRO);
    m_mouseDrawer.draw();
}

void cDrawManager::drawTopBarBackground()
{
    if (m_topBarBmp == nullptr) {
        m_topBarBmp = create_bitmap(game.m_screenW, 30);
        BITMAP *topbarPiece = (BITMAP *)gfxinter[BMP_TOPBAR_BACKGROUND].dat;
        for (int x = 0; x < game.m_screenW; x+= topbarPiece->w) {
            renderDrawer->drawSprite(m_topBarBmp, topbarPiece, x, 0);
        }

        set_palette(m_player->pal);

        renderDrawer->drawSprite(m_topBarBmp, (BITMAP *)gfxinter[BTN_OPTIONS].dat, 1, 0);
    }

    renderDrawer->drawSprite(bmp_screen, m_topBarBmp, 0, 0);

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
    renderDrawer->drawRectFilled(bmp_screen, 0, 0, game.m_screenW, cSideBar::TopBarHeight, makecol(0, 0, 0));
    if (m_optionsBar == nullptr) {
        m_optionsBar = create_bitmap(game.m_screenW, 40);
        clear_to_color(m_optionsBar, m_sidebarColor);

        for (int w = 0; w < (game.m_screenW + 800); w += 789) {
            renderDrawer->drawSprite(m_optionsBar, (BITMAP *)gfxinter[BMP_GERALD_TOP_BAR].dat, w, 31);
        }
    }

    renderDrawer->drawSprite(bmp_screen, m_optionsBar, 0, 0);
}

void cDrawManager::drawNotifications()
{
    std::vector<cPlayerNotification> &notifications = m_player->getNotifications();
//    int y = cSideBar::TopBarHeight + 14; // 12 pixels
    int y = game.m_screenH - 11;
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
        if (event.hasKeys(KEY_TAB, KEY_D)) {
            m_mapDrawer.setDrawWithoutShroudTiles(true);
        }
        if (event.hasKey(KEY_G)) {
            m_mapDrawer.setDrawGrid(true);
        }
    }
}

void cDrawManager::onKeyPressed(const cKeyboardEvent &event)
{
    if (game.isDebugMode()) {
        // one of these we're pressed, that's enough info to revert back as it breaks the
        // mandatory 'both keys must be pressed' state:
        if (event.hasEitherKey(KEY_TAB, KEY_D)) {
            m_mapDrawer.setDrawWithoutShroudTiles(false);
        }
        if (event.hasKey(KEY_G)) {
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
//spline(bmp_screen, points, makecol(255,255,255));//will draw a nice loopy curve
