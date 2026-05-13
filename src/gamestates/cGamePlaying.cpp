#include "gamestates/cGamePlaying.h"
#include "include/definitions.h"
#include "building/cItemBuilder.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/projectiles/cBullets.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "gameobjects/units/cReinforcements.h"
#include "gameobjects/units/cUnits.h"
#include <SDL2/SDL.h>
#include "managers/cDrawManager.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "controls/cGameControlsContext.h"
#include "context/cGameObjectContext.h"
#include "game/cGameInterface.h"
#include "controls/eKeyAction.h"
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/cMap.h"
#include "drawers/cTextDrawer.h"
#include "utils/cExpandingRectangle.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include "sidebar/cSideBar.h"

cGamePlaying::cGamePlaying(sGameServices* services) :
    cGameState(services),
    m_objects(services->objects),
    m_settings(services->settings)
{
    assert(m_objects != nullptr);
    assert(m_settings != nullptr);
    m_TIMER_evaluatePlayerStatus = 5;

    //fix others pointers
    m_interface = m_ctx->getGameInterface();
    assert(m_interface != nullptr);
    m_mapCamera = m_interface->getMapCamera();
    assert(m_mapCamera != nullptr);
    m_reinforcements = m_interface->getReinforcements();
    assert(m_reinforcements != nullptr);
    m_structureFactory = services->objects->getStructureFactory();
    assert(m_structureFactory != nullptr);
    m_drawManager = m_interface->getRenderDrawManager();
    assert(m_drawManager != nullptr);
    m_textDrawer = m_ctx->getTextContext()->getBeneTextDrawer();
    assert(m_textDrawer != nullptr);

    m_controlledPlayer = m_objects->getPlayer(HUMAN);
    m_drawManager->setPlayerToDraw(m_controlledPlayer);
    m_interface->setPlayerToInteractFor(m_controlledPlayer);
}

cGamePlaying::~cGamePlaying()
{
}

void cGamePlaying::centerCameraOnUnits(const std::vector<int>& unitIds)
{
    int sumX = 0, sumY = 0, count = 0;
    int mapWidth = m_objects->getMap().getWidth();
    for (int id : unitIds) {
        cUnit* u = m_objects->getUnit(id);
        if (!u->isValid()) continue;
        sumX += u->getCellX();
        sumY += u->getCellY();
        count++;
    }
    if (count == 0) return;
    int centroidCell = (sumY / count) * mapWidth + (sumX / count);
    m_mapCamera->centerAndJumpViewPortToCell(centroidCell);
}

void cGamePlaying::thinkFast()
{
    m_drawManager->thinkFast_statePlaying();

    m_mapCamera->thinkFast();

    for (cPlayer* pPlayer : m_objects->getPlayers()) {
        pPlayer->thinkFast();
    }

    // structures think
    for (cAbstractStructure *pStructure : m_objects->getStructures()) {
        if (pStructure == nullptr) continue;
        if (pStructure->isValid()) {
            pStructure->thinkFast();       // think about actions going on
            pStructure->think_animation(); // think about animating
            pStructure->think_guard();     // think about 'guarding' the area (turrets only)
        }

        if (pStructure->isDead()) {
            m_structureFactory->deleteStructureInstance(pStructure);
        }
    }

    for (cPlayer* pPlayer : m_objects->getPlayers()) {
        cItemBuilder *itemBuilder = pPlayer->getItemBuilder();
        if (itemBuilder) {
            itemBuilder->thinkFast();
        }
    }

    m_objects->getMap().thinkFast();

    m_interface->reduceShaking();

    // units think (move only)
    for (cUnit &cUnit : *m_objects->getUnits()) {
        if (!cUnit.isValid()) continue;
        cUnit.thinkFast();
    }

    for (cParticle &pParticle : m_objects->getParticles()) {
        if (!pParticle.isValid()) continue;
        pParticle.thinkFast();
    }

    // when not drawing the options, the game does all it needs to do
    // bullets think
    for (cBullet &cBullet : m_objects->getBullets()) {
        if (!cBullet.bAlive) continue;
        cBullet.thinkFast();
    }

    if (!m_trackedUnitIds.empty()) {
        std::erase_if(m_trackedUnitIds, [&](int id) {
            return !m_objects->getUnit(id)->isValid();
        });
        if (!m_trackedUnitIds.empty()) {
            float sumX = 0, sumY = 0;
            cExpandingRectangle bounds;
            for (int id : m_trackedUnitIds) {
                cUnit* u = m_objects->getUnit(id);
                float px = u->getPosX(), py = u->getPosY();
                sumX += px; sumY += py;
                bounds.expand(px, py);
            }
            bounds.expandBy(3 * TILESIZE_WIDTH_PIXELS);
            m_mapCamera->zoomOutToFit(bounds);

            int count = static_cast<int>(m_trackedUnitIds.size());
            m_mapCamera->trackToAbsPosition(sumX / count, sumY / count);
        }
    }
}

