/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

*/

#include <vector>
#include <algorithm>
#include <random>
#include "include/d2tmh.h"
#include "cGame.h"

#include <fmt/core.h>

cGame::cGame() {
    memset(states, 0, sizeof(cGameState *));

    nextState = -1;
    currentState = nullptr;
    screen_x = 800;
    screen_y = 600;
    windowed = false;
    bPlaySound = true;
    bPlayMusic = true;
    bMp3 = false;
    // default INI screen width and height is not loaded
    // if not loaded, we will try automatic setup
    ini_screen_width = -1;
    ini_screen_height = -1;

  version = "0.6.x";

    pMentat = nullptr;
}


void cGame::init() {
    nextState = -1;
    missionWasWon = false;
    currentState = nullptr;
    screenshot = 0;
    bPlaying = true;

    TIMER_evaluatePlayerStatus = 5;

    bSkirmish = false;

    // Alpha (for fading in/out)
    fadeAlpha = 0;                             // 255 = opaque , anything else
    fadeAction = eFadeAction::FADE_IN;           // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

    iMusicVolume = 96; // volume is 0...

    paths_created = 0;

    setState(GAME_INITIALIZE);

    // mentat
    delete pMentat;
    pMentat = nullptr;

    fade_select = 1.0f;

    bFadeSelectDir = true;    // fade select direction

    iRegion = 1;          // what region ? (calumative, from player perspective, NOT the actual region number)
    iMission = 0;         // calculated by mission loading (region -> mission calculation)

    shake_x = 0;
    shake_y = 0;
    TIMER_shake = 0;

    iMusicType = MUSIC_MENU;

    map.init(64, 64);

    initPlayers(false);

    for (int i = 0; i < MAX_UNITS; i++) {
        unit[i].init(i);
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle[i].init();
    }

    // Units & Structures are already initialized in map.init()
    if (game.bMp3 && mp3_music) {
        almp3_stop_autopoll_mp3(mp3_music); // stop auto updateState
    }

    // Load properties
    INI_Install_Game(game_filename);

    mp3_music = NULL;
}

// TODO: Bad smell (duplicate code)
// initialize for missions
void cGame::mission_init() {
    mapCamera->resetZoom();

    // first 15 seconds, do not evaluate player alive status
    TIMER_evaluatePlayerStatus = 5;

    winFlags = 0;
    loseFlags = 0;

    iMusicVolume = 96; // volume is 0...

    paths_created = 0;

    fade_select = 1.0f;

    bFadeSelectDir = true;    // fade select direction

    shake_x = 0;
    shake_y = 0;
    TIMER_shake = 0;

    map.init(64, 64);

    initPlayers(true);

    drawManager->getCreditsDrawer()->setCredits();
}

void cGame::initPlayers(bool rememberHouse) const {
    int maxThinkingAIs = MAX_PLAYERS;
    if (bOneAi) {
        maxThinkingAIs = 1;
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer &pPlayer = players[i];
        int h = pPlayer.getHouse();

        brains::cPlayerBrain *brain = nullptr;
        bool autoSlabStructures = false;

        if (i > HUMAN && i < AI_CPU5) {
            // TODO: playing attribute? (from ai player class?)
            if (!game.bDisableAI) {
                if (maxThinkingAIs > 0) {
                    if (game.bSkirmish) {
                        brain = new brains::cPlayerBrainSkirmish(&pPlayer);
                    } else {
                        brain = new brains::cPlayerBrainCampaign(&pPlayer);
                        autoSlabStructures = true;  // campaign based AI's autoslab structures...
                    }
                }
            }
            maxThinkingAIs--;
        } else if (i == AI_CPU5) {
            brain = new brains::cPlayerBrainFremenSuperWeapon(&pPlayer);
        } else if (i == AI_CPU6) {
            brain = new brains::cPlayerBrainSandworm(&pPlayer);
        }

        pPlayer.init(i, brain);
        pPlayer.setAutoSlabStructures(autoSlabStructures);
        if (rememberHouse) {
            pPlayer.setHouse(h);
        }

        if (bSkirmish) {
            pPlayer.setCredits(2500);
        }
    }
}

/**
 * Thinking every second while in combat
 */
void cGame::thinkSlow_combat() {
    if (TIMER_evaluatePlayerStatus > 0) {
        TIMER_evaluatePlayerStatus--;
    } else {
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

                game.onNotify(event);
            }

            // TODO: event : Player joined/became alive, etc?
        }
        TIMER_evaluatePlayerStatus = 2;
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

void cGame::setMissionWon() {
    missionWasWon = true;
    setState(GAME_WINNING);

    shake_x = 0;
    shake_y = 0;
    TIMER_shake = 0;
    mouse->setTile(MOUSE_NORMAL);

    play_voice(SOUND_VOICE_07_ATR);

    playMusicByType(MUSIC_WIN);

    // copy over
    blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, screen_x, screen_y);

    allegroDrawer->drawCenteredSprite(bmp_winlose, (BITMAP *) gfxinter[BMP_WINNING].dat);
}

void cGame::setMissionLost() {
    missionWasWon = false;
    setState(GAME_LOSING);

    shake_x = 0;
    shake_y = 0;
    TIMER_shake = 0;
    mouse->setTile(MOUSE_NORMAL);

    play_voice(SOUND_VOICE_08_ATR);

    playMusicByType(MUSIC_LOSE);

    // copy over
    blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, screen_x, screen_y);

    allegroDrawer->drawCenteredSprite(bmp_winlose, (BITMAP *) gfxinter[BMP_LOSING].dat);
}

bool cGame::isMissionFailed() const {
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
                return false; // it is meant to win the game by losing all...
            } else {
                return true; // nope, it should lose mission now
            }
        }
    }

    return false;
}

