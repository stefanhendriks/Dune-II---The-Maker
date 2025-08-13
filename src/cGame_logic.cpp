/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

*/
#include "cGame.h"
#include "include/Texture.hpp"
#include "building/cItemBuilder.h"
#include "d2tmc.h"
#include "config.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "gameobjects/units/cReinforcements.h"
#include "gamestates/cChooseHouseGameState.h"
#include "gamestates/cCreditsState.h"
#include "gamestates/cMainMenuGameState.h"
#include "gamestates/cSelectMissionState.h"
#include "gamestates/cOptionsState.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "gamestates/cSetupSkirmishGameState.h"
#include "ini.h"
#include "managers/cDrawManager.h"
#include "managers/cInteractionManager.h"
#include "mentat/AtreidesMentat.h"
#include "mentat/BeneMentat.h"
#include "mentat/HarkonnenMentat.h"
#include "mentat/OrdosMentat.h"
#include "player/cPlayer.h"
#include "player/brains/cPlayerBrainCampaign.h"
#include "player/brains/cPlayerBrainSandworm.h"
#include "player/brains/cPlayerBrainSkirmish.h"
#include "player/brains/superweapon/cPlayerBrainFremenSuperWeapon.h"
#include "sidebar/cBuildingListFactory.h"
#include "sidebar/cSideBarFactory.h"
#include "utils/RNG.hpp"
#include "utils/cLog.h"
#include "utils/cPlatformLayerInit.h"
#include "utils/cSoundPlayer.h"
#include "utils/cScreenInit.h"
#include "utils/d2tm_math.h"
#include "map/cPreviewMaps.h"
#include "map/MapGeometry.hpp"
#include "utils/Color.hpp"

#include "utils/cFileValidator.h"
#include "utils/cIniFile.h"
#include "player/cHousesInfo.h"
#include "utils/Graphics.hpp"
#include "utils/GameSettings.hpp"
#include <format>
#include "context/GameContext.hpp"
#include "context/ContextCreator.hpp"

#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

namespace {

constexpr auto kMinAlpha = 0;
constexpr auto kMaxAlpha = 255;

}

cGame::cGame()
{
    memset(m_states, 0, sizeof(cGameState *));

    m_drawFps = false;
    m_nextState = -1;
    m_currentState = nullptr;
    m_screenW = -1;
    m_screenH = -1;
    m_windowed = false;
    m_allowRepeatingReinforcements = false;
    m_playSound = true;
    m_playMusic = true;
    context = nullptr;
    ctx = nullptr;
    m_mentat = nullptr;

    // create GameContext
    ctx = std::make_unique<GameContext>();
    // create TimeManager
    std::unique_ptr<cTimeManager> timeManager = std::make_unique<cTimeManager>(this);
    //local usage
    m_timeManager = timeManager.get();
    //send to GameContext
    ctx->setTimeManager(std::move(timeManager));
}

void cGame::applySettings(GameSettings *gs)
{
    m_screenW = gs->screenW;
    m_screenH = gs->screenH;
    m_cameraDragMoveSpeed = gs->cameraDragMoveSpeed;
    m_cameraBorderOrKeyMoveSpeed = gs->cameraBorderOrKeyMoveSpeed;
    m_cameraEdgeMove = gs->cameraEdgeMove;
    m_windowed = gs->windowed;
    m_allowRepeatingReinforcements = gs->allowRepeatingReinforcements;
    m_turretsDownOnLowPower = gs->turretsDownOnLowPower;
    m_rocketTurretsDownOnLowPower = gs->rocketTurretsDownOnLowPower;
    m_playMusic = gs->playMusic;
    m_playSound = gs->playSound;
    m_debugMode = gs->debugMode;
    m_drawUnitDebug = gs->drawUnitDebug;
    m_disableAI = gs->disableAI;
    m_oneAi = gs->oneAi;
    m_disableWormAi = gs->disableWormAi;
    m_disableReinforcements = gs->disableReinforcements;
    m_noAiRest = gs->noAiRest;
    m_drawUsages = gs->drawUsages;
    m_gameFilename = gs->gameFilename;
}

void cGame::init()
{
    m_newMusicSample = MUSIC_MENU;
    m_newMusicCountdown = 0;

    m_drawFps = false;
    m_nextState = -1;
    m_missionWasWon = false;
    m_currentState = nullptr;
    m_screenshot = 0;
    m_playing = true;

    m_TIMER_evaluatePlayerStatus = 5;

    m_skirmish = false;
    m_PreviewMaps = std::make_shared<cPreviewMaps>(m_debugMode);

    // Alpha (for fading in/out)
    m_fadeAlpha = kMinAlpha;             // 255 = opaque , anything else
    m_fadeAction = eFadeAction::FADE_IN; // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

    m_musicVolume = 96; // volume is 0...

    m_pathsCreated = 0;

    setState(GAME_INITIALIZE);

    // mentat
    delete m_mentat;
    m_mentat = nullptr;

    m_fadeSelect = 1.0f;

    m_fadeSelectDir = true;    // fade select direction

    m_region = 1;          // what region ? (calumative, from player perspective, NOT the actual region number)
    m_mission = 0;         // calculated by mission loading (region -> mission calculation)

    m_shakeX = 0;
    m_shakeY = 0;
    m_TIMER_shake = 0;

    m_musicType = -1;

    m_cameraDragMoveSpeed=0.5f;
    m_cameraBorderOrKeyMoveSpeed=0.5;
    m_cameraEdgeMove = true;

    global_map.init(64, 64);

    initPlayers(false);

    for (int i = 0; i < MAX_UNITS; i++) {
        unit[i].init(i);
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle[i].init();
    }

    // Units & Structures are already initialized in map.init()
    // Load properties
    INI_Install_Game(m_gameFilename);
}

// TODO: Bad smell (duplicate code)
// initialize for missions
void cGame::missionInit()
{
    mapCamera->resetZoom();

    // first 15 seconds, do not evaluate player alive status
    m_TIMER_evaluatePlayerStatus = 5;

    m_winFlags = 0;
    m_loseFlags = 0;

    m_musicVolume = 96; // volume is 0...

    m_pathsCreated = 0;

    m_fadeSelect = 1.0f;

    m_fadeSelectDir = true;    // fade select direction

    m_shakeX = 0;
    m_shakeY = 0;
    m_TIMER_shake = 0;

    global_map.init(64, 64);

    initPlayers(true);

    drawManager->missionInit();
}

void cGame::initPlayers(bool rememberHouse) const
{
    int maxThinkingAIs = MAX_PLAYERS;
    if (m_oneAi) {
        maxThinkingAIs = 1;
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer &pPlayer = players[i];

        int h = pPlayer.getHouse();

        brains::cPlayerBrain *brain = nullptr;
        bool autoSlabStructures = false;

        if (i > HUMAN && i < AI_CPU5) {
            // TODO: playing attribute? (from ai player class?)
            if (!game.m_disableAI) {
                if (maxThinkingAIs > 0) {
                    if (game.m_skirmish) {
                        brain = new brains::cPlayerBrainSkirmish(&pPlayer);
                    }
                    else {
                        brain = new brains::cPlayerBrainCampaign(&pPlayer);
                        autoSlabStructures = true;  // campaign based AI's autoslab structures...
                    }
                }
            }
            maxThinkingAIs--;
        }
        else if (i == AI_CPU5) {
            brain = new brains::cPlayerBrainFremenSuperWeapon(&pPlayer);
        }
        else if (i == AI_CPU6) {
            if (!game.m_disableWormAi) {
                brain = new brains::cPlayerBrainSandworm(&pPlayer);
            }
        }

        pPlayer.init(i, brain);
        pPlayer.setAutoSlabStructures(autoSlabStructures);
        if (rememberHouse) {
            pPlayer.setHouse(h);
        }

        if (m_skirmish) {
            pPlayer.setCredits(2500);
        }
    }
}

/**
 * Thinking every second while in combat
 */
