/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

*/

#include "building/cItemBuilder.h"
#include "config.h"
#include "context/ContextCreator.hpp"
#include "context/GameContext.hpp"
#include "context/cGameObjectContextCreator.h"
#include "game/cGame.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "include/eGameState.h"

#include "game/cGameConditionChecker.h"
#include "game/cEventEmitter.h"
#include "game/cScreenFader.h"
#include "game/cSDLSystem.h"
#include "game/cScreenShake.h"
#include "game/cScreenShotSaver.h"
#include "game/cTimeCounter.h"
#include "utils/cStructureUtils.h"

#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/units/cReinforcements.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/projectiles/cBullets.h"

#include "gamestates/cChooseHouseState.h"
#include "gamestates/cCreditsState.h"
#include "gamestates/cEditorState.h"
#include "gamestates/cGamePlaying.h"
#include "gamestates/cMainMenuState.h"
#include "gamestates/cMentatState.h"
#include "gamestates/cNewMapEditorState.h"
#include "gamestates/cOptionsState.h"
#include "gamestates/cSelectMissionState.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "gamestates/cSetupSkirmishState.h"
#include "gamestates/cTellHouseState.h"
#include "gamestates/cWinLoseState.h"

#include "gui/GuiConsole.h"

#include "include/sDataCampaign.h"
#include "iniDefine.h"
#include "managers/cDrawManager.h"
#include "gameobjects/map/cPreviewMaps.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/map/cMap.h"
#include "gameobjects/players/cHousesInfo.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "sidebar/cBuildingListFactory.h"
#include "sidebar/cSideBarFactory.h"
#include "gameobjects/units/cUnits.h"

#include "utils/cFileValidator.h"
#include "utils/cFocusManager.h"
#include "utils/cIniFile.h"
#include "utils/Log.h"
#include "utils/Color.hpp"
#include "utils/cSoundPlayer.h"
#include "utils/d2tm_math.h"
#include "utils/InitialGameSettings.hpp"
#include "game/cGameSettings.h"
#include "utils/Graphics.hpp"
#include "utils/common.h"
#include "utils/ini.h"
#include "utils/RNG.hpp"

#include "controls/cGameControlsContext.h"
#include "controls/eKeyAction.h"
#include "gameobjects/map/cMapCamera.h"
#include "game/cTimeManager.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

#include <format>
#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

#include "data/gfxaudio.h"
#include "include/sGameServices.h"
#include "game/cGameInterface.h"
#include "game/cNotificationArea.h"
#include "gamestates/cCreatorState.h"

namespace {
bool isConsoleToggleKey(const cKeyboardEvent &event) {
    return event.isType(eKeyEventType::PRESSED) && event.hasKey(SDL_SCANCODE_GRAVE);
}

bool isConsoleToggleTextInput(const cKeyboardEvent &event) {
    if (!event.hasTextInput()) {
        return false;
    }

    const std::string &textInput = event.getTextInput();
    return textInput == "`" || textInput == "~" || textInput == "²";
    }
}

bool cGame::shouldCaptureTextInput() const
{
    if (m_guiConsole && m_guiConsole->isKeyboardCaptured()) {
        return true;
    }

    return m_currentState && m_currentState->isKeyboardCapturedByUi();
}

void cGame::syncTextInputState() const
{
    if (shouldCaptureTextInput()) {
        SDL_StartTextInput(window);
    } else {
        SDL_StopTextInput(window);
    }
}

cGame::cGame()
{
    m_state = -1;
    m_nextState = -1;
    m_currentState = nullptr;

    m_windowed = false;
    m_playSound = true;
    context = nullptr;
    ctx = nullptr;
    m_pauseWhenLosingFocus = false;

    m_mapCamera = nullptr;
    m_drawManager = nullptr;

    m_cameraDragMoveSpeed=0.5f;
    m_cameraBorderOrKeyMoveSpeed=0.5;
    m_cameraEdgeMove = true;

    m_notificationArea = std::make_unique<cNotificationArea>();

    std::unique_ptr<cGameInterface> gameInterface = std::make_unique<cGameInterface>(this);

    m_structureUtils = std::make_unique<cStructureUtils>();

    // create GameContext
    ctx = std::make_unique<GameContext>();
    // create TimeManager
    std::unique_ptr<cTimeManager> timeManager = std::make_unique<cTimeManager>(this);
    // local usage
    m_timeManager = timeManager.get();
    // send to GameContext
    ctx->setTimeManager(std::move(timeManager));
    ctx->setGameInterface(std::move(gameInterface));
    // focus manager
    m_focusManager = std::make_unique<cFocusManager>(m_timeManager);

    m_infoContext = std::make_unique<cInfoContext>();
    
    m_screenShake = std::make_unique<cScreenShake>();

    m_dataCampaign = std::make_unique<s_DataCampaign>();

    m_cScreenFader = std::make_unique<cScreenFader>();

    m_gameSettings = std::make_unique<cGameSettings>();

    m_gameSettings->m_playMusic = true;
    m_gameSettings->m_drawFps = false;
    m_gameSettings->m_drawTime = false;
    m_gameSettings->m_allowRepeatingReinforcements = false;

    m_gameObjectsContext = cGameObjectsContextCreator::create();
    m_players = m_gameObjectsContext->getPlayers();
    d2tm_assert(m_players != nullptr);

    m_gameConditionChecker = std::make_unique<cGameConditionChecker>(m_gameObjectsContext.get());

    m_eventEmitter = std::make_unique<cEventEmitter>(
        [this](const s_GameEvent &event) {
            dispatchGameEvent(event);
        }
    );

    m_services = std::make_unique<sGameServices>();
    m_services->ctx = ctx.get();
    m_services->objects = m_gameObjectsContext.get();
    m_services->info = m_infoContext.get();
    m_services->settings = m_gameSettings.get();
    m_services->structureUtils = m_structureUtils.get();
    m_services->eventEmitter = m_eventEmitter.get();

    m_buildingListFactory = std::make_unique<cBuildingListFactory>(m_gameSettings.get());
    m_sideBarFactory = std::make_unique<cSideBarFactory>(m_buildingListFactory.get(), m_services.get());

    m_cIni = std::make_unique<cIni>(m_services.get());

    m_creatorState = std::make_unique<cCreatorState>(m_services.get(), m_cIni.get(), m_dataCampaign.get());
}

