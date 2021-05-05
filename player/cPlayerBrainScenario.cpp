#include <algorithm>
#include "../include/d2tmh.h"

cPlayerBrainScenario::cPlayerBrainScenario(cPlayer * player) : cPlayerBrain(player) {
    TIMER_initialDelay = 1000;
}

cPlayerBrainScenario::~cPlayerBrainScenario() {

}

void cPlayerBrainScenario::think() {
    TIMER_initialDelay--;
    if (TIMER_initialDelay > 0) {
        char msg[255];
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainScenario::think(), for player [%d] - Initial Delay still active...", player_->getId());
        logbook(msg);
        return;
    }

    // now do some real stuff


    char msg[255];
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "cPlayerBrainScenario::think(), for player [%d] - FINISHED", player_->getId());
    logbook(msg);
}