void cGamePlaying::thinkNormal()
{
        // units think
        for (int i = 0; i < m_objects->getUnitsSize(); i++) {
            cUnit *cUnit = m_objects->getUnit(i);
            if (cUnit->isValid()) {
                cUnit->think();
            }
        }

        m_drawManager->think();

        //@mira move to Players
        for (int i = 0; i < MAX_PLAYERS; i++) {
            m_objects->getPlayer(i)->think();
        }
}

void cGamePlaying::thinkSlow()
{
    evaluatePlayerStatus(); // so we can call non-const from a const :S

    m_objects->getMap().thinkSlow();

    if (!m_settings->isDisableReinforcements()) {
        m_reinforcements->thinkSlow();
    }

    // starports think per second for deployment (if any)
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = m_objects->getStructures()[i];
        if (pStructure && pStructure->isValid()) {
            pStructure->thinkSlow();
        }
    }

    //@mira move to players
    for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer* player = m_objects->getPlayer(i);
        player->thinkSlow();
    }
}


void cGamePlaying::drawTrackingOverlays() const
{
    if (m_trackedUnitIds.empty()) return;

    float pulse = 0.5f + 0.5f * std::sin(SDL_GetTicks() * 0.004f);
    Uint8 alpha = static_cast<Uint8>(128 + 127 * pulse);
    Color fadingYellow = Color::Yellow.withAlpha(alpha);

    int count = static_cast<int>(m_trackedUnitIds.size());
    std::string label = count == 1 ? "Tracking 1 unit" : std::format("Tracking {} units", count);

    const cRectangle* battlefield = m_interface->getMapViewport();
    m_textDrawer->drawTextCentered(label, battlefield->getX(), battlefield->getWidth(), cSideBar::TopBarHeight + 10, fadingYellow);

    m_renderDrawer->setClippingFor(battlefield->getX(), battlefield->getY(),
                                   battlefield->getX() + battlefield->getWidth(),
                                   battlefield->getY() + battlefield->getHeight());

    for (int id : m_trackedUnitIds) {
        cUnit* u = m_objects->getUnit(id);
        if (!u->isValid()) continue;
        cRectangle box(u->draw_x(), u->draw_y(),
                       static_cast<int>(m_mapCamera->factorZoomLevel(u->getBmpWidth())),
                       static_cast<int>(m_mapCamera->factorZoomLevel(u->getBmpHeight())));
        m_renderDrawer->renderRectColor(box, Color::Yellow, alpha);
    }

    m_renderDrawer->resetClippingFor();
}

void cGamePlaying::draw() const
{
    m_drawManager->drawCombatState();
    drawTrackingOverlays();
    if (m_settings->shouldDrawFps()) {
        m_interface->drawTextFps();
    }

    drawCombatMouse();

    if (m_settings->shouldDrawTime()) {
        m_interface->drawTextTime();
    }
    // MOUSE
    m_drawManager->drawCombatMouse();
}

void cGamePlaying::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (event.eventType == eMouseEventType::MOUSE_RIGHT_BUTTON_PRESSED && m_interface->getMouse()->isMapScrolling()) {
        m_trackedUnitIds.clear();
    }
}

eGameStateType cGamePlaying::getType()
{
    return GAMESTATE_PLAYING;
}

