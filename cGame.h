/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */
#pragma once

#include "controls/cMouse.h"
#include "controls/cKeyboard.h"
#include "data/cAllegroDataRepository.h"
#include "definitions.h"
#include "mentat/cAbstractMentat.h"
#include "observers/cScenarioObserver.h"
#include "utils/cRectangle.h"
#include "utils/cTimeManager.h"
#include "utils/cIniFile.h"

#include <memory>
#include <string>

class cGameControlsContext;
class cGameState;
class cInteractionManager;
class cPlatformLayerInit;
class cPlayer;
class cSoundPlayer;
class cScreenInit;
class cHandleArgument;
class cHousesInfo;
class cReinforcements;

// Naming thoughts:
// member variables, start with m_<camelCasedVariableName>
//
// functions are camelCased()
// exceptions (for now):
// think()
// thinkFast_...()
// thinkSlow_....() --> for now used to distinguish certain speed of "thinking" / invocations
// state_...() --> because elegible for moving away

class cGame : public cScenarioObserver, cInputObserver {

public:
	cGame();
	~cGame();

    void jumpToSelectYourNextConquestMission(int missionNr);

    void setGameFilename(const std::string& filename) {
        m_gameFilename = filename;
    }

    int handleArguments(int argc, char **argv);

	bool m_windowed;			    // windowed
    bool m_allowRepeatingReinforcements; // Dune 2 fix: by default false
	std::string m_version;          // version number, or name.

    // Alpha (for fading in/out)
    int m_fadeAlpha;                // 255 = opaque , anything else
    eFadeAction m_fadeAction;       // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

    // resolution of the game
	int m_screenX;
	int m_screenY;
    int m_iniScreenWidth;
    int m_iniScreenHeight;

    bool m_playSound;               // play sound?
    bool m_disableAI;               // disable AI thinking?
    bool m_oneAi;                   // disable all but one AI brain? (default == false)
    bool m_disableWormAi;                // disable worm AI brain? (default == false)
    bool m_disableReinforcements;   // disable any reinforcements from scenario ini file?
    bool m_drawUsages;              // draw the amount of structures/units/bullets used during combat
    bool m_drawUnitDebug;           // draw the unit debug info (rects, paths, etc)
    bool m_noAiRest;                // Campaign AI does not have long initial REST time
    bool m_playMusic;               // play any music?
    float m_cameraDragMoveSpeed;          // speed of camera when dragging mouse (default = 0.5f)
    float m_cameraBorderOrKeyMoveSpeed;   // speed of camera when hitting mouse border or pressing keys (default = 0.5f)
    bool m_cameraEdgeMove;              // should move map camera when hitting edges of screen

	bool m_playing;				    // playing or not
    bool m_skirmish;                // playing a skirmish game or not
	int m_screenshot;				// screenshot taking number

    int m_region;                   // what region is selected? (changed by cSelectYourNextConquestState class)
	int m_mission;		            // what mission are we playing? (= techlevel)

	int m_pathsCreated;

    int m_musicVolume;              // volume of the music
    int m_musicType;

    cRectangle *m_mapViewport;

    // Initialization functions
    void init();		            // initialize all game variables
    void missionInit();             // initialize variables for mission loading only
    void setupPlayers();            // initialize players only (combat state initialization)
    bool setupGame();               // only call once, to initialize game object (TODO: in constructor?)
    void shutdown();
    void initSkirmish() const;      // initialize combat state to start a skirmish game
    void createAndPrepareMentatForHumanPlayer(bool allowMissionSelect = true);
    void loadScenario();

    void run();			            // run the game (MAIN LOOP)

    void thinkSlow_stateCombat_evaluatePlayerStatus();

    void thinkFast_combat();
    void thinkFast_state();

    void think_audio();
	void think_mentat();
    void think_fading();

    void initiateFadingOut();        // fade out with current screen_bmp, this is a little game loop itself!
    void prepareMentatForPlayer();

	bool isState(int thisState) const;