bool cGame::isMissionWon() const {
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
            if (allAIPlayersAreDestroyed()) {
                return true;
            }
        }
    } else if (hasGameOverConditionAIHasNoBuildings()) {
        if (hasWinConditionAIShouldLoseEverything()) {
            if (allAIPlayersAreDestroyed()) {
                return true;
            }
        }
    }
    return false;
}

bool cGame::allAIPlayersAreDestroyed() const {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (i == HUMAN || i == AI_WORM || i == AI_CPU5) continue; // do not evaluate these players
        cPlayer &player = players[i];
        if (!player.isAlive()) continue;
        return false;
    }
    return true;
}


/**
 * Remember, the 'winFlags' are used to determine when the game is "over". Only then the lose flags are evaluated
 * and used to determine who has won. (According to the SCEN specification).
 * @return
 */
bool cGame::hasWinConditionHumanMustLoseAllBuildings() const {
    return (loseFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0;
}

bool cGame::hasWinConditionAIShouldLoseEverything() const {
    return (loseFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0;
}

/**
 * Game over condition: player has no buildings (WinFlags)
 * @return
 */
bool cGame::hasGameOverConditionPlayerHasNoBuildings() const {
    return (winFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0;
}

/**
 * Game over condition: Spice quota reached by player
 * @return
 */
bool cGame::hasGameOverConditionHarvestForSpiceQuota() const {
    return (winFlags & WINLOSEFLAGS_QUOTA) != 0;
}

bool cGame::hasGameOverConditionAIHasNoBuildings() const {
    return (winFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0;
}

void cGame::think_mentat() {
    if (pMentat) {
        pMentat->think();
    }
}

// TODO: Move to music related class (MusicPlayer?)
void cGame::think_music() {
    if (!game.bPlayMusic) // no music enabled, so no need to think
        return;

    // all this does is repeating music in the same theme.
    if (iMusicType < 0)
        return;

    if (!isMusicPlaying()) {

        if (iMusicType == MUSIC_ATTACK) {
            iMusicType = MUSIC_PEACE; // set back to peace
        }

        playMusicByType(iMusicType);
    }
}

bool cGame::isMusicPlaying() {
    if (bMp3 && mp3_music) {
        int s = almp3_poll_mp3(mp3_music);
        return !(s == ALMP3_POLL_PLAYJUSTFINISHED || s == ALMP3_POLL_NOTPLAYING);
    }

    // MIDI mode:
    return MIDI_music_playing();
}

void cGame::updateState() {
    mouse->updateState(); // calls observers that are interested in mouse input
    keyboard->updateState(); // calls observers that are interested in keyboard input

    if (state != GAME_PLAYING) {
        return;
    }

    // Mission playing state logic
    // TODO: Move this to combat state object
    for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer *pPlayer = &players[i];

        pPlayer->update();

        pPlayer->bDeployedIt = false;

    }
}

void cGame::combat() {
    // -----------------
    cPlayer &humanPlayer = players[HUMAN];
    humanPlayer.bDeployedIt = humanPlayer.bDeployIt;

    drawManager->drawCombatState();
}

// stateMentat logic + drawing mouth/eyes
void cGame::stateMentat(cAbstractMentat *mentat) {
    draw_sprite(bmp_screen, bmp_backgroundMentat, 0, 0);

    mouse->setTile(MOUSE_NORMAL);

    mentat->draw();
    mentat->interact();

    mouse->draw();
}

// draw menu
void cGame::menu() {
    currentState->draw();
}

void cGame::init_skirmish() const {
    game.mission_init();
}

void cGame::handleTimeSlicing() {
    if (iRest > 0) {
        rest(iRest);
    }
}

void cGame::shakeScreenAndBlitBuffer() {
    if (TIMER_shake == 0) {
        TIMER_shake = -1;
    }
    // blitSprite on screen

    if (TIMER_shake > 0) {
        // the more we get to the 'end' the less we 'throttle'.
        // Structure explosions are 6 time units per cell.
        // Max is 9 cells (9*6=54)
        // the max border is then 9. So, we do time / 6
        if (TIMER_shake > 69) TIMER_shake = 69;

        int offset = TIMER_shake / 5;
        if (offset > 9)
            offset = 9;

        shake_x = -abs(offset / 2) + rnd(offset);
        shake_y = -abs(offset / 2) + rnd(offset);

        blit(bmp_screen, bmp_throttle, 0, 0, 0 + shake_x, 0 + shake_y, screen_x, screen_y);
        blit(bmp_throttle, screen, 0, 0, 0, 0, screen_x, screen_y);
    } else {
        // when fading
        if (fadeAlpha == 255) {
            blit(bmp_screen, screen, 0, 0, 0, 0, screen_x, screen_y);
        } else {
            BITMAP *temp = create_bitmap(game.screen_x, game.screen_y);
            assert(temp != NULL);
            clear(temp);
            fblend_trans(bmp_screen, temp, 0, 0, fadeAlpha);
            blit(temp, screen, 0, 0, 0, 0, screen_x, screen_y);
            destroy_bitmap(temp);
        }
    }
}

void cGame::drawState() {
    clear(bmp_screen);

    if (fadeAction == eFadeAction::FADE_OUT) {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    // this makes fade-in happen after fade-out automatically
    if (fadeAlpha == 0) {
        fadeAction = eFadeAction::FADE_IN;
    }

    switch (state) {
        case GAME_TELLHOUSE:
            stateMentat(pMentat);
            break;
        case GAME_PLAYING:
            combat();
            break;
        case GAME_BRIEFING:
            stateMentat(pMentat);
            break;
        case GAME_MENU:
            menu();
            break;
        case GAME_WINNING:
            winning();
            break;
        case GAME_LOSING:
            losing();
            break;
        case GAME_WINBRIEF:
            stateMentat(pMentat);
            break;
        case GAME_LOSEBRIEF:
            stateMentat(pMentat);
            break;
        default:
            currentState->draw();
            // TODO: GAME_STATISTICS, ETC
    }
}

/**
	Main game loop
*/
void cGame::run() {
    set_trans_blender(0, 0, 0, 128);

    while (bPlaying) {
        TimeManager.processTime();
        updateState();
        handleTimeSlicing(); // handle time diff (needs to change!)
        drawState(); // run game state, includes interaction + drawing
        transitionStateIfRequired();
        _interactionManager->interactWithKeyboard(); // generic interaction
        shakeScreenAndBlitBuffer(); // finally, draw the bmp_screen to real screen (double buffering)
        frame_count++;
    }
}

void cGame::shakeScreen(int duration) {
    game.TIMER_shake += duration;
}

/**
	Shutdown the game
*/
void cGame::shutdown() {
    cLogger *logger = cLogger::getInstance();
    logger->logHeader("SHUTDOWN");

    for (int i = 0; i < GAME_MAX_STATES; i++) {
        cGameState *pState = states[i];
        if (pState) {
            if (pState->getType() != eGameStateType::GAMESTATE_SELECT_YOUR_NEXT_CONQUEST) {
                if (currentState == pState) {
                    currentState = nullptr; // reset
                }
                delete pState;
                states[i] = nullptr;
            } else {
                if (currentState == pState) {
                    currentState = nullptr; // reset
                }
                states[i] = nullptr;
            }
        }
    }

    if (currentState != nullptr) {
        assert(false);
        if (currentState->getType() != eGameStateType::GAMESTATE_SELECT_YOUR_NEXT_CONQUEST) {
            // destroy game state object, unless we talk about the region select
            delete currentState;
        } else {
            currentState = nullptr;
        }
    }

    if (soundPlayer) {
        soundPlayer->destroyAllSounds();
    }
    delete soundPlayer;

    delete pMentat;
    delete mapViewport;

    drawManager->destroy();
    delete drawManager;

    delete mapCamera;
    delete _interactionManager;


    cStructureFactory::destroy();
    cSideBarFactory::destroy();
    cBuildingListFactory::destroy();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].destroyAllegroBitmaps();
    }

    delete allegroDrawer;
    delete m_dataRepository;
    delete mouse;
    delete keyboard;

    if (gfxdata) {
        unload_datafile(gfxdata);
    }
    if (gfxinter) {
        unload_datafile(gfxinter);
    }
    if (gfxworld) {
        unload_datafile(gfxworld);
    }
    if (gfxmentat) {
        unload_datafile(gfxmentat);
    }

    // Destroy font of Allegro FONT library
    alfont_destroy_font(game_font);
    alfont_destroy_font(bene_font);

    // Exit the font library (must be first)
    alfont_exit();

    logbook("Allegro FONT library shut down.");

    // MP3 Library
    if (mp3_music) {
        almp3_stop_autopoll_mp3(mp3_music); // stop auto updateState
        almp3_destroy_mp3(mp3_music);
    }

    logbook("Allegro MP3 library shut down.");

    // Now we are all neatly closed, we exit Allegro and return to OS.
    allegro_exit();

    logbook("Allegro shut down.");
    logbook("Thanks for playing.");

    cLogger::destroy();
}

bool cGame::isResolutionInGameINIFoundAndSet() {
    return game.ini_screen_height != -1 && game.ini_screen_width != -1;
}

void cGame::setScreenResolutionFromGameIniSettings() {
    if (game.ini_screen_width < 800) {
        game.ini_screen_width = 800;
        logbook("INI screen width < 800; unsupported; will set to 800.");
    }
    if (game.ini_screen_height < 600) {
        game.ini_screen_height = 600;
        logbook("INI screen height < 600; unsupported; will set to 600.");
    }
    game.screen_x = game.ini_screen_width;
    game.screen_y = game.ini_screen_height;
    char msg[255];
    sprintf(msg, "Resolution %dx%d loaded from ini file.", game.ini_screen_width, game.ini_screen_height);
    cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Resolution from ini file", msg);
}

/**
	Setup the game

	Should not be called twice.
*/
bool cGame::setupGame() {
    cLogger *logger = cLogger::getInstance();

    game.init(); // Must be first!

    logger->clearLogFile();

    logger->logHeader("Dune II - The Maker");
    logger->logCommentLine(""); // whitespace

	logger->logHeader("Version information");
	logger->log(LOG_INFO, COMP_VERSION, "Initializing",
              fmt::format("Version {}, Compiled at {} , {}", game.version, __DATE__, __TIME__));

    // init game
    if (game.windowed) {
        logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Windowed mode");
    } else {
        logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Fullscreen mode");
    }

    // TODO: load eventual game settings (resolution, etc)

    // Logbook notification
    logger->logHeader("Allegro");

    // ALLEGRO - INIT
    if (allegro_init() != 0) {
        logger->log(LOG_FATAL, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_FAILED);
        return false;
    }

    logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_SUCCESS);

    int r = install_timer();
    if (r > -1) {
        logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_SUCCESS);
    } else {
        allegro_message("Failed to install timer");
        logger->log(LOG_FATAL, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_FAILED);
        return false;
    }

    alfont_init();
    logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing ALFONT", "alfont_init()", OUTC_SUCCESS);
    install_keyboard();
    keyboard = new cKeyboard();
    logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Keyboard", "install_keyboard()", OUTC_SUCCESS);
    install_mouse();
    mouse = new cMouse();
    logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Mouse", "install_mouse()", OUTC_SUCCESS);

    /* set up the interrupt routines... */
    game.TIMER_shake = 0;

    LOCK_VARIABLE(allegro_timerUnits);
    LOCK_VARIABLE(allegro_timerGlobal);
    LOCK_VARIABLE(allegro_timerSecond);

    LOCK_FUNCTION(allegro_timerunits);
    LOCK_FUNCTION(allegro_timergametime);
    LOCK_FUNCTION(allegro_timerseconds);

    // Install timers
    install_int(allegro_timerunits, 100); // 100 milliseconds
    install_int(allegro_timergametime,
                5); // 5 milliseconds / hence, in 1 second the gametime has passed 1000/5 = 200 times
    install_int(allegro_timerseconds, 1000); // 1000 milliseconds (seconds)

    logger->log(LOG_INFO, COMP_ALLEGRO, "Set up timer related variables", "LOCK_VARIABLE/LOCK_FUNCTION", OUTC_SUCCESS);

    frame_count = fps = 0;

	// set window title
    auto title = fmt::format("Dune II - The Maker [{}] - (by Stefan Hendriks)", game.version);

	// Set window title
	set_window_title(title.c_str());
	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", title, OUTC_SUCCESS);

    int colorDepth = desktop_color_depth();
    set_color_depth(colorDepth);

    char colorDepthMsg[255];
    sprintf(colorDepthMsg, "Desktop color dept is %d.", colorDepth);
    cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.", colorDepthMsg);


    // TODO: read/write rest value so it does not have to 'fine-tune'
    // but is already set up. Perhaps even offer it in the options screen? So the user
    // can specify how much CPU this game may use?

    if (game.windowed) {
        cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Windowed mode requested.", "");

        if (isResolutionInGameINIFoundAndSet()) {
            setScreenResolutionFromGameIniSettings();
        }

        r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, game.screen_x, game.screen_y, game.screen_x, game.screen_y);

        char msg[255];
        sprintf(msg, "Initializing graphics mode (windowed) with resolution %d by %d, colorDepth %d.", game.screen_x,
                game.screen_y, colorDepth);
        logbook(msg);

        if (r > -1) {
            logger->log(LOG_INFO, COMP_ALLEGRO, msg, "Succesfully created window with graphics mode.", OUTC_SUCCESS);
        } else {
            allegro_message("Failed to initialize graphics mode");
            return false;
        }
    } else {
        /**
         * Fullscreen mode
        */

        bool mustAutoDetectResolution = false;
        if (isResolutionInGameINIFoundAndSet()) {
            setScreenResolutionFromGameIniSettings();
            r = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
            char msg[255];
            sprintf(msg, "Setting up %dx%d resolution from ini file (using colorDepth %d). r = %d",
                    game.ini_screen_width, game.ini_screen_height, colorDepth, r);
            cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Custom resolution from ini file.", msg);
            mustAutoDetectResolution = r < 0;
        } else {
            cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Custom resolution from ini file.",
                                        "No resolution defined in ini file.");
            mustAutoDetectResolution = true;
        }

        // find best possible resolution
        if (mustAutoDetectResolution) {
            char msg[255];
            sprintf(msg, "Autodetecting resolutions at color depth %d", colorDepth);
            cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, msg, "Commencing");
            // find best possible resolution
            cBestScreenResolutionFinder bestScreenResolutionFinder(colorDepth);
            bestScreenResolutionFinder.checkResolutions();
            bool result = bestScreenResolutionFinder.acquireBestScreenResolutionFullScreen();

            // success
            if (result) {
                logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fullscreen)",
                            "Succesfully initialized graphics mode.", OUTC_SUCCESS);
            } else {
                logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fullscreen)",
                            "Failed to initializ graphics mode.", OUTC_FAILED);
                allegro_message(
                        "Fatal error:\n\nCould not start game.\n\nGraphics mode (fullscreen) could not be initialized.");
                return false;
            }
        }
    }

    alfont_text_mode(-1);
    logger->log(LOG_INFO, COMP_ALLEGRO, "Font settings", "Set text mode to -1", OUTC_SUCCESS);


    game_font = alfont_load_font("data/arakeen.fon");

    if (game_font != NULL) {
        logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded arakeen.fon", OUTC_SUCCESS);
        alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
    } else {
        logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load arakeen.fon", OUTC_FAILED);
        allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load arakeen.fon");
        return false;
    }


    bene_font = alfont_load_font("data/benegess.fon");

    if (bene_font != NULL) {
        logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded benegess.fon", OUTC_SUCCESS);
        alfont_set_font_size(bene_font, 10); // set size
    } else {
        logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load benegess.fon", OUTC_FAILED);
        allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load benegess.fon");
        return false;
    }

    small_font = alfont_load_font("data/small.ttf");

    if (small_font != NULL) {
        logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded small.ttf", OUTC_SUCCESS);
        alfont_set_font_size(small_font, 10); // set size
    } else {
        logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load small.ttf", OUTC_FAILED);
        allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load small.ttf");
        return false;
    }

    if (set_display_switch_mode(SWITCH_BACKGROUND) < 0) {
        set_display_switch_mode(SWITCH_PAUSE);
        logbook("Display 'switch and pause' mode set");
    } else {
        logbook("Display 'switch to background' mode set");
    }

	int maxSounds = getAmountReservedVoicesAndInstallSound();

    if (maxSounds > -1) {
        logger->log(LOG_INFO, COMP_SOUND, "Initialization",
                    fmt::format("Successfully installed sound. {} voices reserved", maxSounds),
                    OUTC_SUCCESS);
    } else {
        logger->log(LOG_INFO, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
    }
	soundPlayer = new cSoundPlayer(maxSounds);

    // normal sounds are loud, the music is lower (its background music, so it should not be disturbing)
    iMaxVolume = 220;
    set_volume(iMaxVolume, 110);

    /***
     * Viewport(s)
     */
    mapViewport = new cRectangle(0, cSideBar::TopBarHeight, game.screen_x - cSideBar::SidebarWidth,
                                 game.screen_y - cSideBar::TopBarHeight);

    /***
    Bitmap Creation
    ***/

    bmp_screen = create_bitmap(game.screen_x, game.screen_y);

    if (bmp_screen == NULL) {
        allegro_message("Failed to create a memory bitmap");
        logbook("ERROR: Could not create bitmap: bmp_screen");
        return false;
    } else {
        logbook("Memory bitmap created: bmp_screen");
        clear(bmp_screen);
    }

    bmp_backgroundMentat = create_bitmap(game.screen_x, game.screen_y);

    if (bmp_backgroundMentat == NULL) {
        allegro_message("Failed to create a memory bitmap");
        logbook("ERROR: Could not create bitmap: bmp_backgroundMentat");
        return false;
    } else {
        logbook("Memory bitmap created: bmp_backgroundMentat");
        clear(bmp_backgroundMentat);

        // create only once
        clear_to_color(bmp_backgroundMentat, makecol(8, 8, 16));
        bool offsetX = false;

        float horizon = game.screen_y / 2;
        float centered = game.screen_x / 2;
        for (int y = 0; y < game.screen_y; y++) {
            float diffYToCenter = 1.0f;
            if (y < horizon) {
                diffYToCenter = y / horizon;
            } else {
                diffYToCenter = 1 - ((y - horizon) / horizon);
            }

            for (int x = offsetX ? 0 : 1; x < game.screen_x; x += 2) {
                float diffXToCenter = 1.0f;
                if (x < centered) {
                    diffXToCenter = x / centered;
                } else {
                    diffXToCenter = 1 - ((x - centered) / centered);
                }

                float red = 2 + (12 * diffXToCenter) + (12 * diffYToCenter);
                float green = 2 + (12 * diffXToCenter) + (12 * diffYToCenter);
                float blue = 4 + (24 * diffXToCenter) + (24 * diffYToCenter);
                putpixel(bmp_backgroundMentat, x, y, makecol((int) red, (int) green, (int) blue));
            }
            // flip offset every y row
            offsetX = !offsetX;
        }
    }

    bmp_throttle = create_bitmap(game.screen_x, game.screen_y);

    if (bmp_throttle == NULL) {
        allegro_message("Failed to create a memory bitmap");
        logbook("ERROR: Could not create bitmap: bmp_throttle");
        return false;
    } else {
        logbook("Memory bitmap created: bmp_throttle");
    }

    bmp_winlose = create_bitmap(game.screen_x, game.screen_y);

    if (bmp_winlose == NULL) {
        allegro_message("Failed to create a memory bitmap");
        logbook("ERROR: Could not create bitmap: bmp_winlose");
        return false;
    } else {
        logbook("Memory bitmap created: bmp_winlose");
    }

    bmp_fadeout = create_bitmap(game.screen_x, game.screen_y);

    if (bmp_fadeout == NULL) {
        allegro_message("Failed to create a memory bitmap");
        logbook("ERROR: Could not create bitmap: bmp_fadeout");
        return false;
    } else {
        logbook("Memory bitmap created: bmp_fadeout");
    }

    /*** End of Bitmap Creation ***/
    set_color_conversion(COLORCONV_MOST);

    logbook("Color conversion method set");

    // setup mouse speed
    set_mouse_speed(0, 0);

    logbook("MOUSE: Mouse speed set");

    logger->logHeader("GAME");

    /*** Data files ***/

    // load datafiles
    gfxdata = load_datafile("data/gfxdata.dat");
    if (gfxdata == nullptr) {
        logbook("ERROR: Could not hook/load datafile: gfxdata.dat");
        return false;
    } else {
        logbook("Datafile hooked: gfxdata.dat");
        memcpy(general_palette, gfxdata[PALETTE_D2TM].dat, sizeof general_palette);
    }

    gfxaudio = load_datafile("data/gfxaudio.dat");
    if (gfxaudio == nullptr) {
        logbook("ERROR: Could not hook/load datafile: gfxaudio.dat");
        return false;
    } else {
        logbook("Datafile hooked: gfxaudio.dat");
    }

    gfxinter = load_datafile("data/gfxinter.dat");
    if (gfxinter == nullptr) {
        logbook("ERROR: Could not hook/load datafile: gfxinter.dat");
        return false;
    } else {
        logbook("Datafile hooked: gfxinter.dat");
    }

    gfxworld = load_datafile("data/gfxworld.dat");
    if (gfxworld == nullptr) {
        logbook("ERROR: Could not hook/load datafile: gfxworld.dat");
        return false;
    } else {
        logbook("Datafile hooked: gfxworld.dat");
    }

    gfxmentat = load_datafile("data/gfxmentat.dat");
    if (gfxworld == nullptr) {
        logbook("ERROR: Could not hook/load datafile: gfxmentat.dat");
        return false;
    } else {
        logbook("Datafile hooked: gfxmentat.dat");
    }

    // finally the data repository and drawer interface can be initialized
    m_dataRepository = new cAllegroDataRepository();
    allegroDrawer = new cAllegroDrawer(m_dataRepository);

    // randomize timer
    unsigned int t = (unsigned int) time(0);
    char seedtxt[80];
    sprintf(seedtxt, "Seed is %u", t);
    logbook(seedtxt);
    srand(t);

    game.bPlaying = true;
    game.screenshot = 0;
    game.state = GAME_INITIALIZE;

    set_palette(general_palette);

    // A few messages for the player
    logbook("Initializing:  PLAYERS");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].init(i, nullptr);
    }
    logbook("Setup:  BITMAPS");
    install_bitmaps();
    logbook("Setup:  HOUSES");
    INSTALL_HOUSES();
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
    mapCamera = new cMapCamera(&map);

    delete drawManager;
    drawManager = new cDrawManager(&players[HUMAN]);

    game.init(); // AGAIN!?

    // Now we are ready for the menu state
    game.setState(GAME_MENU);

    // do install_upgrades after game.init, because game.init loads the INI file and then has the very latest
    // unit/structures catalog loaded - which the install_upgrades depends on.
    install_upgrades();

    game.setup_players();

    playMusicByType(MUSIC_MENU);

    // all has installed well. Lets rock and roll.
    return true;

}

