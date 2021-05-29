#include <algorithm>
#include "include/d2tmh.h"

namespace brains {

    cPlayerBrainCampaign::cPlayerBrainCampaign(cPlayer *player) : cPlayerBrain(player) {
        state = ePlayerBrainState::PLAYERBRAIN_PEACEFUL;
        thinkState = ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_REST;
        // timer is substracted every 100 ms with 1 (ie, 10 == 10*100 = 1000ms == 1 second)
        // 10*60 -> 1 minute. * 4 -> 4 minutes
        TIMER_rest = (10 * 60) * 4;
        if (game.bNoAiRest) {
            TIMER_rest = 10;
        }
        myBase = std::vector<S_structurePosition>();
        buildOrders = std::vector<S_buildOrder>();
        discoveredEnemyAtCell = std::set<int>();
    }

    cPlayerBrainCampaign::~cPlayerBrainCampaign() {
        discoveredEnemyAtCell.clear();
        buildOrders.clear();
        myBase.clear();
        missions.clear();
    }

/**
 * Think function called every tick (handleTimerUnits, ~ 100 ms).
  */
    void cPlayerBrainCampaign::think() {
        // for now use a switch statement for this state machine. If we need anything
        // more sophisticated we can always use the State Pattern.
        switch (thinkState) {
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_REST:
                thinkState_Rest();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_SCAN_BASE:
                thinkState_ScanBase();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_MISSIONS:
                thinkState_Missions();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_PROCESS_BUILDORDERS:
                thinkState_ProcessBuildOrders();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_EVALUATE:
                thinkState_Evaluate();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_ENDGAME:
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
        if (event.player == player) {
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

        if (event.eventType == eGameEventType::GAME_EVENT_DISCOVERED) {
            onEntityDiscoveredEvent(event);
        }

        // notify mission about any kind of event
        for (auto &mission : missions) {
            mission.onNotify(event);
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

    void cPlayerBrainCampaign::thinkState_ScanBase() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_ScanBase(), for player [%d]", player->getId());
        logbook(msg);

        // go through base and initiate repairs where needed?
        // TODO:

        // reset timer (for the next time we end up here)
        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_MISSIONS);
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

        if (state == ePlayerBrainState::PLAYERBRAIN_PEACEFUL) {
            // it might send out something to scout?
            if (missions.empty()) {
                // add scouting mission
                std::vector<S_groupKind> group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        type : player->getScoutingUnitType(),
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });

                cPlayerBrainMission someMission(player, ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_EXPLORE, this, group, rnd(5));
                missions.push_back(someMission);
            }
        } else {
            // no longer peaceful
            if (state == ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED) {
                // find any attack missions, keep at max 1 (for now?)
                auto position = std::find_if(missions.begin(), missions.end(), [](const cPlayerBrainMission & mission){ return mission.isAttackingMission(); });
                if (position != missions.end()) {
                    // we have an attack mission
                } else {
                    produceMissions();
                }
            }
        }

        // all missions are allowed to think now
        for (auto &mission : missions) {
            mission.think();
        }

        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_PROCESS_BUILDORDERS);
    }

    void cPlayerBrainCampaign::produceMissions() {
        // TODO: we can also read 'TEAMS' from a Scenario and use that instead of hard-coding?

        int trikeKind = TRIKE;
        if (player->getHouse() == ORDOS) {
            trikeKind = RAIDER;
        }

        int soldierKind = SOLDIER;
        int infantryKind = INFANTRY;

        if (player->getHouse() == HARKONNEN || player->getHouse() == SARDAUKAR) {
            soldierKind = TROOPER;
            infantryKind = TROOPERS;
        }

        // MISSION 2
        if (game.iMission == 2) {
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            group.push_back((S_groupKind) {
                    type : soldierKind,
                    required: 2 + rnd(1),
                    ordered: 0,
                    produced: 0,
            });
            group.push_back((S_groupKind) {
                    type : infantryKind,
                    required: 1 + rnd(1),
                    ordered: 0,
                    produced: 0,
            });
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10));
            return;
        }

        // MISSION 3
        if (game.iMission == 3) {
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        type : QUAD,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        type : trikeKind,
                        required: 1 + rnd(1),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        type : QUAD,
                        required: 2 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15));
            group = std::vector<S_groupKind>();

            group.push_back((S_groupKind) {
                    type : soldierKind,
                    required: 1 + rnd(3),
                    ordered: 0,
                    produced: 0,
            });
            group.push_back((S_groupKind) {
                    type : infantryKind,
                    required: 1 + rnd(2),
                    ordered: 0,
                    produced: 0,
            });
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10));
        }

        // MISSION 4
        if (game.iMission == 4) {
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        type : trikeKind,
                        required: 1 + rnd(1),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        type : QUAD,
                        required: 1 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        type : QUAD,
                        required: 2 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15));
            group = std::vector<S_groupKind>();
            group.push_back((S_groupKind) {
                    type : TANK,
                    required: 1 + rnd(2),
                    ordered: 0,
                    produced: 0,
            });
            group.push_back((S_groupKind) {
                    type : QUAD,
                    required: 1,
                    ordered: 0,
                    produced: 0,
            });
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, 10 + rnd(10));
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        type : infantryKind,
                        required: 2 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10));
            }
        }
    }

    void cPlayerBrainCampaign::addMission(ePlayerBrainMissionKind kind, const std::vector<S_groupKind> &group, int initialDelay) {
        cPlayerBrainMission someMission(player, kind, this, group, initialDelay);
        missions.push_back(someMission);
    }

    void cPlayerBrainCampaign::thinkState_Evaluate() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_Evaluate(), for player [%d]", player->getId());
        logbook(msg);

        if (player->getAmountOfStructuresForType(CONSTYARD) == 0) {
            // no constyards, endgame
            changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_ENDGAME);
            return;
        }

        // Re-iterate over all (pending) buildOrders - and check if we are still going to do that or we might want to
        // up/downplay some priorities and re-sort?
        // Example: Money is short, Harvester is in build queue, but not high in priority?

        // take a little rest, before going into a new loop again?
