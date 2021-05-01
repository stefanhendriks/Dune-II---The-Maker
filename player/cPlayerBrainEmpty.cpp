#include "../include/d2tmh.h"

cPlayerBrainEmpty::cPlayerBrainEmpty(cPlayer * player) : cPlayerBrain(player) {

}

cPlayerBrainEmpty::~cPlayerBrainEmpty() {

}

void cPlayerBrainEmpty::think() {
    char msg[255];
    sprintf(msg, "cPlayerBrainEmpty::think(), for player [%d]", player_->getId());
    logbook(msg);
}
