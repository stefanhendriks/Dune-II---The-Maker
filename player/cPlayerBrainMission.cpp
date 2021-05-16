#include "../include/d2tmh.h"


cPlayerBrainMission::cPlayerBrainMission(cPlayer * player, const ePlayerBrainMissionKind & kind) : player(player), kind(kind), state(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_INITIAL) {
    buildOrders = std::vector<S_buildOrder>();
    group = std::vector<S_groupKind>();
}

cPlayerBrainMission::~cPlayerBrainMission() {

}

std::vector<S_buildOrder> cPlayerBrainMission::think() {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::think(), for player [%d]", player->getId());
    logbook(msg);

    switch (state) {
        case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_INITIAL:
            thinkState_Initial();
            break;
        case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES:
            thinkState_PrepareGatherResources();
            break;
        case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES:
            thinkState_PrepareAwaitResources();
            break;
        case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_SELECT_TARGET:
            thinkState_SelectTarget();
            break;
        case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE:
            thinkState_Execute();
            break;
    }

    return buildOrders;
}

void cPlayerBrainMission::onNotify(const s_GameEvent &event) {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::onNotify(), for player [%d] -> %s", player->getId(), event.toString(event.eventType));
    logbook(msg);

    // PREPARE RESOURCES STATE...
    // check if unit is created we have interest in, if so, mark it as 'mine' (my team). (mark unit as well?)
    // ... once all units are marked, we get to the next state...
}

void cPlayerBrainMission::thinkState_Initial() {
    // determine what kind of attack group we want? (or should be be told upon mission creation?...)
    group.push_back((S_groupKind) {
        type : QUAD,
        amount: 1,
    });

    state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES;
}

void cPlayerBrainMission::thinkState_PrepareGatherResources() {
    // assumes group contains units, just create build orders from the desired group of units...?
    for (auto & unitIWant : group) {
        for (int i = 0; i < unitIWant.amount; i++) {
            buildOrders.push_back((S_buildOrder) {
                    buildType : eBuildType::UNIT,
                    priority : 0,
                    buildId : unitIWant.type,
                    placeAt : -1,
                    state : buildOrder::eBuildOrderState::PROCESSME,
            });
        }
    }

    // now we wait until we have all we need...
    state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES;
}

void cPlayerBrainMission::thinkState_SelectTarget() {
    buildOrders.clear();
    state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE;

    // etc
}

void cPlayerBrainMission::thinkState_Execute() {
    // and execute whatever?? (can merge with select target state?)
}

void cPlayerBrainMission::thinkState_PrepareAwaitResources() {
    // we have all units we need, select target to ...
    state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_SELECT_TARGET;
}