void cGame::applySettings(std::unique_ptr<InitialGameSettings> gs)
{
    m_gameSettings->m_screenW = gs->screenW;

    m_gameSettings->m_screenH = gs->screenH;

    m_cameraDragMoveSpeed = gs->cameraDragMoveSpeed;
    m_cameraBorderOrKeyMoveSpeed = gs->cameraBorderOrKeyMoveSpeed;
    m_cameraEdgeMove = gs->cameraEdgeMove;
    m_windowed = gs->windowed;

    m_gameSettings->m_allowRepeatingReinforcements = gs->allowRepeatingReinforcements;
    m_gameSettings->setTurretsDownOnLowPower(gs->turretsDownOnLowPower);
    m_gameSettings->setRocketTurretsDownOnLowPower(gs->rocketTurretsDownOnLowPower);

    m_gameSettings->m_playMusic = gs->playMusic;

    m_playSound = gs->playSound;

    m_gameSettings->m_debugMode = gs->debugMode;

    m_gameSettings->m_drawUnitDebug = gs->drawUnitDebug;
    m_pauseWhenLosingFocus = gs->pauseWhenLosingFocus;

    m_gameSettings->m_disableAI = gs->disableAI;

    m_gameSettings->m_oneAi = gs->oneAi;
    m_gameSettings->m_disableWormAi = gs->disableWormAi;

    m_gameSettings->m_disableReinforcements = gs->disableReinforcements;
    m_gameSettings->m_noAiRest = gs->noAiRest;
    m_gameSettings->m_drawUsages = gs->drawUsages;
    m_gameFilename = gs->gameFilename;

    // save settings for later use
    m_initialGameSettings = std::move(gs);
}


void cGame::init()
{
    m_infoContext->initializeDefaultInfos();

    auto map = m_gameObjectsContext->getMap();
    map->setTerrainInfo(m_infoContext->getTerrainInfo());
    m_newMusicSample = MUSIC_MENU;
    m_newMusicCountdown = 0;

    m_gameSettings->m_drawFps = false;
    m_nextState = -1;
    m_missionWasWon = false;
    m_currentState = nullptr;
    m_gameSettings->m_playing = true;
    m_gameSettings->m_skirmish = false;
    m_notificationArea->clear();

    m_musicVolume = 96; // volume is 0...

    setState(GAME_INITIALIZE);

    m_cScreenFader->inititialize();

    m_dataCampaign->housePlayer = -1;
    m_dataCampaign->mission = 0;
    m_dataCampaign->region = 1;

    m_screenShake->reset();

    m_gameSettings->m_musicType = -1;

    map->init(64, 64);

    m_players->initPlayers(false, m_gameSettings.get(), m_dataCampaign.get(), m_services.get());

    for (int i = 0; i < m_gameObjectsContext->getUnits()->size(); i++) {
        m_gameObjectsContext->getUnit(i)->init(i);
    }

    for (auto& particle : m_gameObjectsContext->getParticles()) {
        particle.init();
    }

    for (auto& bullet : m_gameObjectsContext->getBullets()) {
        bullet.init();
    }

    // Initialize InfoContext with empty objects that will be populated by cIni::installGame()
    m_infoContext->initializeDefaultInfos();

    // Units & Structures are already initialized in map.init()
    // Load properties
    m_cIni->installGame(m_gameFilename);
}

// TODO: Bad smell (duplicate code)
// initialize for missions
void cGame::missionInit()
{
    m_mapCamera->resetZoom();

    m_gameConditionChecker->missionInit();

    m_musicVolume = 96; // volume is 0...

    m_cScreenFader->inititialize();

    m_screenShake->reset();

    m_gameObjectsContext->getMap()->init(64, 64);
    // @mira: while cMap is created beforce all, need to set up terrain before loading scenario, so we can use it in cIni::installGame() when loading map.
    m_gameObjectsContext->getMap()->setTerrainInfo(m_infoContext->getTerrainInfo());

    for (int i = 0; i < m_gameObjectsContext->getUnits()->size(); i++) {
        m_gameObjectsContext->getUnit(i)->init(i);
    }

    for (auto& particle : m_gameObjectsContext->getParticles()) {
        particle.init();
    }

    for (auto& bullet : m_gameObjectsContext->getBullets()) {
        bullet.init();
    }

    m_players->initPlayers(true, m_gameSettings.get(), m_dataCampaign.get(), m_services.get());

    m_drawManager->missionInit();
}


void cGame::setMissionWon()
{
    m_missionWasWon = true;
    setState(GAME_WINNING);

    m_screenShake->reset();
    m_mouse->setTile(MOUSE_NORMAL);

    m_soundPlayer->playVoice(SOUND_VOICE_07_ATR, m_gameObjectsContext->getPlayer(HUMAN)->getHouse());

    playMusicByType(MUSIC_WIN);

    takeBackGroundScreen();
}

void cGame::setMissionLost()
{
    m_missionWasWon = false;
    setState(GAME_LOSING);

    m_screenShake->reset();
    m_mouse->setTile(MOUSE_NORMAL);

    m_soundPlayer->playVoice(SOUND_VOICE_08_ATR, m_gameObjectsContext->getPlayer(HUMAN)->getHouse());

    playMusicByType(MUSIC_LOSE);

    takeBackGroundScreen();
}