void cGame::thinkSlow_stateCombat_evaluatePlayerStatus()
{
    if (m_TIMER_evaluatePlayerStatus > 0) {
        m_TIMER_evaluatePlayerStatus--;
    }
    else {
        // TODO: Better way is with events (ie created/destroyed). However, there is no such
        // bookkeeping per player *yet*. So instead, for now, we "poll" for this data.
        for (int i = 1; i < MAX_PLAYERS; i++) {
            cPlayer &player = players[i];
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

    if (isMissionFailed()) {
        setMissionLost();
        return;
    }

    if (isMissionWon()) {
        setMissionWon();
        return;
    }
}

void cGame::setMissionWon()
{
    m_missionWasWon = true;
    setState(GAME_WINNING);

    m_shakeX = 0;
    m_shakeY = 0;
    m_TIMER_shake = 0;
    m_mouse->setTile(MOUSE_NORMAL);

    m_soundPlayer->playVoice(SOUND_VOICE_07_ATR, players[HUMAN].getHouse());

    playMusicByType(MUSIC_WIN);

    renderDrawer->beginDrawingToTexture(screenTexture);
    SDL_RenderCopy(renderer, actualRenderer->tex,nullptr, nullptr);
    renderDrawer->endDrawingToTexture();
}

void cGame::setMissionLost()
{
    m_missionWasWon = false;
    setState(GAME_LOSING);

    m_shakeX = 0;
    m_shakeY = 0;
    m_TIMER_shake = 0;
    m_mouse->setTile(MOUSE_NORMAL);

    m_soundPlayer->playVoice(SOUND_VOICE_08_ATR, players[HUMAN].getHouse());

    playMusicByType(MUSIC_LOSE);

    renderDrawer->beginDrawingToTexture(screenTexture);
    SDL_RenderCopy(renderer, actualRenderer->tex,nullptr, nullptr);
    renderDrawer->endDrawingToTexture();
}

bool cGame::isMissionFailed() const
{
    if (hasGameOverConditionHarvestForSpiceQuota()) {
        // check for non-human players if they have met spice quota, if so, they win (and thus human player loses)
        for (int i = 1; i < MAX_PLAYERS; i++) {
            cPlayer &player = players[i];
            if (player.isAlive() && player.hasMetQuota()) {
                return true;
            }
        }
    }

    if (hasGameOverConditionPlayerHasNoBuildings()) {
        cPlayer &humanPlayer = players[HUMAN];
        bool hasNothing = humanPlayer.getAllMyStructuresAsId().empty() && humanPlayer.getAllMyUnits().empty();
        if (hasNothing) {
            /**
             * If any of the bits in “LoseFlags” is set and the corresponding condition holds true
             * the player has won (and the computer has lost)
             */
            if (hasWinConditionHumanMustLoseAllBuildings()) {
                // this means, if any other player has lost all, that player wins, this is not (yet)
                // supported. Mainly because we can't distinguish yet between 'active' and non-active players
                // since we have a fixed list of players.

                // so for now just do this:
                return false; // it is meant to win the game by drawStateLosing all...
            }
            else {
                return true; // nope, it should lose mission now
            }
        }
    }

    return false;
}

bool cGame::isMissionWon() const
{
    cPlayer &humanPlayer = players[HUMAN];
    if (hasGameOverConditionHarvestForSpiceQuota()) {
        if (humanPlayer.hasMetQuota()) {
            return true;
        }
    }

    if (hasGameOverConditionPlayerHasNoBuildings()) {
        if (hasWinConditionHumanMustLoseAllBuildings()) {
            bool hasNothing = humanPlayer.getAllMyStructuresAsId().empty() && humanPlayer.getAllMyUnits().empty();
            if (hasNothing) {
                return true;
            }
        }

        if (hasWinConditionAIShouldLoseEverything()) {
            if (allEnemyAIPlayersAreDestroyed()) {
                return true;
            }
        }
    }
    else if (hasGameOverConditionAIHasNoBuildings()) {
        if (hasWinConditionAIShouldLoseEverything()) {
            if (allEnemyAIPlayersAreDestroyed()) {
                return true;
            }
        }
    }
    return false;
}

bool cGame::allEnemyAIPlayersAreDestroyed() const
{
    cPlayer &humanPlayer = players[HUMAN];
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (i == HUMAN || i == AI_WORM || i == AI_CPU5) continue; // do not evaluate these players
        cPlayer *player = &players[i];
        if (!player->isAlive()) continue;
        if (humanPlayer.isSameTeamAs(player)) continue; // skip allied AI players
        return false;
    }
    return true;
}


/**
 * Remember, the 'm_winFlags' are used to determine when the game is "over". Only then the lose flags are evaluated
 * and used to determine who has won. (According to the SCEN specification).
 * @return
 */
bool cGame::hasWinConditionHumanMustLoseAllBuildings() const
{
    return (m_loseFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0;
}

bool cGame::hasWinConditionAIShouldLoseEverything() const
{
    return (m_loseFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0;
}

/**
 * Game over condition: player has no buildings (WinFlags)
 * @return
 */
bool cGame::hasGameOverConditionPlayerHasNoBuildings() const
{
    return (m_winFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0;
}

/**
 * Game over condition: Spice quota reached by player
 * @return
 */
bool cGame::hasGameOverConditionHarvestForSpiceQuota() const
{
    return (m_winFlags & WINLOSEFLAGS_QUOTA) != 0;
}

bool cGame::hasGameOverConditionAIHasNoBuildings() const
{
    return (m_winFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0;
}

void cGame::think_mentat()
{
    if (m_mentat) {
        m_mentat->think();
    }
}

// think function belongs to combat state (tbd)
void cGame::think_audio()
{
    // m_soundPlayer->think();

    if (!game.m_playMusic) // no music enabled, so no need to think
        return;

    // all this does is repeating music in the same theme.
    if (m_musicType < 0)
        return;

    if (m_newMusicCountdown > 0) {
        m_newMusicCountdown--;
    }

    if (m_newMusicCountdown == 0) {
        m_soundPlayer->playMusic(m_newMusicSample);
        m_newMusicCountdown--; // so we don't keep re-starting music
    }

    if (m_newMusicCountdown < 0) {
        if (!m_soundPlayer->isMusicPlaying()) {
            int desiredMusicType = m_musicType;
            if (m_musicType == MUSIC_ATTACK) {
                desiredMusicType = MUSIC_PEACE; // set back to peace
            }
            playMusicByType(desiredMusicType);
        }
    }
}

void cGame::updateMouseAndKeyboardState()
{
    m_mouse->updateState(); // calls observers that are interested in mouse input
    m_keyboard->updateState(); // calls observers that are interested in keyboard input
}

void cGame::updateGamePlaying()
{
    if (m_state != GAME_PLAYING) {
        return;
    }

    // Mission playing state logic
    // TODO: Move this to combat state object
    for (auto &pPlayer : players) {
        pPlayer.update();
    }
}

void cGame::drawStateCombat()
{
    drawManager->drawCombatState();
    if (m_drawFps) {
        // TEXT alfont_textprintf(bmp_screen, game_font, 0, 44, Color{255, 255, 255), "FPS/REST: %d / %d", game.getFps(), iRest);
        textDrawer->drawText(180,8, Color::black(), std::format("FPS/REST: {}/{}", m_timeManager->getFps(), m_timeManager->getWaitingTime()));
    }

    // for now, call this on game class.
    // TODO: move this "combat" state into own game state class
    drawCombatMouse();

    // MOUSE
    drawManager->drawCombatMouse();
}

// drawStateMentat logic + drawing mouth/eyes
void cGame::drawStateMentat(AbstractMentat *mentat)
{
    m_mouse->setTile(MOUSE_NORMAL);

    mentat->draw();
    // mentat->interact();

    m_mouse->draw();
}

// draw menu
void cGame::drawStateMenu()
{
    m_currentState->draw();
}

void cGame::initSkirmish() const
{
    game.missionInit();
}

void cGame::loadSkirmishMaps() const
{
    m_PreviewMaps->loadSkirmishMaps();
}

void cGame::shakeScreenAndBlitBuffer()
{
    if (m_TIMER_shake == 0) {
        m_TIMER_shake = -1;
    }

    // only in playing state we shake screen
    if (m_state == GAME_PLAYING) {
        // TODO: move the shaking part of the rendering in the playing state object at some time
        // and shake it within the 'bmp_screen', so that the actual double buffering (bmp_screen -> screen) happens
        // always at some point in the main loop, and does not need to know about the shaking logic.

        // blitSprite on screen
        if (m_TIMER_shake > 0) {
            // the more we get to the 'end' the less we 'throttle'.
            // Structure explosions are 6 time units per cell.
            // Max is 9 cells (9*6=54)
            // the max border is then 9. So, we do time / 6
            int shakiness = std::min(m_TIMER_shake, 69);
            float offset = mapCamera->factorZoomLevel(std::min(shakiness / 5, 9));

            m_shakeX = -abs(offset / 2) + RNG::rnd(offset);
            m_shakeY = -abs(offset / 2) + RNG::rnd(offset);

            // @Mira recreate shake screen
            //renderDrawer->blit(bmp_screen, bmp_throttle, 0, 0, 0 + m_shakeX, 0 + m_shakeY, m_screenW, m_screenH);
            //renderDrawer->blit(bmp_throttle, bmp_screen, 0, 0, 0, 0, m_screenW, m_screenH);
        }
        else {
            fadeOutOrBlitScreenBuffer();
        }
    }
    fadeOutOrBlitScreenBuffer();
}

void cGame::fadeOutOrBlitScreenBuffer() const
{
    if (m_fadeAction == FADE_NONE) {
        return;
    }

    // Fading
    assert(m_fadeAlpha >= kMinAlpha);
    assert(m_fadeAlpha <= kMaxAlpha);
}

void cGame::drawState()
{
    if (m_fadeAction == eFadeAction::FADE_OUT) {
        if (screenTexture) {
            renderDrawer->renderSprite(screenTexture,0,0,(Uint8)m_fadeAlpha);
        }
        return;
    }

    // this makes fade-in happen after fade-out automatically
    if (m_fadeAlpha == kMinAlpha) {
        m_fadeAction = eFadeAction::FADE_IN;
    }

    if (m_fadeAction == eFadeAction::FADE_IN && m_fadeAlpha>250) {
        m_fadeAlpha = kMaxAlpha;
        m_fadeAction = eFadeAction::FADE_NONE;
    }

    switch (m_state) {
        case GAME_TELLHOUSE:
            drawStateMentat(m_mentat);
            break;
        case GAME_PLAYING:
            drawStateCombat();
            break;
        case GAME_BRIEFING:
            drawStateMentat(m_mentat);
            break;
        case GAME_MENU:
            drawStateMenu();
            break;
        case GAME_WINNING:
            drawStateWinning();
            break;
        case GAME_LOSING:
            drawStateLosing();
            break;
        case GAME_WINBRIEF:
            drawStateMentat(m_mentat);
            break;
        case GAME_LOSEBRIEF:
            drawStateMentat(m_mentat);
            break;
        default:
            m_currentState->draw();
            // TODO: GAME_STATISTICS, ETC
    }
}

/**
	Main game loop
*/
void cGame::run()
{
    actualRenderer = renderDrawer->createRenderTargetTexture(m_screenW, m_screenH);
    screenTexture = renderDrawer->createRenderTargetTexture(m_screenW, m_screenH);
    SDL_Event event;
    while (m_playing) {
        m_timeManager->processTime();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    m_playing = false;
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    m_keyboard->handleEvent(event);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL:
                    m_mouse->handleEvent(event);
                default:
                    break;
            }
        }
        updateMouseAndKeyboardState();
        updateGamePlaying();

        renderDrawer->beginDrawingToTexture(actualRenderer);
        renderDrawer->renderClearToColor();
        drawState(); // run game state, includes interaction + drawing
        transitionStateIfRequired();
        shakeScreenAndBlitBuffer(); // finally, draw the bmp_screen to real screen (double buffering)

        renderDrawer->endDrawingToTexture();
        renderDrawer->renderSprite(actualRenderer,0,0);
        SDL_RenderPresent(renderer);
        m_timeManager->waitForCPU(); // wait for CPU to catch up, so we don't run too fast
    }
}

void cGame::shakeScreen(int duration)
{
    game.m_TIMER_shake += duration;
}

/**
	Shutdown the game
*/
void cGame::shutdown()
{
    cLogger *logger = cLogger::getInstance();
    logger->logHeader("SHUTDOWN");

    for (int i = 0; i < GAME_MAX_STATES; i++) {
        cGameState *pState = m_states[i];
        if (pState) {
            if (pState->getType() != eGameStateType::GAMESTATE_SELECT_YOUR_NEXT_CONQUEST) {
                if (m_currentState == pState) {
                    m_currentState = nullptr; // reset
                }
                delete pState;
                m_states[i] = nullptr;
            }
            else {
                if (m_currentState == pState) {
                    m_currentState = nullptr; // reset
                }
                m_states[i] = nullptr;
            }
        }
    }

    if (m_currentState != nullptr) {
        assert(false);
        if (m_currentState->getType() != eGameStateType::GAMESTATE_SELECT_YOUR_NEXT_CONQUEST) {
            // destroy game state object, unless we talk about the region select
            delete m_currentState;
        }
        else {
            m_currentState = nullptr;
        }
    }

    if (m_PreviewMaps != nullptr) {
        m_PreviewMaps->destroy();
    }

    delete m_mentat;
    delete m_mapViewport;

    delete drawManager;

    delete mapCamera;

    cStructureFactory::destroy();
    cSideBarFactory::destroy();
    cBuildingListFactory::destroy();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].destroyAllegroBitmaps();
    }

    delete renderDrawer;
    // delete m_dataRepository;
    // m_soundPlayer.reset();
    delete m_mouse;
    delete m_keyboard;

    TTF_CloseFont(game_font);
    TTF_CloseFont(bene_font);
    TTF_CloseFont(small_font);

    logbook("Allegro FONT library shut down.");

    // Release the game dev framework, so that it can do cleanup
    m_PLInit.reset();
}


/**
	Setup the game

	Should not be called twice.
*/
bool cGame::setupGame()
{
    cLogger *logger = cLogger::getInstance();
    logger->setDebugMode(m_debugMode);
    logger->logHeader("Dune II - The Maker");
    logger->logCommentLine(""); // whitespace

    logger->logHeader("Version information");
    logger->log(LOG_INFO, COMP_VERSION, "Initializing",
                std::format("Version {}, Compiled at {} , {}", D2TM_VERSION, __DATE__, __TIME__));

    // SETTINGS.INI
    std::shared_ptr<cIniFile> settings = std::make_shared<cIniFile>("settings.ini", m_debugMode);

    // TODO(SETTINGS) -> The game rules file is loaded via here, but *also* via `INSTALL_GAME()`!
    std::shared_ptr<cIniFile> gamesCfg = std::make_shared<cIniFile>(m_gameFilename, m_debugMode);

    m_reinforcements = std::make_shared<cReinforcements>();
    global_map.setReinforcements(m_reinforcements);

    game.init(); // Must be first! (loads game.ini file at the end, which is required before going on...)

    const std::string &gameDir = settings->getStringValue(SECTION_SETTINGS, "GameDir");
    std::unique_ptr<cFileValidator> settingsValidator = std::make_unique<cFileValidator>(gameDir);
    {
        std::map<eGameDirFileName, std::string> m_transfertMap;
        m_transfertMap[eGameDirFileName::ARRAKEEN] = settings->getStringValue("FONT", "ARRAKEEN");
        m_transfertMap[eGameDirFileName::BENEGESS] = settings->getStringValue("FONT", "BENEGESS");
        m_transfertMap[eGameDirFileName::SMALL] = settings->getStringValue("FONT", "SMALL");

        m_transfertMap[eGameDirFileName::GFXDATA] = settings->getStringValue("DATAFILE", "GFXDATA");
        m_transfertMap[eGameDirFileName::GFXINTER] = settings->getStringValue("DATAFILE", "GFXINTER");
        m_transfertMap[eGameDirFileName::GFXWORLD] = settings->getStringValue("DATAFILE", "GFXWORLD");
        m_transfertMap[eGameDirFileName::GFXMENTAT] = settings->getStringValue("DATAFILE", "GFXMENTAT");
        m_transfertMap[eGameDirFileName::GFXAUDIO] = settings->getStringValue("DATAFILE", "GFXAUDIO");
        settingsValidator->addResources(std::move(m_transfertMap));
    }

    // circumvent: -Werror=unused-function :/
    //eGameDirFileNameString(eGameDirFileName::ARRAKEEN);

    if (!settingsValidator->fileExists()) {
        logger->log(LOG_INFO, COMP_INIT, "Loading settings.ini", "Validation of files within settings.ini failed", OUTC_FAILED);
        std::cerr << "One or more validations failed with resources defined in settings.ini" << std::endl;
        return false;
    }

    // GAME.INI
    const auto title = std::format("Dune II - The Maker [{}] - (by Stefan Hendriks)", D2TM_VERSION);

    // FIXME: eventually, we will want to grab this object in the constructor. But then cGame cannot be a
    // global anymore, because it needs to be destructed before main exits.
    m_PLInit = std::make_unique<cPlatformLayerInit>();

    m_keyboard = new cKeyboard();
    logger->log(LOG_INFO, COMP_INIT, "Initializing Keyboard", "install_keyboard()", OUTC_SUCCESS);

    /* set up the interrupt routines... */
    game.m_TIMER_shake = 0;

    m_Screen = std::make_unique<cScreenInit>(m_screenW, m_screenH, title);
    if (!m_windowed) {
        m_Screen->setFullScreenMode();
    }

    m_screenW = m_Screen->Width();
    m_screenH = m_Screen->Height();
    window = m_Screen->getWindows();
    renderer = m_Screen->getRenderer();

    // create ressources from scratch
    context = std::make_unique<ContextCreator>(renderer, settingsValidator.get());
    // share them to all class what use ctx !
    ctx->setGraphicsContext(context->createGraphicsContext());

    game_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::ARRAKEEN).c_str(),12);
    //Mira TEXT if (game_font != nullptr) {
    //Mira TEXT     logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded " + settingsValidator->getName(eGameDirFileName::ARRAKEEN), OUTC_SUCCESS);
    //Mira TEXT     alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
    //Mira TEXT } else {
    //Mira TEXT     logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load " + settingsValidator->getName(eGameDirFileName::ARRAKEEN), OUTC_FAILED);
    //Mira TEXT     std::cerr << ("Fatal error:\n\nCould not start game.\n\nFailed to load arakeen.fon");
    //Mira TEXT     return false;
    //Mira TEXT }

    bene_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::BENEGESS).c_str(),12);

    gr_bene_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::BENEGESS).c_str(),20);
    //Mira TEXT if (bene_font != nullptr) {
    //Mira TEXT     logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded " + settingsValidator->getName(eGameDirFileName::BENEGESS), OUTC_SUCCESS);
    //Mira TEXT     alfont_set_font_size(bene_font, 10); // set size
    //Mira TEXT } else {
    //Mira TEXT     logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load " + settingsValidator->getName(eGameDirFileName::BENEGESS) , OUTC_FAILED);
    //Mira TEXT     std::cerr << ("Fatal error:\n\nCould not start game.\n\nFailed to load benegess.fon");
    //Mira TEXT     return false;
    //Mira TEXT }

    small_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::SMALL).c_str(),12);
    //Mira TEXT if (small_font != nullptr) {
    //Mira TEXT     logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded " + settingsValidator->getFullName(eGameDirFileName::SMALL), OUTC_SUCCESS);
    //Mira TEXT     alfont_set_font_size(small_font, 10); // set size
    //Mira TEXT } else {
    //Mira TEXT     logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load " + settingsValidator->getFullName(eGameDirFileName::SMALL), OUTC_FAILED);
    //Mira TEXT     std::cerr << ("Fatal error:\n\nCould not start game.\n\nFailed to load small.ttf");
    //Mira TEXT     return false;
    //Mira TEXT }

    textDrawer = std::make_unique<cTextDrawer>(game_font);
    textDrawer->setApplyShadow(false);

    std::unique_ptr<cSoundPlayer> soundPlayer = std::make_unique<cSoundPlayer>(settingsValidator->getFullName(eGameDirFileName::GFXAUDIO));
    m_soundPlayer = soundPlayer.get();
    ctx->setSoundPlayer(std::move(soundPlayer));
    if (!m_playSound) {
        m_soundPlayer->setSoundEnable(true);
    }
    if (!m_playMusic) {
        m_soundPlayer->setMusicEnable(true);
    }

    // do it here, because it depends on fonts to be loaded
    m_mouse = new cMouse();

    /***
     * Viewport(s)
     */
    m_mapViewport = new cRectangle(0, cSideBar::TopBarHeight, game.m_screenW - cSideBar::SidebarWidth, game.m_screenH - cSideBar::TopBarHeight);

    logbook("Color conversion method set");

    logbook("MOUSE: Mouse speed set");

    logger->logHeader("GAME");

    /*** Data files ***/

    // load datafiles
    gfxdata = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXDATA));
    if (gfxdata == nullptr) {
        logger->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXDATA), OUTC_FAILED);
        return false;
    }
    else {
        logger->log(LOG_INFO, COMP_INIT, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXDATA), OUTC_SUCCESS);
        // memcpy(general_palette, gfxdata[PALETTE_D2TM], sizeof general_palette);
    }

    gfxinter = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXINTER));
    if (gfxinter == nullptr) {
        logger->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXINTER), OUTC_FAILED);
        return false;
    }
    else {
        logger->log(LOG_INFO, COMP_INIT, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXINTER), OUTC_SUCCESS);
    }

    // gfxworld = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXWORLD));
    // if (gfxworld == nullptr) {
    //     logger->log(LOG_ERROR, COMP_ALLEGRO, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXWORLD), OUTC_FAILED);
    //     return false;
    // }
    // else {
    //     logger->log(LOG_INFO, COMP_ALLEGRO, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXWORLD), OUTC_SUCCESS);
    // }

    // gfxmentat = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXMENTAT));
    // if (gfxworld == nullptr) {
    //     logger->log(LOG_ERROR, COMP_ALLEGRO, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXMENTAT), OUTC_FAILED);
    //     return false;
    // }
    // else {
    //     logger->log(LOG_INFO, COMP_ALLEGRO, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXMENTAT), OUTC_SUCCESS);
    // }

    renderDrawer = new SDLDrawer(renderer);

    // randomize timer
    auto t = static_cast<unsigned int>(time(nullptr));
    logbook(std::format("Seed is {}", t));
    srand(t);

    game.m_playing = true;
    game.m_screenshot = 0;
    game.m_state = GAME_INITIALIZE;

    logbook("Setup:  HOUSES");
    m_Houses = std::make_shared<cHousesInfo>();
    m_Houses->INSTALL_HOUSES(gamesCfg);
    // A few messages for the player
    logbook("Initializing:  PLAYERS");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].init(i, nullptr);
        players[i].setHousesInfo(m_Houses);
    }
    logbook("Setup:  BITMAPS");
    install_bitmaps();
    logbook("Setup:  STRUCTURES");
    install_structures();
    logbook("Setup:  PROJECTILES");
    install_bullets();
    logbook("Setup:  UNITS");
    install_units();
    logbook("Setup:  SPECIALS");
    install_specials();
    logbook("Setup:  PARTICLES");
    install_particles();

    delete mapCamera;
    mapCamera = new cMapCamera(&global_map, game.m_cameraDragMoveSpeed, game.m_cameraBorderOrKeyMoveSpeed, game.m_cameraEdgeMove);

    INI_Install_Game(m_gameFilename);
    // Now we are ready for the menu state
    game.setState(GAME_MENU);

    // do install_upgrades after game.init, because game.init loads the INI file and then has the very latest
    // unit/structures catalog loaded - which the install_upgrades depends on.
    install_upgrades();
    cPlayer *humanPlayer = &players[HUMAN];

    delete drawManager;
    drawManager = new cDrawManager(humanPlayer);

    // Must be after drawManager, because the cInteractionManager constructor depends on drawManager
    m_interactionManager = std::make_unique<cInteractionManager>(humanPlayer);

    game.setupPlayers();

    playMusicByTypeForStateTransition(MUSIC_MENU);

    m_mouse->setMouseObserver(m_interactionManager.get());
    m_keyboard->setKeyboardObserver(m_interactionManager.get());

    // all has installed well. Let's rock and roll.
    SDL_ShowCursor(false);
    return true;
}