    // Use this instead
	void setNextStateToTransitionTo(int newState);

    // Game acts as a facade, delegates to sound player
    void playSound(int sampleId); // Maximum volume
    void playSound(int sampleId, int vol);

    /**
     * Play sound with distance taken into account. iDistance is the distance outside screen.
     * Meaning <= 1 is ON SCREEN > 1 means distance from screen.
    */
    void playSoundWithDistance(int sampleId, int iOnScreen);

    void playVoice(int sampleId, int playerId);
    void playMusicByTypeForStateTransition(int iType);
    bool playMusicByType(int iType, int playerId = HUMAN, bool triggerWithVoice = false);

    int getMaxVolume();

    int getColorFadeSelected(int r, int g, int b) {
        // Fade with all rgb
        return getColorFadeSelected(r, g, b, true, true, true);
    }

    int getColorFadeSelectedRed(int r, int g, int b) {
        return getColorFadeSelected(r, g, b, true, false, false);
    }

    int getColorFadeSelectedGreen(int r, int g, int b) {
        return getColorFadeSelected(r, g, b, false, true, false);
    }

    int getColorFadeSelectedBlue(int r, int g, int b) {
        return getColorFadeSelected(r, g, b, false, false, true);
    }

    int getColorFadeSelected(int r, int g, int b, bool rFlag, bool gFlag, bool bFlag);

    int getColorFadeSelected(int color);

    cMouse *getMouse() {
        return m_mouse; // NOOOO
    }

    void setPlayerToInteractFor(cPlayer *pPlayer);

    // Event handling
    void onNotifyGameEvent(const s_GameEvent &event) override;
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void onEventSpecialLaunch(const s_GameEvent &event);

    static const char* stateString(const int &state) {
        switch (state) {
            case GAME_INITIALIZE: return "GAME_INITIALIZE";
            case GAME_OVER: return "GAME_OVER";
            case GAME_MENU: return "GAME_MENU";
            case GAME_PLAYING: return "GAME_PLAYING";
            case GAME_BRIEFING: return "GAME_BRIEFING";
            case GAME_EDITING: return "GAME_EDITING";
            case GAME_OPTIONS: return "GAME_OPTIONS";
            case GAME_REGION: return "GAME_REGION";
            case GAME_SELECT_HOUSE: return "GAME_SELECT_HOUSE";
            case GAME_TELLHOUSE: return "GAME_TELLHOUSE";
            case GAME_WINNING: return "GAME_WINNING";
            case GAME_WINBRIEF: return "GAME_WINBRIEF";
            case GAME_LOSEBRIEF: return "GAME_LOSEBRIEF";
            case GAME_LOSING: return "GAME_LOSING";
            case GAME_SETUPSKIRMISH: return "GAME_SETUPSKIRMISH";
            case GAME_CREDITS: return "GAME_CREDITS";
            case GAME_MISSIONSELECT: return "GAME_MISSIONSELECT";
            default:
                assert(false);
                break;
        }
        return "";
    }

    void shakeScreen(int duration);
    void reduceShaking();

    cAllegroDataRepository * getDataRepository() {
        return m_dataRepository;
    }

    int getColorPlaceNeutral();

    int getColorPlaceBad();

    int getColorPlaceGood();

    void setWinFlags(int value);
    void setLoseFlags(int value);

    void setMissionLost();
    void setMissionWon();

    // FPS related
    bool isRunningAtIdealFps();
    int getFps();

    void prepareMentatToTellAboutHouse(int house);

    void drawCombatMouse();

    void think_state();

    void thinkSlow();

    bool isTurretsDownOnLowPower() { return m_turretsDownOnLowPower; }
    void setTurretsDownOnLowPower(bool value) { m_turretsDownOnLowPower = value; }

    bool isRocketTurretsDownOnLowPower() { return m_rocketTurretsDownOnLowPower; }
    void setRocketTurretsDownOnLowPower(bool value) { m_rocketTurretsDownOnLowPower = value; }