// think function belongs to combat state (tbd)
void cGame::thinkFast_audio()
{
    if (!m_gameSettings->isPlayMusic()) // no music enabled, so no need to think
        return;

    // all this does is repeating music in the same theme.
    if (m_gameSettings->m_musicType < 0)
        return;

    if (m_newMusicCountdown > 0) {
        m_newMusicCountdown--;
    }

    if (m_newMusicCountdown == 0) {
        m_soundPlayer->playMusic(m_newMusicSample);
        m_newMusicCountdown--; // so we don't keep re-starting music
    }

    if (m_newMusicCountdown < 0) {
        if (!m_soundPlayer->getMusicEnabled()) return;
        if (!m_soundPlayer->isMusicPlaying()) {
            int desiredMusicType = m_gameSettings->m_musicType;
            if (m_gameSettings->m_musicType == MUSIC_ATTACK) {
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

void cGame::initSkirmish()
{
    missionInit();
}

// void cGame::loadSkirmishMaps() const
// {
//     m_gameObjectsContext->getPreviewMaps()->loadSkirmishMaps();
// }

void cGame::shakeScreenAndBlitBuffer()
{
    m_screenShake->update(m_state, GAME_PLAYING, m_mapCamera);
    fadeOutOrBlitScreenBuffer();
}

void cGame::fadeOutOrBlitScreenBuffer() const
{
    if (m_cScreenFader->getAction() == eFadeAction::None) {
        return;
    }
}

void cGame::drawState()
{
    if (m_cScreenFader->getAction() == eFadeAction::FadeOut) {
        if (screenTexture) {
            m_renderDrawer->renderSprite(screenTexture,0,0,m_cScreenFader->getAlpha());
        }
        return;
    }

    // this makes fade-in happen after fade-out automatically
    if (m_cScreenFader->getAction()  == eFadeAction::None) {
        m_cScreenFader->startFadeIn();
    }
    if (m_cScreenFader->getAction()  == eFadeAction::FadeIn && m_cScreenFader->getAlpha()>Uint8(250)) {
       m_cScreenFader->startFadeNone();
    }

    m_currentState->draw();
    m_notificationArea->draw(m_textDrawer, 14, m_gameSettings->getScreenH() - 64);
    m_guiConsole->draw();
}

/**
	Main game loop
*/
void cGame::run()
{
    actualRenderer = m_renderDrawer->createRenderTargetTexture(m_gameSettings->m_screenW, m_gameSettings->m_screenH);
    screenTexture = m_renderDrawer->createRenderTargetTexture(m_gameSettings->m_screenW, m_gameSettings->m_screenH);
    SDL_Event event;
    while (m_gameSettings->m_playing) {
        if (m_focusManager->isGameWindowActive()) {
            m_timeManager->processTime();
        }
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    m_gameSettings->m_playing = false;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                case SDL_EVENT_WINDOW_MINIMIZED:
                    m_focusManager->onWindowFocusLost();
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                case SDL_EVENT_WINDOW_RESTORED:
                    m_focusManager->onWindowFocusGained();
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    m_Screen->onPixelSizeChanged();
                    break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                case SDL_EVENT_TEXT_INPUT:
                    m_keyboard->handleEvent(event);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                case SDL_EVENT_MOUSE_MOTION:
                case SDL_EVENT_MOUSE_WHEEL:
                    m_mouse->handleEvent(event);
                default:
                    break;
            }
        }

        if (!m_focusManager->isGameWindowActive()) {
            m_timeManager->waitForCPU(); // wait for CPU to catch up, so we don't run too fast
            continue; // skip the rest of the loop when we don't have focus
        }

        updateMouseAndKeyboardState();
        m_currentState->update();
        //Logger::print(LOG_INFO , COMP_NONE, "m_currentState", "m_currentState {}", gameStateToString(m_currentState->getType()));

        m_renderDrawer->beginDrawingToTexture(actualRenderer);
        m_renderDrawer->renderClearToColor();
        drawState(); // run game state, includes interaction + drawing
        transitionStateIfRequired();
        shakeScreenAndBlitBuffer();

        m_renderDrawer->endDrawingToTexture();
        m_renderDrawer->renderClearToColor();  // SDL3 persists its intermediate texture between frames; clear before composite
        m_renderDrawer->renderSprite(actualRenderer, m_screenShake->getX(), m_screenShake->getY());
        SDL_RenderPresent(renderer);
        m_timeManager->waitForCPU(); // wait for CPU to catch up, so we don't run too fast
    }
}

void cGame::shakeScreen(int duration)
{
    m_screenShake->shake(duration);
}

/**
	Shutdown the game
*/
void cGame::shutdown()
{
    m_gameObjectsContext->getParticles().reset();
    Logger::info(COMP_NONE, "cGame::shutdown", "=== SHUTDOWN ===");

    m_currentState = nullptr;
    m_creatorState->destroyAllStates();

    delete m_mapViewport;

    delete m_drawManager;

    delete m_mapCamera;

    m_players->destroyAllegroBitmaps();

    delete m_mouse;
    delete m_keyboard;

    // Release all Graphics/Texture objects now, while the renderer is still valid.
    // ~cSDLSystem() calls SDL_Quit() later (when cGame itself is destroyed),
    // but some Graphics shared_ptrs may still be alive at that point and their
    // ~Graphics() would call SDL_DestroyTexture with an already-dead renderer.
    ctx.reset();
    context.reset();

    Logger::info(COMP_NONE, "cGame::shutdown", "Allegro FONT library shut down.");
}


/**
	Setup the game

	Should not be called twice.
*/
bool cGame::setupGame()
{
    Logger::g_loggerInstance->setDebug(m_gameSettings->m_debugMode);
    Logger::info(COMP_NONE, "cGame::setupGame", "=== Dune II - The Maker ===");
    Logger::info(COMP_VERSION, "cGame::setupGame", "=== Version information ===");
    Logger::info(COMP_VERSION, "Initializing", "Version {}, Compiled at {} , {}", D2TM_VERSION, __DATE__, __TIME__);

    // SETTINGS.INI
    std::shared_ptr<cIniFile> settings = std::make_shared<cIniFile>("settings.ini");
    std::shared_ptr<cIniFile> gamesCfg = std::make_shared<cIniFile>(m_gameFilename);

    m_reinforcements = std::make_unique<cReinforcements>();
    m_reinforcements->serviceInit(m_services.get());
    m_gameObjectsContext->getMap()->setReinforcements(m_reinforcements.get());

    init(); // Must be first! (loads ini file at the end, which is required before going on...)

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
        m_transfertMap[eGameDirFileName::GFXEDITOR] = settings->getStringValue("DATAFILE", "GFXEDITOR");
        settingsValidator->addResources(std::move(m_transfertMap));
    }

    if (!settingsValidator->fileExists()) {
        Logger::info(COMP_INIT, "Loading settings.ini", "Validation of files within settings.ini failed");
        std::cerr << "One or more validations failed with resources defined in settings.ini" << std::endl;
        return false;
    }

    // INI
    const auto title = std::format("Dune II - The Maker [{}] - (by Stefan Hendriks)", D2TM_VERSION);

    m_keyboard = new cKeyboard();
    Logger::info(COMP_INIT, "Initializing Keyboard", "install_keyboard()");

    if (settings->hasSection("KEYS")) {
        const cSection keysSection = settings->getSection("KEYS");
        m_keyboard->loadKeyBindings(&keysSection);
    } else {
        m_keyboard->loadKeyBindings();
    }
    Logger::info(COMP_INIT, "Initializing Key Bindings", "Loaded from [KEYS] section");

    m_Screen = std::make_unique<cSDLSystem>(m_gameSettings->m_screenW, m_gameSettings->m_screenH, title);
    if (!m_windowed) {
        m_Screen->setFullScreenMode();
    }

    m_gameSettings->m_screenW = m_Screen->Width();
    m_gameSettings->m_screenH = m_Screen->Height();
    window = m_Screen->getWindows();
    renderer = m_Screen->getRenderer();

    // create ressources from scratch
    context = std::make_unique<ContextCreator>(renderer, settingsValidator.get());
    // share Graphics to all class what use ctx !
    ctx->setGraphicsContext(context->createGraphicsContext());
    // creation SDLDrawer and send it to GameContext, so it can be used by all classes that have access to GameContext
    std::unique_ptr<SDLDrawer> renderDrawer = std::make_unique<SDLDrawer>(renderer);
    m_renderDrawer = renderDrawer.get();
    ctx->setSDLDrawer(std::move(renderDrawer));
    // share Text to all class what use ctx !
    ctx->setTextContext(context->createTextContext(m_gameSettings.get(), m_renderDrawer));
    //m_gameObjectsContext->getMap()->setGameContext(ctx.get());

    m_textDrawer = ctx->getTextContext()->getGameTextDrawer();

    std::unique_ptr<cSoundPlayer> soundPlayer = std::make_unique<cSoundPlayer>(settingsValidator->getFullName(eGameDirFileName::GFXAUDIO));
    m_soundPlayer = soundPlayer.get();
    ctx->setSoundPlayer(std::move(soundPlayer));
    if (!m_playSound) {
        m_soundPlayer->setSoundEnabled(false);
    }
    if (!m_gameSettings->m_playMusic) {
        m_soundPlayer->setMusicEnabled(false);
    }

    m_notificationArea->setDrawer(m_renderDrawer);
    m_guiConsole = std::make_unique<GuiConsole>(
        m_renderDrawer,
        m_textDrawer,
        m_notificationArea.get(),
        m_gameSettings->getScreenW(),
        m_gameSettings->getScreenH());

    auto previewMaps = m_gameObjectsContext->getPreviewMaps();
    previewMaps->setRenderDrawer(m_renderDrawer); // inject render drawer into cPreviewMaps that is part of game objects context, so it can be used to create textures for map previews
    previewMaps->loadSkirmishMaps(); // load skirmish maps, so they are ready when we need them in the skirmish setup state

    // do it here, because it depends on fonts to be loaded
    m_mouse = new cMouse(ctx.get());
    m_mouse->setSettings(m_gameSettings.get());

    /***
     * Viewport(s)
     */
    m_mapViewport = new cRectangle(0, cSideBar::TopBarHeight, m_gameSettings->getScreenW() - cSideBar::SidebarWidth, m_gameSettings->getScreenH() - cSideBar::TopBarHeight);

    Logger::info(COMP_INIT, "cGame::setupGame", "Color conversion method set");

    Logger::info(COMP_INIT, "cGame::setupGame", "MOUSE: Mouse speed set");

    Logger::info(COMP_INIT, "cGame::setupGame", "=== GAME ===");

    // randomize timer
    auto t = static_cast<unsigned int>(time(nullptr));
    Logger::info(COMP_INIT, "cGame::setupGame", "Seed is {}", t);
    srand(t);

    m_gameSettings->m_playing = true;
    m_state = GAME_INITIALIZE;

    Logger::info(COMP_INIT, "cGame::setupGame", "Setup:  HOUSES");
    m_Houses = std::make_unique<cHousesInfo>();
    m_Houses->installHouses(gamesCfg);
    // A few messages for the player
    Logger::info(COMP_INIT, "cGame::setupGame", "Initializing:  PLAYERS");
    m_players->setupPlayers(m_Houses.get());
    cInfoContextCreator infoCreator(ctx->getGraphicsContext());
    infoCreator.installInfos(*m_infoContext);

    if (m_mapCamera != nullptr)
        delete m_mapCamera;

    m_mapCamera = new cMapCamera(m_gameObjectsContext->getMap(), m_cameraDragMoveSpeed, m_cameraBorderOrKeyMoveSpeed, m_cameraEdgeMove, m_gameSettings.get(), m_mouse);
    m_services->mapCamera = m_mapCamera;
    m_structureUtils->serviceInit(m_services.get());

    m_cIni->installGame(m_gameFilename);
    // Now we are ready for the menu state
    setState(GAME_MENU);

    m_focusManager->setEnabled(m_pauseWhenLosingFocus);

    // do install_upgrades after init, because init loads the INI file and then has the very latest
    // unit/structures catalog loaded - which the install_upgrades depends on.
    m_infoContext->setUpgradeInfos(infoCreator.createUpgradeInfos(m_infoContext.get(), m_gameSettings.get()));
    cPlayer *humanPlayer = m_gameObjectsContext->getPlayer(HUMAN);

    m_drawManager = new cDrawManager(ctx.get(), humanPlayer, m_services.get());
    m_services->drawManager = m_drawManager;
    m_services->mapViewport = m_mapViewport;

    setupPlayers();

    playMusicByTypeForStateTransition(MUSIC_MENU);

    m_mouse->setMouseObserver(this);
    m_keyboard->setKeyboardObserver(this);

    // I need m_renderDrawer to create cPreviewMaps
    // m_PreviewMaps = std::make_shared<cPreviewMaps>();

    // m_gameObjectsContext->getMap()->setGameContext(ctx.get());
    // Injection of services
    m_gameObjectsContext->serviceInit(m_services.get());

    // all has installed well. Let's rock and roll.
    SDL_HideCursor();
    return true;
}

/**
 * Set up players
 * (Elegible for combat state object initialization)
 */
void cGame::setupPlayers()
{
    m_players->setupRuntimePlayerComponents(m_sideBarFactory.get(), m_mouse, m_dataCampaign->mission, m_services.get());
    setPlayerToInteractFor(m_gameObjectsContext->getPlayer(0));
}

bool cGame::isState(int thisState) const
{
    return (m_state == thisState);
}

void cGame::jumpToSelectYourNextConquestMission(int missionNr)
{
    cGameState *state = m_creatorState->getOrCreateState(eGameState::REGION, true);
    auto *pState = dynamic_cast<cSelectYourNextConquestState *>(state);
    if (!pState) {
        Logger::error(COMP_GAME, "cGame::jumpToSelectYourNextConquestMission", "Failed to create cSelectYourNextConquestState for mission {}. Aborting.", missionNr);
        return;
    }

    pState->calculateOffset();
    pState->installWorld();

    cPlayer *humanPlayer = m_gameObjectsContext->getPlayer(HUMAN);
    int missionZeroBased = missionNr - 1;
    m_dataCampaign->mission = missionZeroBased;

    // a 'missionX.ini' file is from 1 til (including) 8
    // to play mission 2 (passed as missionNr param), we have to load up mission1.ini
    // meaning we have to use the 'zero based' value here
    pState->fastForwardUntilMission(missionZeroBased, humanPlayer->getHouse());
    pState->regionSetupNextMission(missionZeroBased, humanPlayer->getHouse());
}

void cGame::setState(int newState)
{
    if (newState == m_state) {
        // ignore
        return;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    Logger::info(COMP_GAME, "cGame::setState", "Setting state from {}(={}) to {}(={})", m_state, stateToString(m_state), newState, stateToString(newState));
#pragma GCC diagnostic pop

    if (newState > -1) {
        bool deleteOldState = (newState != GAME_REGION &&
                               newState != GAME_PLAYING &&
                               newState != GAME_EDITOR &&
                               newState != GAME_OPTIONS); // don't delete these states, but re-use!

        auto mapToCreatorState = [](int state, eGameState &mapped) -> bool {
            switch (state) {
                case GAME_MENU: mapped = eGameState::MENU; return true;
                case GAME_PLAYING: mapped = eGameState::PLAYING; return true;
                case GAME_BRIEFING: mapped = eGameState::BRIEFING; return true;
                case GAME_EDITOR: mapped = eGameState::EDITOR; return true;
                case GAME_OPTIONS: mapped = eGameState::OPTIONS; return true;
                case GAME_REGION: mapped = eGameState::REGION; return true;
                case GAME_SELECT_HOUSE: mapped = eGameState::SELECT_HOUSE; return true;
                case GAME_TELLHOUSE: mapped = eGameState::TELLHOUSE; return true;
                case GAME_WINNING: mapped = eGameState::WINNING; return true;
                case GAME_WINBRIEF: mapped = eGameState::WINBRIEF; return true;
                case GAME_LOSEBRIEF: mapped = eGameState::LOSEBRIEF; return true;
                case GAME_LOSING: mapped = eGameState::LOSING; return true;
                case GAME_SETUPSKIRMISH: mapped = eGameState::SETUPSKIRMISH; return true;
                case GAME_CREDITS: mapped = eGameState::CREDITS; return true;
                case GAME_MISSIONSELECT: mapped = eGameState::MISSIONSELECT; return true;
                case GAME_NEW_MAP_EDITOR: mapped = eGameState::NEW_MAP_EDITOR; return true;
                default:
                    return false;
            }
        };

        if (newState == GAME_PLAYING) {
            // make sure to delete options menu now
            cGameState *optionsState = nullptr;
            if (m_creatorState->hasState(eGameState::OPTIONS)) {
                optionsState = m_creatorState->getState(eGameState::OPTIONS);
            }

            if (m_currentState == optionsState) {
                m_currentState = nullptr;
            }
            m_creatorState->destroyState(eGameState::OPTIONS);
        }

        if (newState == GAME_REGION) {
            // make sure to delete options menu now
            cGameState *optionsState = nullptr;
            if (m_creatorState->hasState(eGameState::OPTIONS)) {
                optionsState = m_creatorState->getState(eGameState::OPTIONS);
            }

            if (m_currentState == optionsState) {
                m_currentState = nullptr;
            }
            m_creatorState->destroyState(eGameState::OPTIONS);
        }

        if (newState == GAME_MISSIONSELECT) {
            deleteOldState = true; // delete old missionselect state everytime
        }

        cPlayer *humanPlayer = m_gameObjectsContext->getPlayer(HUMAN);

        cGameState *existingStatePtr = nullptr;
        eGameState targetCreatorState = eGameState::MENU;
        if (mapToCreatorState(newState, targetCreatorState)) {
            if (deleteOldState) {
                m_creatorState->destroyState(targetCreatorState);
            }
            if (m_creatorState->hasState(targetCreatorState)) {
                existingStatePtr = m_creatorState->getState(targetCreatorState);
            }
        }

        if (existingStatePtr) {
            // no need for re-creating state
            if (newState == GAME_REGION) {
                playMusicByTypeForStateTransition(MUSIC_CONQUEST);

                // came from a win/lose brief state, so make sure to set up the next state
                if (m_state == GAME_WINBRIEF || m_state == GAME_LOSEBRIEF) {
                    // because `GAME_REGION` == if (existingStatePtr->getType() == GAMESTATE_SELECT_YOUR_NEXT_CONQUEST ||
                    auto *pState = dynamic_cast<cSelectYourNextConquestState *>(existingStatePtr);

                    if (m_dataCampaign->mission > 1) {
                        pState->conquerRegions();
                    }

                    if (m_missionWasWon) {
                        // we won
                        pState->regionSetupNextMission(m_dataCampaign->mission, humanPlayer->getHouse());
                    }
                    else {
                        // OR: did not win
                        pState->regionSetupLostMission();
                    }
                }
            }
            else if (newState == GAME_OPTIONS) {
                // This feels awkward. For now, I'll keep it (if it works), but this will change
                // once we have a proper game playing state and we need to transition properly between states
                takeBackGroundScreen();
                auto *pState = dynamic_cast<cOptionsState *>(existingStatePtr);
                // you cannot 'go back' to mission select
                if (m_state != GAME_MISSIONSELECT) {
                    pState->setPrevState(m_state);
                }
                else {
                    pState->refresh(); // rebuilds UI windows, but keeps background
                }
            }
            else if (newState == GAME_PLAYING) {
                    m_currentState = existingStatePtr;

                    m_drawManager->reset();
                    m_drawManager->missionInit();
                    // evaluate all players, so we have initial 'alive' values set properly
                    m_players->evaluateStillAliveForAI();
                    m_gameObjectsContext->getParticles().reset();
                    // in-between solution until we have a proper combat state object
                    m_drawManager->init();

                    // handle update
                    s_GameEvent event {
                        .eventType = eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN,
                    };
                    // the game is about to begin!
                    onNotifyGameEvent(event);
                    m_timeManager->startTimer();
            }

            m_currentState = existingStatePtr;
        }
        else {
            cGameState *newStatePtr = nullptr;

            if (newState == GAME_REGION) {
                auto *pState = dynamic_cast<cSelectYourNextConquestState *>(m_creatorState->getState(eGameState::REGION));
                pState->calculateOffset();
                Logger::info(COMP_INIT, "cGame::loadScenario", "Setup:  WORLD");
                pState->installWorld();
                if (m_dataCampaign->mission > 1) {
                    pState->conquerRegions();
                }
                // first creation
                pState->regionSetupNextMission(m_dataCampaign->mission, humanPlayer->getHouse());
                playMusicByTypeForStateTransition(MUSIC_CONQUEST);

                newStatePtr = pState;
            }
            else if (newState == GAME_SETUPSKIRMISH) {
                m_players->initPlayers(false, m_gameSettings.get(), m_dataCampaign.get(), m_services.get());
                newStatePtr = m_creatorState->getState(eGameState::SETUPSKIRMISH);
                playMusicByTypeForStateTransition(MUSIC_MENU);
            }
            else if (newState == GAME_CREDITS) {
                newStatePtr = m_creatorState->getState(eGameState::CREDITS);
            }
            else if (newState == GAME_EDITOR) {
                newStatePtr = m_creatorState->getState(eGameState::EDITOR);
            }
            else if (newState == GAME_MENU) {
                m_gameSettings->m_cheatMode = false;
                newStatePtr = m_creatorState->getState(eGameState::MENU);
                playMusicByTypeForStateTransition(MUSIC_MENU);
            }
            else if (newState == GAME_NEW_MAP_EDITOR) {
                newStatePtr = m_creatorState->getState(eGameState::NEW_MAP_EDITOR);
            }
            else if (newState == GAME_SELECT_HOUSE) {
                newStatePtr = m_creatorState->getState(eGameState::SELECT_HOUSE);
            }
            else if (newState == GAME_MISSIONSELECT) {
                m_mouse->setTile(MOUSE_NORMAL);
                newStatePtr = m_creatorState->getState(eGameState::MISSIONSELECT);
            }
            else if (newState == GAME_OPTIONS) {
                takeBackGroundScreen();
                m_mouse->setTile(MOUSE_NORMAL);
                if (m_state == GAME_PLAYING) {
                    // so we don't draw mouse cursor
                    m_drawManager->drawCombatState();
                    m_timeManager->pauseTimer();
                }
                else {
                    // we fall back what was on screen, (which includes mouse cursor for now)
                }
                newStatePtr = m_creatorState->getState(eGameState::OPTIONS);
            }
            else if (newState == GAME_PLAYING) {
                if (m_state == GAME_OPTIONS) {
                    m_timeManager->restartTimer();
                    takeBackGroundScreen();
                    // we came from options menu, notify mouse
                    humanPlayer->getGameControlsContext()->onFocusMouseStateEvent();
                }
                else {
                    m_dataCampaign->housePlayer = humanPlayer->getHouse();
                    newStatePtr = m_creatorState->getState(eGameState::PLAYING);
                    m_currentState = newStatePtr;

                    m_drawManager->reset();
                    // evaluate all players, so we have initial 'alive' values set properly
                    m_players->evaluateStillAliveForAI();
                    m_gameObjectsContext->getParticles().reset();
                    // in-between solution until we have a proper combat state object
                    m_drawManager->init();

                    // handle update
                    s_GameEvent event {
                        .eventType = eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN,
                    };
                    // the game is about to begin!
                    onNotifyGameEvent(event);
                    m_timeManager->startTimer();
                }
            }
            else if (newState == GAME_LOSING) {
                newStatePtr = m_creatorState->getState(eGameState::LOSING);
            }
            else if (newState == GAME_WINNING) {
                newStatePtr = m_creatorState->getState(eGameState::WINNING);
            }
            else if (newState == GAME_TELLHOUSE) {
                m_dataCampaign->housePlayer = m_gameObjectsContext->getPlayer(HUMAN)->getHouse();
                newStatePtr = m_creatorState->getState(eGameState::TELLHOUSE);
                playMusicByTypeForStateTransition(MUSIC_BRIEFING);
            }
            else if (newState == GAME_BRIEFING) {
                m_gameSettings->m_cheatMode = false;
                newStatePtr = m_creatorState->getState(eGameState::BRIEFING);
                playMusicByTypeForStateTransition(MUSIC_BRIEFING);
            } else if (newState == GAME_WINBRIEF) {
                newStatePtr = m_creatorState->getState(eGameState::WINBRIEF);
                playMusicByTypeForStateTransition(MUSIC_BRIEFING);
            } else if (newState == GAME_LOSEBRIEF) {
                newStatePtr = m_creatorState->getState(eGameState::LOSEBRIEF);
                playMusicByTypeForStateTransition(MUSIC_BRIEFING);
            }

            m_currentState = newStatePtr;
        }
    }
    m_state = newState;
}

void cGame::thinkFast_fading()
{
    m_cScreenFader->update();
}

cGame::~cGame()
{
}

void cGame::prepareMentatForPlayer()
{
    if (m_state == GAME_BRIEFING) {
        missionInit();
        setupPlayers();
        auto *pState = dynamic_cast<cMentatState *>(m_creatorState->getState(eGameState::BRIEFING));
        pState->prepareMentat(m_dataCampaign->housePlayer);
    }
    else if (m_state == GAME_WINBRIEF) {
        auto *pState = dynamic_cast<cMentatState *>(m_creatorState->getState(eGameState::WINBRIEF));
        pState->prepareMentat(m_dataCampaign->housePlayer);
    }
    else if (m_state == GAME_LOSEBRIEF) {
        auto *pState = dynamic_cast<cMentatState *>(m_creatorState->getState(eGameState::LOSEBRIEF));
        pState->prepareMentat(m_dataCampaign->housePlayer);
    }
}

void cGame::prepareMentatToTellAboutHouse(int house)
{
    m_gameObjectsContext->getPlayer(HUMAN)->setHouse(house);
    m_dataCampaign->housePlayer = house;
    if (!m_creatorState->hasState(eGameState::TELLHOUSE)) {
        m_creatorState->getState(eGameState::TELLHOUSE);
        playMusicByTypeForStateTransition(MUSIC_BRIEFING);
    } else {
        Logger::warn(COMP_GAME, "cGame::prepareMentatToTellAboutHouse", "cMentatState for TELLHOUSE already exists. Not creating a new one.");
    }
}

void cGame::loadScenario()
{
    if (!m_creatorState->hasState(eGameState::BRIEFING)) {
        Logger::error(COMP_GAME, "cGame::loadScenario", "Failed to load scenario because cMentatState is not yet created. Aborting.");
        return;
    }

    auto *pState = dynamic_cast<cMentatState *>(m_creatorState->getState(eGameState::BRIEFING));
    if (!pState) return; // state not yet created; prepareMentat() in the constructor will load the scenario
    pState->loadScenario(m_reinforcements.get());
}

void cGame::goingToWinLoseBrief(int value)
{
    setState(value);
    prepareMentatForPlayer();

}

void cGame::changeStateFromMentat()
{
    if (isState(GAME_BRIEFING)) {
        // proceed, play mission (it is already loaded before we got here)
        setNextStateToTransitionTo(GAME_PLAYING);
        m_drawManager->missionInit();

        // CENTER MOUSE
        setMousePosition(m_gameSettings->getScreenW() / 2, m_gameSettings->getScreenH() / 2);

        initiateFadingOut();

        playMusicByType(MUSIC_PEACE);
        return;
    }

    if (m_gameSettings->isSkirmish()) {
        if (isState(GAME_WINBRIEF) || isState(GAME_LOSEBRIEF)) {
            // regardless of drawStateWinning or drawStateLosing, always go back to main menu
            setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
            initSkirmish();
            initiateFadingOut();
        }
        else {
            Logger::warn(COMP_GAME, "cProceedButtonCommand", "pressed in skirmish mode but state is not WINBRIEF nor LOSEBRIEF");
        }
        return;
    }

    // NOT a skirmish game
    // won mission, transition to region selection (Select your next Conquest)
    if (isState(GAME_WINBRIEF)) {
        setNextStateToTransitionTo(GAME_REGION);

        initiateFadingOut();
        return;
    }

    // lost mission
    if (isState(GAME_LOSEBRIEF)) {
        missionInit();
        // lost mission > 1, so we go back to region select
        if (m_dataCampaign->mission > 1)   {
            setNextStateToTransitionTo(GAME_REGION);

            m_dataCampaign->mission--; // we did not win
        }
        else {
            // mission 1 failed, really?..., back to mentat with briefing
            setNextStateToTransitionTo(GAME_BRIEFING);
            prepareMentatForPlayer();
            playMusicByType(MUSIC_BRIEFING);
        }

        initiateFadingOut();
        return;
    }
}

void cGame::thinkFast()
{
    thinkFast_fading();
    thinkFast_audio();
    m_notificationArea->thinkFast();
    if (m_currentState) {
        m_currentState->thinkFast();
    }
    if (m_drawManager) {
        m_drawManager->thinkFast();
    }
}

void cGame::setPlayerToInteractFor(cPlayer *pPlayer)
{
    m_controlledPlayer = pPlayer;
}

void cGame::emitGameEvent(const s_GameEvent &event)
{
    if (m_eventEmitter) {
        m_eventEmitter->emit(event);
    }
}

void cGame::dispatchGameEvent(const s_GameEvent &event)
{
    m_gameObjectsContext->getMap()->onNotifyGameEvent(event);

    if (m_currentState) {
        m_currentState->onNotifyGameEvent(event);
    }

    if (event.eventType == eGameEventType::GAME_EVENT_NOTIFICATION) {
        if (const auto *notifEvent = std::get_if<NotificationEvent>(&event.data)) {
            if (notifEvent->player == nullptr || notifEvent->player == m_controlledPlayer) {
                m_notificationArea->addNotification(notifEvent->message, notifEvent->type);
            }
        }
    }

    m_gameObjectsContext->getPlayers()->onNotifyGameEvent(event);
}

void cGame::onNotifyGameEvent(const s_GameEvent &event)
{
    emitGameEvent(event);
}

void cGame::reduceShaking() const {
    m_screenShake->reduce();
}


Color cGame::getColorFadeSelectedLimited(Color color, float minFade)
{
    return m_cScreenFader->getColorFadeSelectedLimited(color, minFade);
}

Color cGame::getColorFadeSelected(int r, int g, int b, bool rFlag, bool gFlag, bool bFlag)
{
    return m_cScreenFader->getColorFadeSelected(r,g,b,rFlag,gFlag,bFlag);
}

void cGame::setWinFlags(int value)
{
    m_gameConditionChecker->setWinFlags(value);
}

void cGame::setLoseFlags(int value)
{
    m_gameConditionChecker->setLoseFlags(value);
}

void cGame::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_drawManager) {
        // keep mouse drawer updates before state handling to preserve legacy ordering
        m_drawManager->getMouseDrawer()->onNotify(event);
    }

    if (m_guiConsole->isVisible()) {
        m_guiConsole->onNotifyMouseEvent(event);
    }

    // pass through any classes that are interested
    if (m_currentState && m_cScreenFader->getAction() != eFadeAction::FadeOut) {
        m_currentState->onNotifyMouseEvent(event);
    }

    syncTextInputState();
}

