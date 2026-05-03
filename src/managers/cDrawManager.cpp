#include "cDrawManager.h"

#include "controls/cGameControlsContext.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "gameobjects/units/cUnits.h"
#include "utils/Graphics.hpp"
#include "utils/texture_utils.h"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/cMap.h"
#include "drawers/cSideBarDrawer.h"

#include <SDL2/SDL.h>
#include <cassert>

cDrawManager::cDrawManager(GameContext *ctx, cPlayer *thePlayer) :
    m_sidebarColor(Color{214, 149, 20,255}),
    m_player(thePlayer),
    m_ctx(ctx),
    m_textDrawer(ctx->getTextContext()->getGameTextDrawer()),
    m_renderDrawer(ctx->getSDLDrawer()),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get()),
    m_gfxdata(ctx->getGraphicsContext()->gfxdata.get())
{
    assert(m_player!=nullptr);
    assert(ctx != nullptr);
    assert(m_textDrawer != nullptr);
    assert(m_renderDrawer != nullptr);
    assert(m_gfxdata != nullptr);
    assert(m_gfxinter != nullptr);
    this->reset();
}

void cDrawManager::reset()
{
    m_sidebarColor = Color{214, 149, 20,255};
    m_sidebarDrawer = std::make_unique<cSideBarDrawer>(m_ctx, m_player);
    m_creditsDrawer = std::make_unique<CreditsDrawer>(m_ctx, m_player);
    m_orderDrawer = std::make_unique<cOrderDrawer>(m_ctx, m_player);
    m_mapDrawer = std::make_unique<cMapDrawer>(m_ctx, &game.m_gameObjectsContext->getMap(), m_player, game.m_mapCamera);
    m_miniMapDrawer = std::make_unique<cMiniMapDrawer>(m_ctx, &game.m_gameObjectsContext->getMap(), m_player, game.m_mapCamera);
    m_particleDrawer = std::make_unique<cParticleDrawer>();
    m_messageDrawer = std::make_unique<cMessageDrawer>(m_ctx);
    m_placeitDrawer = std::make_unique<cPlaceItDrawer>(m_ctx,m_player);
    m_structureDrawer = std::make_unique<cStructureDrawer>(m_ctx);
    m_btnOptions = createPlayerTextureFromIndexedSurfaceWithPalette(
        m_player, m_gfxinter->getSurface(BTN_OPTIONS), TransparentColorIndex
    );
    m_mouseDrawer = new cMouseDrawer(m_player, m_ctx->getTextContext()->getSmallTextDrawer());
}

cDrawManager::~cDrawManager()
{
    if (m_btnOptions) {
        delete m_btnOptions;
    }
}

void cDrawManager::drawCombatState()
{
    // MAP
    m_renderDrawer->setClippingFor(0, cSideBar::TopBarHeight, game.m_mapCamera->getWindowWidth(), game.m_gameSettings->getScreenH());
    m_mapDrawer->drawTerrain();

    m_structureDrawer->drawStructuresFirstLayer();

    // draw layer 1 (beneath units, on top of terrain)
    m_particleDrawer->determineParticlesToDraw(*game.m_mapViewport);
    m_particleDrawer->drawLowerLayer();

    game.m_gameObjectsContext->getMap().draw_units();

    game.m_gameObjectsContext->getMap().draw_bullets();

    m_structureDrawer->drawStructuresSecondLayer();

    game.m_gameObjectsContext->getMap().draw_units_2nd();

    m_particleDrawer->drawTopLayer();
    m_structureDrawer->drawStructuresHealthBars();

    m_mapDrawer->drawShroud();

    drawRallyPoint();

    m_renderDrawer->resetClippingFor();

    // GUI
    drawSidebar();

    drawOptionBar();

    m_renderDrawer->setClippingFor(0, cSideBar::TopBarHeight, game.m_mapCamera->getWindowWidth(), game.m_mapCamera->getWindowHeight() + cSideBar::TopBarHeight);
    drawStructurePlacing();
    m_renderDrawer->resetClippingFor();

    drawTopBarBackground();
    drawCredits();

    // THE MESSAGE
    drawMessage();

    drawNotifications();

    m_renderDrawer->resetClippingFor();

    if (game.m_gameSettings->isDrawUsages()) {
        drawDebugInfoUsages();
        m_particleDrawer->drawDebugInfo(m_textDrawer);
    }
}