/**
 * Set up players
 * (Elegible for combat state object initialization)
 */
void cGame::setupPlayers()
{
    // make sure each player has an own item builder
    for (int i = HUMAN; i < MAX_PLAYERS; i++) {
        cPlayer *thePlayer = &players[i];

        auto *buildingListUpdater = new cBuildingListUpdater(thePlayer);
        thePlayer->setBuildingListUpdater(buildingListUpdater);

        auto *itemBuilder = new cItemBuilder(thePlayer, buildingListUpdater);
        thePlayer->setItemBuilder(itemBuilder);

        auto *sidebar = cSideBarFactory::getInstance()->createSideBar(thePlayer);
        thePlayer->setSideBar(sidebar);

        auto *orderProcesser = new cOrderProcesser(thePlayer);
        thePlayer->setOrderProcesser(orderProcesser);

        auto *gameControlsContext = new cGameControlsContext(thePlayer, this->m_mouse);
        thePlayer->setGameControlsContext(gameControlsContext);

        // set tech level
        thePlayer->setTechLevel(game.m_mission);
    }
    setPlayerToInteractFor(&players[0]);
}

bool cGame::isState(int thisState) const
{
    return (m_state == thisState);
}

void cGame::jumpToSelectYourNextConquestMission(int missionNr)
{
    cGameState *existingStatePtr = m_states[GAME_REGION];
    if (existingStatePtr) {
        delete existingStatePtr;
        m_states[GAME_REGION] = nullptr;
    }

    cSelectYourNextConquestState *pState = new cSelectYourNextConquestState(ctx.get(),game);
    m_states[GAME_REGION] = pState;

    pState->calculateOffset();
    pState->INSTALL_WORLD();

    cPlayer &humanPlayer = players[HUMAN];
    int missionZeroBased = missionNr - 1;
    m_mission = missionZeroBased;

    // a 'missionX.ini' file is from 1 til (including) 8
    // to play mission 2 (passed as missionNr param), we have to load up mission1.ini
    // meaning we have to use the 'zero based' value here
    pState->fastForwardUntilMission(missionZeroBased, humanPlayer.getHouse());
    pState->REGION_SETUP_NEXT_MISSION(missionZeroBased, humanPlayer.getHouse());
}

