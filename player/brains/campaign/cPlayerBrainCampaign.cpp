#include <algorithm>
#include "include/d2tmh.h"

namespace brains {

    cPlayerBrainCampaign::cPlayerBrainCampaign(cPlayer *player) : cPlayerBrain(player) {
        state = ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_WAITING;
        TIMER_initialDelay = 100;
        TIMER_scanBase = 50;
        myBase = std::vector<S_structurePosition>();
        buildOrders = std::vector<S_buildOrder>();
    }

    cPlayerBrainCampaign::~cPlayerBrainCampaign() {

    }

/**
 * Think function called every tick (handleTimerUnits, ~ 100 ms).
  */
    void cPlayerBrainCampaign::think() {
        // for now use a switch statement for this state machine. If we need anything
        // more sophisticated we can always use the State Pattern.
        switch (state) {
            case ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_WAITING:
                thinkState_Waiting();
                return;
            case ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_SCAN_BASE:
                thinkState_ScanBase();
                return;
            case ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_MISSIONS:
                thinkState_Missions();
                return;
            case ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_PROCESS_BUILDORDERS:
                thinkState_ProcessBuildOrders();
                return;
            case ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_EVALUATE:
                thinkState_Evaluate();
                return;
            case ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_ENDGAME:
                thinkState_EndGame();
                return;
        }

        // now do some real stuff

        char msg[255];
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainCampaign::think(), for player [%d] - FINISHED", player->getId());
        logbook(msg);
    }

    void cPlayerBrainCampaign::addBuildOrder(S_buildOrder order) {
//    // check if we can find a similar build order
//    for (auto & buildOrder : buildOrders) {
//        if (buildOrder.buildType != order.buildType) continue;
//        if (buildOrder.buildId != order.buildId) continue;
//
//        if (order.buildType == eBuildType::STRUCTURE) {
//            if (buildOrder.placeAt != order.placeAt) continue;
//        }
//
//        // found same, if so, then simply increase priority?
////        buildOrder.priority++;
//        return; // stop
//    }
        buildOrders.push_back(order);

        // re-order based on priority
        std::sort(buildOrders.begin(), buildOrders.end(), [](const S_buildOrder &lhs, const S_buildOrder &rhs) {
            return lhs.priority > rhs.priority;
        });

        char msg[255];
        sprintf(msg,
                "cPlayerBrainCampaign::addBuildOrder() - player [%d / %s] results into the following build orders:",
                player->getId(), player->getHouseName().c_str());
        logbook(msg);

        int id = 0;
        for (auto &buildOrder : buildOrders) {
            memset(msg, 0, sizeof(msg));
            if (buildOrder.buildType == eBuildType::UNIT) {
                sprintf(msg, "[%d] - type = UNIT, buildId = %d (=%s), priority = %d", id, buildOrder.buildId,
                        units[buildOrder.buildId].name, buildOrder.priority);
            } else if (buildOrder.buildType == eBuildType::STRUCTURE) {
                sprintf(msg, "[%d] - type = STRUCTURE, buildId = %d (=%s), priority = %d, place at %d", id,
                        buildOrder.buildId, structures[buildOrder.buildId].name, buildOrder.priority,
                        buildOrder.placeAt);
            } else if (buildOrder.buildType == eBuildType::SPECIAL) {
                sprintf(msg, "[%d] - type = SPECIAL, buildId = %d (=%s), priority = %d", id, buildOrder.buildId,
                        specials[buildOrder.buildId].description, buildOrder.priority);
            } else if (buildOrder.buildType == eBuildType::BULLET) {
                sprintf(msg, "[%d] - type = SPECIAL, buildId = %d (=NOT YET IMPLEMENTED), priority = %d", id,
                        buildOrder.buildId, buildOrder.priority);
            }
            logbook(msg);

            id++;
        }
    }

