#include <algorithm>

#include "include/d2tmh.h"

// these are kinds of missions we can execute (has more elaborate logic here)
#include "cPlayerBrainMissionKindAttack.h"
#include "cPlayerBrainMissionKindExplore.h"
#include "cPlayerBrainMissionKindDeathHand.h"
#include "cPlayerBrainMissionKindSaboteur.h"
#include "cPlayerBrainMissionKindFremen.h"
#include "cPlayerBrainMission.h"


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
        TIMER_awaitingGatheringResoures = -1;
        missionKind = nullptr;
        specialEventMakesStateSwitchToSelectTarget = false;
        missionWithUnits = true; // by default, the missions are executed with units

        switch (kind) {
            case PLAYERBRAINMISSION_KIND_ATTACK:
                missionKind = new cPlayerBrainMissionKindAttack(player, this);
                break;
            case PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR:
                missionKind = new cPlayerBrainMissionKindSaboteur(player, this);
                specialEventMakesStateSwitchToSelectTarget = true;
                break;
            case PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND:
                missionKind = new cPlayerBrainMissionKindDeathHand(player, this);
                specialEventMakesStateSwitchToSelectTarget = true;
                missionWithUnits = false; // this is a non-unit mission
                break;
            case PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN:
                missionKind = new cPlayerBrainMissionKindFremen(player, this);
                specialEventMakesStateSwitchToSelectTarget = true;
                missionWithUnits = false; // this is a non-unit mission , Fremen units are not our own
                break;
            case PLAYERBRAINMISSION_KIND_DEFEND:
                //TODO: Defend mission...
//                missionKind = new cPlayerBrainMissionKindAttack(player, this);
                break;
            case PLAYERBRAINMISSION_KIND_EXPLORE:
                missionKind = new cPlayerBrainMissionKindExplore(player, this);
                break;
        }

        char msg[255];
        sprintf(msg, "Constructing cPlayerBrainMission of type %s", ePlayerBrainMissionKindString(kind));
        log(msg);

        log("Items to produce for mission:");
        for (auto &item : group) {
            char msg[255];
            sprintf(msg, "Item buildType [%s], type/buildId[%d=%s], amount required [%d]", eBuildTypeString(item.buildType), item.type,
                    toStringBuildTypeSpecificType(item.buildType, item.type), item.required);
            log(msg);
            assert(item.type > -1 && "type/buildId must be > -1 !");
        }
    }

    cPlayerBrainMission::~cPlayerBrainMission() {
        if (missionKind) {
            delete missionKind;
        }
        player = nullptr;
        brain = nullptr;
    }

    /**
     * Called every 5 ms.
     */
    void cPlayerBrainMission::think() {
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
        sprintf(msg, "cPlayerBrainMission::onNotify() -> %s", event.toString(event.eventType));
        log(msg);

        switch(event.eventType) {
            case GAME_EVENT_CANNOT_BUILD:
                onEventCannotBuild(event);
                break;
            case GAME_EVENT_CANNOT_CREATE_PATH:
                onEventCannotCreatePath(event);
                break;
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
                                if (sSpecialInfo[thingIWant.type].providesType == eBuildType::UNIT) {
                                    // it provides a unit AND the kind of unit it provides matches that what
                                    // has been created... then we *also* are interested.
                                    if (sSpecialInfo[thingIWant.type].providesTypeId == event.entitySpecificType) {
                                        // in case we have multiple entries with same type we check for produced vs required

                                        // do not look at produced property, because we should have only ONE SPECIAL KIND
                                        // of mission anyway. (ie one FREMEN or one SABOTEUR producing, etc)
                                        // meaning we assign any unit that derived from this to our group.

                                        // assign this unit to my team and my mission
                                        units.push_back(entityUnit.iID);
                                        entityUnit.assignMission(uniqueIdentifier);

                                        thingIWant.produced++; // increase produced amount
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
    }

    /**
     * Scans our 'units' list for the unitIdToRemove, if present, remove it.
     * @param unitIdToRemove
     */
    void cPlayerBrainMission::removeUnitIdFromListIfPresent(int unitIdToRemove) {
        auto position = std::find(units.begin(), units.end(), unitIdToRemove);
        if (position != units.end()) {
            // found unit in our list, so someone of ours got destroyed!
            char msg[255];
            sprintf(msg, "removeUnitIdFromListIfPresent [%d] has removed unit from the list!", unitIdToRemove);
            log(msg);
            units.erase(position);
            unit[unitIdToRemove].unAssignMission();
            log("These units are still available:");
            logUnits();
        }
    }

    void cPlayerBrainMission::thinkState_InitialDelay() {
        char msg[255];
        sprintf(msg, "thinkState_InitialDelay(), delay = %d", TIMER_delay);
        log(msg);
        if (TIMER_delay > 0) {
            TIMER_delay--;
        }

        if (TIMER_delay < 1) {
            changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES);
        }

    }

    void cPlayerBrainMission::thinkState_PrepareGatherResources() {
        char msg[255];
        sprintf(msg, "thinkState_PrepareGatherResources()");
        log(msg);

        log("thinkState_PrepareGatherResources() : this is the army I have so far...");
        logUnits();

        const std::vector<int> &allMyUnits = player->getAllMyUnits();
        
        // assumes group contains units, just create build orders from the desired group of units...?
        for (auto &thingIWant : group) {
            assert(thingIWant.type > -1 && "cPlayerBrainMission - expected to have a type/buildId of > -1!");
            if (thingIWant.produced < thingIWant.required) {
                if (thingIWant.ordered <= thingIWant.produced) { // order one at a time only

                    if (thingIWant.buildType == eBuildType::UNIT) {
                        for (auto &unitId : allMyUnits) {
                            cUnit &pUnit = unit[unitId];
                            if (!pUnit.isValid()) continue;
                            if (pUnit.getType() != thingIWant.type) continue;
                            if (!pUnit.isIdle()) continue;
                            if (pUnit.isUnableToMove()) continue; // don't assign units with missions that can't move
                            if (pUnit.isAssignedAnyMission()) continue; // already assigned to a mission

                            // claim it for this mission!
                            pUnit.assignMission(this->uniqueIdentifier);
                            units.push_back(pUnit.iID);
                            thingIWant.produced++; // update bookkeeping, no need to order it, we already have it
                            char msg[255];
                            sprintf(msg, "Found idle unit [%d] to assign to mission [%d]", unitId, this->uniqueIdentifier);
                            log(msg);

                            if (thingIWant.produced == thingIWant.required) break; // stop searching, we have everything
                        }

                        if (thingIWant.produced == thingIWant.required) {
                            // we could assemble all of them with idle units, so skip
                            continue; // notice, we continue in the 'group' for loop here!
                        }
                    } else if (thingIWant.buildType == eBuildType::SPECIAL) {
                        if (player->isSpecialAwaitingPlacement()) {
                            thingIWant.produced = thingIWant.required;
                            continue;
                        }
                    }

                    // else, we order stuff, if we didn't already?
                    while (thingIWant.ordered < thingIWant.required) {
                        brain->addBuildOrder((S_buildOrder) {
                                buildType : thingIWant.buildType,
                                priority : 0,
                                buildId : thingIWant.type,
                                placeAt : -1,
                                state : buildOrder::eBuildOrderState::PROCESSME,
                        });
//                        // amount of 'think' iterations to wait before we bail the mission
//                        // brains 'normal thinking' is 100ms every tick. The missions have much faster thinking (5ms).
//                        // hence, we should compensate for that to give the AI brain some time to deal with any changes
//                        // Especially because the brains use resttime, it reduces the speed at which the AI brains operate.
//                        // ie, this means, a mission runs 20x faster (5ms vs 100ms) + waits atleast a second (10*100ms), so
//                        // in order to give the brain enough time the additional ticks to add would be:
//                        // 1 brain tick = 20 mission ticks
//                        // 1 second of brain ticks (10/RestTime ticks) = 200 mission ticks
//                        int seconds = 2; // additional second to give brain some more time to determine what to do.
//                        int oneSecondInBrainTicks = cPlayerBrain::RestTime * 20;
//                        int additionalTicksToWaitForBrainThinking = oneSecondInBrainTicks * seconds;
//                        int timeToWait = cBuildingListItem::getTotalBuildTimeInTicks(thingIWant.buildType, thingIWant.type) + additionalTicksToWaitForBrainThinking; // plus extra ticks, for the AI brain thinking
//                        TIMER_awaitingGatheringResoures += timeToWait;
//                        char msg[255];
//                        sprintf(msg, "Added build order, will wait %d ms to complete build order(s).")

                        // just give it 5 minutes to build this item
                        // this mission thinks at fast rate, but the brain itself is running at a slow think rate.
                        // the normal brain will take some time to process the build orders.
                        // so wait just 5 minutes, that should be more then enough to process + wait for the build
                        // to be completed?
                        cSideBar *pSideBar = player->getSideBar();
                        int awaitingResourcesTimeToIncrease = 15;
                        if (thingIWant.buildType == UNIT) {
                            cBuildingListItem *pItem = pSideBar->getBuildingListItem(sUnitInfo[thingIWant.type].listId,
                                                                                     thingIWant.type);

                            if (pItem) {
                                int msToWaitBeforeGivingUp = ((1+(float)pItem->getTotalBuildTimeInMs()) * 1.25f);
                                awaitingResourcesTimeToIncrease = fastThinkMsToTicks(msToWaitBeforeGivingUp);
                            } else {
                                // this should not happen!?
                            }
                        } else if (thingIWant.buildType == STRUCTURE) {
                            cBuildingListItem *pItem = pSideBar->getBuildingListItem(LIST_CONSTYARD,
                                                                                     thingIWant.type);
                            if (pItem) {
                                int msToWaitBeforeGivingUp = ((1+(float)pItem->getTotalBuildTimeInMs()) * 1.25f);
                                awaitingResourcesTimeToIncrease = fastThinkMsToTicks(msToWaitBeforeGivingUp);
                            } else {
                                // this should not happen!?
                            }
                        } else if (thingIWant.buildType == SPECIAL) {
                            // "sSpecialInfo" are from the "palace" list
                            cBuildingListItem *pItem = pSideBar->getBuildingListItem(LIST_PALACE,
                                                                                     thingIWant.type);
                            if (pItem) {
                                int msToWaitBeforeGivingUp = ((1+(float)pItem->getTotalBuildTimeInMs() * 1.25f));
                                awaitingResourcesTimeToIncrease = fastThinkMsToTicks(msToWaitBeforeGivingUp);
                            } else {
                                // this should not happen!?
                            }
                        }

                        // in case we have very low
                        if (awaitingResourcesTimeToIncrease <= 15) {
                            int msForFiveMinutes = 1000 * 60 * 5;
                            awaitingResourcesTimeToIncrease = fastThinkMsToTicks(msForFiveMinutes);
                        }

                        int msForOneMinute = 1000 * 60; // for allowing the mission to process build orders
                        awaitingResourcesTimeToIncrease += fastThinkMsToTicks(msForOneMinute);

                        TIMER_awaitingGatheringResoures += awaitingResourcesTimeToIncrease;

                        // remember we placed a build order
                        thingIWant.ordered++;
                    }
                }
            } else {
                char msg[255];
                sprintf(msg, "Produced all required [%d] for type %s", thingIWant.produced,
                        toStringBuildTypeSpecificType(thingIWant.buildType, thingIWant.type));
                log(msg);
            }
        }

        if (!missionWithUnits) {
            if (!player->isSpecialAwaitingPlacement()) {
                changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES);
            } else {
                // in reality this event already has been sent, but this mission did not exist yet
                cBuildingListItem *item = player->getSpecialAwaitingPlacement();
                s_GameEvent event {
                        .eventType = eGameEventType::GAME_EVENT_SPECIAL_SELECT_TARGET,
                        .entityType = item->getBuildType(),
                        .entityID = -1,
                        .player = player,
                        .entitySpecificType = item->getBuildId(),
                        .atCell = -1,
                        .isReinforce = false,
                        .buildingListItem = item // mandatory for this event!
                };
                // fake notification for missionKind so that its state gets updated...
                missionKind->onNotify(event);
            }
            return; // bail, mission without units will wait for events
        }

        // all other (normal) missions can go to select target state if we acquired all units
        if (producedAllRequiredUnits()) {
            char msg[255];
            sprintf(msg,
                    "thinkState_PrepareGatherResources() : nothing left to build",
                    player->getId());
            log(msg);
            changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
            return;
        }

        // in all other cases, wait for resources
        changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES);
    }

    void cPlayerBrainMission::logUnits() {
        for (auto &myUnitId : units) {
            cUnit &myUnit = unit[myUnitId];
            char msg[255];
            sprintf(msg, "logUnits() : Unit %d, type %d (%s)", myUnit.iID, myUnit.iType, myUnit.getUnitType().name);
            log(msg);
        }
    }

    /**
     * Checks if all things this mission requires are produced.
     * @return
     */
    bool cPlayerBrainMission::producedAllRequiredUnits() {
        for (auto &unitIWant : group) {
            if (unitIWant.produced < unitIWant.required) {
                return false;
            }
        }
        return true;
    }

    void cPlayerBrainMission::thinkState_SelectTarget() {
        char msg[255];
        sprintf(msg, "thinkState_SelectTarget()");
        log(msg);

        if (missionKind) {
            if (missionKind->think_SelectTarget()) {
                changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE);
            }
        } else {
            changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_EXECUTE);
        }
    }

    void cPlayerBrainMission::thinkState_Execute() {
        char msg[255];
        sprintf(msg, "thinkState_Execute()");
        log(msg);

        if (missionKind) {
            missionKind->think_Execute();
        }

        if (!missionWithUnits) {
            return;
        }

        // From here on out, we assume the mission has units. And thus, it should change state whenever it has no more
        // units.

        if (units.empty()) {
            char msg[255];
            sprintf(msg,
                    "thinkState_Execute() : group of units is destroyed. Setting state to PLAYERBRAINMISSION_STATE_ENDED");
            log(msg);

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
            log("thinkState_Execute(): team is no longer valid / alive. Going to PLAYERBRAINMISSION_STATE_ENDED state. (HACK)");
            changeState(PLAYERBRAINMISSION_STATE_ENDED);
            return;
        }
    }

    void cPlayerBrainMission::thinkState_PrepareAwaitResources() {
        if (TIMER_awaitingGatheringResoures % 10 == 0) {
            char msg[255];
            sprintf(msg, "thinkState_PrepareAwaitResources() - TIMER_awaitingGatheringResoures = [%d]", TIMER_awaitingGatheringResoures);
            log(msg);
        }

        if (specialEventMakesStateSwitchToSelectTarget) {
            // keep waiting, because we wait for a specific event to happen
            return;
        }

        // wait for things to be produced...
        if (TIMER_awaitingGatheringResoures > 0) {
            TIMER_awaitingGatheringResoures--;
        } else {
            log("Done with waiting for resources.");
            // we're done waiting...
            if (units.empty()) {
                log("No units attached to mission, setting to ENDED state.");
                // end mission, when we have no units to work with
                changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_ENDED);
            } else {
                log("Units are attached to mission (see log lines after this), setting to PLAYERBRAINMISSION_STATE_SELECT_TARGET state.");
                logUnits();
                // execute missions with what we have, so just select a target and go!
                changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_SELECT_TARGET);
            }
        }
    }

    bool cPlayerBrainMission::isEnded() const {
        return state == PLAYERBRAINMISSION_STATE_ENDED;
    }

    void cPlayerBrainMission::changeState(ePlayerBrainMissionState newState) {
        char msg[255];
        sprintf(msg, "changeState() - from %s to %s",
                ePlayerBrainMissionStateString(state),
                ePlayerBrainMissionStateString(newState));
        log(msg);
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
        this->TIMER_awaitingGatheringResoures = rhs.TIMER_awaitingGatheringResoures;
        this->missionWithUnits = rhs.missionWithUnits;
        this->specialEventMakesStateSwitchToSelectTarget = rhs.specialEventMakesStateSwitchToSelectTarget;

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
        TIMER_delay(src.TIMER_delay),
        TIMER_awaitingGatheringResoures(src.TIMER_awaitingGatheringResoures),
        missionWithUnits(src.missionWithUnits),
        specialEventMakesStateSwitchToSelectTarget(src.specialEventMakesStateSwitchToSelectTarget)
    {
        if (src.missionKind) {
            missionKind = src.missionKind->clone(src.player, this);
        } else {
            missionKind = nullptr;
        }
    }

    /**
     * returns true, if the mission is no longer in 'gather resources' state(s)
     * @return
     */
    bool cPlayerBrainMission::isDoneGatheringResources() {
        return state != ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES &&
                state != ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_INITIAL_DELAY &&
                state != ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES;
    }

    void cPlayerBrainMission::log(const char *txt) {
        char msg[1024];
        sprintf(msg, "cPlayerBrainMission [%d, %s, %s] | %s",
                uniqueIdentifier,
                ePlayerBrainMissionKindString(kind),
                ePlayerBrainMissionStateString(state),
                txt);
        player->log(msg);
    }

    void cPlayerBrainMission::onEventCannotBuild(const s_GameEvent &event) {
        // event is for us
        if (event.player != player) {
            return;
        }

        // it is an event about my own stuff
        if (state == PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES || state == PLAYERBRAINMISSION_STATE_PREPARE_GATHER_RESOURCES) {
            if (event.entityType == UNIT) {
                // check if we wanted to build this thing, but it is impossible for now, so discard it.
                for (auto &thingIWant : group) {
                    if (thingIWant.type == event.entitySpecificType) {
                        // set to same value, so we don't want to produce it anymore.
                        thingIWant.produced = thingIWant.required;
                        char msg[255];
                        sprintf(msg, "Cannot build [%d, %s], found a match and removed it from things I want.",
                                event.entitySpecificType,
                                toStringBuildTypeSpecificType(event.entityType, event.entitySpecificType)
                                );
                        log(msg);
                    }
                }
            }
        }
    }

    void cPlayerBrainMission::onEventCannotCreatePath(const s_GameEvent &event) {
        // it is an event about my own stuff
        if (event.player == player) {
            if (isDoneGatheringResources()) {
                if (event.entityType == UNIT) {
                    // check if this is about an id in our units group
                    removeUnitIdFromListIfPresent(event.entityID);
                }
            }
        }
    }

}