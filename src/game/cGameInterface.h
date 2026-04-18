#pragma once

class cGame;
class cMouse;
class cPlayer;
class cDrawManager;
class cMapCamera;
class cReinforcements;
struct s_GameEvent;

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
    void setMissionWon() const;
    void setMissionLost() const;

    void onNotifyGameEvent(const s_GameEvent &event) const;

    void prepareMentatToTellAboutHouse(int house) const;

    void setTransitionToWithFadingOut(int newState) const;
    void initiateFadingOut() const;
    void setNextStateToTransitionTo(int newState) const;
    void loadSkirmishMaps() const;
    void initSkirmish() const;
    void setPlayerToInteractFor(cPlayer *pPlayer) const;

    void checkMissionWinOrFail() const;

    void drawTextFps() const;
    void drawTextTime() const;

    void reduceShaking() const;
private:
    cGame* m_igame = nullptr;
};