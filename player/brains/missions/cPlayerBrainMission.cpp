#include <algorithm>

#include "include/d2tmh.h"

namespace brains {

    cPlayerBrainMission::cPlayerBrainMission(cPlayer *player, const ePlayerBrainMissionKind &kind,
                                             cPlayerBrainCampaign *brain, std::vector<S_groupKind> group) : player(
            player), kind(kind), state(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES), brain(brain),
                                                                                                            group(group) {
        units = std::vector<int>();
        targetCell = -1;
        targetStructureID = -1;
        targetUnitID = -1;
    }

    cPlayerBrainMission::~cPlayerBrainMission() {

    }

    void cPlayerBrainMission::think() {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::think(), for player [%d]", player->getId());
        logbook(msg);

        switch (state) {
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
            case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_ENDED:
                // do nothing
                break;
        }
    }

    void cPlayerBrainMission::onNotify(const s_GameEvent &event) {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::onNotify(), for player [%d] -> %s", player->getId(), event.toString(event.eventType));
        logbook(msg);

        switch(event.eventType) {
            case GAME_EVENT_CREATED:
                onEventCreated(event);
                break;
            case GAME_EVENT_DESTROYED:
                onEventDestroyed(event);
                break;
            case GAME_EVENT_DEVIATED:
                onEventDeviated(event);
                break;
        }
    }

    void cPlayerBrainMission::onEventDeviated(const s_GameEvent &event) {
        if (event.entityType == UNIT) {
            cUnit &entityUnit = unit[event.entityID];
            if (entityUnit.getPlayer() != player) {
                // not our unit, if it was in our units list, remove it.
                removeUnitIdFromListIfPresent(event.entityID);
            } else {
                // the unit is ours, if it was a target, then we can forget it.
                if (targetUnitID == event.entityID) {
                    // our target got deviated, so it is no longer a threat
                    targetUnitID = -1;
                    targetCell = -1;
                    state = PLAYERBRAINMISSION_STATE_SELECT_TARGET;
                }
            }
        }
    }

    void cPlayerBrainMission::onEventDestroyed(const s_GameEvent &event) {
        if (event.entityType == UNIT) {
            if (event.player == player) {
                removeUnitIdFromListIfPresent(event.entityID);

                // it is an event about my own stuff
                if (state == PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES || state == PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES) {
                    cUnit &entityUnit = unit[event.entityID];

                    auto position = std::find(units.begin(), units.end(), event.entityID);
                    if (position != units.end()) {
                        // found unit in our list, so a unit we produced before and assigned to our
                        // team got destroyed!

                        // update bookkeeping that we have to produce it again
                        for (auto &unitIWant : group) {
                            if (unitIWant.type == event.entitySpecificType) {
                                // in case we have multiple entries with same type we check if > 0
                                if (unitIWant.produced > 0) {
                                    unitIWant.produced--; // decrease produced amount
                                    break;
                                }
                            }
                        }
                    }

                    state = PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES;
                }
            } else {
                // our target got destroyed
                if (targetUnitID == event.entityID) {
                    targetUnitID = -1;
                    targetCell = -1;
                    state = PLAYERBRAINMISSION_STATE_SELECT_TARGET;
                }
            }
        } else if (event.entityType == STRUCTURE) {
            if (event.player != player) {
                // our target got destroyed
                if (targetStructureID == event.entityID) {
                    targetStructureID = -1;
                    targetCell = -1;
                    state = PLAYERBRAINMISSION_STATE_SELECT_TARGET;
                }
            }
        }
    }