void cGame::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (isConsoleToggleKey(event)) {
        m_guiConsole->toggle();
        syncTextInputState();
        return;
    }

    if (m_guiConsole->isVisible()) {
        if (m_guiConsole->isKeyboardCaptured() && isConsoleToggleTextInput(event)) {
            return;
        }

        m_guiConsole->onNotifyKeyboardEvent(event);
        if (m_guiConsole->isKeyboardCaptured()) {
            syncTextInputState();
            return;
        }
    }

    // pass through any classes that are interested
    if (m_currentState) {
        m_currentState->onNotifyKeyboardEvent(event);

        if (m_currentState->isKeyboardCapturedByUi()) {
            syncTextInputState();
            return;
        }
    }

    syncTextInputState();

    switch (event.getType()) {
        case eKeyEventType::HOLD:
            onKeyDownGame(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressedGame(event);
            break;
        default:
            break;
    }
}

void cGame::transitionStateIfRequired()
{
    if (m_nextState > -1) {
        setState(m_nextState);

        if (m_nextState == GAME_BRIEFING) {
            playMusicByType(MUSIC_BRIEFING);
            prepareMentatForPlayer();
        }

        m_nextState = -1;
    }
}

void cGame::setNextStateToTransitionTo(int newState)
{
    m_nextState = newState;
}

