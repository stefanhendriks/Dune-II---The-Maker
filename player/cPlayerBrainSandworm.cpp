#include "../include/d2tmh.h"

cPlayerBrainSandworm::cPlayerBrainSandworm(cPlayer * player) : cPlayerBrain(player) {

}

cPlayerBrainSandworm::~cPlayerBrainSandworm() {

}

void cPlayerBrainSandworm::think() {
    char msg[255];
    sprintf(msg, "cPlayerBrainSandworm::think(), for player [%d]", player_->getId());
    logbook(msg);
}