/**
 * Set up players
 */
void cGame::setup_players() {
    mouse->setMouseObserver(nullptr);
    keyboard->setKeyboardObserver(nullptr);

    // make sure each player has an own item builder
    for (int i = HUMAN; i < MAX_PLAYERS; i++) {
        cPlayer *thePlayer = &players[i];

        cBuildingListUpdater *buildingListUpdater = new cBuildingListUpdater(thePlayer);
        thePlayer->setBuildingListUpdater(buildingListUpdater);

        cItemBuilder *itemBuilder = new cItemBuilder(thePlayer, buildingListUpdater);
        thePlayer->setItemBuilder(itemBuilder);

        cSideBar *sidebar = cSideBarFactory::getInstance()->createSideBar(thePlayer);
        thePlayer->setSideBar(sidebar);

        cOrderProcesser *orderProcesser = new cOrderProcesser(thePlayer);
        thePlayer->setOrderProcesser(orderProcesser);

        cGameControlsContext *gameControlsContext = new cGameControlsContext(thePlayer, this->mouse);
        thePlayer->setGameControlsContext(gameControlsContext);

        // set tech level
        thePlayer->setTechLevel(game.iMission);
    }

    delete _interactionManager;
    cPlayer *humanPlayer = &players[HUMAN];
    _interactionManager = new cInteractionManager(humanPlayer);
    mouse->setMouseObserver(_interactionManager);
    keyboard->setKeyboardObserver(_interactionManager);
}

