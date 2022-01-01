#include "include/d2tmh.h"

#include "cPlayerBrainMissionKindFremen.h"

namespace brains {

    cPlayerBrainMissionKindFremen::cPlayerBrainMissionKindFremen(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
        specificEventTypeToGoToSelectTargetState = eGameEventType::GAME_EVENT_CREATED; // saboteur created
        specificBuildTypeToGoToSelectTargetState = sSpecialInfo[SPECIAL_FREMEN].providesType;
        specificBuildIdToGoToSelectTargetState = sSpecialInfo[SPECIAL_FREMEN].providesTypeId;
        specificPlayerForEventToGoToSelectTargetState = &players[AI_CPU5];
    }

    cPlayerBrainMissionKindFremen::~cPlayerBrainMissionKindFremen() {

    }

    bool cPlayerBrainMissionKindFremen::think_SelectTarget() {
        return true;
    }

    void cPlayerBrainMissionKindFremen::think_Execute() {
        mission->changeState(PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES); // keep awaiting resources
    }

    void cPlayerBrainMissionKindFremen::onNotify(const s_GameEvent &event) {
        cPlayerBrainMissionKind::onNotify(event);
    }

    cPlayerBrainMissionKind *cPlayerBrainMissionKindFremen::clone(cPlayer *player, cPlayerBrainMission * mission) {
        cPlayerBrainMissionKindFremen *copy = new cPlayerBrainMissionKindFremen(player, mission);
        copy->specificEventTypeToGoToSelectTargetState = specificEventTypeToGoToSelectTargetState;
        copy->specificPlayerForEventToGoToSelectTargetState = specificPlayerForEventToGoToSelectTargetState;
        copy->specificBuildTypeToGoToSelectTargetState = specificBuildTypeToGoToSelectTargetState;
        copy->specificBuildIdToGoToSelectTargetState = specificBuildIdToGoToSelectTargetState;
        return copy;
    }

    void cPlayerBrainMissionKindFremen::onNotify_SpecificStateSwitch(const s_GameEvent &) {
        // NOOP
    }

}