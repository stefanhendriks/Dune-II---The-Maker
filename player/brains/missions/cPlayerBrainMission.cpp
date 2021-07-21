#include <algorithm>

#include "include/d2tmh.h"

// these are kinds of missions we can execute (has more elaborate logic here)
#include "cPlayerBrainMissionKindAttack.h"
#include "cPlayerBrainMissionKindExplore.h"
#include "cPlayerBrainMissionKindDeathHand.h"
#include "cPlayerBrainMissionKindSaboteur.h"

namespace brains {

    cPlayerBrainMission::cPlayerBrainMission(cPlayer *player, const ePlayerBrainMissionKind &kind,
                                             cPlayerBrain *brain, std::vector<S_groupKind> group,
                                             int initialDelay, int uniqueId) :
                                             player(player),
                                             kind(kind),
                                             state(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_INITIAL_DELAY),
                                             brain(brain),
                                             group(group),
                                             TIMER_delay(initialDelay),
                                             uniqueIdentifier(uniqueId) {
        units = std::vector<int>();

        missionKind = nullptr;
        switch (kind) {
            case PLAYERBRAINMISSION_KIND_ATTACK:
                missionKind = new cPlayerBrainMissionKindAttack(player, this);
                break;
            case PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR:
                missionKind = new cPlayerBrainMissionKindSaboteur(player, this);
                break;
            case PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND:
                missionKind = new cPlayerBrainMissionKindDeathHand(player, this);
                break;
            case PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN:
                missionKind = new cPlayerBrainMissionKindAttack(player, this);
                break;
            case PLAYERBRAINMISSION_KIND_DEFEND:
                //TODO: Defend mission...
//                missionKind = new cPlayerBrainMissionKindAttack(player, this);
                break;
            case PLAYERBRAINMISSION_KIND_EXPLORE:
                missionKind = new cPlayerBrainMissionKindExplore(player, this);
                break;
        }
    }

    cPlayerBrainMission::~cPlayerBrainMission() {
        if (missionKind) {
            delete missionKind;
        }
    }

    void cPlayerBrainMission::think() {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::think(), for player [%d]", player->getId());
        logbook(msg);

        switch (state) {
            case ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_INITIAL_DELAY:
                thinkState_InitialDelay();
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

        if (missionKind) {
            missionKind->onNotify(event);
        }
    }

    void cPlayerBrainMission::onEventDeviated(const s_GameEvent &event) {
        if (event.entityType == UNIT) {
            cUnit &entityUnit = unit[event.entityID];
            if (entityUnit.getPlayer() != player) {
                // not our unit, if it was in our units list, remove it.
                removeUnitIdFromListIfPresent(event.entityID);
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

                    changeState(PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES);
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
                        // update bookkeeping that we have produced something
                        for (auto &thingIWant : group) {
                            // straightforward type match
                            if (thingIWant.type == event.entitySpecificType) {
                                // in case we have multiple entries with same type we check for produced vs required
                                if (thingIWant.produced < thingIWant.required) {
                                    // assign this unit to my team and my mission
                                    units.push_back(entityUnit.iID);
                                    entityUnit.assignMission(uniqueIdentifier);

                                    thingIWant.produced++; // increase produced amount
                                    break;
                                }
                            } else if (thingIWant.buildType == eBuildType::SPECIAL) {
                                // or a special kind of thing I ordered should produce a unit
                                if (specials[thingIWant.type].providesType == eBuildType::UNIT) {
                                    // it provides a unit AND the kind of unit it provides matches that what
                                    // has been created... then we *also* are interested.
                                    if (specials[thingIWant.type].providesTypeId == event.entitySpecificType) {
                                        // in case we have multiple entries with same type we check for produced vs required

                                        // do not look at produced property, because we should have only ONE SPECIAL KIND
                                        // of mission anyway. (ie one FREMEN or one SABOTEUR producing, etc)
                                        // meaning we assign any unit that derived from this to our group.

//                                        if (thingIWant.produced < thingIWant.required) {
                                            // assign this unit to my team and my mission
                                            units.push_back(entityUnit.iID);
                                            entityUnit.assignMission(uniqueIdentifier);

                                            thingIWant.produced++; // increase produced amount
                                            break;
//                                        }
                                    }
                                }
                            }

                        }

                        changeState(PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES);
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

    void cPlayerBrainMission::thinkState_InitialDelay() {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::thinkState_InitialDelay(), for player [%d], delay = %d", player->getId(), TIMER_delay);
        logbook(msg);
        if (TIMER_delay > 0) {
            TIMER_delay--;
        }

        if (TIMER_delay < 1) {
            // start gathering resources now
            changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES);
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
            changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
            return;
        }

        // assumes group contains units, just create build orders from the desired group of units...?
        for (auto &thingIWant : group) {
            if (thingIWant.produced < thingIWant.required) {
                if (thingIWant.ordered <= thingIWant.produced) { // order one at a time only
                    brain->addBuildOrder((S_buildOrder) {
                            buildType : thingIWant.buildType,
                            priority : 0,
                            buildId : thingIWant.type,
                            placeAt : -1,
                            state : buildOrder::eBuildOrderState::PROCESSME,
                    });
                    // remember we placed a build order
                    thingIWant.ordered++;
                }
            }
        }

        // now we wait until it is finished
        changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES);
    }

    void cPlayerBrainMission::thinkState_SelectTarget() {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::thinkState_SelectTarget(), for player [%d]", player->getId());
        logbook(msg);

        if (missionKind) {
            // on successful selecting target, go to execute state.
            // TODO: make sure to not repeat select target step every frame? (have some delay?)
            if (missionKind->think_SelectTarget()) {
                changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE);
            }
        } else {
            changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE);
        }
    }

