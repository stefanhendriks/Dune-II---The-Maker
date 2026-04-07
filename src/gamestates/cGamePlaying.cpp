#include "gamestates/cGamePlaying.h"
#include "include/d2tmc.h"
#include "game/cGame.h"
#include "include/definitions.h"
#include "building/cItemBuilder.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "config.h"
#include "data/gfxinter.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "gameobjects/units/cReinforcements.h"
#include <SDL2/SDL.h>
#include <format>
#include "managers/cDrawManager.h"
#include "player/cPlayer.h"
#include "player/cPlayers.h"
#include "controls/cGameControlsContext.h"
#include "controls/eKeyAction.h"
#include "map/cMapCamera.h"

#include <cassert>

cGamePlaying::cGamePlaying(cGame &theGame, GameContext* ctx) : cGameState(theGame, ctx)
{
    assert(ctx != nullptr);
    m_reinforcements = game.getReinforcements();
    m_TIMER_evaluatePlayerStatus = 5;
    game.m_pathsCreated = 0;
}

cGamePlaying::~cGamePlaying()
{

}

void cGamePlaying::thinkFast()
{
    game.m_drawManager->thinkFast_statePlaying();

    game.m_mapCamera->thinkFast();

    for (cPlayer &pPlayer : game.m_Players) {
        pPlayer.thinkFast();
    }

    // structures think
    for (cAbstractStructure *pStructure : game.m_pStructures) {
        if (pStructure == nullptr) continue;
        if (pStructure->isValid()) {
            pStructure->thinkFast();       // think about actions going on
            pStructure->think_animation(); // think about animating
            pStructure->think_guard();     // think about 'guarding' the area (turrets only)
        }

        if (pStructure->isDead()) {
            cStructureFactory::getInstance()->deleteStructureInstance(pStructure);
        }
    }

    for (cPlayer &pPlayer : game.m_Players) {
        cItemBuilder *itemBuilder = pPlayer.getItemBuilder();
        if (itemBuilder) {
            itemBuilder->thinkFast();
        }
    }

    game.m_map.thinkFast();

    game.reduceShaking();

    // units think (move only)
    for (cUnit &cUnit : game.m_Units) {
        if (!cUnit.isValid()) continue;
        cUnit.thinkFast();
    }

    for (cParticle &pParticle : game.m_particles) {
        if (!pParticle.isValid()) continue;
        pParticle.thinkFast();
    }

    // when not drawing the options, the game does all it needs to do
    // bullets think
    for (cBullet &cBullet : game.g_Bullets) {
        if (!cBullet.bAlive) continue;
        cBullet.thinkFast();
    }
}

void cGamePlaying::thinkNormal()
{
        // units think
        for (int i = 0; i < game.m_Units.size(); i++) {
            cUnit &cUnit = game.getUnit(i);
            if (cUnit.isValid()) {
                cUnit.think();
            }
        }

        game.m_drawManager->think();

        for (int i = 0; i < MAX_PLAYERS; i++) {
            game.getPlayer(i).think();
        }
}

void cGamePlaying::thinkSlow()
{
    evaluatePlayerStatus(); // so we can call non-const from a const :S

    game.m_pathsCreated = 0;

    //@mira
    if (!game.m_disableReinforcements) {
        m_reinforcements->thinkSlow();
    }

    // starports think per second for deployment (if any)
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = game.m_pStructures[i];
        if (pStructure && pStructure->isValid()) {
            pStructure->thinkSlow();
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer &player = game.getPlayer(i);
        player.thinkSlow();
    }
}