void cGamePlaying::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    logbook(event.toString());

    m_drawManager->onNotifyKeyboardEvent(event);

    switch (event.getType()) {
        case eKeyEventType::HOLD:
            onKeyDownGamePlaying(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressedGamePlaying(event);
            break;
        default:
            break;
    }
}


void cGamePlaying::evaluatePlayerStatus()
{
    if (m_TIMER_evaluatePlayerStatus > 0) {
        m_TIMER_evaluatePlayerStatus--;
    }
    else {
        // TODO: Better way is with events (ie created/destroyed). However, there is no such
        // bookkeeping per player *yet*. So instead, for now, we "poll" for this data.
        for (int i = 0; i < MAX_PLAYERS; i++) {
            cPlayer* player = m_objects->getPlayer(i);
            bool isAlive = player->isAlive();
            // evaluate all players regardless if they are alive or not (who knows, they became alive?)
            player->evaluateStillAlive();

            if (isAlive && !player->isAlive()) {
                const s_GameEvent event {
                    .eventType = eGameEventType::GAME_EVENT_PLAYER_DEFEATED,
                    .data = CommonEvent {
                        .entityType = eBuildType::SPECIAL,
                        .player = player
                    }
                };
                m_interface->onNotifyGameEvent(event);
            }
            // TODO: event : Player joined/became alive, etc?
        }
        m_TIMER_evaluatePlayerStatus = 2;
    }

    m_interface->checkMissionWinOrFail();
}

void cGamePlaying::drawCombatMouse() const
{
    auto m_mouse = m_interface->getMouse();
    if (m_mouse->isBoxSelecting()) {
        m_renderDrawer->renderRectColor(m_mouse->getBoxSelectRectangle(), m_interface->getColorFadeSelectedLimited(Color::White, 0.5f));
    }

    if (m_mouse->isMapScrolling()) {
        cPoint startPoint = m_mouse->getDragLineStartPoint();
        cPoint endPoint = m_mouse->getDragLineEndPoint();
        m_renderDrawer->renderLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, m_interface->getColorFadeSelectedLimited(Color::White, 0.5f));
    }
    m_mouse->draw();

    if (m_settings->isDebugMode()) {
        int mouseCell = m_objects->getPlayer(HUMAN)->getGameControlsContext()->getMouseCell();
        m_textDrawer->drawText(0, cSideBar::TopBarHeight + 1, std::format("MouseCell {}", mouseCell));
    }
}

void cGamePlaying::missionInit()
{
    m_objects->getMap().thinkSlow();
    m_TIMER_evaluatePlayerStatus = 5;
    m_controlledPlayer = m_objects->getPlayer(HUMAN);
}