void cGame::setState(int newState)
{
    if (newState == m_state) {
        // ignore
        return;
    }

    logbook(std::format("Setting state from {}(={}) to {}(={})", m_state, stateString(m_state), newState, stateString(newState)));

    if (newState > -1) {
        bool deleteOldState = (newState != GAME_REGION &&
                               newState != GAME_PLAYING &&
                               newState != GAME_OPTIONS); // don't delete these m_states, but re-use!

        if (newState == GAME_PLAYING) {
            // make sure to delete options menu now
            delete m_states[GAME_OPTIONS];
            m_states[GAME_OPTIONS] = nullptr;
        }

        if (newState == GAME_REGION) {
            // make sure to delete options menu now
            delete m_states[GAME_OPTIONS];
            m_states[GAME_OPTIONS] = nullptr;
        }

        if (newState == GAME_MISSIONSELECT) {
            deleteOldState = true; // delete old options state everytime
        }

        if (deleteOldState) {
            delete m_states[newState];
            m_states[newState] = nullptr;
        }

        cPlayer &humanPlayer = players[HUMAN];

        cGameState *existingStatePtr = m_states[newState];

        if (existingStatePtr) {
            // no need for re-creating state
            if (newState == GAME_REGION) {
                playMusicByTypeForStateTransition(MUSIC_CONQUEST);

                // came from a win/lose brief state, so make sure to set up the next state
                if (m_state == GAME_WINBRIEF || m_state == GAME_LOSEBRIEF) {
                    // because `GAME_REGION` == if (existingStatePtr->getType() == GAMESTATE_SELECT_YOUR_NEXT_CONQUEST ||
                    auto *pState = dynamic_cast<cSelectYourNextConquestState *>(existingStatePtr);

                    if (game.m_mission > 1) {
                        pState->conquerRegions();
                    }

                    if (m_missionWasWon) {
                        // we won
                        pState->REGION_SETUP_NEXT_MISSION(game.m_mission, humanPlayer.getHouse());
                    }
                    else {
                        // OR: did not win
                        pState->REGION_SETUP_LOST_MISSION();
                    }
                }
            }
            else if (newState == GAME_OPTIONS) {
                // This feels awkward. For now, I'll keep it (if it works), but this will change
                // once we have a proper game playing state and we need to transition properly between
                // states

                auto *pState = dynamic_cast<cOptionsState *>(existingStatePtr);
                // you cannot 'go back' to mission select
                if (m_state != GAME_MISSIONSELECT) {
                    pState->setPrevState(m_state);
                }
                else {
                    pState->refresh(); // rebuilds UI windows, but keeps background
                }
            }

            m_currentState = existingStatePtr;
        }
        else {
            cGameState *newStatePtr = nullptr;

            if (newState == GAME_REGION) {
                cSelectYourNextConquestState *pState = new cSelectYourNextConquestState(ctx.get(),game);

                pState->calculateOffset();
                logbook("Setup:  WORLD");
                pState->INSTALL_WORLD();
                if (game.m_mission > 1) {
                    pState->conquerRegions();
                }
                // first creation
                pState->REGION_SETUP_NEXT_MISSION(game.m_mission, humanPlayer.getHouse());

                playMusicByTypeForStateTransition(MUSIC_CONQUEST);

                newStatePtr = pState;
            }
            else if (newState == GAME_SETUPSKIRMISH) {
                initPlayers(false);
                newStatePtr = new cSetupSkirmishGameState(*this, m_PreviewMaps);
                playMusicByTypeForStateTransition(MUSIC_MENU);
            }
            else if (newState == GAME_CREDITS) {
                newStatePtr = new cCreditsState(*this);
            }
            else if (newState == GAME_MENU) {
                newStatePtr = new cMainMenuGameState(*this);
                playMusicByTypeForStateTransition(MUSIC_MENU);
            }
            else if (newState == GAME_SELECT_HOUSE) {
                newStatePtr = new cChooseHouseGameState(*this);
            }
            else if (newState == GAME_MISSIONSELECT) {
                m_mouse->setTile(MOUSE_NORMAL);
                //SDL_Surface *background = SDL_CreateRGBSurface(0,m_screenW, m_screenH,32,0,0,0,255);
                // renderDrawer->drawSprite(background, bmp_screen, 0, 0);
                newStatePtr = new cSelectMissionState(*this, /*background,*/ m_state);
            }
            else if (newState == GAME_OPTIONS) {
                m_mouse->setTile(MOUSE_NORMAL);
                //SDL_Surface *background = SDL_CreateRGBSurface(0,m_screenW, m_screenH,32,0,0,0,255);
                if (m_state == GAME_PLAYING) {
                    // so we don't draw mouse cursor
                    drawManager->drawCombatState();
                }
                else {
                    // we fall back what was on screen, (which includes mouse cursor for now)
                }

                // renderDrawer->drawSprite(background, bmp_screen, 0, 0);
                newStatePtr = new cOptionsState(*this, ctx.get(), /*background,*/ m_state);
            }
            else if (newState == GAME_PLAYING) {
                if (m_state == GAME_OPTIONS) {
                    // we came from options menu, notify mouse
                    humanPlayer.getGameControlsContext()->onFocusMouseStateEvent();
                }
                else {
                    // re-create drawManager
                    delete drawManager;
                    drawManager = new cDrawManager(&humanPlayer);

                    // evaluate all players, so we have initial 'alive' values set properly
                    for (int i = 1; i < MAX_PLAYERS; i++) {
                        cPlayer &player = players[i];
                        player.evaluateStillAlive();
                    }

                    // in-between solution until we have a proper combat state object
                    drawManager->init();

                    // handle update
                    s_GameEvent event{
                        .eventType = eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN,
                    };
                    // the game is about to begin!
                    game.onNotifyGameEvent(event);
                }
            }

            m_states[newState] = newStatePtr;
            m_currentState = newStatePtr;
        }
    }

    m_state = newState;
}