bool cGame::isState(int thisState) {
    return (state == thisState);
}

void cGame::setState(int newState) {
    if (newState == state) {
        // ignore
        return;
    }

    char msg[255];
    sprintf(msg, "Setting state from %d(=%s) to %d(=%s)", state, stateString(state), newState, stateString(newState));
    logbook(msg);

    if (newState > -1) {
        bool deleteOldState = (newState != GAME_REGION &&
                               newState != GAME_PLAYING); // don't delete these states, but re-use!
        if (state == GAME_OPTIONS && newState == GAME_SETUPSKIRMISH) {
            deleteOldState = false; // so we don't lose data when we go back
        }

        if (deleteOldState) {
            delete states[newState];
            states[newState] = nullptr;
        }

        cGameState *existingStatePtr = states[newState];
        if (existingStatePtr) {
            if (currentState->getType() == GAMESTATE_SELECT_YOUR_NEXT_CONQUEST) {
                cSelectYourNextConquestState *pState = dynamic_cast<cSelectYourNextConquestState *>(currentState);

                if (missionWasWon) {
                    // we won
                    if (game.iMission > 1) {
                        pState->conquerRegions();
                    }
                    pState->REGION_SETUP_NEXT_MISSION(game.iMission, players[HUMAN].getHouse());
                } else {
                    // OR: did not win
                    pState->REGION_SETUP_LOST_MISSION();
                }
            }

            currentState = existingStatePtr;
        } else {
            cGameState *newStatePtr = nullptr;

            if (newState == GAME_REGION) {
                cSelectYourNextConquestState *pState = new cSelectYourNextConquestState(game);

                pState->calculateOffset();
                logbook("Setup:  WORLD");
                pState->INSTALL_WORLD();
                if (game.iMission > 1) {
                    pState->conquerRegions();
                }
                // first creation
                pState->REGION_SETUP_NEXT_MISSION(game.iMission, players[HUMAN].getHouse());

                newStatePtr = pState;
            } else if (newState == GAME_SETUPSKIRMISH) {
                initPlayers(false);
                newStatePtr = new cSetupSkirmishGameState(*this);
            } else if (newState == GAME_MENU) {
                newStatePtr = new cMainMenuGameState(*this);
            } else if (newState == GAME_SELECT_HOUSE) {
                newStatePtr = new cChooseHouseGameState(*this);
            } else if (newState == GAME_OPTIONS) {
                BITMAP *background = create_bitmap(screen_x, screen_y);
                allegroDrawer->drawSprite(background, bmp_screen, 0, 0);
                newStatePtr = new cOptionsState(*this, background, state);
            } else if (newState == GAME_PLAYING) {
                // evaluate all players, so we have initial 'alive' values set properly
                for (int i = 1; i < MAX_PLAYERS; i++) {
                    cPlayer &player = players[i];
                    player.evaluateStillAlive();
                }

                // handle update
                s_GameEvent event{
                        .eventType = eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN,
                };
                // the game is about to begin!
                game.onNotify(event);
            }

            states[newState] = newStatePtr;
            currentState = newStatePtr;
        }
    }

    state = newState;
}

