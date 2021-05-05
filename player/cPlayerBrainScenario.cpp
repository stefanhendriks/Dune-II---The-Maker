#include <algorithm>
#include "../include/d2tmh.h"

cPlayerBrainScenario::cPlayerBrainScenario(cPlayer * player) : cPlayerBrain(player) {

}

cPlayerBrainScenario::~cPlayerBrainScenario() {

}

void cPlayerBrainScenario::think() {
    char msg[255];
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "cPlayerBrainScenario::think(), for player [%d] - FINISHED", player_->getId());
    logbook(msg);
}