    void cPlayerBrainMission::thinkState_Execute() {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::thinkState_Execute(), for player [%d]", player->getId());
        logbook(msg);

        if (missionKind) {
            missionKind->think_Execute();
        }

        if (units.empty()) {
            char msg[255];
            sprintf(msg,
                    "cPlayerBrainMission::thinkState_Execute(), for player [%d], group of units is destroyed. Setting state to PLAYERBRAINMISSION_STATE_ENDED",
                    player->getId());
            logbook(msg);

            changeState(PLAYERBRAINMISSION_STATE_ENDED);
            return;
        }

        bool teamIsStillAlive = false;
        for (auto &myUnit : units) {
            cUnit &aUnit = unit[myUnit];
            if (aUnit.isValid() &&
                aUnit.isAssignedMission(uniqueIdentifier)) { // in case this unit ID was re-spawned...
                teamIsStillAlive = true;
                break;
            }
        }

        if (!teamIsStillAlive) {
            // HACK HACK: somehow missed an event?
            logbook("cPlayerBrainMission::thinkState_Execute(): team is no longer valid / alive. Going to PLAYERBRAINMISSION_STATE_ENDED state. (HACK)");
            changeState(PLAYERBRAINMISSION_STATE_ENDED);
            return;
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

    void cPlayerBrainMission::changeState(ePlayerBrainMissionState newState) {
        char msg[255];
        sprintf(msg, "cPlayerBrainMission::changeState(), for player [%d] - mission [%s] - from %s to %s", player->getId(),
                missionKind ? missionKind->toString() : "NO_MISSION",
                ePlayerBrainMissionStateString(state),
                ePlayerBrainMissionStateString(newState));
        logbook(msg);
        state = newState;
    }

    std::vector<int> & cPlayerBrainMission::getUnits() {
        return units;
    }

    cPlayerBrainMission &cPlayerBrainMission::operator=(const cPlayerBrainMission &rhs) {
        // Guard self assignment
        if (this == &rhs)
            return *this;

        this->player = rhs.player;
        this->state = rhs.state;
        if (rhs.missionKind) {
            this->missionKind = rhs.missionKind->clone(player, this);
        } else {
            this->missionKind = nullptr;
        }
        this->uniqueIdentifier = rhs.uniqueIdentifier;
        this->units = rhs.units;
        this->group = rhs.group;
        this->brain = rhs.brain;
        this->kind = rhs.kind;
        this->TIMER_delay = rhs.TIMER_delay;

        return *this;
    }

    cPlayerBrainMission::cPlayerBrainMission(const cPlayerBrainMission &src) :
        player(src.player),
        state(src.state),
        uniqueIdentifier(src.uniqueIdentifier),
        units(src.units),
        group(src.group),
        brain(src.brain),
        kind(src.kind),
        TIMER_delay(src.TIMER_delay)
    {
        if (src.missionKind) {
            missionKind = src.missionKind->clone(src.player, this);
        } else {
            missionKind = nullptr;
        }
    }

}