void cGamePlaying::onKeyDownGamePlaying(const cKeyboardEvent &event)
{
    const cPlayer* humanPlayer = m_objects->getPlayer(HUMAN);

    if (event.isAction(eKeyAction::ATTACK_MODE)) {
        int iGroup = event.getGroupNumber();
        if (iGroup > 0) {
            humanPlayer->markUnitsForGroup(iGroup);
        }
    }

    if (m_settings->isDebugMode()) { // debug mode has additional keys
        if (event.hasKey(SDL_SCANCODE_TAB)) {
            onKeyDownDebugMode(event);
        }

        if (event.isAction(eKeyAction::DEBUG_CLEAR_SHROUD_AT_CURSOR)) {
            int mouseCell = humanPlayer->getGameControlsContext()->getMouseCell();
            if (mouseCell > -1) {
                m_objects->getMap().clearShroud(mouseCell, 6, HUMAN);
            }
        }
    }

    if (!m_trackedUnitIds.empty() &&
        (event.isAction(eKeyAction::SCROLL_LEFT) || event.isAction(eKeyAction::SCROLL_RIGHT) ||
         event.isAction(eKeyAction::SCROLL_UP)   || event.isAction(eKeyAction::SCROLL_DOWN))) {
        m_trackedUnitIds.clear();
    }

    if (event.isAction(eKeyAction::ZOOM_RESET)) {
        m_mapCamera->resetZoom();
    }

    if (event.isAction(eKeyAction::CENTER_ON_HOME)) {
        m_mapCamera->centerAndJumpViewPortToCell(humanPlayer->getFocusCell());
    }

    if (event.isAction(eKeyAction::CENTER_ON_STRUCTURE)) {
        cAbstractStructure *selectedStructure = humanPlayer->getSelectedStructure();
        if (selectedStructure) {
            m_mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    if (event.isAction(eKeyAction::OPEN_MENU)) {
        m_interface->setNextStateToTransitionTo(GAME_OPTIONS);
    }

    if (event.isAction(eKeyAction::TOGGLE_FPS)) {
        m_settings->setDrawFps(true);
    }
}


void cGamePlaying::onKeyPressedGamePlaying(const cKeyboardEvent &event)
{
    cPlayer* humanPlayer = m_objects->getPlayer(HUMAN);

    if (event.isAction(eKeyAction::TRACK_UNIT)) {
        if (!m_trackedUnitIds.empty()) {
            m_trackedUnitIds.clear();
        } else {
            auto selected = m_objects->getPlayer(HUMAN)->getSelectedUnits();
            if (!selected.empty()) {
                m_trackedUnitIds = selected;
            }
        }
    }

    int iGroup = event.getGroupNumber();
    if (iGroup > 0) {
        Uint32 now = SDL_GetTicks();
        bool doublePress = (iGroup == m_lastGroupKeyPressed &&
                            now - m_lastGroupKeyPressedTick < 400u);
        m_lastGroupKeyPressed = iGroup;
        m_lastGroupKeyPressedTick = now;

        if (doublePress) {
            auto unitIds = humanPlayer->getAllMyUnitsForGroupNr(iGroup);
            if (!unitIds.empty()) {
                cExpandingRectangle bounds;
                for (int id : unitIds) {
                    cUnit* u = m_objects->getUnit(id);
                    bounds.expand(u->getPosX(), u->getPosY());
                }
                bounds.expandBy(3 * TILESIZE_WIDTH_PIXELS);
                m_mapCamera->zoomOutToFit(bounds);
                centerCameraOnUnits(unitIds);
            }
        }
    }

    if (event.isAction(eKeyAction::TOGGLE_FPS)) {
        m_settings->setDrawFps(false);
    }

    if (event.isAction(eKeyAction::TOGGLE_TIME_DISPLAY)) {
        m_settings->setDrawTime(! m_settings->shouldDrawTime());
    }

    if (event.isAction(eKeyAction::DEPLOY_UNIT)) {
        for (int i = 0; i < m_objects->getUnitsSize(); i++) {
            cUnit *u = m_objects->getUnit(i);
            if (u->isSelected() && u->iType == MCV && u->getPlayer()->isHuman()) {
                bool canPlace = u->getPlayer()->canPlaceStructureAt(u->getCell(), CONSTYARD, u->iID).success;
                if (canPlace) {
                    int iLocation = u->getCell();
                    u->die(false, false);
                    humanPlayer->placeStructure(iLocation, CONSTYARD, 100);
                }
            }
        }
    }

    if (event.isAction(eKeyAction::CENTER_ON_HOME)) {
        m_mapCamera->centerAndJumpViewPortToCell(humanPlayer->getFocusCell());
    }

    if (event.isAction(eKeyAction::CENTER_ON_STRUCTURE)) {
        cAbstractStructure *selectedStructure = humanPlayer->getSelectedStructure();
        if (selectedStructure) {
            m_mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    cAbstractStructure *selectedStructure = humanPlayer->getSelectedStructure();
    if (selectedStructure) {
        if (event.isAction(eKeyAction::DEPLOY_UNIT)) {
            if (selectedStructure->getType() == REPAIR) {
                s_GameEvent deployEvent {
                    .eventType = eGameEventType::GAME_EVENT_DEPLOY_UNIT,
                    .data = CommonEvent {
                        .entityType = eBuildType::UNKNOWN,
                        .player = humanPlayer
                    }
                };
                selectedStructure->onNotifyGameEvent(deployEvent);
            }
        }
        if (event.isAction(eKeyAction::SET_PRIMARY)) {
            humanPlayer->setPrimaryBuildingForStructureType(selectedStructure->getType(), selectedStructure->getStructureId());
        }
    }
}

void cGamePlaying::tryDebugSwitchToPlayer(int playerIndex)
{
    cPlayer *targetPlayer = m_objects->getPlayer(playerIndex);
    if (targetPlayer->getHouse() == GENERALHOUSE || !targetPlayer->isAlive()) {
        m_controlledPlayer->addNotification(
            std::format("Player {} is not active.", playerIndex), eNotificationType::NEUTRAL);
        return;
    }
    m_controlledPlayer = targetPlayer;
    m_drawManager->setPlayerToDraw(m_controlledPlayer);
    m_interface->setPlayerToInteractFor(m_controlledPlayer);
}

void cGamePlaying::onKeyDownDebugMode(const cKeyboardEvent &event)
{
    const cPlayer *humanPlayer = m_objects->getPlayer(HUMAN);

    if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_0)) {
        tryDebugSwitchToPlayer(0);
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_1)) {
        tryDebugSwitchToPlayer(1);
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_2)) {
        tryDebugSwitchToPlayer(2);
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_3)) {
        tryDebugSwitchToPlayer(3);
    }

    if (event.isAction(eKeyAction::DEBUG_WIN)) {
        m_interface->setMissionWon();
    }

    if (event.isAction(eKeyAction::DEBUG_LOSE)) {
        m_interface->setMissionLost();
    }

    if (event.isAction(eKeyAction::DEBUG_GIVE_CREDITS)) {
        for (int i = 0; i < AI_WORM; i++) {
            m_objects->getPlayer(i)->setCredits(5000);
        }
    }

    if (event.isAction(eKeyAction::DEBUG_DESTROY_AT_CURSOR)) {
        int mc = m_controlledPlayer->getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = m_objects->getMap().getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                m_objects->getUnit(idOfUnitAtCell)->die(true, false);
            }

            int idOfStructureAtCell = m_objects->getMap().getCellIdStructuresLayer(mc);
            if (idOfStructureAtCell > -1) {
                m_objects->getStructures()[idOfStructureAtCell]->die();
            }

            idOfUnitAtCell = m_objects->getMap().getCellIdWormsLayer(mc);
            if (idOfUnitAtCell > -1) {
                m_objects->getUnit(idOfUnitAtCell)->die(false, false);
            }
        }
    }

    if (event.isAction(eKeyAction::DEBUG_DAMAGE_AT_CURSOR)) {
        int mc = humanPlayer->getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = m_objects->getMap().getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                cUnit *pUnit = m_objects->getUnit(idOfUnitAtCell);
                int damageToTake = pUnit->getHitPoints() - 25;
                if (damageToTake > 0) {
                    pUnit->takeDamage(damageToTake, -1, -1);
                }
            }
        }
    } else if (event.isAction(eKeyAction::DEBUG_REVEAL_MAP)) {
        m_objects->getMap().clear_all(m_controlledPlayer->getId());
    }

    if (event.isAction(eKeyAction::DEBUG_KILL_CARRYALLS)) {
        const std::vector<int> &myUnitsForType = humanPlayer->getAllMyUnitsForType(CARRYALL);
        for (auto &unitId : myUnitsForType) {
            cUnit *pUnit = m_objects->getUnit(unitId);
            pUnit->die(true, false);
        }
    }

    if (event.isAction(eKeyAction::DEBUG_SPAWN_ORNITHOPTER)) {
        int mc = m_controlledPlayer->getGameControlsContext()->getMouseCell();
        cUnits::unitCreate(mc, ORNITHOPTER, m_controlledPlayer->getId(), false, false);
    }

    if (event.isAction(eKeyAction::DEBUG_SPAWN_WORM)) {
        int mc = humanPlayer->getGameControlsContext()->getMouseCell();
        cUnits::unitCreate(mc, SANDWORM, AI_WORM, false, false);
    }
}

void cGamePlaying::update()
{
    for (auto* pPlayer : m_objects->getPlayers()) {
        pPlayer->update();
    }
}