    bool isDebugMode() { return m_debugMode; }
    void setDebugMode(bool value) { m_debugMode = value; }

private:
    /**
     * Variables start here
     */
    bool m_debugMode;               // ...

    // if true, then turrets won't do anything on low power (both gun and rocket turrets)
    bool m_turretsDownOnLowPower;

    // if true, rocket turrets will not fire rockets when low power
    bool m_rocketTurretsDownOnLowPower;

	std::string m_gameFilename;

    std::unique_ptr<cPlatformLayerInit> m_PLInit;
    std::unique_ptr<cScreenInit> m_Screen;
    std::unique_ptr<cInteractionManager> m_interactionManager;
    cAllegroDataRepository *m_dataRepository;

    std::unique_ptr<cSoundPlayer> m_soundPlayer;

    std::shared_ptr<cReinforcements> m_reinforcements;

    cMouse *m_mouse;
    cKeyboard *m_keyboard;

    cTimeManager m_timeManager;

    std::unique_ptr<cHandleArgument> m_handleArgument;
    std::shared_ptr<cHousesInfo> m_Houses;

    bool m_missionWasWon;               // hack: used for state transitioning :/

	int m_state;

    int m_newMusicSample;
    int m_newMusicCountdown;

	cAbstractMentat *m_mentat;          // TODO: Move this into a m_currentState class (as field)?

    float m_fadeSelect;                 // fade color when selected
    bool m_fadeSelectDir;               // fade select direction

    bool m_drawFps;

    // screen shaking
    int m_shakeX;
    int m_shakeY;

    int m_TIMER_shake;
    int m_TIMER_evaluatePlayerStatus;

    // win/lose flags
    int8_t m_winFlags, m_loseFlags;

    int m_frameCount, m_fps;            // fps and such

    int m_nextState;

    // the current game state we are running
    cGameState *m_currentState;

    cGameState *m_states[GAME_MAX_STATES];

    bool loadSettings(std::shared_ptr<cIniFile> settings);
    void updateMouseAndKeyboardStateAndGamePlaying(); // ugly name, to point out this does two things :/
    void drawState();           // draws currentState, or calls any of the other functions which don't have state obj yet
    void drawStateCombat();		// the combat part (main) of the game
    void drawStateMenu();		// main menu
    void drawStateWinning();    // drawStateWinning (during combat you get the window "you have been successful"),
                                // after clicking you get to debrief

    void drawStateLosing();     // drawStateLosing (during combat you get the window "you have lost"),
                                // after clicking you get to debrief


    void drawStateMentat(cAbstractMentat *mentat);  // state mentat talking and interaction

    void shakeScreenAndBlitBuffer();
    void handleTimeSlicing();

    bool isResolutionInGameINIFoundAndSet();
    void setScreenResolutionFromGameIniSettings();

    void initPlayers(bool rememberHouse) const;

    void install_bitmaps();

    [[nodiscard]] bool isMissionWon() const;

    [[nodiscard]] bool isMissionFailed() const;

    [[nodiscard]] bool hasGameOverConditionHarvestForSpiceQuota() const;

    [[nodiscard]] bool hasGameOverConditionPlayerHasNoBuildings() const;

    [[nodiscard]] bool hasWinConditionHumanMustLoseAllBuildings() const;

    [[nodiscard]] bool hasWinConditionAIShouldLoseEverything() const;

    [[nodiscard]] bool allEnemyAIPlayersAreDestroyed() const;

    [[nodiscard]] bool hasGameOverConditionAIHasNoBuildings() const;

    void transitionStateIfRequired();

    void setState(int newState);

    void saveBmpScreenToDisk();

    // Combat state specific event handling for now
    void onNotifyKeyboardEventGamePlaying(const cKeyboardEvent &event);
    void onKeyDownGamePlaying(const cKeyboardEvent &event);
    void onKeyPressedGamePlaying(const cKeyboardEvent &event);

    void thinkSlow_state();

    void onKeyDownDebugMode(const cKeyboardEvent &event);
};
