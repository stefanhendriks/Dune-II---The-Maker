#include "include/d2tmh.h"

#include "cPlayerBrainMissionKindFremen.h"

namespace brains {

    cPlayerBrainMissionKindFremen::cPlayerBrainMissionKindFremen(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
    }

    cPlayerBrainMissionKindFremen::~cPlayerBrainMissionKindFremen() {

    }

    bool cPlayerBrainMissionKindFremen::think_SelectTarget() {
    }

    void cPlayerBrainMissionKindFremen::think_Execute() {
        mission->changeState(PLAYERBRAINMISSION_STATE_ENDED); // end immediately, because fremen super weapon logic is within other class
    }

    void cPlayerBrainMissionKindFremen::onNotify(const s_GameEvent &event) {

    }

    cPlayerBrainMissionKind *cPlayerBrainMissionKindFremen::clone(cPlayer *player, cPlayerBrainMission * mission) {
        cPlayerBrainMissionKindFremen *copy = new cPlayerBrainMissionKindFremen(player, mission);
        return copy;
    }

}