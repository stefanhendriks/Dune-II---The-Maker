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

    void cPlayerBrainEmpty::onNotifyGameEvent(const s_GameEvent &) {
        // NOOP
    }

    void cPlayerBrainEmpty::addBuildOrder(S_buildOrder) {
        // NOOP
    }

    void cPlayerBrainEmpty::thinkFast() {
        // NOOP
    }

}