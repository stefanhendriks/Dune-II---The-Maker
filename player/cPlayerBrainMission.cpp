#include "../include/d2tmh.h"


cPlayerBrainMission::cPlayerBrainMission(cPlayer * player, const ePlayerBrainMissionKind & kind, cPlayerBrainScenario * brain) : player(player), kind(kind), state(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_INITIAL), brain(brain) {
    group = std::vector<S_groupKind>();
    units = std::vector<int>();
    target = -1;
    targetStructureID = -1;
    targetUnitID = -1;
}

cPlayerBrainMission::~cPlayerBrainMission() {

}

void cPlayerBrainMission::think() {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::think(), for player [%d]", player->getId());
    logbook(msg);

    // Refactor into state objects? (depending on complexity this might be better)
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
}

void cPlayerBrainMission::onNotify(const s_GameEvent &event) {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::onNotify(), for player [%d] -> %s", player->getId(), event.toString(event.eventType));
    logbook(msg);

    if (event.entityOwnerID != player->getId()) {
        // not concerning my entities
        return;
    }

    if (event.eventType == GAME_EVENT_CREATED) {
        // since we are awaiting resources, we claim it.
        if (state == PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES) {
            if (event.entityType == eBuildType::UNIT) {
                cUnit &entityUnit = unit[event.entityID];

                // this unit has not been assigned to a mission yet
                if (!entityUnit.isAssignedAnyMission()) {
                    // assign this unit to my team and my mission
                    units.push_back(event.entityID);
                    entityUnit.assignMission(this);

                    // update bookkeeping that we have produced something
                    for (auto &unitIWant : group) {
                        if (unitIWant.type == event.entitySpecificType) {
                            // in case we have multiple entries with same type we check for produced vs required
                            if (unitIWant.produced < unitIWant.required) {
                                unitIWant.produced++; // increase produced amount
                                break;
                            }
                        }
                    }

                    state = PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES;
                }
            }
        }
    }
}

void cPlayerBrainMission::thinkState_Initial() {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::thinkState_Initial(), for player [%d]", player->getId());
    logbook(msg);

    // determine what kind of attack group we want? (or should be be told upon mission creation?...)
    group.push_back((S_groupKind) {
        type : QUAD,
        required: 3,
        ordered: 0,
        produced: 0,
    });

    state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES;
}

void cPlayerBrainMission::thinkState_PrepareGatherResources() {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::thinkState_PrepareGatherResources(), for player [%d]", player->getId());
    logbook(msg);

    bool somethingLeftToBuild = false;
    for (auto & unitIWant : group) {
        if (unitIWant.produced < unitIWant.required) {
            somethingLeftToBuild = true;
            break;
        }
    }

    if (!somethingLeftToBuild) {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::thinkState_PrepareGatherResources(), for player [%d] - nothing left to build", player->getId());
        logbook(msg);
        state = PLAYERBRAINMISSION_STATE_SELECT_TARGET;
        return;
    }

    // assumes group contains units, just create build orders from the desired group of units...?
    for (auto & unitIWant : group) {
        if (unitIWant.produced < unitIWant.required) {
            if (unitIWant.ordered <= unitIWant.produced) { // order one at a time only
                brain->addBuildOrder((S_buildOrder) {
                        buildType : eBuildType::UNIT,
                        priority : 0,
                        buildId : unitIWant.type,
                        placeAt : -1,
                        state : buildOrder::eBuildOrderState::PROCESSME,
                });
                // remember we placed a build order
                unitIWant.ordered++;
            }
        }
    }

    // now we wait until it is finished
    state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES;
}

void cPlayerBrainMission::thinkState_SelectTarget() {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::thinkState_SelectTarget(), for player [%d]", player->getId());
    logbook(msg);

    // and execute whatever?? (can merge with select target state?)
    const std::vector<int> &myStructureIds = player[0].getAllMyStructuresAsId();
    if(!myStructureIds.empty()) {
        const int &first = myStructureIds.front();
        target = structure[first]->getCell();
        targetStructureID = structure[first]->getStructureId();
    }

    state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE;
}

void cPlayerBrainMission::thinkState_Execute() {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::thinkState_Execute(), for player [%d]", player->getId());
    logbook(msg);

    for (auto & myUnit : units) {
        cUnit &aUnit = unit[myUnit];
        if (!aUnit.isValid()) {

        } else {
            if (aUnit.isIdle()) {
                logbook("cPlayerBrainMission::thinkState_Execute(): Ordering unit to attack!");
                UNIT_ORDER_ATTACK(myUnit, target, -1, targetStructureID, -1);
            }
        }
    }
}

void cPlayerBrainMission::thinkState_PrepareAwaitResources() {
    char msg[255];
    sprintf(msg, "cPlayerBrainMission::thinkState_PrepareAwaitResources(), for player [%d]", player->getId());
    logbook(msg);
    // wait for things to be produced...
}
