#include "include/d2tmh.h"

namespace brains {

    cPlayerBrainEmpty::cPlayerBrainEmpty(cPlayer *player) : cPlayerBrain(player) {

    }

    cPlayerBrainEmpty::~cPlayerBrainEmpty() {

    }

    void cPlayerBrainEmpty::think() {
//        char msg[255];
//        sprintf(msg, "cPlayerBrainEmpty::think(), for player [%d]", player->getId());
//        logbook(msg);
    }

    void cPlayerBrainEmpty::onNotify(const s_GameEvent &event) {

    }

}