void cGame::think_fading() {
    // Fading / pulsating of selected stuff
    static float fadeSelectIncrement = (1 / 256.0f);
    if (bFadeSelectDir) {
        fade_select += fadeSelectIncrement;

        // when 255, then fade back
        if (fade_select > 0.99) {
            fade_select = 1.0f;
            bFadeSelectDir = false;
        }

        return;
    }

    fade_select -= fadeSelectIncrement;
    // not too dark,
    // 0.03125
    if (fade_select < 0.3125f) {
        bFadeSelectDir = true;
    }
}

cGame::~cGame() {
    // cannot do this, because when game is being quit, and the cGame object being deleted, Allegro has been shut down
    // already, so the deletion of drawManager has to happen *before* that, hence look in shutdown() method
//    if (drawManager) {
//        delete drawManager;
//    }
}

void cGame::prepareMentatForPlayer() {
    int house = players[HUMAN].getHouse();
    if (state == GAME_BRIEFING) {
        game.mission_init();
        game.setup_players();
        INI_Load_scenario(house, iRegion, pMentat);
        INI_LOAD_BRIEFING(house, iRegion, INI_BRIEFING, pMentat);
    } else if (state == GAME_WINBRIEF) {
        if (rnd(100) < 50) {
            pMentat->loadScene("win01");
        } else {
            pMentat->loadScene("win02");
        }
        INI_LOAD_BRIEFING(house, iRegion, INI_WIN, pMentat);
    } else if (state == GAME_LOSEBRIEF) {
        if (rnd(100) < 50) {
            pMentat->loadScene("lose01");
        } else {
            pMentat->loadScene("lose02");
        }
        INI_LOAD_BRIEFING(house, iRegion, INI_LOSE, pMentat);
    }
}