    void cPlayerBrainMission::onEventCreated(const s_GameEvent &event) {
        // since we are assembling resources, listen to these events.
        if (event.player == player) {
            // it is an event about my own stuff
            if (state == PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES || state == PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES) {
                // unit got created, not reinforced
                if (event.entityType == UNIT && !event.isReinforce) {
                    cUnit &entityUnit = unit[event.entityID];

                    // this unit has not been assigned to a mission yet
                    if (!entityUnit.isAssignedAnyMission()) {
                        // assign this unit to my team and my mission
                        units.push_back(entityUnit.iID);
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

    /**
     * Scans our 'units' list for the unitIdToRemove, if present, remove it.
     * @param unitIdToRemove
     */
    void cPlayerBrainMission::removeUnitIdFromListIfPresent(int unitIdToRemove) {
        auto position = std::find(units.begin(), units.end(), unitIdToRemove);
        if (position != units.end()) {
            // found unit in our list, so someone of ours got destroyed!
            units.erase(position);
        }
    }

    void cPlayerBrainMission::thinkState_PrepareGatherResources() {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::thinkState_PrepareGatherResources(), for player [%d]", player->getId());
        logbook(msg);

        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainMission::thinkState_PrepareGatherResources(), for player [%d]: this is the army I have so far...", player->getId());
        logbook(msg);

        for (auto &myUnitId : units) {
            cUnit &myUnit = unit[myUnitId];
            memset(msg, 0, sizeof(msg));
            sprintf(msg, "cPlayerBrainMission::thinkState_PrepareGatherResources(), for player [%d]: Unit %d, type %d (%s)", player->getId(), myUnit.iID, myUnit.iType, myUnit.getUnitType().name);
            logbook(msg);
        }

        bool somethingLeftToBuild = false;
        for (auto &unitIWant : group) {
            if (unitIWant.produced < unitIWant.required) {
                somethingLeftToBuild = true;
                break;
            }
        }

        if (!somethingLeftToBuild) {
            char msg[255];
            sprintf(msg,
                    "cPlayerBrainMission::thinkState_PrepareGatherResources(), for player [%d] - nothing left to build",
                    player->getId());
            logbook(msg);
            state = PLAYERBRAINMISSION_STATE_SELECT_TARGET;
            return;
        }

        // assumes group contains units, just create build orders from the desired group of units...?
        for (auto &unitIWant : group) {
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

        if (kind == ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_ATTACK) {
            // and execute whatever?? (can merge with select target state?)
            for (int i = 0; i < MAX_STRUCTURES; i++) {
                cAbstractStructure *theStructure = structure[i];
                if (!theStructure) continue;
                if (!theStructure->isValid()) continue;
                if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies and self
                // enemy structure
                targetCell = theStructure->getCell();
                targetStructureID = theStructure->getStructureId();
                if (rnd(100) < 25) {
                    break; // this way we kind of have randomly another target...
                }
            }
        } else if (kind == ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_EXPLORE) {
            targetCell = map.getRandomCellWithinMapWithSafeDistanceFromBorder(2);
        }

        state = ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE;
    }

    void cPlayerBrainMission::thinkState_Execute() {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::thinkState_Execute(), for player [%d]", player->getId());
        logbook(msg);

        if (units.empty()) {
            char msg[255];
            sprintf(msg, "cPlayerBrainMission::thinkState_Execute(), for player [%d], group of units is destroyed. Setting state to PLAYERBRAINMISSION_STATE_ENDED", player->getId());
            logbook(msg);

            state = PLAYERBRAINMISSION_STATE_ENDED;
            return;
        }

        bool teamIsStillAlive = false;
        for (auto &myUnit : units) {
            cUnit &aUnit = unit[myUnit];
            if (aUnit.isValid() &&
                aUnit.isAssignedMission(this)) { // in case this unit ID was re-spawned...
                teamIsStillAlive = true;
                break;
            }
        }

        if (!teamIsStillAlive) {
            // HACK HACK: somehow missed an event?
            logbook("cPlayerBrainMission::thinkState_Execute(): team is no longer valid / alive. Going to PLAYERBRAINMISSION_STATE_ENDED state. (HACK)");
            state = PLAYERBRAINMISSION_STATE_ENDED;
            return;
        }


        // executing mission
        if (kind == ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_ATTACK) {
           for (auto &myUnit : units) {
                cUnit &aUnit = unit[myUnit];
                if (aUnit.isValid() && aUnit.isIdle()) {
                    logbook("cPlayerBrainMission::thinkState_Execute(): Ordering unit to attack!");
                    UNIT_ORDER_ATTACK(myUnit, targetCell, -1, targetStructureID, -1);
                }
            }
        } else if (kind == ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_EXPLORE) {
            for (auto &myUnit : units) {
                cUnit &aUnit = unit[myUnit];
                if (aUnit.isValid()) {
                    if (aUnit.isIdle()) {
                        if (map.distance(aUnit.getCell(), targetCell) < 4) {
                            targetCell = -1;
                            state = PLAYERBRAINMISSION_STATE_SELECT_TARGET; // select new target
                        } else {
                            aUnit.move_to(targetCell);
                        }
                    } else {
                        if (map.distance(aUnit.getCell(), targetCell) < 2) {
                            // almost there. Select new target.
                            targetCell = -1;
                            state = PLAYERBRAINMISSION_STATE_SELECT_TARGET; // select new target
                        }
                    }
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

    bool cPlayerBrainMission::isEnded() const {
        return state == PLAYERBRAINMISSION_STATE_ENDED;
    }

}