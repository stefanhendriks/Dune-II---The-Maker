#ifndef D2TM_CSETUPSKIRMISHGAMESTATE_H
#define D2TM_CSETUPSKIRMISHGAMESTATE_H

class cGame;

#include <drawers/cTextDrawer.h>
#include "cGameState.h"

struct s_SkirmishPlayer {
    bool bPlaying;
    int startingUnits;
    int iCredits;
    int iHouse;
    int team;
};

class cSetupSkirmishGameState : public cGameState {

public:
    cSetupSkirmishGameState(cGame& theGame);
    ~cSetupSkirmishGameState() override;

    void think() override;
    void draw() override;
    void interact() override;

    eGameStateType getType() override;

private:
    cTextDrawer textDrawer;
    s_SkirmishPlayer skirmishPlayer[MAX_PLAYERS];
    cMouse * mouse;

    bool spawnWorms;
};

#endif //D2TM_CSETUPSKIRMISHGAMESTATE_H