void cGame::createAndPrepareMentatForHumanPlayer() {
    delete pMentat;
    int houseIndex = players[0].getHouse();
    if (houseIndex == ATREIDES) {
        pMentat = new cAtreidesMentat();
    } else if (houseIndex == HARKONNEN) {
        pMentat = new cHarkonnenMentat();
    } else if (houseIndex == ORDOS) {
        pMentat = new cOrdosMentat();
    } else {
        // fallback
        pMentat = new cBeneMentat();
    }
    prepareMentatForPlayer();
    pMentat->speak();
}

void cGame::prepareMentatToTellAboutHouse(int house) {
    delete pMentat;
    pMentat = new cBeneMentat();
    pMentat->setHouse(house);
    // create new stateMentat
    if (house == ATREIDES) {
        INI_LOAD_BRIEFING(ATREIDES, 0, INI_DESCRIPTION, pMentat);
        pMentat->loadScene("platr"); // load planet of atreides
    } else if (house == HARKONNEN) {
        INI_LOAD_BRIEFING(HARKONNEN, 0, INI_DESCRIPTION, pMentat);
        pMentat->loadScene("plhar"); // load planet of harkonnen
    } else if (house == ORDOS) {
        INI_LOAD_BRIEFING(ORDOS, 0, INI_DESCRIPTION, pMentat);
        pMentat->loadScene("plord"); // load planet of ordos
    } else {
        pMentat->setSentence(0, "Looks like you choose an unknown house");
    }
    // todo: Sardaukar, etc? (Super Dune 2 features)
    pMentat->speak();
}

