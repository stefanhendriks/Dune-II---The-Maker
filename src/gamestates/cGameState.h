#pragma once

#include "controls/cKeyboardEvent.h"
#include "observers/cInputObserver.h"
#include "sMouseEvent.h"

class cGame;

enum eGameStateType {
    GAMESTATE_MAIN_MENU,
    GAMESTATE_CREDITS,
    GAMESTATE_CHOOSE_HOUSE,
    GAMESTATE_SELECT_YOUR_NEXT_CONQUEST,
    GAMESTATE_OPTIONS,
    GAMESTATE_SETUP_SKIRMISH_GAME,
    GAMESTATE_MISSION_SELECT,
};

class cGameState : cInputObserver {

public:
    explicit cGameState(cGame &theGame);
    virtual ~cGameState() = default;

    virtual void thinkFast() = 0;
    virtual void draw() const = 0;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;
    virtual void onNotifyKeyboardEvent(const cKeyboardEvent &event) override = 0;
    // Do note: not every game state is a scenario observer!

    virtual eGameStateType getType() = 0;

protected:
    // the state of the game itself
    cGame &game;

private:

};

