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

#include <cassert>

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
}

cGamePlaying::~cGamePlaying()
{
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


void cGamePlaying::draw() const
{
    m_drawManager->drawCombatState();
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

void cGamePlaying::onNotifyMouseEvent(const s_MouseEvent& )
{
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
        m_renderDrawer->renderRectColor(m_mouse->getBoxSelectRectangle(), Color::Blue);
    }

    if (m_mouse->isMapScrolling()) {
        cPoint startPoint = m_mouse->getDragLineStartPoint();
        cPoint endPoint = m_mouse->getDragLineEndPoint();
        m_renderDrawer->renderLine( startPoint.x, startPoint.y, endPoint.x, endPoint.y, Color{255,255,255,255});
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

void cGamePlaying::onKeyDownDebugMode(const cKeyboardEvent &event)
{
    const cPlayer *humanPlayer = m_objects->getPlayer(HUMAN);

    if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_0)) {
        m_controlledPlayer = m_objects->getPlayer(0);
        m_drawManager->setPlayerToDraw(m_controlledPlayer);
        m_interface->setPlayerToInteractFor(m_controlledPlayer);
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_1)) {
        m_controlledPlayer = m_objects->getPlayer(1);
        m_drawManager->setPlayerToDraw(m_controlledPlayer);
        m_interface->setPlayerToInteractFor(m_controlledPlayer);
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_2)) {
        m_controlledPlayer = m_objects->getPlayer(2);
        m_drawManager->setPlayerToDraw(m_controlledPlayer);
        m_interface->setPlayerToInteractFor(m_controlledPlayer);
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_3)) {
        m_controlledPlayer = m_objects->getPlayer(3);
        m_drawManager->setPlayerToDraw(m_controlledPlayer);
        m_interface->setPlayerToInteractFor(m_controlledPlayer);
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
        int mc = humanPlayer->getGameControlsContext()->getMouseCell();
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
        m_objects->getMap().clear_all(HUMAN);
    }

    if (event.isAction(eKeyAction::DEBUG_KILL_CARRYALLS)) {
        const std::vector<int> &myUnitsForType = humanPlayer->getAllMyUnitsForType(CARRYALL);
        for (auto &unitId : myUnitsForType) {
            cUnit *pUnit = m_objects->getUnit(unitId);
            pUnit->die(true, false);
        }
    }

    if (event.isAction(eKeyAction::DEBUG_SPAWN_ORNITHOPTER)) {
        int mc = humanPlayer->getGameControlsContext()->getMouseCell();
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
