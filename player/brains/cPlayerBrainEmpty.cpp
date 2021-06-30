#include "include/d2tmh.h"
#include "cPlayerBrainEmpty.h"


namespace brains {

    cPlayerBrainEmpty::cPlayerBrainEmpty(cPlayer *player) : cPlayerBrain(player) {

    }

    cPlayerBrainEmpty::~cPlayerBrainEmpty() {

    }

    void cPlayerBrainEmpty::think() {
        // NOOP
    }

    void cPlayerBrainEmpty::onNotify(const s_GameEvent &event) {
        // NOOP
    }

    void cPlayerBrainEmpty::addBuildOrder(S_buildOrder order) {
        // NOOP
    }

}