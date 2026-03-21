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


cGamePlaying::cGamePlaying(cGame &theGame, GameContext* ctx) : cGameState(theGame, ctx)
{
    m_reinforcements = game.getReinforcements();
    m_TIMER_evaluatePlayerStatus = 5;
    game.m_pathsCreated = 0;
}

cGamePlaying::~cGamePlaying()
{

}

void cGamePlaying::thinkFast()
{
    global_drawManager->thinkFast_statePlaying();

    global_mapCamera->thinkFast();

    for (cPlayer &pPlayer : game.getPlayers()) {
        pPlayer.thinkFast();
    }

    // structures think
    for (cAbstractStructure *pStructure : g_pStructure) {
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

    for (cPlayer &pPlayer : game.getPlayers()) {
        cItemBuilder *itemBuilder = pPlayer.getItemBuilder();
        if (itemBuilder) {
            itemBuilder->thinkFast();
        }
    }

    global_map.thinkFast();

    game.reduceShaking();

    // units think (move only)
    for (cUnit &cUnit : g_Unit) {
        if (!cUnit.isValid()) continue;
        cUnit.thinkFast();
    }

    for (cParticle &pParticle : g_Particles) {
        if (!pParticle.isValid()) continue;
        pParticle.thinkFast();
    }

    // when not drawing the options, the game does all it needs to do
    // bullets think
    for (cBullet &cBullet : g_Bullets) {
        if (!cBullet.bAlive) continue;
        cBullet.thinkFast();
    }
}

void cGamePlaying::thinkNormal()
{
        // units think
        for (int i = 0; i < MAX_UNITS; i++) {
            cUnit &cUnit = g_Unit[i];
            if (cUnit.isValid()) {
                cUnit.think();
            }
        }

        global_drawManager->think();

        for (int i = 0; i < MAX_PLAYERS; i++) {
            game.getPlayers()[i].think();
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
        cAbstractStructure *pStructure = g_pStructure[i];
        if (pStructure && pStructure->isValid()) {
            pStructure->thinkSlow();
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer &player = game.getPlayers()[i];
        player.thinkSlow();
    }
}


void cGamePlaying::draw() const
{
    global_drawManager->drawCombatState();
    if (m_game.m_drawFps) {
        game.drawTextFps();
    }

    drawCombatMouse();

    if (m_game.m_drawTime) {
        game.drawTextTime();
    }
    // MOUSE
    global_drawManager->drawCombatMouse();
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

    global_drawManager->onNotifyKeyboardEvent(event);

    switch (event.eventType) {
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
            cPlayer &player = game.getPlayers()[i];
            bool isAlive = player.isAlive();
            // evaluate all players regardless if they are alive or not (who knows, they became alive?)
            player.evaluateStillAlive();

            if (isAlive && !player.isAlive()) {
                s_GameEvent event{
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
    const cPlayer &humanPlayer = game.getPlayers()[HUMAN];

    bool createGroup = event.hasKey(SDL_SCANCODE_LCTRL) || event.hasKey(SDL_SCANCODE_RCTRL);
    if (createGroup) {
        int iGroup = event.getGroupNumber();

        if (iGroup > 0) {
            humanPlayer.markUnitsForGroup(iGroup);
        }
    }

    if (game.isDebugMode()) { // debug mode has additional keys
        if (event.hasKey(SDL_SCANCODE_TAB)) {
            onKeyDownDebugMode(event);
        }

        if (event.hasKey(SDL_SCANCODE_F4)) {
            int mouseCell = humanPlayer.getGameControlsContext()->getMouseCell();
            if (mouseCell > -1) {
                global_map.clearShroud(mouseCell, 6, HUMAN);
            }
        }
    }

    if (event.hasKey(SDL_SCANCODE_Z)) {
        global_mapCamera->resetZoom();
    }

    if (event.hasKey(SDL_SCANCODE_H)) {
        global_mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
    }

    // Center on the selected structure
    if (event.hasKey(SDL_SCANCODE_C)) {
        cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
        if (selectedStructure) {
            global_mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
        game.setNextStateToTransitionTo(GAME_OPTIONS);
    }

    if (event.hasKey(SDL_SCANCODE_F)) {
        game.m_drawFps = true;
    }
}

void cGamePlaying::onKeyPressedGamePlaying(const cKeyboardEvent &event)
{
    cPlayer &humanPlayer = game.getPlayers()[HUMAN];

    if (event.hasKey(SDL_SCANCODE_F)) {
        m_game.m_drawFps = false;
    }

    if (event.hasKey(SDL_SCANCODE_BACKSLASH)) {
        m_game.m_drawTime = ! m_game.m_drawTime;
    }

    if (event.hasKey(SDL_SCANCODE_D)) {
        for (int i = 0; i < MAX_UNITS; i++) {
            cUnit &u = g_Unit[i];
            if (u.bSelected && u.iType == MCV && u.getPlayer()->isHuman()) {
                bool canPlace = u.getPlayer()->canPlaceStructureAt(u.getCell(), CONSTYARD, u.iID).success;
                if (canPlace) {
                    int iLocation = u.getCell();
                    u.die(false, false);
                    humanPlayer.placeStructure(iLocation, CONSTYARD, 100);
                }
            }
        }
    }

    if (event.hasKey(SDL_SCANCODE_H)) {
        global_mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
    }

    // Center on the selected structure
    if (event.hasKey(SDL_SCANCODE_C)) {
        cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
        if (selectedStructure) {
            global_mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
    if (selectedStructure) {
        // depending on type of structure, a key could mean a different thing?
        // so, kind of like event.hasKey(selectedStructure->KeyForDeploying()) ?
        // and then perform?
        if (event.hasKey(SDL_SCANCODE_D)) {
            if (selectedStructure->getType() == REPAIR) { // this should be done differently?
                s_GameEvent event {
                    .eventType = eGameEventType::GAME_EVENT_DEPLOY_UNIT,
                    .entityType = eBuildType::UNKNOWN,
                    .entityID = -1,
                    .player = &humanPlayer
                };
                selectedStructure->onNotifyGameEvent(event);
            }
        }
        // other keys for other structures?
        // like: repair/stop repairing?
    }
}

void cGamePlaying::onKeyDownDebugMode(const cKeyboardEvent &event)
{
    const cPlayer &humanPlayer = game.getPlayers()[HUMAN];

    if (event.hasKey(SDL_SCANCODE_0)) {
        global_drawManager->setPlayerToDraw(&game.getPlayers()[0]);
        game.setPlayerToInteractFor(&game.getPlayers()[0]);
    }
    else if (event.hasKey(SDL_SCANCODE_1)) {
        global_drawManager->setPlayerToDraw(&game.getPlayers()[1]);
        game.setPlayerToInteractFor(&game.getPlayers()[1]);
    }
    else if (event.hasKey(SDL_SCANCODE_2)) {
        global_drawManager->setPlayerToDraw(&game.getPlayers()[2]);
        game.setPlayerToInteractFor(&game.getPlayers()[2]);
    }
    else if (event.hasKey(SDL_SCANCODE_3)) {
        global_drawManager->setPlayerToDraw(&game.getPlayers()[3]);
        game.setPlayerToInteractFor(&game.getPlayers()[3]);
    }

    // WIN MISSION
    if (event.hasKey(SDL_SCANCODE_F2)) {
        game.setMissionWon();
    }

    // LOSE MISSION
    if (event.hasKey(SDL_SCANCODE_F3)) {
        game.setMissionLost();
    }

    // GIVE CREDITS TO ALL PLAYERS
    if (event.hasKey(SDL_SCANCODE_F4)) {
        for (int i = 0; i < AI_WORM; i++) {
            game.getPlayers()[i].setCredits(5000);
        }
    }

    //DESTROY UNIT OR BUILDING
    if (event.hasKeys(SDL_SCANCODE_F4, SDL_SCANCODE_LSHIFT)) {
        int mc = humanPlayer.getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = global_map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                g_Unit[idOfUnitAtCell].die(true, false);
            }

            int idOfStructureAtCell = global_map.getCellIdStructuresLayer(mc);
            if (idOfStructureAtCell > -1) {
                g_pStructure[idOfStructureAtCell]->die();
            }

            idOfUnitAtCell = global_map.getCellIdWormsLayer(mc);
            if (idOfUnitAtCell > -1) {
                g_Unit[idOfUnitAtCell].die(false, false);
            }
        }
    }

    //DESTROY UNIT OR BUILDING
    if (event.hasKeys(SDL_SCANCODE_F5, SDL_SCANCODE_LSHIFT)) {
        int mc = humanPlayer.getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = global_map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                cUnit &pUnit = g_Unit[idOfUnitAtCell];
                int damageToTake = pUnit.getHitPoints() - 25;
                if (damageToTake > 0) {
                    pUnit.takeDamage(damageToTake, -1, -1);
                }
            }
        }
    }
    else {
        // REVEAL MAP
        if (event.hasKey(SDL_SCANCODE_F5)) {
            global_map.clear_all(HUMAN);
        }
    }

    if (event.hasKey(SDL_SCANCODE_F6)) {
        // kill all carry-all's
        const std::vector<int> &myUnitsForType = humanPlayer.getAllMyUnitsForType(CARRYALL);
        for (auto &unitId : myUnitsForType) {
            cUnit &pUnit = g_Unit[unitId];
            pUnit.die(true, false);
        }
    }
}

void cGamePlaying::update()
{
    for (auto &pPlayer : game.getPlayers()) {
        pPlayer.update();
    }
}
