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
#include "definitions.h"
#include "include/eGameState.h"
#include "utils/cRectangle.h"
#include "observers/cScenarioObserver.h"
#include "utils/Color.hpp"
#include "utils/Log.h"

#include <memory>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class cGameControlsContext;
class cGameObjectContext;
class cInfoContext;
class cGameState;
class cInteractionManager;
class cPlatformLayerInit;
class cPlayer;
class cPlayers;
class cSoundPlayer;
class cScreenInit;
class cHousesInfo;
class cReinforcements;
class cPreviewMaps;
struct InitialGameSettings;
class cGameSettings;

class cStructureFactory;
class cSideBarFactory;
class cBuildingListFactory;

class ContextCreator;
class GameContext;
class cScreenShake;
class cFocusManager;
class cGameConditionChecker;
class cScreenFader;
class SDLDrawer;
class cMapCamera;
class cDrawManager;
class cTimeManager;
class cStructureUtils;
class Texture;

struct s_DataCampaign;
struct s_PreviewMap;
// Naming thoughts:
// member variables, start with m_<camelCasedVariableName>
//
// functions are camelCased()
// exceptions (for now):
// think()
// thinkFast_...()
// thinkSlow_....() --> for now used to distinguish certain speed of "thinking" / invocations
// state_...() --> because elegible for moving away

struct sGameServices;

class cGame : public cScenarioObserver, cInputObserver {

public:
    cGame();
    ~cGame();

    void jumpToSelectYourNextConquestMission(int missionNr);

    void setGameFilename(const std::string &filename) {
        m_gameFilename = filename;
    }

    void getLog(cLog *log) {
        assert(log != nullptr);
        m_log = log;
    }
    cRectangle *m_mapViewport;
    // TODO: move these to a another class that we can pass around, instead of having them as global variables.
    // begin
    cMapCamera					*m_mapCamera;
    cDrawManager                *m_drawManager;
    std::unique_ptr<cStructureUtils>   m_structureUtils;
    // end

    //todo: this should get moved to private, but not yet.
    std::unique_ptr<cGameObjectContext> m_gameObjectsContext;
    
    //todo: this should get moved to private, but not yet.
    std::unique_ptr<cInfoContext> m_infoContext;

    //todo: this should get moved to private, but not yet.
    std::unique_ptr<cGameSettings> m_gameSettings;

    // std::unique_ptr<cStructureFactory> m_structureFactory;
    std::unique_ptr<cSideBarFactory> m_sideBarFactory;
    std::unique_ptr<cBuildingListFactory> m_buildingListFactory;


    // Initialization functions
    void init();		            // initialize all game variables
    void missionInit();             // initialize variables for mission loading only
    void setupPlayers();            // initialize players only (combat state initialization)
    bool setupGame();               // only call once, to initialize game object (TODO: in constructor?)
    void shutdown();
    void initSkirmish() const;      // initialize combat state to start a skirmish game
    void loadSkirmishMaps() const;
    void loadScenario();

    void run();			            // run the game (MAIN LOOP)

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

    Color getColorFadeSelected(int r, int g, int b, bool rFlag = true, bool gFlag = true, bool bFlag = true);

    cMouse *getMouse() {
        return m_mouse; // NOOOO
    }

    void setPlayerToInteractFor(cPlayer *pPlayer);
    void setMousePosition(int w, int h);


    // Event handling
    void onNotifyGameEvent(const s_GameEvent &event) override;
    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void onEventSpecialLaunch(const LaunchDeathHandEvent &event) const;
    void onEventEntityDestroyed(const s_GameEvent & event);
    void onEventCreateUnit(const DeployUnitEvent &event);

    void shakeScreen(int duration);
    void reduceShaking() const;

    void setWinFlags(int value);
    void setLoseFlags(int value);

    void setMissionLost();
    void setMissionWon();

    void prepareMentatToTellAboutHouse(int house);

    void thinkFast();
    void thinkNormal();
    void thinkSlow();
    void thinkCache();

