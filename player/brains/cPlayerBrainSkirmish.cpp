#include <algorithm>
#include "include/d2tmh.h"
#include "cPlayerBrainSkirmish.h"


namespace brains {

    cPlayerBrainSkirmish::cPlayerBrainSkirmish(cPlayer *player) : cPlayerBrain(player) {
        state = ePlayerBrainState::PLAYERBRAIN_PEACEFUL;
        thinkState = ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_REST;
//         timer is substracted every 100 ms with 1 (ie, 10 == 10*100 = 1000ms == 1 second)
//         10*60 -> 1 minute. * 4 -> 4 minutes
//        TIMER_rest = (10 * 60) * 4;
        TIMER_rest = 100;
        if (game.bNoAiRest) {
            TIMER_rest = 10;
        }
        myBase = std::vector<S_structurePosition>();
        buildOrders = std::vector<S_buildOrder>();
        discoveredEnemyAtCell = std::set<int>();
        economyState = ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_NORMAL;
        COUNT_badEconomy = 0;
    }

    cPlayerBrainSkirmish::~cPlayerBrainSkirmish() {
        discoveredEnemyAtCell.clear();
        buildOrders.clear();
        myBase.clear();
        missions.clear();
    }

    /**
     * Think function called every tick (handleTimerUnits, ~ 100 ms).
     */
    void cPlayerBrainSkirmish::think() {
        // for now use a switch statement for this state machine. If we need anything
        // more sophisticated we can always use the State Pattern.
        switch (thinkState) {
            case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_REST:
                thinkState_Rest();
                return;
            case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_BASE:
                thinkState_Base();
                return;
            case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_MISSIONS:
                thinkState_Missions();
                return;
            case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_PROCESS_BUILDORDERS:
                thinkState_ProcessBuildOrders();
                return;
            case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_EVALUATE:
                thinkState_Evaluate();
                return;
            case ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_ENDGAME:
                thinkState_EndGame();
                return;
        }

        // now do some real stuff

        char msg[255];
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainSkirmish::think(), for player [%d] - FINISHED", player->getId());
        logbook(msg);
    }