void cGame::think_fading()
{
    // Fading of the entire screen
    if (m_fadeAction == eFadeAction::FADE_OUT) {
        m_fadeAlpha -= 2;
        if (m_fadeAlpha < kMinAlpha) {
            m_fadeAlpha = kMinAlpha;
            m_fadeAction = eFadeAction::FADE_NONE;
        }
    }
    else if (m_fadeAction == eFadeAction::FADE_IN) {
        m_fadeAlpha += 2;
        if (m_fadeAlpha > kMaxAlpha) {
            m_fadeAlpha = kMaxAlpha;
            m_fadeAction = eFadeAction::FADE_NONE;
        }
    }

    // Fading / pulsating of selected stuff
    static constexpr float fadeSelectIncrement = 1 / 256.0f;
    if (m_fadeSelectDir) {
        m_fadeSelect += fadeSelectIncrement;

        // when 255, then fade back
        if (m_fadeSelect > 0.99) {
            m_fadeSelect = 1.0f;
            m_fadeSelectDir = false;
        }

        return;
    }

    m_fadeSelect -= fadeSelectIncrement;
    // not too dark,
    // 0.03125
    if (m_fadeSelect < 0.3125f) {
        m_fadeSelectDir = true;
    }
}

cGame::~cGame()
{}

void cGame::prepareMentatForPlayer()
{
    int house = players[HUMAN].getHouse();
    if (m_state == GAME_BRIEFING) {
        game.missionInit();
        game.setupPlayers();
        INI_Load_scenario(house, m_region, m_mentat, m_reinforcements.get());
        INI_LOAD_BRIEFING(house, m_region, INI_BRIEFING, m_mentat);
    }
    else if (m_state == GAME_WINBRIEF) {
        if (RNG::rnd(100) < 50) {
            m_mentat->loadScene("win01");
        }
        else {
            m_mentat->loadScene("win02");
        }
        INI_LOAD_BRIEFING(house, m_region, INI_WIN, m_mentat);
    }
    else if (m_state == GAME_LOSEBRIEF) {
        if (RNG::rnd(100) < 50) {
            m_mentat->loadScene("lose01");
        }
        else {
            m_mentat->loadScene("lose02");
        }
        INI_LOAD_BRIEFING(house, m_region, INI_LOSE, m_mentat);
    }
}