    void cPlayerBrainCampaign::onNotify(const s_GameEvent &event) {
        if (event.entityOwnerID == player->getId()) {
            // events about my structures
            if (event.entityType == eBuildType::STRUCTURE) {
                switch (event.eventType) {
                    case eGameEventType::GAME_EVENT_DESTROYED:
                        onMyStructureDestroyed(event);
                        break;
                    case eGameEventType::GAME_EVENT_CREATED:
                        onMyStructureCreated(event);
                        break;
                    case eGameEventType::GAME_EVENT_DAMAGED:
                        onMyStructureAttacked(event);
                        // help I'm under attack.. do something smart
                        break;
                    case eGameEventType::GAME_EVENT_DECAY:
                        onMyStructureDecayed(event);
                        // should repair when under 75%?
                        break;
                }
            }
        }

        // Notify my missions about creations/destroyed things only (for now?)
        if (event.eventType == eGameEventType::GAME_EVENT_DESTROYED ||
            event.eventType == eGameEventType::GAME_EVENT_CREATED ||
            event.eventType == eGameEventType::GAME_EVENT_DEVIATED) {
            for (auto &mission : missions) {
                mission.onNotify(event);
            }
        }
    }

    void cPlayerBrainCampaign::onMyStructureCreated(const s_GameEvent &event) {
        // a structure was created, update our baseplan
        cAbstractStructure *pStructure = structure[event.entityID];
        int placedAtCell = pStructure->getCell();
        bool foundExistingStructureInBase = false;
        for (auto &structurePosition : myBase) {
            if (!structurePosition.isDestroyed) continue; // not destroyed, hence cannot be rebuilt

            if (structurePosition.cell == placedAtCell) {
                assert(structurePosition.type == event.entitySpecificType); // should be same structure type...
                // seems my structure, same location.
                structurePosition.structureId = event.entityID;
                structurePosition.isDestroyed = false; // no longer destroyed!
                foundExistingStructureInBase = true;
            }
        }

        if (!foundExistingStructureInBase) {
            char msg[255];
            sprintf(msg,
                    "cPlayerBrainCampaign::onNotify() - player [%d / %s] concluded to add structure %s to base register:",
                    player->getId(), player->getHouseName().c_str(), pStructure->getS_StructuresType().name);
            logbook(msg);

            // new structure placed, update base register
            S_structurePosition position = {
                    .cell = pStructure->getCell(),
                    .type = pStructure->getType(),
                    .structureId = pStructure->getStructureId(),
                    .isDestroyed = pStructure->isDead()
            };
            myBase.push_back(position);
        }
    }

    void cPlayerBrainCampaign::onMyStructureDestroyed(const s_GameEvent &event) {
        // a structure got destroyed, figure out which one it is in my base plan, and update its state
        for (auto &structurePosition : myBase) {
            if (structurePosition.structureId == event.entityID) {
                // this structure got destroyed, so mark it as destroyed in my base plan
                structurePosition.isDestroyed = true;
                // and add order to rebuild it
                addBuildOrder((S_buildOrder) {
                        buildType : STRUCTURE,
                        priority : 1,
                        buildId : structurePosition.type,
                        placeAt : structurePosition.cell,
                        state : buildOrder::PROCESSME,
                });
            }
        }
    }

    void cPlayerBrainCampaign::onMyStructureAttacked(const s_GameEvent &event) {
        if (player->hasEnoughCreditsFor(50)) {
            cAbstractStructure *pStructure = structure[event.entityID];
            if (!pStructure->isRepairing()) {
                s_Structures &sStructures = structures[event.entitySpecificType];
                if (pStructure->getHitPoints() < sStructures.hp * 0.75) {
                    pStructure->startRepairing();
                }
            }
        }
    }

    void cPlayerBrainCampaign::onMyStructureDecayed(const s_GameEvent &event) {
        if (player->hasEnoughCreditsFor(50)) {
            cAbstractStructure *pStructure = structure[event.entityID];
            if (!pStructure->isRepairing()) {
                s_Structures &sStructures = structures[event.entitySpecificType];
                if (pStructure->getHitPoints() < sStructures.hp * 0.75) {
                    pStructure->startRepairing();
                }
            }
        }
    }

    void cPlayerBrainCampaign::thinkState_Waiting() {
        TIMER_initialDelay--;
        if (TIMER_initialDelay > 0) {
            char msg[255];
            sprintf(msg, "cPlayerBrainCampaign::thinkState_Waiting(), for player [%d] - Initial Delay still active...",
                    player->getId());
            logbook(msg);
            return;
        }

        // delay is done, now go into the scan base/missions/evaluate loop
        state = ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_SCAN_BASE;
    }