    void cPlayerBrainSkirmish::addBuildOrder(S_buildOrder order) {
        // check if we can find a similar build order
        if (order.buildType == eBuildType::STRUCTURE) {
            for (auto &buildOrder : buildOrders) {
                if (buildOrder.buildType != order.buildType) continue;
                if (buildOrder.buildId != order.buildId) continue;

                if (order.buildType == eBuildType::STRUCTURE) {
                    if (buildOrder.placeAt != order.placeAt) continue;
                }

                // found same, if so, then simply increase priority?
                //        buildOrder.priority++;
                return; // stop
            }
        }
        buildOrders.push_back(order);

        // re-order based on priority
        std::sort(buildOrders.begin(), buildOrders.end(), [](const S_buildOrder &lhs, const S_buildOrder &rhs) {
            return lhs.priority > rhs.priority;
        });

        char msg[255];
        sprintf(msg,
                "cPlayerBrainSkirmish::addBuildOrder() - player [%d / %s] results into the following build orders:",
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

    void cPlayerBrainSkirmish::onNotify(const s_GameEvent &event) {
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

    void cPlayerBrainSkirmish::onMyStructureCreated(const s_GameEvent &event) {
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
                    "cPlayerBrainSkirmish::onNotify() - player [%d / %s] concluded to add structure %s to base register:",
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

    void cPlayerBrainSkirmish::onMyStructureDestroyed(const s_GameEvent &event) {
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

    void cPlayerBrainSkirmish::onMyStructureAttacked(const s_GameEvent &event) {
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

    void cPlayerBrainSkirmish::onMyStructureDecayed(const s_GameEvent &event) {
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

    void cPlayerBrainSkirmish::thinkState_Base() {
        char msg[255];
        sprintf(msg, "cPlayerBrainSkirmish::thinkState_ScanBase(), for player [%d]", player->getId());
        logbook(msg);

        // structure placement is done in thinkState_ProcessBuildOrders() !

        if (!player->isBuildingStructure() && !player->isBuildingStructureAwaitingPlacement() && !hasBuildOrderQueuedForStructure()) {
            // think about what structure to build AND where to place it
            const s_SkirmishPlayer_PlaceForStructure &result = thinkAboutNextStructureToBuildAndPlace();
            // add it to the build queue

            if (result.structureType > -1 && result.cell > -1) {
                addBuildOrder((S_buildOrder) {
                        buildType : STRUCTURE,
                        priority : 1,
                        buildId : result.structureType,
                        placeAt : result.cell,
                        state : buildOrder::PROCESSME,
                });
            }
        }

        // think about upgrades as well
        if (economyState == PLAYERBRAIN_ECONOMY_STATE_NORMAL) {
            if (player->hasAtleastOneStructure(LIGHTFACTORY)) {
                if (player->getHouse() != HARKONNEN) {
                    eCantBuildReason reason = player->canBuildUnit(QUAD);
                    if (reason == eCantBuildReason::REQUIRES_UPGRADE) {
                        // all need to upgrade except Harkonnen
                        cBuildingListItem *pItem = player->isUpgradeAvailableToGrantUnit(QUAD);
                        if (pItem) {
                            player->startUpgrading(pItem->getBuildId());
                        } else {
                            assert(false && "Expected to be able to upgrade for QUAD");
                        }
                    }
                }
            }
        }

        // reset timer (for the next time we end up here)
        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_MISSIONS);
    }

    void cPlayerBrainSkirmish::thinkState_Missions() {
        char msg[255];
        sprintf(msg, "cPlayerBrainSkirmish::thinkState_Missions(), for player [%d]", player->getId());
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

        // all missions are allowed to think now
        for (auto &mission : missions) {
            mission.think();
        }

        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_PROCESS_BUILDORDERS);
    }

    void cPlayerBrainSkirmish::produceMissions() {
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

        std::vector<S_groupKind> group = std::vector<S_groupKind>();

        if (economyState == ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_IMPROVE) {
            int idealAmountHarvesters = player->getAmountOfStructuresForType(REFINERY) * 2;
            int amountOfHarvesters = player->getAmountOfUnitsForType(HARVESTER);
            if (player->hasAtleastOneStructure(HEAVYFACTORY)) {
                if (amountOfHarvesters < idealAmountHarvesters) {
                    if (!hasMission(MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_HARVESTER)) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : HARVESTER,
                                required: 1,
                                ordered: 0,
                                produced: 0,
                        });
                        addMission(ePlayerBrainMissionKind::PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(15), MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_HARVESTER);
                    }
                }
            }

            // TODO: make this even smarter (ie don't build when also building harvester, etc)
            if (player->hasAtleastOneStructure(HIGHTECH)) {
                int idealAmountOfCarryAlls = (amountOfHarvesters/2)+1;
                if (player->getAmountOfUnitsForType(CARRYALL) < idealAmountOfCarryAlls) {
                    if (!hasMission(MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_CARRYALL)) {
                        group.push_back((S_groupKind) {
                                buildType: eBuildType::UNIT,
                                type : CARRYALL,
                                required: 1,
                                ordered: 0,
                                produced: 0,
                        });
                        addMission(ePlayerBrainMissionKind::PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(15), MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_CARRYALL);
                    }
                }
            }
        }

        if (state == ePlayerBrainState::PLAYERBRAIN_PEACEFUL) {
            //
            return;
        }

        if (state == ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED) {
            if (!hasMission(1)) {
                // TODO: depending on upgrades available; create appropiate army, for now depend on non-upgradable things
                if (player->hasAtleastOneStructure(HEAVYFACTORY)) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : TANK,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                }
                if (player->hasAtleastOneStructure(LIGHTFACTORY)) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : trikeKind,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                }

                if (player->hasAtleastOneStructure(WOR)) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : TROOPER,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                }

                if (player->hasAtleastOneStructure(BARRACKS)) {
                    group.push_back((S_groupKind) {
                            buildType: eBuildType::UNIT,
                            type : SOLDIER,
                            required: 2,
                            ordered: 0,
                            produced: 0,
                    });
                }

                addMission(ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), 1);
            }
            return;
        }

        // ..?
    }

    bool cPlayerBrainSkirmish::hasMission(const int id) {
        auto position = std::find_if(missions.begin(), missions.end(), [&id](const cPlayerBrainMission & mission){ return mission.getUniqueId() == id; });
        bool hasMission = position != missions.end();
        return hasMission;
    }

    void cPlayerBrainSkirmish::addMission(ePlayerBrainMissionKind kind, const std::vector<S_groupKind> &group,
                                          int initialDelay,
                                          int id) {
        cPlayerBrainMission someMission(player, kind, this, group, initialDelay, id);
        missions.push_back(someMission);
    }

    void cPlayerBrainSkirmish::thinkState_Evaluate() {
        char msg[255];
        sprintf(msg, "cPlayerBrainSkirmish::thinkState_Evaluate(), for player [%d], credits [%d], COUNT_badEconomy [%d], economyState [%s]", player->getId(), player->getCredits(), COUNT_badEconomy,
                ePlayerBrainSkirmishEconomyStateString(economyState));
        logbook(msg);

        if (player->getAmountOfStructuresForType(CONSTYARD) == 0) {
            // no constyards, endgame
            changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_ENDGAME);
            return;
        }

        // Re-iterate over all (pending) buildOrders - and check if we are still going to do that or we might want to
        // up/downplay some priorities and re-sort?
        // Example: Money is short, Harvester is in build queue, but not high in priority?
        if (player->hasAtleastOneStructure(REFINERY)) {
            if (economyState == PLAYERBRAIN_ECONOMY_STATE_NORMAL) {
                if (player->getCredits() < 150) {
                    // count the times we are in this shape, after a certain time we switch to IMPROVE state
                    COUNT_badEconomy++;
                    if (COUNT_badEconomy > 10) {
                        changeEconomyStateTo(PLAYERBRAIN_ECONOMY_STATE_IMPROVE);
                    }
                }
            } else if (economyState == PLAYERBRAIN_ECONOMY_STATE_IMPROVE) {
                if (player->getCredits() > 150) {
                    COUNT_badEconomy--;
                    if (COUNT_badEconomy < 3) {
                        COUNT_badEconomy = 0;
                        changeEconomyStateTo(PLAYERBRAIN_ECONOMY_STATE_NORMAL);
                    }
                }
            }
        }

        // take a little rest, before going into a new loop again?
        TIMER_rest = 25;
        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_REST);
    }

    void cPlayerBrainSkirmish::thinkState_EndGame() {
        // ...
//    char msg[255];
//    sprintf(msg, "cPlayerBrainSkirmish::thinkState_EndGame(), for player [%d]", player->getId());
//    logbook(msg);
    }

    void cPlayerBrainSkirmish::thinkState_ProcessBuildOrders() {
        char msg[255];
        sprintf(msg, "cPlayerBrainSkirmish::thinkState_ProcessBuildOrders(), for player [%d]", player->getId());
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

            struct S_buildOrder * matchingOrder = nullptr;
            for (auto &buildOrder : buildOrders) {
                if (buildOrder.buildType != eBuildType::STRUCTURE) {
                    continue;
                }
                if (buildOrder.buildId == structureType) {
                    matchingOrder = &buildOrder;
                }
            }

            if (matchingOrder) {
                int iCll = matchingOrder->placeAt;

                cBuildingListItem *pItem = player->getStructureBuildingListItemBeingBuilt();
                const s_PlaceResult &placeResult = cStructureFactory::getInstance()->canPlaceStructureAt(iCll,
                                                                                                         pItem->getBuildId());

                if (placeResult.success) {
                    player->placeStructure(iCll, pItem);
                    matchingOrder->state = buildOrder::eBuildOrderState::REMOVEME;
                } else {
                    // unable to place it, we determined previously that we could, so what to do?

                    bool changedPlacePosition = false;
                    if (placeResult.badTerrain) {
                        // pick a new spot?
                        int newCell = findCellToPlaceStructure(pItem->getBuildId());
                        if (newCell < 0) {
                            // cancel building this thing
                            player->cancelBuildingListItem(pItem);
                            matchingOrder->state = buildOrder::eBuildOrderState::REMOVEME;
                        } else {
                            // update the cell, it will be retried in the next think iteration
                            matchingOrder->placeAt = newCell;
                            changedPlacePosition = true;
                        }
                    }

                    // find any unit that is from us, and move it away
                    // if there is any enemy player, then find a new place.
                    if (!placeResult.unitIds.empty()) {
                        for (auto &unitId : placeResult.unitIds) {
                            cUnit &aUnit = unit[unitId];
                            if (!aUnit.isValid()) continue;
                            if (aUnit.getPlayer() == player) {
                                // move it
                                aUnit.move_to(map.getRandomCellFrom(aUnit.getCell(), 3));
                            } else {
                                // if (aUnit.getPlayer()->isSameTeamAs(player)) { // who knows, a teammate is willing to move away when it is AI?
                                if (!changedPlacePosition) {
                                    int newCell = findCellToPlaceStructure(pItem->getBuildId());
                                    if (newCell < 0) {
                                        // cancel building this thing
                                        player->cancelBuildingListItem(pItem);
                                        matchingOrder->state = buildOrder::eBuildOrderState::REMOVEME;
                                    } else {
                                        // update the cell, it will be retried in the next think iteration
                                        matchingOrder->placeAt = newCell;
                                        changedPlacePosition = true;
                                    }
                                }
                            }
                        }
                    }
                    
                    // find any unit that is from us, and move it away
                    // if there is any enemy player, then find a new place.
                    if (!placeResult.structureIds.empty()) {
                        if (!changedPlacePosition) {
                            int newCell = findCellToPlaceStructure(pItem->getBuildId());
                            if (newCell < 0) {
                                // cancel building this thing
                                player->cancelBuildingListItem(pItem);
                                matchingOrder->state = buildOrder::eBuildOrderState::REMOVEME;
                            } else {
                                // update the cell, it will be retried in the next think iteration
                                matchingOrder->placeAt = newCell;
                                changedPlacePosition = true;
                            }
                        }
                    }
                }
            } else {
                // whut? there is something in progress, but we are not aware of it? cancel build
                player->cancelStructureBuildingListItemBeingBuilt();
            }
        }

        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_EVALUATE);
    }

    void cPlayerBrainSkirmish::changeThinkStateTo(const ePlayerBrainSkirmishThinkState& newState) {
        char msg[255];
        sprintf(msg, "cPlayerBrainSkirmish::changeThinkStateTo(), for player [%d] - from %s to %s", player->getId(),
                ePlayerBrainSkirmishThinkStateString(thinkState),
                ePlayerBrainSkirmishThinkStateString(newState));
        logbook(msg);
        this->thinkState = newState;
    }

    void cPlayerBrainSkirmish::changeEconomyStateTo(const ePlayerBrainSkirmishEconomyState &newState) {
        char msg[255];
        sprintf(msg, "cPlayerBrainSkirmish::changeEconomyStateTo(), for player [%d] - from %s to %s", player->getId(),
                ePlayerBrainSkirmishEconomyStateString(economyState),
                ePlayerBrainSkirmishEconomyStateString(newState));
        logbook(msg);
        this->economyState = newState;
    }

    void cPlayerBrainSkirmish::thinkState_Rest() {
        if (TIMER_rest > 0) {
            TIMER_rest--;
            char msg[255];
            sprintf(msg, "cPlayerBrainSkirmish::thinkState_Rest(), for player [%d] - rest %d", player->getId(), TIMER_rest);
            logbook(msg);
            return;
        }

        // resting is done, now go into the base/missions/evaluate loop
        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_BASE);
    }

    void cPlayerBrainSkirmish::onEntityDiscoveredEvent(const s_GameEvent &event) {
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

    s_SkirmishPlayer_PlaceForStructure cPlayerBrainSkirmish::thinkAboutNextStructureToBuildAndPlace() {
        s_SkirmishPlayer_PlaceForStructure result;

        // think about structure type first
        int structureTypeToBuild = getStructureToBuild();

        // then where to place it
        int cellToPlaceStructureAt = -1;
        if (structureTypeToBuild > -1) {
            cellToPlaceStructureAt = findCellToPlaceStructure(structureTypeToBuild);
        }

        result.structureType = structureTypeToBuild;
        result.cell = cellToPlaceStructureAt;
        return result;

    }

    /**
     * A very crude way to determine which structure to build.
     * This can be improved much further still, but that will be done another time...
     * @return
     */
    int cPlayerBrainSkirmish::getStructureToBuild() const {
        if (!player->bEnoughPower()) {
            return WINDTRAP;
        }

        int structureIWantToBuild = getStructureIdToBuildWithoutConsideringPowerUsage();

        // determine if we have enough power for the thing we want to build, if not, build a windtrap first...
        if (structureIWantToBuild > -1 && !player->hasEnoughPowerFor(structureIWantToBuild)) {
            return WINDTRAP;
        }
        
        // can build it?
        eCantBuildReason reason = player->canBuildStructure(structureIWantToBuild);
        if (reason != eCantBuildReason::NONE) {
            // there is a reason why we cannot build it
            switch (reason) {
                case REQUIRES_UPGRADE:
                    // build the upgrade instead
                    if (structureIWantToBuild == RTURRET) {
                        // upgrade ...
                        cBuildingListItem *pItem = player->isUpgradeAvailableToGrantStructure(RTURRET);
                        if (!pItem) {
                            // we first need to upgrade to SLAB4 before we can upgrade to RTURRET
                            pItem = player->isUpgradeAvailableToGrantStructure(SLAB4);
                        }

                        if (pItem != nullptr) {
                            // we have something to upgrade
                            if (!player->startUpgrading(pItem->getBuildId())) {
                                // failed to build upgrade.
                            } else {
                                // we should wait a bit before re-evaluating base building
                            }
                        }

                        // in both cases, we should first wait, so return -1 anyways
                        return -1;
                    } else if (structureIWantToBuild == SLAB4) {

                    }
                    break;
                case REQUIRES_STRUCTURE:
                    // update state, we don't have a constyard anymore!
                    break;
                case NOT_ENOUGH_MONEY:
                    // economy is bad? set state for that
                    break;
                case ALREADY_BUILDING:
                    // already building something, so don't return any id to build
                    return -1;
            }
        }

        return structureIWantToBuild;
    }

    int cPlayerBrainSkirmish::getStructureIdToBuildWithoutConsideringPowerUsage() const {
        player->logStructures();

        if (!player->hasAtleastOneStructure(REFINERY))      return REFINERY;
        if (economyState == PLAYERBRAIN_ECONOMY_STATE_IMPROVE) {
            if (player->getAmountOfStructuresForType(REFINERY) < 3) {
                return REFINERY;
            }
        }
        // play around with the order... (depending on difficulty, or type of ai, etc?)
        if (!player->hasAtleastOneStructure(LIGHTFACTORY))  return LIGHTFACTORY;
        if (!player->hasAtleastOneStructure(RADAR))  return RADAR;
        if (!player->hasAtleastOneStructure(HEAVYFACTORY))  return HEAVYFACTORY;
        if (!player->hasAtleastOneStructure(STARPORT))  return STARPORT;

        if (player->getAmountOfStructuresForType(TURRET) < 2)  return TURRET;
        if (player->getAmountOfStructuresForType(RTURRET) < 6)  return RTURRET;

        if (!player->hasAtleastOneStructure(PALACE))  return PALACE;

        // nothing to build
        return -1;
    }

    int cPlayerBrainSkirmish::findCellToPlaceStructure(int structureType) {
        // find place (fast, if possible), where to place it
        // ignore any units (we can move them out of the way). But do take
        // terrain and other structures into consideration!

        int centerOfBase = player->getFocusCell();
        
        const std::vector<int> &allMyStructuresAsId = player->getAllMyStructuresAsId();
        std::vector<int> potentialCells = std::vector<int>();

        int iWidth = structures[structureType].bmp_width / TILESIZE_WIDTH_PIXELS;
        int iHeight = structures[structureType].bmp_height / TILESIZE_HEIGHT_PIXELS;

        cStructureFactory *pStructureFactory = cStructureFactory::getInstance();

        for (auto &id : allMyStructuresAsId) {
            cAbstractStructure * aStructure = structure[id];

            // go around any structure, and try to find a cell where we can place a structure.
            int iStartX = map.getCellX(aStructure->getCell());
            int iStartY = map.getCellY(aStructure->getCell());

            int iEndX = iStartX + aStructure->getWidth(); // not plus 1 because iStartX is 1st cell
            int iEndY = iStartY + aStructure->getHeight(); // not plus 1 because iStartY is 1st cell

            // start is topleft/above structure, but also take size of the structure to place
            // into acount. So ie, a structure of 2x2 will be attempted (at first) at y - 2.
            // attempt at 'top' first:
            int topLeftX = iStartX - iWidth;
            int topLeftY = iStartY - iHeight;

            // check: from top left to top right
            for (int sx = topLeftX; sx < iEndX; sx++) {
                int cell = map.makeCell(sx, topLeftY);

                bool canPlaceStructureAt = pStructureFactory->canPlaceStructureAt(cell, structureType).success;
                if (canPlaceStructureAt) {
                    potentialCells.push_back(cell);
                }
            }

            int bottomLeftX = topLeftX;
            int bottomLeftY = iEndY;
            // check: from bottom left to bottom right
            for (int sx = bottomLeftX; sx < iEndX; sx++) {
                int cell = map.makeCell(sx, bottomLeftY);

                bool canPlaceStructureAt = pStructureFactory->canPlaceStructureAt(cell, structureType).success;
                if (canPlaceStructureAt) {
                    potentialCells.push_back(cell);
                }
            }

            // left to structure (not from top!)
            int justLeftX = topLeftX;
            int justLeftY = iStartY;
            for (int sy = justLeftY; sy < iEndY; sy++) {
                int cell = map.makeCell(justLeftX, sy);

                bool canPlaceStructureAt = pStructureFactory->canPlaceStructureAt(cell, structureType).success;
                if (canPlaceStructureAt) {
                    potentialCells.push_back(cell);
                }
            }

            // right to structure (not top!)
            int justRightX = iEndX;
            int justRightY = iStartY;
            for (int sy = justRightY; sy < iEndY; sy++) {
                int cell = map.makeCell(justRightX, sy);

                bool canPlaceStructureAt = pStructureFactory->canPlaceStructureAt(cell, structureType).success;
                if (canPlaceStructureAt) {
                    potentialCells.push_back(cell);
                }
            }

            // if we have found any we randomly abort
            if (!potentialCells.empty()) {
                if (rnd(100) < 33) {
                    break;
                }
            }
        }

        if (!potentialCells.empty()) {
            if (structureType == TURRET || structureType == RTURRET) {
//                // first shuffle, before going through the list
//                std::random_shuffle(potentialCells.begin(), potentialCells.end());
//
//                std::vector<int> potentialFurtherCells = std::vector<int>();
//                int found = 0;
//                double distance = 128; // arbitrary distance as 'border'
//                for (auto &potentialCell : potentialCells) {
//                    double dist = map.distance(centerOfBase, potentialCell);
//                    if (dist > distance) {
//                        potentialFurtherCells.push_back(potentialCell);
//                        found++;
//                        if (found > 5) {
//                            break;
//                        }
//                    }
//                }
//
//                if (!potentialFurtherCells.empty()) {
//                    // shuffle the 5 'furthest'
//                    std::random_shuffle(potentialFurtherCells.begin(), potentialFurtherCells.end());
//                }

                // for now pick random position, but in the future do something more smart
                std::random_shuffle(potentialCells.begin(), potentialCells.end());
            } else {
                // found one, shuffle, and then return the first
                std::random_shuffle(potentialCells.begin(), potentialCells.end());
            }
            return potentialCells.front();
        }

        return -1;
    }

    bool cPlayerBrainSkirmish::hasBuildOrderQueuedForStructure() {
        for (auto &buildOrder : buildOrders) {
            if (buildOrder.buildType == eBuildType::STRUCTURE && buildOrder.state != buildOrder::REMOVEME) {
                return true;
            }
        }
        return false;
    }

}