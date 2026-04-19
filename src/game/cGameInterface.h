#pragma once

#include "utils/Color.hpp"

class cGame;
class cMouse;
class cPlayer;
class cDrawManager;
class cMapCamera;
class cReinforcements;
struct s_GameEvent;
struct s_PreviewMap;
class Texture;
class cDrawManager;

class cGameInterface
{
public:
    cGameInterface(cGame* game);
    virtual ~cGameInterface() = default;

    cMouse* getMouse() const;
    void drawCursor() const;
    cDrawManager* getDrawManager() const;
    cMapCamera* getMapCamera() const;
    cReinforcements* getReinforcements() const;
    cDrawManager* getRenderDrawManager() const;
    void setMissionWon() const;
    void setMissionLost() const;
    void missionInit() const;
    void setupPlayers() const;

    void onNotifyGameEvent(const s_GameEvent &event) const;

    void prepareMentatToTellAboutHouse(int house) const;
    void prepareMentatForPlayer() const;
    void loadScenario() const;

    void setTransitionToWithFadingOut(int newState) const;
    void initiateFadingOut() const;
    void setNextStateToTransitionTo(int newState) const;
    void loadSkirmishMaps() const;
    void initSkirmish() const;
    void loadMapFromEditor(s_PreviewMap *map) const;
    void setPlayerToInteractFor(cPlayer *pPlayer) const;
    void jumpToSelectYourNextConquestMission(int index) const;
    void loadMapFromEditor(int iSkirmishMap) const;
    void checkMissionWinOrFail() const;
    void goingToWinLoseBrief(int winOrLoseBrief) const;
    Texture* getScreenTexture() const;
    void drawTextFps() const;
    void drawTextTime() const;
    int getCurrentState() const;

    void reduceShaking() const;
    Color getColorFadeSelected(int r, int g, int b) const;
    void setWinFlags(int value) const;
    void setLoseFlags(int value) const;
    bool playMusicByType(int iType) const;
private:
    cGame* m_igame = nullptr;
};