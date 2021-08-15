#include <algorithm>
#include "include/d2tmh.h"
#include "cPlayerBrainCampaign.h"


namespace brains {

    cPlayerBrainCampaign::cPlayerBrainCampaign(cPlayer *player) : cPlayerBrain(player) {
        state = ePlayerBrainState::PLAYERBRAIN_PEACEFUL;
        thinkState = ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_REST;
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

    void cPlayerBrainCampaign::think() {
        // for now use a switch statement for this state machine. If we need anything
        // more sophisticated we can always use the State Pattern.
        switch (thinkState) {
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_REST:
                thinkState_Rest();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_SCAN_BASE:
                thinkState_ScanBase();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_MISSIONS:
                thinkState_Missions();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_PROCESS_BUILDORDERS:
                thinkState_ProcessBuildOrders();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_EVALUATE:
                thinkState_Evaluate();
                return;
            case ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_ENDGAME:
                thinkState_EndGame();
                return;
        }

        // now do some real stuff

        char msg[255];
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainCampaign::think() - FINISHED");
        player->log(msg);
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
                "cPlayerBrainCampaign::addBuildOrder() - results into the following build orders:");
        player->log(msg);

        int id = 0;
        for (auto &buildOrder : buildOrders) {
            memset(msg, 0, sizeof(msg));
            if (buildOrder.buildType == eBuildType::UNIT) {
                sprintf(msg, "[%d] - type = UNIT, buildId = %d (=%s), priority = %d, state = %s", id, buildOrder.buildId,
                        unitInfo[buildOrder.buildId].name, buildOrder.priority, eBuildOrderStateString(buildOrder.state));
            } else if (buildOrder.buildType == eBuildType::STRUCTURE) {
                sprintf(msg, "[%d] - type = STRUCTURE, buildId = %d (=%s), priority = %d, place at %d, state = %s", id,
                        buildOrder.buildId, structures[buildOrder.buildId].name, buildOrder.priority,
                        buildOrder.placeAt, eBuildOrderStateString(buildOrder.state));
            } else if (buildOrder.buildType == eBuildType::SPECIAL) {
                sprintf(msg, "[%d] - type = SPECIAL, buildId = %d (=%s), priority = %d, state = %s", id, buildOrder.buildId,
                        specialInfo[buildOrder.buildId].description, buildOrder.priority, eBuildOrderStateString(buildOrder.state));
            } else if (buildOrder.buildType == eBuildType::BULLET) {
                sprintf(msg, "[%d] - type = SPECIAL, buildId = %d (=NOT YET IMPLEMENTED), priority = %d, state = %s", id,
                        buildOrder.buildId, buildOrder.priority, eBuildOrderStateString(buildOrder.state));
            }
            player->log(msg);

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
                    "cPlayerBrainCampaign::onNotify() - concluded to add structure %s to base register:",
                    pStructure->getS_StructuresType().name);
            player->log(msg);

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
        sprintf(msg, "cPlayerBrainCampaign::thinkState_ScanBase()");
        player->log(msg);

        // reset timer (for the next time we end up here)
        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_MISSIONS);
    }

    void cPlayerBrainCampaign::thinkState_Missions() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_Missions()");
        player->log(msg);

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
            if (!hasMission(99)) {
                // add scouting mission
                std::vector<S_groupKind> group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : player->getScoutingUnitType(),
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });

                cPlayerBrainMission someMission(player, ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_EXPLORE, this, group, rnd(5), 99);
                missions.push_back(someMission);
            }
        } else {
            // no longer peaceful
            if (state == ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED) {
                produceMissions();
            }
        }

        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_PROCESS_BUILDORDERS);
    }

    void cPlayerBrainCampaign::produceMissions() {
        // TODO: we can also read 'TEAMS' from a Scenario and use that instead of hard-coding?
        int trikeKind = TRIKE;
        if (player->getHouse() == ORDOS) {
            trikeKind = RAIDER;
        } else if (player->getHouse() == HARKONNEN) {
            trikeKind = QUAD;
        }

        int soldierKind = SOLDIER;
        int infantryKind = INFANTRY;

        if (player->getHouse() == HARKONNEN || player->getHouse() == SARDAUKAR) {
            soldierKind = TROOPER;
            infantryKind = TROOPERS;
        }

        if (game.iMission == 2) {
            produceLevel2Missions(soldierKind, infantryKind);
            return;
        }

        if (game.iMission == 3) {
            produceLevel3Missions(trikeKind, soldierKind, infantryKind);
            return;
        }

        if (game.iMission == 4) {
            produceLevel4Missions(trikeKind, infantryKind);
            return;
        }

        if (game.iMission == 5) {
            produceLevel5Missions(trikeKind, infantryKind);
            return;
        }

        if (game.iMission == 6) {
            produceLevel6Missions(trikeKind, infantryKind);
            return;
        }

        if (game.iMission == 7) {
            produceLevel7Missions(trikeKind, infantryKind);
            return;
        }

        if (game.iMission == 8) {
            produceLevel8Missions(trikeKind, infantryKind);
            return;
        }

        if (game.iMission == 9) {
            produceLevel9Missions(trikeKind, infantryKind);
            return;
        }
    }

    void cPlayerBrainCampaign::produceLevel5Missions(int trikeKind, int infantryKind) {
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        if (!hasMission(0)) {
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : trikeKind,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 0);
        }

        if (!hasMission(1)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
            }
            if (player->getHouse() != ORDOS) {
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : LAUNCHER,
                            required: 1 + rnd(2),
                            ordered: 0,
                            produced: 0,
                    });
                }
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 1 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 4 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 1);
        }

        if (!hasMission(2)) {
            // 25% chance we want another different attack force
            if (rnd(100) < 25) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                if (player->getHouse() != ORDOS) {
                    if (rnd(100) < 75) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : LAUNCHER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                    }
                }
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, 10 + rnd(10), 2);
            }
        }
        if (!hasMission(3)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : infantryKind,
                        required: 2 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 3);
            }
        }
        // build additional harvester (we want 2 harvesters in total)
        int harvesters = player->getAmountOfUnitsForType(HARVESTER);
        if (harvesters < 2) {
            if (!hasMission(4)) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : HARVESTER,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 4);
            } else {
                // we already have mission 4, meaning we have the additional harvester.
                // If we have only 1 harvester than that means the harvester we got
                // from our refinery got destroyed. Rebuild that one; and assign that to mission "4".
                if (!hasMission(5)) {
                    if (harvesters > 0) {
                        group = std::vector<S_groupKind>();
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : HARVESTER,
                                required: 1,
                                ordered: 0,
                                produced: 0,
                        });
                        // different mission ID
                        addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 5);
                    }
                }
            }
        }
    }

    void cPlayerBrainCampaign::produceLevel6Missions(int trikeKind, int infantryKind) {
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        if (!hasMission(0)) {
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : trikeKind,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 0);
        }

        if (!hasMission(1)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
            }
            if (player->getHouse() != ORDOS) {
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : LAUNCHER,
                            required: 1 + rnd(3),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : TANK,
                            required: 2 + rnd(4),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SIEGETANK,
                            required: 3 + rnd(4),
                            ordered: 0,
                            produced: 0,
                    });
                }
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : SIEGETANK,
                        required: 3 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 1);
        }

        if (!hasMission(2)) {
            // 25% chance we want another different attack force
            if (rnd(100) < 25) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SIEGETANK,
                            required: 1 + rnd(2),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (player->getHouse() != ORDOS) {
                    if (rnd(100) < 75) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : LAUNCHER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                    }
                }
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, 10 + rnd(10), 2);
            }
        }
        if (!hasMission(3)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : infantryKind,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 3);
            }
        }
        // build additional harvester (we want 2 harvesters in total)
        int harvesters = player->getAmountOfUnitsForType(HARVESTER);
        if (harvesters < 4) {
            if (!hasMission(4)) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : HARVESTER,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 4);
            } else {
                // we already have mission 4, meaning we have the additional harvester.
                // If we have only 1 harvester than that means the harvester we got
                // from our refinery got destroyed (2 refineries in this mission).
                // Rebuild that one; and assign that to mission "4".
                if (!hasMission(5)) {
                    if (harvesters > 0) {
                        group = std::vector<S_groupKind>();
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : HARVESTER,
                                required: 2,
                                ordered: 0,
                                produced: 0,
                        });
                        // different mission ID
                        addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 5);
                    }
                }
            }
        }
    }

    void cPlayerBrainCampaign::produceLevel7Missions(int trikeKind, int infantryKind) {
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        if (!hasMission(0)) {
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : trikeKind,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 0);
        }

        if (!hasMission(1)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
            }
            if (player->getHouse() != ORDOS) {
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : LAUNCHER,
                            required: 1 + rnd(3),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : TANK,
                            required: 2 + rnd(4),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SIEGETANK,
                            required: 3 + rnd(4),
                            ordered: 0,
                            produced: 0,
                    });
                }
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : SIEGETANK,
                        required: 3 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 1);
        }

        if (!hasMission(2)) {
            // 25% chance we want another different attack force
            if (rnd(100) < 25) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SIEGETANK,
                            required: 1 + rnd(2),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (player->getHouse() != ORDOS) {
                    if (rnd(100) < 75) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : LAUNCHER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                    }
                }
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, 10 + rnd(10), 2);
            }
        }
        if (!hasMission(3)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : infantryKind,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 3);
            }
        }
        // build additional harvester (we want 2 harvesters in total)
        int harvesters = player->getAmountOfUnitsForType(HARVESTER);
        if (harvesters < 4) {
            if (!hasMission(4)) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : HARVESTER,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 4);
            } else {
                // we already have mission 4, meaning we have the additional harvester.
                // If we have only 1 harvester than that means the harvester we got
                // from our refinery got destroyed (2 refineries in this mission).
                // Rebuild that one; and assign that to mission "4".
                if (!hasMission(5)) {
                    if (harvesters > 0) {
                        group = std::vector<S_groupKind>();
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : HARVESTER,
                                required: 2,
                                ordered: 0,
                                produced: 0,
                        });
                        // different mission ID
                        addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 5);
                    }
                }
            }
        }
        // build ornithopters
        if (player->getHouse() == ATREIDES || player->getHouse() == ORDOS) {
            if (player->hasAtleastOneStructure(HIGHTECH)) {
                if (!hasMission(6)) {
                    group = std::vector<S_groupKind>();
                    if (rnd(100) < 10) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : ORNITHOPTER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                        addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 6);
                    }
                }
            }
        }
        // build carry-alls if required
        if (player->hasAtleastOneStructure(HIGHTECH)) {
            if (!hasMission(7)) {
                group = std::vector<S_groupKind>();
                if (rnd(100) < 10) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : CARRYALL,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                    addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(10), 7);
                }
            }
        }
    }

    void cPlayerBrainCampaign::produceLevel8Missions(int trikeKind, int infantryKind) {
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        if (!hasMission(0)) {
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : trikeKind,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 0);
        }

        if (!hasMission(1)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
            }
            if (player->getHouse() != ORDOS) {
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : LAUNCHER,
                            required: 1 + rnd(3),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : TANK,
                            required: 2 + rnd(4),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SIEGETANK,
                            required: 3 + rnd(4),
                            ordered: 0,
                            produced: 0,
                    });
                }
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : SIEGETANK,
                        required: 3 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 1);
        }

        if (!hasMission(2)) {
            // 25% chance we want another different attack force
            if (rnd(100) < 25) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SIEGETANK,
                            required: 1 + rnd(2),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (player->getHouse() != ORDOS) {
                    if (rnd(100) < 75) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : LAUNCHER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                    }
                }
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, 10 + rnd(10), 2);
            }
        }
        if (!hasMission(3)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : infantryKind,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 3);
            }
        }
        // build additional harvester (we want 2 harvesters in total)
        int harvesters = player->getAmountOfUnitsForType(HARVESTER);
        if (harvesters < 4) {
            if (!hasMission(4)) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : HARVESTER,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 4);
            } else {
                // we already have mission 4, meaning we have the additional harvester.
                // If we have only 1 harvester than that means the harvester we got
                // from our refinery got destroyed (2 refineries in this mission).
                // Rebuild that one; and assign that to mission "4".
                if (!hasMission(5)) {
                    if (harvesters > 0) {
                        group = std::vector<S_groupKind>();
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : HARVESTER,
                                required: 2,
                                ordered: 0,
                                produced: 0,
                        });
                        // different mission ID
                        addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 5);
                    }
                }
            }
        }
        // build ornithopters
        if (player->getHouse() == ATREIDES || player->getHouse() == ORDOS) {
            if (player->hasAtleastOneStructure(HIGHTECH)) {
                if (!hasMission(6)) {
                    group = std::vector<S_groupKind>();
                    if (rnd(100) < 10) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : ORNITHOPTER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                        addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 6);
                    }
                }
            }
        }
        // build carry-alls if required
        if (player->hasAtleastOneStructure(HIGHTECH)) {
            if (!hasMission(7)) {
                group = std::vector<S_groupKind>();
                if (rnd(100) < 10) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : CARRYALL,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                    addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(10), 7);
                }
            }
        }
        if (player->hasAtleastOneStructure(PALACE)) {
            int MISSION_ID_SUPERWEAPON = 8;

            if (player->getHouse() == ATREIDES) {
                if (!hasMission(MISSION_ID_SUPERWEAPON)) {
                    group = std::vector<S_groupKind>();
                    // no need to add resources to mission, they are auto-produced.
                    // TODO: Think of a way to make this script/configurable
                    addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN, group, rnd(10), MISSION_ID_SUPERWEAPON);
                }
            } else if (player->getHouse() == HARKONNEN || player->getHouse() == SARDAUKAR) {
                if (!hasMission(MISSION_ID_SUPERWEAPON)) {
                    group = std::vector<S_groupKind>();
                    // no need to add resources to mission, they are auto-produced.
                    // TODO: Think of a way to make this script/configurable
                    addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND, group, rnd(10), MISSION_ID_SUPERWEAPON);
                }
            } else if (player->getHouse() == ORDOS) {
                if (!hasMission(MISSION_ID_SUPERWEAPON)) {
                    group = std::vector<S_groupKind>();
                    // no need to add resources to mission, they are auto-produced.
                    // TODO: Think of a way to make this script/configurable
                    addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR, group, rnd(10), MISSION_ID_SUPERWEAPON);
                }
            }
        }
    }

    void cPlayerBrainCampaign::produceLevel9Missions(int trikeKind, int infantryKind) {
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        if (!hasMission(0)) {
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : trikeKind,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 0);
        }

        if (!hasMission(1)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
            }
            if (player->getHouse() != ORDOS) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : LAUNCHER,
                        required: 1 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
                if (rnd(100) < 35) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : TANK,
                            required: 2 + rnd(4),
                            ordered: 0,
                            produced: 0,
                    });
                }
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : SIEGETANK,
                        required: 3 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });

                if (player->getHouse() == ATREIDES) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SONICTANK,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                } else if (player->getHouse() == HARKONNEN || player->getHouse() == SARDAUKAR) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : DEVASTATOR,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                }
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : SIEGETANK,
                        required: 3 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : DEVIATOR,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 1);
        }

        if (!hasMission(2)) {
            // 25% chance we want another different attack force
            if (rnd(100) < 25) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : TANK,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                if (rnd(100) < 75) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SIEGETANK,
                            required: 1 + rnd(2),
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (player->getHouse() != ORDOS) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : LAUNCHER,
                            required: 1 + rnd(2),
                            ordered: 0,
                            produced: 0,
                    });
                }
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, 10 + rnd(10), 2);
            }
        }
        if (!hasMission(3)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 5) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : infantryKind,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 3);
            }
        }
        // build additional harvester (we want 2 harvesters in total)
        int harvesters = player->getAmountOfUnitsForType(HARVESTER);
        if (harvesters < 4) {
            if (!hasMission(4)) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : HARVESTER,
                        required: 2,
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 4);
            } else {
                // we already have mission 4, meaning we have the additional harvester.
                // If we have only 1 harvester than that means the harvester we got
                // from our refinery got destroyed (2 refineries in this mission).
                // Rebuild that one; and assign that to mission "4".
                if (!hasMission(5)) {
                    if (harvesters > 0) {
                        group = std::vector<S_groupKind>();
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : HARVESTER,
                                required: 2,
                                ordered: 0,
                                produced: 0,
                        });
                        // different mission ID
                        addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 5);
                    }
                }
            }
        }
        // build ornithopters
        if (player->getHouse() == ATREIDES || player->getHouse() == ORDOS) {
            if (player->hasAtleastOneStructure(HIGHTECH)) {
                if (!hasMission(6)) {
                    group = std::vector<S_groupKind>();
                    if (rnd(100) < 10) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : ORNITHOPTER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                        addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 6);
                    }
                }
            }
        }
        // build carry-alls if required
        if (player->hasAtleastOneStructure(HIGHTECH)) {
            if (!hasMission(7)) {
                group = std::vector<S_groupKind>();
                if (rnd(100) < 10) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : CARRYALL,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                    addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(10), 7);
                }
            }
        }
        // build ornithopters
        if (player->getHouse() == ATREIDES || player->getHouse() == ORDOS) {
            if (player->hasAtleastOneStructure(HIGHTECH)) {
                if (!hasMission(6)) {
                    group = std::vector<S_groupKind>();
                    if (rnd(100) < 10) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : ORNITHOPTER,
                                required: 1 + rnd(2),
                                ordered: 0,
                                produced: 0,
                        });
                        addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 6);
                    }
                }
            }
        }
        // build carry-alls if required
        if (player->hasAtleastOneStructure(HIGHTECH)) {
            if (!hasMission(7)) {
                group = std::vector<S_groupKind>();
                if (rnd(100) < 10) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : CARRYALL,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                    addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(10), 7);
                }
            }
        }
        if (player->hasAtleastOneStructure(PALACE)) {
            if (player->getHouse() == ATREIDES) {
                if (!hasMission(8)) {
                    group = std::vector<S_groupKind>();
                    // no need to add resources to mission, they are auto-produced.
                    // TODO: Think of a way to make this script/configurable
                    addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN, group, rnd(10), 8);
                }
            } else if (player->getHouse() == HARKONNEN || player->getHouse() == SARDAUKAR) {
                if (!hasMission(8)) {
                    group = std::vector<S_groupKind>();
                    // no need to add resources to mission, they are auto-produced.
                    // TODO: Think of a way to make this script/configurable
                    addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND, group, rnd(10), 8);
                }
            } else if (player->getHouse() == ORDOS) {
                if (!hasMission(8)) {
                    group = std::vector<S_groupKind>();
                    // no need to add resources to mission, they are auto-produced.
                    // TODO: Think of a way to make this script/configurable
                    addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR, group, rnd(10), 8);
                }
            }
        }
    }

    void cPlayerBrainCampaign::produceLevel4Missions(int trikeKind, int infantryKind) {
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        if (!hasMission(0)) {
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : trikeKind,
                        required: 1 + rnd(1),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 2 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 0);
        }
        if (!hasMission(1)) {
            group = std::vector<S_groupKind>();
            group.push_back((S_groupKind) {
                    buildType: eBuildType::UNIT,
                    type : TANK,
                    required: 1 + rnd(3),
                    ordered: 0,
                    produced: 0,
            });
            group.push_back((S_groupKind) {
                    buildType: eBuildType::UNIT,
                    type : QUAD,
                    required: 1,
                    ordered: 0,
                    produced: 0,
            });
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, 10 + rnd(10), 1);
        }
        if (!hasMission(2)) {
            group = std::vector<S_groupKind>();
            if (rnd(100) < 50) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : infantryKind,
                        required: 2 + rnd(4),
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 2);
            }
        }

        // build additional harvester (we want 2 harvesters in total)
        int harvesters = player->getAmountOfUnitsForType(HARVESTER);
        if (harvesters < 2) {
            if (!hasMission(3)) {
                group = std::vector<S_groupKind>();
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : HARVESTER,
                        required: 1,
                        ordered: 0,
                        produced: 0,
                });
                addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 3);
            } else {
                // we already have mission 3, meaning we have the additional harvester.
                // If we have only 1 harvester than that means the harvester we got
                // from our refinery got destroyed. Rebuild that one; and assign that to mission "4".
                if (!hasMission(4)) {
                    if (harvesters > 0) {
                        group = std::vector<S_groupKind>();
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : HARVESTER,
                                required: 1,
                                ordered: 0,
                                produced: 0,
                        });
                        // different mission ID
                        addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(25), 4);
                    }
                }
            }
        }
    }

    void cPlayerBrainCampaign::produceLevel3Missions(int trikeKind, int soldierKind, int infantryKind) {
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        if (!hasMission(0)) {
            if (player->getHouse() != HARKONNEN && player->getHouse() != SARDAUKAR) {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 1 + rnd(2),
                        ordered: 0,
                        produced: 0,
                });
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : trikeKind,
                        required: 1 + rnd(1),
                        ordered: 0,
                        produced: 0,
                });
            } else {
                group.push_back((S_groupKind) {
                        buildType: eBuildType::UNIT,
                        type : QUAD,
                        required: 2 + rnd(3),
                        ordered: 0,
                        produced: 0,
                });
            }
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 0);
        }
        if (!hasMission(1)) {
            group = std::vector<S_groupKind>();

            group.push_back((S_groupKind) {
                    buildType: eBuildType::UNIT,
                    type : soldierKind,
                    required: 1 + rnd(3),
                    ordered: 0,
                    produced: 0,
            });
            group.push_back((S_groupKind) {
                    buildType: eBuildType::UNIT,
                    type : infantryKind,
                    required: 1 + rnd(2),
                    ordered: 0,
                    produced: 0,
            });
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 1);
        }
    }

    void cPlayerBrainCampaign::produceLevel2Missions(int soldierKind, int infantryKind) {
        if (!hasMission(0)) {
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            group.push_back((S_groupKind) {
                    buildType: eBuildType::UNIT,
                    type : soldierKind,
                    required: 2 + rnd(1),
                    ordered: 0,
                    produced: 0,
            });
            group.push_back((S_groupKind) {
                    buildType: eBuildType::UNIT,
                    type : infantryKind,
                    required: 1 + rnd(1),
                    ordered: 0,
                    produced: 0,
            });
            addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(10), 0);
        }
    }

    bool cPlayerBrainCampaign::hasMission(const int id) {
        auto position = std::find_if(missions.begin(), missions.end(), [&id](const cPlayerBrainMission & mission){ return mission.getUniqueId() == id; });
        bool hasMission = position != missions.end();
        return hasMission;
    }

    void cPlayerBrainCampaign::addMission(ePlayerBrainMissionKind kind, const std::vector<S_groupKind> &group,
                                          int initialDelay,
                                          int id) {
        cPlayerBrainMission someMission(player, kind, this, group, initialDelay, id);
        missions.push_back(someMission);
    }

    void cPlayerBrainCampaign::thinkState_Evaluate() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_Evaluate()");
        player->log(msg);

        if (player->getAmountOfStructuresForType(CONSTYARD) == 0) {
            // no constyards, endgame
            changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_ENDGAME);
            return;
        }

        // Re-iterate over all (pending) buildOrders - and check if we are still going to do that or we might want to
        // up/downplay some priorities and re-sort?
        // Example: Money is short, Harvester is in build queue, but not high in priority?

        // take a little rest, before going into a new loop again?
        TIMER_rest = cPlayerBrain::RestTime;
        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_REST);
    }

    void cPlayerBrainCampaign::thinkState_EndGame() {
        // ...
//    char msg[255];
//    sprintf(msg, "cPlayerBrainCampaign::thinkState_EndGame(), for player [%d]", player->getId());
//    logbook(msg);
    }

    void cPlayerBrainCampaign::thinkState_ProcessBuildOrders() {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::thinkState_ProcessBuildOrders()");
        player->log(msg);

        // check if we can find a similar build order
        for (auto &buildOrder : buildOrders) {
            if (buildOrder.state != buildOrder::eBuildOrderState::PROCESSME)
                continue; // only process those which are marked

            assert(buildOrder.buildId > -1 && "(cPlayerBrainCampaign) A build order with no buildId got in the buildOrders list, which is not allowed!");

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
            } else if (buildOrder.buildType == eBuildType::SPECIAL) {
                if (player->startBuildingSpecial(buildOrder.buildId)) {
                    buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
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

        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_EVALUATE);
    }

    void cPlayerBrainCampaign::changeThinkStateTo(const ePlayerBrainCampaignThinkState& newState) {
        char msg[255];
        sprintf(msg, "cPlayerBrainCampaign::changeThinkStateTo(), from %s to %s",
                ePlayerBrainCampaignThinkStateString(thinkState),
                ePlayerBrainCampaignThinkStateString(newState));
        player->log(msg);
        this->thinkState = newState;
    }

    void cPlayerBrainCampaign::thinkState_Rest() {
        if (TIMER_rest > 0) {
            TIMER_rest--;
            char msg[255];
            sprintf(msg, "cPlayerBrainCampaign::thinkState_Rest(), rest %d", TIMER_rest);
            player->log(msg);
            return;
        }

        // resting is done, now go into the scan base/missions/evaluate loop
        changeThinkStateTo(ePlayerBrainCampaignThinkState::PLAYERBRAIN_CAMPAIGN_STATE_SCAN_BASE);
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
            // non peaceful state, what to do? react? etc.
        }
    }

    void cPlayerBrainCampaign::thinkFast() {
        for (auto &mission : missions) {
            mission.think();
        }
    }

}