void cDrawManager::drawDebugInfoUsages() const
{
    int unitsUsed = 0;
    for (int i = 0; i < game.m_gameObjectsContext->getUnitsSize(); i++) {
        if (game.m_gameObjectsContext->getUnit(i)->isValid()) {
            unitsUsed++;
        }
    }

    int structuresUsed = 0;
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = game.m_gameObjectsContext->getStructures()[i];
        if (pStructure) {
            structuresUsed++;
        }
    }

    int bulletsUsed = 0;
    for (const auto& bullet : game.m_gameObjectsContext->getBullets()) {
        if (bullet.bAlive) {
            bulletsUsed++;
        }
    }

    int particlesUsed = 0;
    for (const auto& particle : game.m_gameObjectsContext->getParticles()) {
        if (particle.isValid()) {
            particlesUsed++;
        }
    }

    int startY = 74;
    int height = 14;
    m_textDrawer->drawText(0, startY, std::format("Units {}/{}", unitsUsed, game.m_gameObjectsContext->getUnitsSize()));
    m_textDrawer->drawText(0, startY + 1*height, std::format("Structures %d/%d", structuresUsed, MAX_STRUCTURES));
    m_textDrawer->drawText(0, startY + 2*height, std::format("Bullets %d/%d", bulletsUsed, game.m_gameObjectsContext->getBullets().size()));
    m_textDrawer->drawText(0, startY + 3*height, std::format("Particles %d/%d", particlesUsed, game.m_gameObjectsContext->getParticles().size()));
}

void cDrawManager::drawCredits()
{
    m_creditsDrawer->draw();
}

void cDrawManager::drawRallyPoint()
{
    cPlayer* humanPlayer = game.m_gameObjectsContext->getPlayer(HUMAN);
    if (humanPlayer->selected_structure < 0) return;
    cAbstractStructure *theStructure = game.m_gameObjectsContext->getStructures()[humanPlayer->selected_structure];
    if (!theStructure) return;
    int rallyPointCell = theStructure->getRallyPoint();
    if (rallyPointCell < 0) return;

    int drawX = game.m_mapCamera->getWindowXPositionFromCell(rallyPointCell);
    int drawY = game.m_mapCamera->getWindowYPositionFromCell(rallyPointCell);

    SDL_Surface *mouseMoveBitmap = m_gfxdata->getSurface(MOUSE_MOVE);

    int rallyPointWidthScaled = game.m_mapCamera->factorZoomLevel(mouseMoveBitmap->w);
    int rallyPointHeightScaled = game.m_mapCamera->factorZoomLevel(mouseMoveBitmap->h);
    cRectangle dest = {drawX, drawY, rallyPointWidthScaled, rallyPointHeightScaled};
    m_renderDrawer->renderStrechFullSprite(m_gfxdata->getTexture(MOUSE_MOVE), dest);

    int startX = theStructure->iDrawX() + game.m_mapCamera->factorZoomLevel(theStructure->getWidthInPixels() / 2);
    int startY = theStructure->iDrawY() + game.m_mapCamera->factorZoomLevel(theStructure->getHeightInPixels() / 2);

    int offset = (mouseMoveBitmap->w/2);
    drawX = game.m_mapCamera->getWindowXPositionFromCellWithOffset(rallyPointCell, offset);
    drawY = game.m_mapCamera->getWindowYPositionFromCellWithOffset(rallyPointCell, offset);

    int endX = drawX;
    int endY = drawY;

    m_renderDrawer->renderLine( startX, startY, endX, endY, game.m_gameObjectsContext->getPlayer(HUMAN)->getMinimapColor());
}

void cDrawManager::drawSidebar()
{
    m_renderDrawer->setClippingFor(game.m_gameSettings->getScreenW() - cSideBar::SidebarWidth, 0, game.m_gameSettings->getScreenW(), game.m_gameSettings->getScreenH());
    m_sidebarDrawer->draw();
    m_miniMapDrawer->draw();
    m_renderDrawer->resetClippingFor();
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

    m_placeitDrawer->draw(itemToPlace, m_player->getGameControlsContext()->getMouseCell());
}

void cDrawManager::drawMessage()
{
    m_messageDrawer->draw();
    // TODO: replace messageDrawer with drawMessageBar?
    // messageBarDrawer->drawMessageBar();
}

void cDrawManager::drawCombatMouse()
{
    drawMouse();

    cGameControlsContext *context = m_player->getGameControlsContext();
    if (m_drawToolTip && context->isMouseOnBattleField() ) {
        m_mouseDrawer->drawToolTip();
    }
}

void cDrawManager::drawMouse()
{
    m_mouseDrawer->draw();
}