void cGame::loadScenario() {
    int iHouse = players[HUMAN].getHouse();
    INI_Load_scenario(iHouse, game.iRegion, pMentat);
}

void cGame::think_state() {
    if (currentState) {
        currentState->thinkFast();
    }
}

void cGame::setPlayerToInteractFor(cPlayer *pPlayer) {
    _interactionManager->setPlayerToInteractFor(pPlayer);
}

void cGame::onNotify(const s_GameEvent &event) {
    logbook(s_GameEvent::toString(event).c_str());

    map.onNotify(event);

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
        players[i].onNotify(event);
    }
}

void cGame::onEventSpecialLaunch(const s_GameEvent &event) {
    cBuildingListItem *itemToDeploy = event.buildingListItem;
    int iMouseCell = event.atCell;
    cPlayer *player = event.player;
    if (itemToDeploy->isTypeSpecial()) {
        const s_SpecialInfo &special = itemToDeploy->getSpecialInfo();

        int deployCell = -1;
        if (special.deployTargetType == eDeployTargetType::TARGET_SPECIFIC_CELL) {
            deployCell = iMouseCell;
        } else if (special.deployTargetType == eDeployTargetType::TARGET_INACCURATE_CELL) {
            int precision = special.deployTargetPrecision;
            int mouseCellX = map.getCellX(iMouseCell) - precision;
            int mouseCellY = map.getCellY(iMouseCell) - precision;

            int posX = mouseCellX + rnd((precision * 2) + 1);
            int posY = mouseCellY + rnd((precision * 2) + 1);
            FIX_POS(posX, posY);

            char msg[255];
            sprintf(msg,
                    "eDeployTargetType::TARGET_INACCURATE_CELL, mouse cell X,Y = %d,%d - target pos =%d,%d - precision %d",
                    mouseCellY, mouseCellY, posX, posY,
                    precision);
            logbook(msg);

            deployCell = map.makeCell(posX, posY);
        }


        if (special.providesType == eBuildType::BULLET) {
            // from where
            int structureId = structureUtils.findStructureBy(player->getId(), special.deployAtStructure, false);
            if (structureId > -1) {
                cAbstractStructure *pStructure = structure[structureId];
                if (pStructure && pStructure->isValid()) {
                    play_sound_id(SOUND_PLACE);
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

                    game.onNotify(newEvent);
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
        player->bDeployIt = false;
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

    game.onNotify(newEvent);
}

void cGame::reduceShaking() {
    if (TIMER_shake > 0) {
        TIMER_shake--;
    }
}

void cGame::install_bitmaps() {
    m_dataRepository->loadBitmapAt(D2TM_BITMAP_ICON_POWER, "./data/bmp/icon_power_sidebar3.bmp");
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_ATTACK, MOUSE_ATTACK);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_DOWN, MOUSE_DOWN);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_LEFT, MOUSE_LEFT);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_MOVE, MOUSE_MOVE);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_NORMAL, MOUSE_NORMAL);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_PICK, MOUSE_PICK);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_RALLY, MOUSE_RALLY);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_REPAIR, MOUSE_REPAIR);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_RIGHT, MOUSE_RIGHT);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_UP, MOUSE_UP);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(MOUSE_FORBIDDEN, MOUSE_FORBIDDEN);

    // Particle stuff
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_MOVE, MOVE_INDICATOR);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_ATTACK, ATTACK_INDICATOR);

    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_TRIKE, EXPLOSION_TRIKE);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_SMOKE, OBJECT_SMOKE);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_SMOKE_SHADOW, OBJECT_SMOKE_SHADOW);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_TRACK_DIA, TRACK_DIA);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_TRACK_HOR, TRACK_HOR);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_TRACK_VER, TRACK_VER);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_TRACK_DIA2, TRACK_DIA2);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_BULLET_PUF, BULLET_PUF);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_FIRE, EXPLOSION_FIRE);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_WORMEAT, OBJECT_WORMEAT);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_TANK_ONE, EXPLOSION_TANK_ONE);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_TANK_TWO, EXPLOSION_TANK_TWO);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_STRUCTURE01,
                                                      EXPLOSION_STRUCTURE01);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_STRUCTURE02,
                                                      EXPLOSION_STRUCTURE02);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_GAS, EXPLOSION_GAS);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_WORMTRAIL, OBJECT_WORMTRAIL);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_DEADINF01, OBJECT_DEADINF01);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_DEADINF02, OBJECT_DEADINF02);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_TANKSHOOT, OBJECT_TANKSHOOT);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_SIEGESHOOT, OBJECT_SIEGESHOOT);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_SQUISH01, EXPLOSION_SQUISH01);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_SQUISH02, EXPLOSION_SQUISH02);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_SQUISH03, EXPLOSION_SQUISH03);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_ORNI, EXPLOSION_ORNI);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_SIEGEDIE, OBJECT_SIEGEDIE);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_CARRYPUFF, OBJECT_CARRYPUFF);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_ROCKET, EXPLOSION_ROCKET);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_ROCKET_SMALL,
                                                      EXPLOSION_ROCKET_SMALL);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_OBJECT_BOOM01, OBJECT_BOOM01);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_OBJECT_BOOM02, OBJECT_BOOM02);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_OBJECT_BOOM03, OBJECT_BOOM03);
    m_dataRepository->loadBitmapFromDataFileGfxDataAt(D2TM_BITMAP_PARTICLE_EXPLOSION_BULLET, EXPLOSION_BULLET);

}