    void cPlayerBrainCampaign::thinkState_ScanBase() {
        if (TIMER_scanBase > 0) {
            TIMER_scanBase--;
            return;
        }

        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_ScanBase(), for player [%d]", player->getId());
        logbook(msg);

        // go through base and initiate repairs where needed?
        // TODO:

        // reset timer (for the next time we end up here)
        TIMER_scanBase = 25;
        state = ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_MISSIONS;
    }

    void cPlayerBrainCampaign::thinkState_Missions() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_Missions(), for player [%d]", player->getId());
        logbook(msg);

        // delete any missions which are ended
        missions.erase(
                std::remove_if(
                        missions.begin(),
                        missions.end(),
                        [](cPlayerBrainMission m) { return m.isEnded(); }),
                missions.end()
        );

        if (missions.empty()) {
            // create attack mission
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            group.push_back((S_groupKind) {
                type : QUAD,
                required: 3,
                ordered: 0,
                produced: 0,
            });
            group.push_back((S_groupKind) {
                type : TANK,
                required: 2,
                ordered: 0,
                produced: 0,
            });

            cPlayerBrainMission someMission(player, ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_ATTACK, this, group);
            missions.push_back(someMission);
        }

        // all missions are allowed to think now
        for (auto &mission : missions) {
            mission.think();
        }

        state = ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_PROCESS_BUILDORDERS;
    }

    void cPlayerBrainCampaign::thinkState_Evaluate() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_Evaluate(), for player [%d]", player->getId());
        logbook(msg);

        if (player->getAmountOfStructuresForType(CONSTYARD) == 0) {
            // no constyards, endgame
            state = ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_ENDGAME;
            return;
        }

        // Re-iterate over all (pending) buildOrders - and check if we are still going to do that or we might want to
        // up/downplay some priorities and re-sort?
        // Example: Money is short, Harvester is in build queue, but not high in priority?

        // loop to scan_base which has a delay of 250 ticks
        state = ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_SCAN_BASE;
    }

    void cPlayerBrainCampaign::thinkState_EndGame() {
        // ...
//    char msg[255];
//    sprintf(msg, "cPlayerBrainCampaign::thinkState_EndGame(), for player [%d]", player->getId());
//    logbook(msg);
    }

    void cPlayerBrainCampaign::thinkState_ProcessBuildOrders() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_ProcessBuildOrders(), for player [%d]", player->getId());
        logbook(msg);

        // check if we can find a similar build order
        for (auto &buildOrder : buildOrders) {
            if (buildOrder.state != buildOrder::eBuildOrderState::PROCESSME)
                continue; // only process those which are marked

            if (buildOrder.buildType == eBuildType::STRUCTURE) {
                if (player->canBuildStructure(buildOrder.buildId) == eCantBuildReason::NONE) {
                    if (player->startBuildingStructure(buildOrder.buildId)) {
                        buildOrder.state = buildOrder::eBuildOrderState::BUILDING;
                    }
                }
            } else if (buildOrder.buildType == eBuildType::UNIT) {
                if (player->canBuildUnit(buildOrder.buildId) == eCantBuildReason::NONE) {
                    if (player->startBuildingUnit(buildOrder.buildId)) {
                        buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                    }
                }
            }
        }

        // delete any buildOrders which are flagged to remove
        buildOrders.erase(
                std::remove_if(
                        buildOrders.begin(),
                        buildOrders.end(),
                        [](const S_buildOrder &o) { return o.state == buildOrder::eBuildOrderState::REMOVEME; }),
                buildOrders.end()
        );

        if (player->isBuildingStructureAwaitingPlacement()) {
            int structureType = player->getStructureTypeBeingBuilt();
            for (auto &buildOrder : buildOrders) {
                if (buildOrder.buildType != eBuildType::STRUCTURE) {
                    continue;
                }

                if (buildOrder.buildId == structureType) {
                    int iCll = buildOrder.placeAt;

                    cBuildingListItem *pItem = player->getStructureBuildingListItemBeingBuilt();
                    player->placeStructure(iCll, pItem);

                    buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                    break;
                }
            }
        }

        state = ePlayerBrainScenarioState::PLAYERBRAIN_SCENARIO_STATE_EVALUATE;
    }

}