#include "../include/d2tmh.h"

cPlayerBrainFremenSuperWeapon::cPlayerBrainFremenSuperWeapon(cPlayer * player) : cPlayerBrain(player) {

}

cPlayerBrainFremenSuperWeapon::~cPlayerBrainFremenSuperWeapon() {

}

void cPlayerBrainFremenSuperWeapon::think() {
    char msg[255];
    sprintf(msg, "cPlayerBrainFremenSuperWeapon::think(), for player [%d]", player_->getId());
    logbook(msg);
}