void cDrawManager::drawTopBarBackground()
{
    Texture *topbarPiece = m_gfxinter->getTexture(BMP_TOPBAR_BACKGROUND);
    for (int x = 0; x < game.m_gameSettings->getScreenW(); x+= topbarPiece->w) {
        m_renderDrawer->renderSprite(topbarPiece, x, 0);
    }

    m_renderDrawer->renderSprite(m_btnOptions, 1, 0);

    //HACK HACK: for now do it like this, instead of using an actual GUI object here
    cRectangle optionsRect = cRectangle(0,0, 162, 30);
    if (game.getMouse()->isLeftButtonClicked() && game.getMouse()->isOverRectangle(&optionsRect)) {
        game.setNextStateToTransitionTo(GAME_OPTIONS);
    }
}

void cDrawManager::setPlayerToDraw(cPlayer *playerToDraw)
{
    m_player = playerToDraw;
    m_creditsDrawer->setPlayer(playerToDraw);
    m_sidebarDrawer->setPlayer(playerToDraw);
    m_orderDrawer->setPlayer(playerToDraw);
    m_miniMapDrawer->setPlayer(playerToDraw);
    m_mapDrawer->setPlayer(playerToDraw);
}

void cDrawManager::drawOptionBar()
{
    // upper bar
    m_renderDrawer->renderRectFillColor(0, 0, game.m_gameSettings->getScreenW(), cSideBar::TopBarHeight, Color{0, 0, 0,255});
    m_renderDrawer->renderRectFillColor(0,game.m_gameSettings->getScreenW(), 40,32, Color{214,149,20,255});

    for (int w = 0; w < (game.m_gameSettings->getScreenW() + 800); w += 789) {
        m_renderDrawer->renderSprite(m_gfxinter->getTexture(BMP_GERALD_TOP_BAR), w, 31);
    }
}

void cDrawManager::drawNotifications()
{
    std::vector<cPlayerNotification> &notifications = m_player->getNotifications();
//    int y = cSideBar::TopBarHeight + 14; // 12 pixels
    int y = game.m_gameSettings->getScreenH() - 44;
    for (auto &notification : notifications) {
        m_textDrawer->drawText(4, y, notification.getColor(), notification.getMessage().c_str());
        y-=15;
    }
}

void cDrawManager::think()
{
    m_miniMapDrawer->think();
}

void cDrawManager::init()
{
    m_miniMapDrawer->init();
}

void cDrawManager::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_sidebarDrawer->onNotifyMouseEvent(event);
}

void cDrawManager::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    m_sidebarDrawer->onNotifyKeyboardEvent(event);

    switch (event.getType()) {
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
    if (game.m_gameSettings->isDebugMode()) {
        if (event.hasKeys(SDL_SCANCODE_TAB, SDL_SCANCODE_D)) {
            m_mapDrawer->setDrawWithoutShroudTiles(true);
        }
        if (event.hasKey(SDL_SCANCODE_G)) {
            m_mapDrawer->setDrawGrid(true);
        }
    }
    if (event.hasKey(SDL_SCANCODE_T)) {
        m_drawToolTip = true;
    }
}

void cDrawManager::onKeyPressed(const cKeyboardEvent &event)
{
    if (game.m_gameSettings->isDebugMode()) {
        // one of these we're pressed, that's enough info to revert back as it breaks the
        // mandatory 'both keys must be pressed' state:
        if (event.hasEitherKey(SDL_SCANCODE_TAB, SDL_SCANCODE_D)) {
            m_mapDrawer->setDrawWithoutShroudTiles(false);
        }
        if (event.hasKey(SDL_SCANCODE_G)) {
            m_mapDrawer->setDrawGrid(false);
        }
    }
    if (event.hasKey(SDL_SCANCODE_T)) {
        m_drawToolTip = false;
    }
}

void cDrawManager::missionInit()
{
    m_creditsDrawer->setCredits();
    m_messageDrawer->initCombatPosition();
}

void cDrawManager::thinkFast_statePlaying()
{
    m_creditsDrawer->thinkFast();
}

void cDrawManager::thinkFast()
{
    m_messageDrawer->thinkFast();
}

void cDrawManager::drawMessageBar()
{
    m_messageDrawer->draw();
}

void cDrawManager::setMessage(std::string msg, bool keepMessage)
{
    m_messageDrawer->setMessage(msg, keepMessage);
}

void cDrawManager::setKeepMessage(bool value)
{
    m_messageDrawer->setKeepMessage(value);
}

void cDrawManager::regionInit(int offsetX, int offsetY)
{
    m_messageDrawer->initRegionPosition(offsetX, offsetY);
}

bool cDrawManager::hasMessage()
{
    return m_messageDrawer->hasMessage();
}

cBuildingListDrawer *cDrawManager::getBuildingListDrawer() {
    return m_sidebarDrawer->getBuildingListDrawer();
}