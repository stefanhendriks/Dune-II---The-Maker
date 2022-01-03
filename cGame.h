/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */
#pragma once

#include <controls/cMouse.h>
#include <controls/cKeyboard.h>
#include <observers/cScenarioObserver.h>
#include <data/cAllegroDataRepository.h>
#include <observers/cScenarioObserver.h>
#include <utils/cRectangle.h>
#include <utils/cSoundPlayer.h>
#include "definitions.h"

class cAbstractMentat;
class cGameControlsContext;
class cGameState;
class cInteractionManager;
class cPlayer;

class cGame : public cScenarioObserver, cInputObserver {

public:

	cGame();
	~cGame();

	std::string game_filename;

	bool windowed;				// windowed
	char version[15];			// version number, or name.

    // Alpha (for fading in/out)
    int fadeAlpha;           // 255 = opaque , anything else
    eFadeAction fadeAction;    // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

    // resolution of the game
	int screen_x;
	int screen_y;
    int ini_screen_width;
    int ini_screen_height;

    bool bPlaySound;            // play sound?
    bool bDisableAI;            // disable AI thinking?
    bool bOneAi;                // disable all but one AI brain? (default == false)
    bool bDisableReinforcements;// disable any reinforcements from scenario ini file?
    bool bDrawUsages;           // draw the amount of structures/units/bullets used during combat
    bool bDrawUnitDebug;        // draw the unit debug info (rects, paths, etc)
    bool bNoAiRest;             // Campaign AI does not have long initial REST time
    bool bPlayMusic;            // play any music?
    bool bMp3;                  // use mp3 files instead of midi

	bool bPlaying;				// playing or not
    bool bSkirmish;             // playing a skirmish game  or not
	int screenshot;				// screenshot taking number

	void init();		// initialize all game variables
	void mission_init(); // initialize variables for mission loading only
	void run();			// run the game

    int iRegion;        // what region is selected? (changed by cSelectYourNextConquestState class)
	int iMission;		// what mission are we playing? (= techlevel)

	int paths_created;

    int iMusicVolume;       // volume of the mp3 / midi


    int iMusicType;

    void thinkSlow_combat();

    void winning();       // winning (during combat you get the window "you have been successful"), after clicking you get to debrief
    void losing();        // losing (during combat you get the window "you have lost"), after clicking you get to debrief
    void options();

	void setup_players();

    void think_music();

	void think_mentat();

    void START_FADING_OUT(); // fade out with current screen_bmp, this is a little game loop itself!

    void prepareMentatForPlayer();

	bool setupGame();
	void shutdown();

	bool isState(int thisState);

    // Use this instead
	void setNextStateToTransitionTo(int newState);

	int getMaxVolume() { return iMaxVolume; }

	cSoundPlayer * getSoundPlayer() {
	    return soundPlayer;
	}

	int getGroupNumberFromKeyboard();
    int getGroupNumberFromScanCode(int scanCode);

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

    void think_fading();

    cRectangle * mapViewport;

    void init_skirmish() const;

    void createAndPrepareMentatForHumanPlayer();

    void loadScenario();

    void think_state();

    cMouse *getMouse() {
        return mouse; // NOOOO
    }

    void shakeScreen(int duration);

    void setPlayerToInteractFor(cPlayer *pPlayer);

    void onNotify(const s_GameEvent &event) override;
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override;

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
            default:
                assert(false);
                break;
        }
        return "";
    }

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

    bool isRunningAtIdealFps();

    void resetFrameCount();

    void setFps();

    int getFps();

    void prepareMentatToTellAboutHouse(int house);

    void drawCombatMouse();

private:
    /**
     * Variables start here
     */
    cInteractionManager *_interactionManager;
    cAllegroDataRepository *m_dataRepository;

    cSoundPlayer *soundPlayer;
    cMouse *mouse;
    cKeyboard *keyboard;

    bool missionWasWon; // hack: used for state transitioning :/

	int state;

	int iMaxVolume;

	cAbstractMentat *pMentat; // TODO: Move this into a currentState class (as field)?

    float fade_select;        // fade color when selected
    bool bFadeSelectDir;    // fade select direction

    // screen shaking
    int shake_x;
    int shake_y;
    int TIMER_shake;

    int TIMER_evaluatePlayerStatus;

    // win/lose flags
    int8_t winFlags, loseFlags;

    int frame_count, fps;  // fps and such

    int nextState;

    // the current game state we are running
    cGameState *currentState;

    cGameState *states[GAME_MAX_STATES];

    void updateState();
    void combat();		// the combat part (main) of the game
    bool isMusicPlaying();

    void stateMentat(cAbstractMentat *mentat);  // state mentat talking and interaction
    void menu();		// main menu

    void drawState();
    void shakeScreenAndBlitBuffer();
    void handleTimeSlicing();

    bool isResolutionInGameINIFoundAndSet();
    void setScreenResolutionFromGameIniSettings();

    void install_bitmaps();

    bool isMissionWon() const;

    bool isMissionFailed() const;

    bool hasGameOverConditionHarvestForSpiceQuota() const;

    bool hasGameOverConditionPlayerHasNoBuildings() const;

    bool hasWinConditionHumanMustLoseAllBuildings() const;

    bool hasWinConditionAIShouldLoseEverything() const;

    bool allAIPlayersAreDestroyed() const;

    bool hasGameOverConditionAIHasNoBuildings() const;

    void transitionStateIfRequired();

    void setState(int newState);

    void initPlayers(bool rememberHouse) const;

    void takeScreenshot();

    void onNotifyKeyboardEventGamePlaying(const s_KeyboardEvent &event);

    void onKeyDownGamePlaying(const s_KeyboardEvent &event);

    void onKeyPressedGamePlaying(const s_KeyboardEvent &event);
};