void cGamePlaying::draw() const
{
    game.m_drawManager->drawCombatState();
    if (m_game.m_drawFps) {
        game.drawTextFps();
    }

    drawCombatMouse();

    if (m_game.m_drawTime) {
        game.drawTextTime();
    }
    // MOUSE
    game.m_drawManager->drawCombatMouse();
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

    game.m_drawManager->onNotifyKeyboardEvent(event);

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
            cPlayer &player = game.getPlayer(i);
            bool isAlive = player.isAlive();
            // evaluate all players regardless if they are alive or not (who knows, they became alive?)
            player.evaluateStillAlive();

            if (isAlive && !player.isAlive()) {
                s_GameEvent event {
                    .eventType = eGameEventType::GAME_EVENT_PLAYER_DEFEATED,
                    .entityType = eBuildType::SPECIAL,
                    .entityID = -1,
                    .player = &player
                };

                game.onNotifyGameEvent(event);
            }
            // TODO: event : Player joined/became alive, etc?
        }
        m_TIMER_evaluatePlayerStatus = 2;
    }

    game.checkMissionWinOrFail();
}

void cGamePlaying::drawCombatMouse() const
{
    auto m_mouse = game.getMouse();
    if (m_mouse->isBoxSelecting()) {
        m_renderDrawer->renderRectColor(m_mouse->getBoxSelectRectangle(),255,255,255,255);
    }

    if (m_mouse->isMapScrolling()) {
        cPoint startPoint = m_mouse->getDragLineStartPoint();
        cPoint endPoint = m_mouse->getDragLineEndPoint();
        m_renderDrawer->renderLine( startPoint.x, startPoint.y, endPoint.x, endPoint.y, Color{255,255,255,255});
    }
    m_mouse->draw();
}

void cGamePlaying::missionInit()
{
    game.m_pathsCreated = 0;
    m_TIMER_evaluatePlayerStatus = 5;
}