void cGame::saveBmpScreenToDisk()
{
    // Screenshot key fires during updateMouseAndKeyboardState(), before
    // beginDrawingToTexture(actualRenderer). The default render target is
    // SDL3's logical presentation texture which has been cleared to black.
    // Read from actualRenderer directly — it holds the last complete frame.
    SDL_SetRenderTarget(renderer, actualRenderer->tex.get());
    bool saved = cScreenShotSaver::saveScreen(renderer, m_gameSettings->m_screenW, m_gameSettings->m_screenH);
    SDL_SetRenderTarget(renderer, nullptr);
    if (saved) {
        m_notificationArea->addNotification("Screenshot saved.", eNotificationType::NEUTRAL);
    }
}

void cGame::addNotification(const std::string &message, eNotificationType type)
{
    m_notificationArea->addNotification(message, type);
}

void cGame::onKeyDownGame(const cKeyboardEvent &)
{
    
}

void cGame::onKeyPressedGame(const cKeyboardEvent &event)
{
    if (event.isAction(eKeyAction::SCREENSHOT)) {
        saveBmpScreenToDisk();
    }

    if (event.isAction(eKeyAction::TOGGLE_MUSIC)) {
        m_gameSettings->m_playMusic = !m_gameSettings->m_playMusic;
        if (!m_gameSettings->isPlayMusic()) {
            m_soundPlayer->stopMusic();
        }
        else {
            m_soundPlayer->playMusic(m_newMusicSample);
        }
        m_notificationArea->addNotification("Music toggle successfully", eNotificationType::NEUTRAL);
    }

    if (event.isAction(eKeyAction::MUSIC_VOLUME_DOWN)) {
        m_soundPlayer->changeMusicVolume(-10);
        m_notificationArea->addNotification("Music volume down", eNotificationType::NEUTRAL);
    }

    if (event.isAction(eKeyAction::MUSIC_VOLUME_UP)) {
        m_soundPlayer->changeMusicVolume(10);
        m_notificationArea->addNotification("Music volume up", eNotificationType::NEUTRAL);
    }

    if (event.isAction(eKeyAction::GAME_SPEED_UP)) {
        auto timerManager = ctx->getTimeManager();
        timerManager->setGlobalSpeedVariation(-1);
        m_notificationArea->addNotification("Game speed up", eNotificationType::NEUTRAL);
    }

    if (event.isAction(eKeyAction::GAME_SPEED_DOWN)) {
        auto timerManager = ctx->getTimeManager();
        timerManager->setGlobalSpeedVariation(1);
        m_notificationArea->addNotification("Game speed down", eNotificationType::NEUTRAL);
    }

    if (event.isAction(eKeyAction::TOGGLE_FULLSCREEN)) {
        if (m_windowed) {
            m_Screen->setFullScreenMode();
            m_windowed = false;
        } else {
            m_Screen->setWindowMode();
            m_windowed = true;
        }
    }

    if (event.isAction(eKeyAction::TOGGLE_CHEAT)) {
        m_gameSettings->m_cheatMode = !m_gameSettings->m_cheatMode;
        auto message = std::format("Cheat mode {}", m_gameSettings->m_cheatMode ? "enabled" : "disabled");
        Logger::info(COMP_CHEATS, "Cheat mode", "{}", message);
        m_notificationArea->addNotification(message, eNotificationType::OTHER);
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
    float maxDistance = m_mapCamera->divideByZoomLevel(m_gameObjectsContext->getMapGeometry()->getMaxDistanceInPixels() / 2);
    float distanceNormalized = 1.0 - (iDistance / maxDistance);

    float volume = m_soundPlayer->getMaxVolume() * distanceNormalized;

    // zoom factor influences volume (more zoomed in means louder)
    float volumeFactor = m_mapCamera->factorZoomLevel(0.7f);
    int iVolFactored = volumeFactor * volume;

    Logger::debug(COMP_SOUND, "cGame::playSoundWithDistance", "iDistance [{}], distanceNormalized [{}] maxDistance [{}], m_zoomLevel [{}], volumeFactor [{}], volume [{}], iVolFactored [{}]",
        iDistance, distanceNormalized, maxDistance, m_mapCamera->getZoomLevel(), volumeFactor, volume, iVolFactored);

    playSound(sampleId, iVolFactored);
}


void cGame::playVoice(int sampleId, int playerId)
{
    m_soundPlayer->playVoice(sampleId, m_gameObjectsContext->getPlayer(playerId)->getHouse());
}

void cGame::playMusicByTypeForStateTransition(int iType)
{
    if (m_gameSettings->m_musicType != iType) {
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

    Logger::info(COMP_SOUND, "cGame::playMusicByType", "iType = {}. playerId = {}, triggerWithVoice = {}", iType, playerId, triggerWithVoice);

    if (triggerWithVoice) {
        if (iType == m_gameSettings->m_musicType) {
            Logger::info(COMP_SOUND, "cGame::playMusicByType", "m_musicType = {}, iType is {}, so bailing", m_gameSettings->m_musicType, iType);
            return false;
        }
    }

    m_gameSettings->m_musicType = iType;
    Logger::info(COMP_SOUND, "cGame::playMusicByType", "m_musicType = {}", m_gameSettings->m_musicType);

    if (!m_gameSettings->m_playMusic) {
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
        int houseIndex = m_gameObjectsContext->getPlayer(HUMAN)->getHouse();
        if (houseIndex == ATREIDES) {
            sampleId = MIDI_MENTAT_ATR;
        }
        else if (houseIndex == HARKONNEN) {
            sampleId = MIDI_MENTAT_HAR;
        }
        else if (houseIndex == ORDOS) {
            sampleId = MIDI_MENTAT_ORD;
        }
        else if (houseIndex == SARDAUKAR) {
            sampleId = MIDI_MENTAT_HAR; // no @SARDAUKAR srd music, so use harkonnen one
        }
        else {
            d2tm_assert(false && "Undefined house.");
        }
    }
    else {
        d2tm_assert(false && "Undefined music type.");
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

void cGame::thinkNormal()
{
    if (m_currentState) {
        m_currentState->thinkNormal();
    }
}

void cGame::thinkSlow()
{
    if (m_currentState) {
        m_currentState->thinkSlow();
    }
    m_timeManager->capFps();
    m_timeManager->adaptWaitingTime();
}

void cGame::thinkCache()
{
    ctx->resetCache();
}

void cGame::onKeyDownDebugMode(const cKeyboardEvent &event)
{
    const cPlayer *humanPlayer = m_gameObjectsContext->getPlayer(HUMAN);
    if (event.isAction(eKeyAction::DEBUG_WIN)) {
        setMissionWon();
    }

    if (event.isAction(eKeyAction::DEBUG_LOSE)) {
        setMissionLost();
    }

    if (event.isAction(eKeyAction::DEBUG_GIVE_CREDITS)) {
        for (int i = 0; i < AI_WORM; i++) {
            m_gameObjectsContext->getPlayer(i)->setCredits(5000);
        }
    }

    if (event.isAction(eKeyAction::DEBUG_DESTROY_AT_CURSOR)) {
        int mc = humanPlayer->getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = m_gameObjectsContext->getMap()->getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                m_gameObjectsContext->getUnit(idOfUnitAtCell)->die(true, false);
            }

            int idOfStructureAtCell = m_gameObjectsContext->getMap()->getCellIdStructuresLayer(mc);
            if (idOfStructureAtCell > -1) {
                m_gameObjectsContext->getStructures()[idOfStructureAtCell]->die();
            }

            idOfUnitAtCell = m_gameObjectsContext->getMap()->getCellIdWormsLayer(mc);
            if (idOfUnitAtCell > -1) {
                m_gameObjectsContext->getUnit(idOfUnitAtCell)->die(false, false);
            }

            idOfUnitAtCell = m_gameObjectsContext->getMap()->getCellIdAirUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                m_gameObjectsContext->getUnit(idOfUnitAtCell)->die(false, false);
            }
        }
    }

    if (event.isAction(eKeyAction::DEBUG_DAMAGE_AT_CURSOR)) {
        int mc = humanPlayer->getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = m_gameObjectsContext->getMap()->getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                cUnit *pUnit = m_gameObjectsContext->getUnit(idOfUnitAtCell);
                int damageToTake = pUnit->getHitPoints() - 25;
                if (damageToTake > 0) {
                    pUnit->takeDamage(damageToTake, -1, -1);
                }
            }
        }
    } else if (event.isAction(eKeyAction::DEBUG_REVEAL_MAP)) {
        for (int i = 0; i < AI_WORM; i++) {
            m_gameObjectsContext->getMap()->clear_all(i);
        }
    }

    if (event.isAction(eKeyAction::DEBUG_KILL_CARRYALLS)) {
        const std::vector<int> &myUnitsForType = humanPlayer->getAllMyUnitsForType(CARRYALL);
        for (auto &unitId : myUnitsForType) {
            cUnit *pUnit = m_gameObjectsContext->getUnit(unitId);
            pUnit->die(true, false);
        }
    }
}