void cGame::createAndPrepareMentatForHumanPlayer(bool allowMissionSelect)
{
    delete m_mentat;
    int houseIndex = players[HUMAN].getHouse();
    if (houseIndex == ATREIDES) {
        m_mentat = new AtreidesMentat(ctx.get(), allowMissionSelect);
    }
    else if (houseIndex == HARKONNEN) {
        m_mentat = new HarkonnenMentat(ctx.get(), allowMissionSelect);
    }
    else if (houseIndex == ORDOS) {
        m_mentat = new OrdosMentat(ctx.get(), allowMissionSelect);
    }
    else {
        // fallback
        m_mentat = new BeneMentat(ctx.get());
    }
    prepareMentatForPlayer();
    m_mentat->speak();
}

void cGame::prepareMentatToTellAboutHouse(int house)
{
    delete m_mentat;
    m_mentat = new BeneMentat(ctx.get());
    m_mentat->setHouse(house);
    // create new drawStateMentat
    if (house == ATREIDES) {
        INI_LOAD_BRIEFING(ATREIDES, 0, INI_DESCRIPTION, m_mentat);
        m_mentat->loadScene("platr"); // load planet of atreides
    }
    else if (house == HARKONNEN) {
        INI_LOAD_BRIEFING(HARKONNEN, 0, INI_DESCRIPTION, m_mentat);
        m_mentat->loadScene("plhar"); // load planet of harkonnen
    }
    else if (house == ORDOS) {
        INI_LOAD_BRIEFING(ORDOS, 0, INI_DESCRIPTION, m_mentat);
        m_mentat->loadScene("plord"); // load planet of ordos
    }
    else {
        m_mentat->setSentence(0, "Looks like you choose an unknown house");
    }
    // todo: Sardaukar, etc? (Super Dune 2 features)
    m_mentat->speak();
}

void cGame::loadScenario()
{
    int iHouse = players[HUMAN].getHouse();
    INI_Load_scenario(iHouse, game.m_region, m_mentat, m_reinforcements.get());
}

void cGame::thinkFast_state()
{
    think_audio();
    think_mentat();

    if (m_currentState) {
        m_currentState->thinkFast();
    }

    // THINKING ONLY WHEN PLAYING / COMBAT (no state object yet)
    if (isState(GAME_PLAYING)) {
        drawManager->thinkFast_statePlaying();
        thinkFast_combat();
    }

    if (drawManager) {
        drawManager->thinkFast();
    }
}

void cGame::thinkFast_combat()
{
    mapCamera->thinkFast();

    for (cPlayer &pPlayer : players) {
        pPlayer.thinkFast();
    }

    // structures think
    for (cAbstractStructure *pStructure : structure) {
        if (pStructure == nullptr) continue;
        if (pStructure->isValid()) {
            pStructure->thinkFast();           // think about actions going on
            pStructure->think_animation(); // think about animating
            pStructure->think_guard();     // think about 'guarding' the area (turrets only)
        }

        if (pStructure->isDead()) {
            cStructureFactory::getInstance()->deleteStructureInstance(pStructure);
        }
    }

    for (cPlayer &pPlayer : players) {
        cItemBuilder *itemBuilder = pPlayer.getItemBuilder();
        if (itemBuilder) {
            itemBuilder->thinkFast();
        }
    }

    global_map.thinkFast();

    game.reduceShaking();

    // units think (move only)
    for (cUnit &cUnit : unit) {
        if (!cUnit.isValid()) continue;
        cUnit.thinkFast();
    }

    for (cParticle &pParticle : particle) {
        if (!pParticle.isValid()) continue;
        pParticle.thinkFast();
    }

    // when not drawing the options, the game does all it needs to do
    // bullets think
    for (cBullet &cBullet : bullet) {
        if (!cBullet.bAlive) continue;
        cBullet.thinkFast();
    }
}

void cGame::setPlayerToInteractFor(cPlayer *pPlayer)
{
    m_interactionManager->setPlayerToInteractFor(pPlayer);
}

void cGame::onNotifyGameEvent(const s_GameEvent &event)
{
    logbook(s_GameEvent::toString(event));

    global_map.onNotifyGameEvent(event);

    // game itself handles events
    switch (event.eventType) {
        case eGameEventType::GAME_EVENT_SPECIAL_LAUNCH:
            onEventSpecialLaunch(event);
            break;
        default:
            break;
    }

    // players handle events
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].onNotifyGameEvent(event);
    }
}

void cGame::onEventSpecialLaunch(const s_GameEvent &event)
{
    cBuildingListItem *itemToDeploy = event.buildingListItem;
    int iMouseCell = event.atCell;
    cPlayer *player = event.player;
    if (itemToDeploy->isTypeSpecial()) {
        const s_SpecialInfo &special = itemToDeploy->getSpecialInfo();

        int deployCell = -1;
        if (special.deployTargetType == eDeployTargetType::TARGET_SPECIFIC_CELL) {
            deployCell = iMouseCell;
        }
        else if (special.deployTargetType == eDeployTargetType::TARGET_INACCURATE_CELL) {
            int precision = special.deployTargetPrecision;
            int mouseCellX = global_map.getCellX(iMouseCell) - precision;
            int mouseCellY = global_map.getCellY(iMouseCell) - precision;

            int posX = mouseCellX + RNG::rnd((precision * 2) + 1);
            int posY = mouseCellY + RNG::rnd((precision * 2) + 1);
            cPoint::split(posX, posY) = global_map.fixCoordinatesToBeWithinMap(posX, posY);

            logbook(std::format(
                        "eDeployTargetType::TARGET_INACCURATE_CELL, mouse cell X,Y = {},{} - target pos ={},{} - precision {}",
                        mouseCellY, mouseCellY, posX, posY,precision)
                   );

            deployCell = global_map.getGeometry()->makeCell(posX, posY);
        }


        if (special.providesType == eBuildType::BULLET) {
            // from where
            int structureId = structureUtils.findStructureBy(player->getId(), special.deployAtStructure, false);
            if (structureId > -1) {
                cAbstractStructure *pStructure = structure[structureId];
                if (pStructure && pStructure->isValid()) {
                    m_soundPlayer->playSound(SOUND_PLACE);
                    create_bullet(special.providesTypeId, pStructure->getCell(), deployCell, -1, structureId);

                    // notify game that the item just has been finished!
                    s_GameEvent newEvent{
                        .eventType = eGameEventType::GAME_EVENT_SPECIAL_LAUNCHED,
                        .entityType = itemToDeploy->getBuildType(),
                        .entityID = -1,
                        .player = pStructure->getPlayer(),
                        .entitySpecificType = itemToDeploy->getBuildId(),
                        .atCell = -1,
                        .isReinforce = false,
                        .buildingListItem = itemToDeploy
                    };

                    game.onNotifyGameEvent(newEvent);
                }
            }
        }
    }

    itemToDeploy->decreaseTimesToBuild();
    itemToDeploy->setDeployIt(false);
    itemToDeploy->setIsBuilding(false);
    itemToDeploy->resetProgress();
    if (itemToDeploy->getTimesToBuild() < 1) {
        player->getItemBuilder()->removeItemFromList(itemToDeploy);
    }

    if (player) {
        player->getGameControlsContext()->toPreviousState();
    }

    // notify game that the item just has been finished!
    s_GameEvent newEvent{
        .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED,
        .entityType = itemToDeploy->getBuildType(),
        .entityID = -1,
        .player = player,
        .entitySpecificType = itemToDeploy->getBuildId(),
        .atCell = -1,
        .isReinforce = false,
        .buildingListItem = nullptr
    };

    game.onNotifyGameEvent(newEvent);
}

void cGame::reduceShaking()
{
    if (m_TIMER_shake > 0) {
        m_TIMER_shake--;
    }
}

void cGame::install_bitmaps()
{
    //Mira rip this function
}

Color cGame::getColorFadeSelected(int r, int g, int b, bool rFlag, bool gFlag, bool bFlag)
{
    unsigned char desiredRed = rFlag ? r * m_fadeSelect : r;
    unsigned char desiredGreen = gFlag ? g * m_fadeSelect : g;
    unsigned char desiredBlue = bFlag ? b * m_fadeSelect : b;
    return Color{desiredRed, desiredGreen, desiredBlue,255};
}