void cGamePlaying::onKeyDownGamePlaying(const cKeyboardEvent &event)
{
    const cPlayer &humanPlayer = game.getPlayer(HUMAN);

    if (event.isAction(eKeyAction::ATTACK_MODE)) {
        int iGroup = event.getGroupNumber();
        if (iGroup > 0) {
            humanPlayer.markUnitsForGroup(iGroup);
        }
    }

    if (game.isDebugMode()) {
        if (event.hasKey(SDL_SCANCODE_TAB)) {
            onKeyDownDebugMode(event);
        }

        if (event.isAction(eKeyAction::DEBUG_CLEAR_SHROUD_AT_CURSOR)) {
            int mouseCell = humanPlayer.getGameControlsContext()->getMouseCell();
            if (mouseCell > -1) {
                game.m_map.clearShroud(mouseCell, 6, HUMAN);
            }
        }
    }

    if (event.isAction(eKeyAction::ZOOM_RESET)) {
        game.m_mapCamera->resetZoom();
    }

    if (event.isAction(eKeyAction::CENTER_ON_HOME)) {
        game.m_mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
    }

    if (event.isAction(eKeyAction::CENTER_ON_STRUCTURE)) {
        cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
        if (selectedStructure) {
            game.m_mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    if (event.isAction(eKeyAction::OPEN_MENU)) {
        game.setNextStateToTransitionTo(GAME_OPTIONS);
    }

    if (event.isAction(eKeyAction::TOGGLE_FPS)) {
        game.m_drawFps = true;
    }
}


void cGamePlaying::onKeyPressedGamePlaying(const cKeyboardEvent &event)
{
    cPlayer &humanPlayer = game.getPlayer(HUMAN);

    if (event.isAction(eKeyAction::TOGGLE_FPS)) {
        m_game.m_drawFps = false;
    }

    if (event.isAction(eKeyAction::TOGGLE_TIME_DISPLAY)) {
        m_game.m_drawTime = !m_game.m_drawTime;
    }

    if (event.isAction(eKeyAction::DEPLOY_UNIT)) {
        for (int i = 0; i < game.m_Units.size(); i++) {
            cUnit &u = game.getUnit(i);
            if (u.isSelected() && u.iType == MCV && u.getPlayer()->isHuman()) {
                bool canPlace = u.getPlayer()->canPlaceStructureAt(u.getCell(), CONSTYARD, u.iID).success;
                if (canPlace) {
                    int iLocation = u.getCell();
                    u.die(false, false);
                    humanPlayer.placeStructure(iLocation, CONSTYARD, 100);
                }
            }
        }
    }

    if (event.isAction(eKeyAction::CENTER_ON_HOME)) {
        game.m_mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
    }

    if (event.isAction(eKeyAction::CENTER_ON_STRUCTURE)) {
        cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
        if (selectedStructure) {
            game.m_mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
    if (selectedStructure) {
        if (event.isAction(eKeyAction::DEPLOY_UNIT)) {
            if (selectedStructure->getType() == REPAIR) {
                s_GameEvent deployEvent {
                    .eventType = eGameEventType::GAME_EVENT_DEPLOY_UNIT,
                    .entityType = eBuildType::UNKNOWN,
                    .entityID = -1,
                    .player = &humanPlayer
                };
                selectedStructure->onNotifyGameEvent(deployEvent);
            }
        }
        if (event.isAction(eKeyAction::SET_PRIMARY)) {
            humanPlayer.setPrimaryBuildingForStructureType(selectedStructure->getType(), selectedStructure->getStructureId());
        }
    }
}

void cGamePlaying::onKeyDownDebugMode(const cKeyboardEvent &event)
{
    const cPlayer &humanPlayer = game.getPlayer(HUMAN);

    if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_0)) {
        game.m_drawManager->setPlayerToDraw(&game.getPlayer(0));
        game.setPlayerToInteractFor(&game.getPlayer(0));
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_1)) {
        game.m_drawManager->setPlayerToDraw(&game.getPlayer(1));
        game.setPlayerToInteractFor(&game.getPlayer(1));
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_2)) {
        game.m_drawManager->setPlayerToDraw(&game.getPlayer(2));
        game.setPlayerToInteractFor(&game.getPlayer(2));
    }
    else if (event.isAction(eKeyAction::DEBUG_SWITCH_PLAYER_3)) {
        game.m_drawManager->setPlayerToDraw(&game.getPlayer(3));
        game.setPlayerToInteractFor(&game.getPlayer(3));
    }

    if (event.isAction(eKeyAction::DEBUG_WIN)) {
        game.setMissionWon();
    }

    if (event.isAction(eKeyAction::DEBUG_LOSE)) {
        game.setMissionLost();
    }

    if (event.isAction(eKeyAction::DEBUG_GIVE_CREDITS)) {
        for (int i = 0; i < AI_WORM; i++) {
            game.getPlayer(i).setCredits(5000);
        }
    }

    if (event.isAction(eKeyAction::DEBUG_DESTROY_AT_CURSOR)) {
        int mc = humanPlayer.getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = game.m_map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                game.getUnit(idOfUnitAtCell).die(true, false);
            }

            int idOfStructureAtCell = game.m_map.getCellIdStructuresLayer(mc);
            if (idOfStructureAtCell > -1) {
                game.m_pStructures[idOfStructureAtCell]->die();
            }

            idOfUnitAtCell = game.m_map.getCellIdWormsLayer(mc);
            if (idOfUnitAtCell > -1) {
                game.getUnit(idOfUnitAtCell).die(false, false);
            }
        }
    }

    if (event.isAction(eKeyAction::DEBUG_DAMAGE_AT_CURSOR)) {
        int mc = humanPlayer.getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = game.m_map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                cUnit &pUnit = game.getUnit(idOfUnitAtCell);
                int damageToTake = pUnit.getHitPoints() - 25;
                if (damageToTake > 0) {
                    pUnit.takeDamage(damageToTake, -1, -1);
                }
            }
        }
    } else if (event.isAction(eKeyAction::DEBUG_REVEAL_MAP)) {
        game.m_map.clear_all(HUMAN);
    }

    if (event.isAction(eKeyAction::DEBUG_KILL_CARRYALLS)) {
        const std::vector<int> &myUnitsForType = humanPlayer.getAllMyUnitsForType(CARRYALL);
        for (auto &unitId : myUnitsForType) {
            cUnit &pUnit = game.getUnit(unitId);
            pUnit.die(true, false);
        }
    }
}

void cGamePlaying::update()
{
    for (auto &pPlayer : game.m_Players) {
        pPlayer.update();
    }
}