void cGame::setMousePosition(int w, int h)
{
    m_mouse->setCursorPosition(window, w,h);
}

// Fading between menu items
void cGame::initiateFadingOut()
{
    m_cScreenFader->startFadeOut();

    m_renderDrawer->beginDrawingToTexture(screenTexture);
    SDL_RenderTexture(renderer, actualRenderer->tex.get(), nullptr, nullptr);
    m_renderDrawer->endDrawingToTexture();
}

void cGame::takeBackGroundScreen()
{
    m_renderDrawer->beginDrawingToTexture(screenTexture);
    SDL_RenderTexture(renderer, actualRenderer->tex.get(), nullptr, nullptr);
    m_renderDrawer->endDrawingToTexture();
}

s_DataCampaign* cGame::getDataCampaign() const
{
    return m_dataCampaign.get();
}

int cGame::getCurrentState() const
{
    return m_state;
}

void cGame::drawTextFps() const
{
    m_textDrawer->drawText(180,8, Color::Black, std::format("FPS/REST: {}/{}", m_timeManager->getFps(), m_timeManager->getWaitingTime()), false);
}

void cGame::drawTextTime() const
{
    auto time = m_timeManager->getCurrentTime();
    m_textDrawer->drawText(m_gameSettings->getScreenW()- cSideBar::SidebarWidth-75, cSideBar::TopBarHeight + 1, Color::White, time);
    time = m_timeManager->getCurrentTimer();
    m_textDrawer->drawText(m_gameSettings->getScreenW()- cSideBar::SidebarWidth-75, cSideBar::TopBarHeight + 1+15, Color::White, time);
}

void cGame::checkMissionWinOrFail()
{
    if (m_gameConditionChecker->isMissionFailed()) {
        setMissionLost();
        return;
    }

    if (m_gameConditionChecker->isMissionWon()) {
        setMissionWon();
        return;
    }
}

void cGame::loadMapFromEditor(int map)
{
    setState(GAME_EDITOR);
    auto *pState = dynamic_cast<cEditorState*>(m_creatorState->getState(eGameState::EDITOR));
    pState->loadMap(map);
}