Color cGame::getColorPlaceNeutral()
{
    return Color{242, 174, 36,64};
}

Color cGame::getColorPlaceBad()
{
    return Color{160, 0, 0,64};
}

Color cGame::getColorPlaceGood()
{
    return Color{64, 255, 64,64};
}

void cGame::setWinFlags(int value)
{
    if (game.isDebugMode()) {
        logbook(std::format("Changing m_winFlags from {} to {}", m_winFlags, value));
    }
    m_winFlags = value;
}

void cGame::setLoseFlags(int value)
{
    if (game.isDebugMode()) {
        logbook(std::format("Changing m_loseFlags from {} to {}", m_loseFlags, value));
    }
    m_loseFlags = value;
}

void cGame::onNotifyMouseEvent(const s_MouseEvent &event)
{
    // pass through any classes that are interested
    if (m_currentState) {
        m_currentState->onNotifyMouseEvent(event);
    }

    // if (m_state == GAME_BRIEFING ||
    //         m_state == GAME_WINBRIEF ||
    //         m_state == GAME_LOSEBRIEF
    //    ) {
        if (m_mentat) {
            m_mentat->onNotifyMouseEvent(event);
        }
    // }
}

void cGame::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    // pass through any classes that are interested
    if (m_currentState) {
        m_currentState->onNotifyKeyboardEvent(event);
    }

    // take screenshot
    if (event.isType(eKeyEventType::PRESSED) && event.hasKey(SDL_SCANCODE_F11)) {
        saveBmpScreenToDisk();
    }

    // TODO: this has to be its own state class. Then this if is no longer needed.
    if (m_state == GAME_PLAYING) {
        onNotifyKeyboardEventGamePlaying(event);
    }
}

void cGame::transitionStateIfRequired()
{
    if (m_nextState > -1) {
        setState(m_nextState);

        if (m_nextState == GAME_BRIEFING) {
            playMusicByType(MUSIC_BRIEFING);
            game.createAndPrepareMentatForHumanPlayer();
        }

        m_nextState = -1;
    }
}

void cGame::setNextStateToTransitionTo(int newState)
{
    m_nextState = newState;
}

void cGame::drawCombatMouse()
{
    if (m_mouse->isBoxSelecting()) {
        renderDrawer->renderRectColor(m_mouse->getBoxSelectRectangle(),255,255,255,255);
    }

    if (m_mouse->isMapScrolling()) {
        cPoint startPoint = m_mouse->getDragLineStartPoint();
        cPoint endPoint = m_mouse->getDragLineEndPoint();
        renderDrawer->renderLine( startPoint.x, startPoint.y, endPoint.x, endPoint.y, Color{255,255,255,255});
    }

    m_mouse->draw();
}

void cGame::saveBmpScreenToDisk()
{
    std::string filename = std::format("{}x{}_{:0>4}.bmp", m_screenW, m_screenH, m_screenshot);
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, m_screenW, m_screenH, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        std::cerr << "Erreur lors de la création de la surface: " << SDL_GetError() << std::endl;
        return;
    }
    // Lire les pixels depuis le framebuffer
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
        std::cerr << "Erreur lors de la lecture des pixels: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    std::string name = std::format("screenshot{}.bmp",m_screenshot);
    SDL_SaveBMP(surface, filename.c_str());
    SDL_FreeSurface(surface);

    // shows a message in-game, would be even better to have this 'globally' (not depending on state), kind of like
    // a Quake console perhaps?
    cPlayer &humanPlayer = players[HUMAN];
    humanPlayer.addNotification(std::format("Screenshot saved {}.", filename), eNotificationType::NEUTRAL);

    m_screenshot++;
}

