#ifndef D2TM_CGAMESTATE_H
#define D2TM_CGAMESTATE_H

#include "../observers/cMouseObserver.h"

enum eGameStateType {
    GAMESTATE_MAIN_MENU,
    GAMESTATE_CHOOSE_HOUSE,
    GAMESTATE_SELECT_YOUR_NEXT_CONQUEST,
    GAMESTATE_OPTIONS,
    GAMESTATE_SETUP_SKIRMISH_GAME,
};

class cGameState : cMouseObserver {

public:
    cGameState(cGame& theGame);
    virtual ~cGameState();

    virtual void thinkFast() = 0;
    virtual void draw() const = 0;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;

    virtual eGameStateType getType() = 0;

protected:
    // the state of the game itself
    cGame& game;

private:

};


#endif //D2TM_CGAMESTATE_H
