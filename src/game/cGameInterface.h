#pragma once

class cGame;
class cMouse;

class cGameInterface
{
public:
    cGameInterface(cGame* game);
    virtual ~cGameInterface() = default;

    cMouse* getMouse() const;
    void drawCursor() const;

    void prepareMentatToTellAboutHouse(int house) const;

    void setTransitionToWithFadingOut(int newState) const;
private:
    cGame* m_igame = nullptr;
};