int cGame::getColorFadeSelected(int r, int g, int b, bool rFlag, bool gFlag, bool bFlag) {
    int desiredRed = rFlag ? r * fade_select : r;
    int desiredGreen = gFlag ? g * fade_select : g;
    int desiredBlue = bFlag ? b * fade_select : b;
    return makecol(desiredRed, desiredGreen, desiredBlue);
}

int cGame::getColorFadeSelected(int color) {
    return getColorFadeSelected(getr(color), getg(color), getb(color));
}

int cGame::getColorPlaceNeutral() {
    return getColorFadeSelected(makecol(242, 174, 36));
}

int cGame::getColorPlaceBad() {
    return getColorFadeSelected(makecol(160, 0, 0));
}

int cGame::getColorPlaceGood() {
    return getColorFadeSelected(makecol(64, 255, 64));
}

void cGame::setWinFlags(int value) {
    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "Changing winFlags from %d to %d", winFlags, value);
        logbook(msg);
    }
    winFlags = value;
}

void cGame::setLoseFlags(int value) {
    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "Changing loseFlags from %d to %d", loseFlags, value);
        logbook(msg);
    }
    loseFlags = value;
}

bool cGame::isRunningAtIdealFps() {
    return fps < IDEAL_FPS;
}

void cGame::resetFrameCount() {
    frame_count = 0;
}

void cGame::setFps() {
    fps = frame_count;
}

int cGame::getFps() {
    return fps;
}

void cGame::onNotifyMouseEvent(const s_MouseEvent &event) {
    // pass through any classes that are interested
    if (currentState) {
        currentState->onNotifyMouseEvent(event);
    }
}

void cGame::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    // pass through any classes that are interested
    if (currentState) {
        currentState->onNotifyKeyboardEvent(event);
    }

    // take screenshot
    if (event.isType(eKeyEventType::PRESSED) && event.hasKey(KEY_F11)) {
        takeScreenshot();
    }

    // TODO: this has to be its own state class. Then this if is no longer needed.
    if (state == GAME_PLAYING) {
        onNotifyKeyboardEventGamePlaying(event);
    }
}

void cGame::transitionStateIfRequired() {
    if (nextState > -1) {
        setState(nextState);

        if (nextState == GAME_BRIEFING) {
            playMusicByType(MUSIC_BRIEFING);
            game.createAndPrepareMentatForHumanPlayer();
        }

        nextState = -1;
    }
}

void cGame::setNextStateToTransitionTo(int newState) {
    nextState = newState;
}

void cGame::drawCombatMouse() {
    if (mouse->isBoxSelecting()) {
        allegroDrawer->drawRectangle(bmp_screen, mouse->getBoxSelectRectangle(),
                                     game.getColorFadeSelected(255, 255, 255));
    }

    if (mouse->isMapScrolling()) {
        cPoint startPoint = mouse->getDragLineStartPoint();
        cPoint endPoint = mouse->getDragLineEndPoint();
        allegroDrawer->drawLine(bmp_screen, startPoint.x, startPoint.y, endPoint.x, endPoint.y,
                                game.getColorFadeSelected(255, 255, 255));
    }

    mouse->draw();
}

void cGame::takeScreenshot() {
    char filename[25];

    if (screenshot < 10) {
        sprintf(filename, "%dx%d_000%d.bmp", screen_x, screen_y, screenshot);
    } else if (screenshot < 100) {
        sprintf(filename, "%dx%d_00%d.bmp", screen_x, screen_y, screenshot);
    } else if (screenshot < 1000) {
        sprintf(filename, "%dx%d_0%d.bmp", screen_x, screen_y, screenshot);
    } else {
        sprintf(filename, "%dx%d_%d.bmp", screen_x, screen_y, screenshot);
    }

    save_bmp(filename, bmp_screen, general_palette);

    screenshot++;
}

void cGame::onNotifyKeyboardEventGamePlaying(const cKeyboardEvent &event) {
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

void cGame::onKeyDownGamePlaying(const cKeyboardEvent &event) {
    const cPlayer *humanPlayer = &players[HUMAN];

    bool createGroup = event.hasKey(KEY_LCONTROL) || event.hasKey(KEY_RCONTROL);
    if (createGroup) {
        int iGroup = event.getGroupNumber();

        if (iGroup > 0) {
            humanPlayer->markUnitsForGroup(iGroup);
        }
    }

}

void cGame::onKeyPressedGamePlaying(const cKeyboardEvent &event) {
    cPlayer &humanPlayer = players[HUMAN];

    if (event.hasKey(KEY_H)) {
        mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
    }

    // Center on the selected structure
    if (event.hasKey(KEY_C)) {
        cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
        if (selectedStructure) {
            mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
        }
    }
}