    bool isTurretsDownOnLowPower() {
        return m_turretsDownOnLowPower;
    }

    bool isRocketTurretsDownOnLowPower() {
        return m_rocketTurretsDownOnLowPower;
    }

    void applySettings(std::unique_ptr<InitialGameSettings> gs);
    void changeStateFromMentat();
    void loadMapFromEditor(int map);
    void loadMapFromEditor(s_PreviewMap *map);

    Texture* getScreenTexture() const {
        return screenTexture;
    }
    void takeBackGroundScreen();

    void goingToWinLoseBrief(int value);

    cReinforcements* getReinforcements() const;

    s_DataCampaign* getDataCampaign() const;

    int getCurrentState() const;

    void drawTextFps() const;
    void drawTextTime() const;
    void checkMissionWinOrFail();

private:
    bool m_playSound;                       // play sound?
    bool m_windowed;        			    // windowed
    bool m_pauseWhenLosingFocus;            // pausing the game when losing focus
    float m_cameraDragMoveSpeed;            // speed of camera when dragging mouse (default = 0.5f)
    float m_cameraBorderOrKeyMoveSpeed;     // speed of camera when hitting mouse border or pressing keys (default = 0.5f)
    bool m_cameraEdgeMove;                  // should move map camera when hitting edges of screen
    int m_musicVolume;                      // volume of the music

    // if true, then turrets won't do anything on low power (both gun and rocket turrets)
    bool m_turretsDownOnLowPower;
    // if true, rocket turrets will not fire rockets when low power
    bool m_rocketTurretsDownOnLowPower;

    std::string m_gameFilename;

    std::unique_ptr<cPlatformLayerInit> m_PLInit;
    std::unique_ptr<cScreenInit> m_Screen;
    std::unique_ptr<cInteractionManager> m_interactionManager;
    std::unique_ptr<cFocusManager> m_focusManager;

    cSoundPlayer* m_soundPlayer;

    std::shared_ptr<cPreviewMaps> m_PreviewMaps;

    std::shared_ptr<cReinforcements> m_reinforcements;
    std::unique_ptr<InitialGameSettings> m_initialGameSettings;

    // Non-owning shortcut to the players container (owned by m_gameObjectsContext)
    cPlayers* m_players = nullptr;

    cMouse *m_mouse;
    cKeyboard *m_keyboard;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Texture *screenTexture=nullptr;
    Texture *actualRenderer= nullptr;
    cTextDrawer* m_textDrawer = nullptr;
    cTimeManager* m_timeManager;
    SDLDrawer *m_renderDrawer = nullptr;

    std::unique_ptr<cHousesInfo> m_Houses;
    bool m_missionWasWon;               // hack: used for state transitioning :/

    int m_newMusicSample;
    int m_newMusicCountdown;

    std::unique_ptr<cScreenShake> m_screenShake;

    void thinkFast_audio();
    void thinkFast_fading();


    int m_state;
    int m_nextState;
    // the current game state we are running
    cGameState *m_currentState;
    cGameState *m_states[GAME_MAX_STATES];
    void transitionStateIfRequired();
    void setState(int newState);

    void updateMouseAndKeyboardState();
    void drawState();

    void shakeScreenAndBlitBuffer();

    void saveBmpScreenToDisk();

    void onKeyDownGame(const cKeyboardEvent &event);
    void onKeyPressedGame(const cKeyboardEvent &event);
    void onKeyDownDebugMode(const cKeyboardEvent &event);

    void fadeOutOrBlitScreenBuffer() const;

    std::unique_ptr<GameContext> ctx;
    std::unique_ptr<ContextCreator> context;
    std::unique_ptr<s_DataCampaign> m_dataCampaign;
    std::unique_ptr<cGameConditionChecker> m_gameConditionChecker;
    std::unique_ptr<cScreenFader> m_cScreenFader;

    std::unique_ptr<sGameServices> m_services;

    cLog* m_log = nullptr;
};
