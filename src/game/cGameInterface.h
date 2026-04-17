#pragma once

class cGame;
class cMouse;
class cPlayer;
class cDrawManager;
class cMapCamera;

class cGameInterface
{
public:
    cGameInterface(cGame* game);
    virtual ~cGameInterface() = default;

    cMouse* getMouse() const;
    void drawCursor() const;
    cDrawManager* getDrawManager() const;
    cMapCamera* getMapCamera() const;

    void prepareMentatToTellAboutHouse(int house) const;

    void setTransitionToWithFadingOut(int newState) const;

    void setPlayerToInteractFor(cPlayer *pPlayer) const;
private:
    cGame* m_igame = nullptr;
};