//        TIMER_rest = 5 + rnd(15);
        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_REST);
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

        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_EVALUATE);
    }

    void cPlayerBrainCampaign::changeThinkStateTo(const ePlayerBrainCampaignThinkState& newState) {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::changeThinkStateTo(), for player [%d] - from %s to %s", player->getId(),
                ePlayerBrainCampaignThinkStateString(thinkState),
                ePlayerBrainCampaignThinkStateString(newState));
        logbook(msg);
        this->thinkState = newState;
    }

    void cPlayerBrainCampaign::thinkState_Rest() {
        TIMER_rest--;
        if (TIMER_rest > 0) {
            char msg[255];
            sprintf(msg, "cPlayerBrainCampaign::thinkState_Rest(), for player [%d] - rest %d", player->getId(), TIMER_rest);
            logbook(msg);
            return;
        }

        // resting is done, now go into the scan base/missions/evaluate loop
        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_SCENARIO_STATE_SCAN_BASE);
    }

    void cPlayerBrainCampaign::onEntityDiscoveredEvent(const s_GameEvent &event) {
        if (state == ePlayerBrainState::PLAYERBRAIN_PEACEFUL) {
            bool wormsign = event.entityType == eBuildType::UNIT && event.entitySpecificType == SANDWORM;
            if (!wormsign) {
                if (event.player == player) {
                    // i discovered something
                    if (event.entityType == eBuildType::UNIT) {
                        cUnit &cUnit = unit[event.entityID];
                        if (cUnit.isValid() && !cUnit.getPlayer()->isSameTeamAs(player)) {
                            // found enemy unit
                            state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                            TIMER_rest = 0; // if we where still 'resting' then stop this now.
                            discoveredEnemyAtCell.insert(event.atCell);
                        }
                    } else if (event.entityType == eBuildType::STRUCTURE) {
                        cAbstractStructure *pStructure = structure[event.entityID];
                        if (!pStructure->getPlayer()->isSameTeamAs(player)) {
                            // found enemy structure
                            state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                            TIMER_rest = 0; // if we where still 'resting' then stop this now.
                            discoveredEnemyAtCell.insert(event.atCell);
                        }
                    }
                } else {
                    // event.player == player who discovered something
                    if (event.player == &players[AI_WORM]) {
                        // ignore anything that the WORM AI player detected.
                    } else if (!event.player->isSameTeamAs(player)) {
                        if (event.entityType == eBuildType::UNIT) {
                            cUnit &cUnit = unit[event.entityID];
                            // the other player discovered a unit of mine
                            if (cUnit.isValid() && cUnit.getPlayer() == player) {
                                // found my unit
                                state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                                TIMER_rest = 0; // if we where still 'resting' then stop this now.
                                discoveredEnemyAtCell.insert(cUnit.getCell());
                            }
                        } else if (event.entityType == eBuildType::STRUCTURE) {
                            cAbstractStructure *pStructure = structure[event.entityID];
                            // the other player discovered a structure of mine
                            if (pStructure->getPlayer() == player) {
                                // found my structure
                                state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                                TIMER_rest = 0; // if we where still 'resting' then stop this now.
//                                discoveredEnemyAtCell.insert(pStructure.getCell());
                                // TODO: Record we have found an enemy structure...
                            }
                        }
                    } else {
                        // discovered teammate... (do something else?)
                    }
                }
            } else {

            }
        } else {
            // what to do?
        }
    }
}