void cGame::onNotifyKeyboardEventGamePlaying(const cKeyboardEvent &event)
{
    logbook(event.toString());

    drawManager->onNotifyKeyboardEvent(event);

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

void cGame::onKeyDownGamePlaying(const cKeyboardEvent &event)
{
    const cPlayer &humanPlayer = players[HUMAN];

    bool createGroup = event.hasKey(SDL_SCANCODE_LCTRL) || event.hasKey(SDL_SCANCODE_RCTRL);
    if (createGroup) {
        int iGroup = event.getGroupNumber();

        if (iGroup > 0) {
            humanPlayer.markUnitsForGroup(iGroup);
        }
    }

    if (isDebugMode()) { // debug mode has additional keys
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
        mapCamera->resetZoom();
    }

    if (event.hasKey(SDL_SCANCODE_H)) {
        mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
    }

    // Center on the selected structure
    if (event.hasKey(SDL_SCANCODE_C)) {
        cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
        if (selectedStructure) {
            mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
        game.setNextStateToTransitionTo(GAME_OPTIONS);
    }

    if (event.hasKey(SDL_SCANCODE_F)) {
        m_drawFps = true;
    }
}

void cGame::onKeyPressedGamePlaying(const cKeyboardEvent &event)
{
    cPlayer &humanPlayer = players[HUMAN];

    if (event.hasKey(SDL_SCANCODE_F)) {
        m_drawFps = false;
    }

    if (event.hasKey(SDL_SCANCODE_D)) {
        for (int i = 0; i < MAX_UNITS; i++) {
            cUnit &u = unit[i];
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

    if (event.hasKey(SDL_SCANCODE_M) || event.hasKey(SDL_SCANCODE_MUTE)) {
        game.m_playMusic = !game.m_playMusic;
        if (!game.m_playMusic) {
            m_soundPlayer->stopMusic();
        }
        else {
            m_soundPlayer->playMusic(m_newMusicSample);
        }
    }

    if (event.hasKey(SDL_SCANCODE_O) || event.hasKey(SDL_SCANCODE_VOLUMEDOWN)) {
        m_soundPlayer->changeMusicVolume(-10);
    }

    if (event.hasKey(SDL_SCANCODE_P) || event.hasKey(SDL_SCANCODE_VOLUMEUP) ) {
        m_soundPlayer->changeMusicVolume(10);
    }

    if (event.hasKey(SDL_SCANCODE_H)) {
        mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
    }

    // Center on the selected structure
    if (event.hasKey(SDL_SCANCODE_C)) {
        cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
        if (selectedStructure) {
            mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }

    if (event.hasKeys(SDL_SCANCODE_LALT,SDL_SCANCODE_RETURN)) {
        if (m_windowed) {
            m_Screen->setFullScreenMode();
            m_windowed = false;
        } else {
            m_Screen->setWindowMode();
            m_windowed = true;
        }
    }

    cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
    if (selectedStructure) {
        // depending on type of structure, a key could mean a different thing?
        // so, kind of like event.hasKey(selectedStructure->KeyForDeploying()) ?
        // and then perform?
        if (event.hasKey(SDL_SCANCODE_D)) {
            if (selectedStructure->getType() == REPAIR) { // this should be done differently?
                s_GameEvent e{
                    .eventType = eGameEventType::GAME_EVENT_DEPLOY_UNIT,
                    .entityType = eBuildType::UNKNOWN,
                    .entityID = -1,
                    .player = &humanPlayer
                };
                selectedStructure->onNotifyGameEvent(e);
            }
        }
        // other keys for other structures?
        // like: repair/stop repairing?
    }
}

void cGame::playSound(int sampleId)
{
    m_soundPlayer->playSound(sampleId);
}

void cGame::playSound(int sampleId, int vol)
{
    m_soundPlayer->playSound(sampleId, vol);
}

void cGame::playSoundWithDistance(int sampleId, int iDistance)
{
    if (iDistance <= 1) { // means "on screen" (meaning fixed volume, and no need for panning)
        playSound(sampleId);
        return;
    }

    // zoom factor influences distance we can 'hear'. The closer up, the less max distance. Unzoomed, this is half the map.
    // where when unit is at half map, we can hear it only a bit.
    float maxDistance = mapCamera->divideByZoomLevel(global_map.getMaxDistanceInPixels() / 2);
    float distanceNormalized = 1.0 - (iDistance / maxDistance);

    float volume = m_soundPlayer->getMaxVolume() * distanceNormalized;

    // zoom factor influences volume (more zoomed in means louder)
    float volumeFactor = mapCamera->factorZoomLevel(0.7f);
    int iVolFactored = volumeFactor * volume;

    if (game.isDebugMode()) {
        logbook(std::format("iDistance [{}], distanceNormalized [{}] maxDistance [{}], m_zoomLevel [{}], volumeFactor [{}], volume [{}], iVolFactored [{}]",
                            iDistance, distanceNormalized, maxDistance, mapCamera->getZoomLevel(), volumeFactor, volume, iVolFactored));
    }

    playSound(sampleId, iVolFactored);
}


void cGame::playVoice(int sampleId, int playerId)
{
    m_soundPlayer->playVoice(sampleId, players[playerId].getHouse());
}

void cGame::playMusicByTypeForStateTransition(int iType)
{
    if (m_musicType != iType) {
        m_newMusicCountdown = 0;
        playMusicByType(iType, HUMAN, false);
    }
}

bool cGame::playMusicByType(int iType, int playerId, bool triggerWithVoice)
{
    if (playerId != HUMAN) {
        // skip music we want to play for non human player
        return false;
    }

    logbook(std::format("cGame::playMusicByType - iType = {}. playerId = {}, triggerWithVoice = {}", iType, playerId, triggerWithVoice));

    if (triggerWithVoice) {
        if (iType == m_musicType) {
            logbook(std::format("m_musicType = {}, iType is {}, so bailing", m_musicType, iType));
            return false;
        }
    }

    m_musicType = iType;
    logbook(std::format("m_musicType = {}", m_musicType));

    if (!m_playMusic) {
        return false; // todo: have a 'no-sound soundplayer' instead of doing this :/
    }

    if (m_newMusicCountdown > 0) {
        // do not interfere with previous 'change to music' thing?
        return false;
    }


    int sampleId = MIDI_MENU;
    if (iType == MUSIC_WIN) {
        sampleId = MIDI_WIN01 + RNG::rnd(3);
    }
    else if (iType == MUSIC_LOSE) {
        sampleId = MIDI_LOSE01 + RNG::rnd(6);
    }
    else if (iType == MUSIC_ATTACK) {
        sampleId = MIDI_ATTACK01 + RNG::rnd(6);
    }
    else if (iType == MUSIC_PEACE) {
        sampleId = MIDI_BUILDING01 + RNG::rnd(9);
    }
    else if (iType == MUSIC_MENU) {
        sampleId = MIDI_MENU;
    }
    else if (iType == MUSIC_CONQUEST) {
        sampleId = MIDI_SCENARIO;
    }
    else if (iType == MUSIC_BRIEFING) {
        int houseIndex = players[HUMAN].getHouse();
        if (houseIndex == ATREIDES) {
            sampleId = MIDI_MENTAT_ATR;
        }
        else if (houseIndex == HARKONNEN) {
            sampleId = MIDI_MENTAT_HAR;
        }
        else if (houseIndex == ORDOS) {
            sampleId = MIDI_MENTAT_ORD;
        }
        else {
            assert(false && "Undefined house.");
        }
    }
    else {
        assert(false && "Undefined music type.");
    }

    if (triggerWithVoice) {
        // voice triggered music (ie "Enemy unit approaching"), so have music stop a bit
        if (isState(GAME_PLAYING)) {
            m_newMusicCountdown = 400; // wait a bit longer
        }
        else {
            m_newMusicCountdown = 0;
        }
        m_soundPlayer->stopMusic();
    }
    else {
        // instant switch
        m_newMusicCountdown = 0;
    }

    m_newMusicSample = sampleId;
    return true;
}

int cGame::getMaxVolume()
{
    return m_soundPlayer->getMaxVolume();
}

/**
 * Called every 100ms
 */
void cGame::think_state()
{
    if (game.isState(GAME_PLAYING)) {
        // TODO: state->think()
        // units think
        for (int i = 0; i < MAX_UNITS; i++) {
            cUnit &cUnit = unit[i];
            if (cUnit.isValid()) {
                cUnit.think();
            }
        }

        drawManager->think();

        for (int i = 0; i < MAX_PLAYERS; i++) {
            players[i].think();
        }

    }
}

/**
 * Called every second
 */
void cGame::thinkSlow()
{
    thinkSlow_state();

    m_timeManager->capFps();
    m_timeManager->adaptWaitingTime();
}

void cGame::thinkSlow_state()
{
    if (isState(GAME_PLAYING)) {
        thinkSlow_stateCombat_evaluatePlayerStatus(); // so we can call non-const from a const :S

        m_pathsCreated = 0;

        if (!m_disableReinforcements) {
            m_reinforcements->thinkSlow();
        }

        // starports think per second for deployment (if any)
        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *pStructure = structure[i];
            if (pStructure && pStructure->isValid()) {
                pStructure->thinkSlow();
            }
        }

        for (int i = 0; i < MAX_PLAYERS; i++) {
            cPlayer &player = players[i];
            player.thinkSlow();
        }

    } // game specific stuff

}

void cGame::onKeyDownDebugMode(const cKeyboardEvent &event)
{
    const cPlayer &humanPlayer = players[HUMAN];

    if (event.hasKey(SDL_SCANCODE_0)) {
        drawManager->setPlayerToDraw(&players[0]);
        game.setPlayerToInteractFor(&players[0]);
    }
    else if (event.hasKey(SDL_SCANCODE_1)) {
        drawManager->setPlayerToDraw(&players[1]);
        game.setPlayerToInteractFor(&players[1]);
    }
    else if (event.hasKey(SDL_SCANCODE_2)) {
        drawManager->setPlayerToDraw(&players[2]);
        game.setPlayerToInteractFor(&players[2]);
    }
    else if (event.hasKey(SDL_SCANCODE_3)) {
        drawManager->setPlayerToDraw(&players[3]);
        game.setPlayerToInteractFor(&players[3]);
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
            players[i].setCredits(5000);
        }
    }

    //DESTROY UNIT OR BUILDING
    if (event.hasKeys(SDL_SCANCODE_F4, SDL_SCANCODE_LSHIFT)) {
        int mc = humanPlayer.getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = global_map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                unit[idOfUnitAtCell].die(true, false);
            }

            int idOfStructureAtCell = global_map.getCellIdStructuresLayer(mc);
            if (idOfStructureAtCell > -1) {
                structure[idOfStructureAtCell]->die();
            }

            idOfUnitAtCell = global_map.getCellIdWormsLayer(mc);
            if (idOfUnitAtCell > -1) {
                unit[idOfUnitAtCell].die(false, false);
            }
        }
    }

    //DESTROY UNIT OR BUILDING
    if (event.hasKeys(SDL_SCANCODE_F5, SDL_SCANCODE_LSHIFT)) {
        int mc = humanPlayer.getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = global_map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                cUnit &pUnit = unit[idOfUnitAtCell];
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
            cUnit &pUnit = unit[unitId];
            pUnit.die(true, false);
        }
    }

    if (event.hasKey(SDL_SCANCODE_F7)) {
//        shakeScreen(200); // shake for 1 second (fast timer)
        game.m_TIMER_shake = 200;
    }

}

void cGame::setMousePosition(int w, int h)
{
    m_mouse->setCursorPosition(window, w,h);
}

void cGame::execute(AbstractMentat &mentat)
{
    if (game.isState(GAME_BRIEFING)) {
        // proceed, play mission (it is already loaded before we got here)
        game.setNextStateToTransitionTo(GAME_PLAYING);
        drawManager->missionInit();

        // CENTER MOUSE
        game.setMousePosition(game.m_screenW / 2, game.m_screenH / 2);

        game.initiateFadingOut();

        game.playMusicByType(MUSIC_PEACE);
        return;
    }

    if (game.m_skirmish) {
        if (game.isState(GAME_WINBRIEF) || game.isState(GAME_LOSEBRIEF)) {
            // regardless of drawStateWinning or drawStateLosing, always go back to main menu
            game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
            game.initSkirmish();
            game.initiateFadingOut();
        }
        else {
            logbook("cProceedButtonCommand pressed, in skirmish mode and state is not WINBRIEF nor LOSEBRIEF!?");
        }
        return;
    }

    // NOT a skirmish game

    // won mission, transition to region selection (Select your next Conquest)
    if (game.isState(GAME_WINBRIEF)) {
        game.setNextStateToTransitionTo(GAME_REGION);

        game.initiateFadingOut();
        return;
    }

    // lost mission
    if (game.isState(GAME_LOSEBRIEF)) {
        game.missionInit();
        // lost mission > 1, so we go back to region select
        if (game.m_mission > 1)   {
            game.setNextStateToTransitionTo(GAME_REGION);

            game.m_mission--; // we did not win
        }
        else {
            // mission 1 failed, really?..., back to mentat with briefing
            game.setNextStateToTransitionTo(GAME_BRIEFING);
            game.prepareMentatForPlayer();
            game.playMusicByType(MUSIC_BRIEFING);
            mentat.resetSpeak();
        }

        game.initiateFadingOut();
        return;
    }
}
