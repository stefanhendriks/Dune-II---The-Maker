#include "cPlayerBrainSkirmish.h"

#include "actions/cRespondToThreatAction.h"
#include "enums.h"
#include "d2tmc.h"
#include "player/cPlayer.h"

#include <fmt/core.h>

#include <algorithm>

namespace brains {

    cPlayerBrainSkirmish::cPlayerBrainSkirmish(cPlayer *player) :
        cPlayerBrain(player),
        m_state(ePlayerBrainState::PLAYERBRAIN_PEACEFUL),
        m_thinkState(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_REST) {
//         timer is subtracted every 100 ms with 1 (ie, 10 == 10*100 = 1000ms == 1 second)
//         10*60 -> 1 minute. * 4 -> 4 minutes
//        m_TIMER_rest = (10 * 60) * 4;
        m_TIMER_rest = rnd(25); // todo: based on difficulty?
        if (game.m_noAiRest) {
            m_TIMER_rest = 10;
        }
        m_TIMER_produceMissionCooldown = 0;
        m_TIMER_ai = 0; // increased every 100 ms with 1. (ie 10 ticks is 1 second)
        m_myBase = std::vector<S_structurePosition>();
        m_buildOrders = std::vector<S_buildOrder>();
        m_discoveredEnemyAtCell = std::set<int>();
        m_economyState = ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_NORMAL;
        m_economyScore = 50;
        m_centerOfBaseCell = 0;
    }

    cPlayerBrainSkirmish::~cPlayerBrainSkirmish() {
        m_discoveredEnemyAtCell.clear();
        m_buildOrders.clear();
        m_myBase.clear();
        m_missions.clear();
    }

    void cPlayerBrainSkirmish::think() {
        m_TIMER_ai++;
        // for now use a switch statement for this state machine. If we need anything
        // more sophisticated we can always use the State Pattern.
        switch (m_thinkState) {
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

        log("think() - FINISHED");
    }

    void cPlayerBrainSkirmish::addBuildOrder(S_buildOrder order) {
        // check if we can find a similar build order
        if (order.buildType == eBuildType::STRUCTURE) {
            for (auto &buildOrder : m_buildOrders) {
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
        m_buildOrders.push_back(order);

        // re-order based on priority
        std::sort(m_buildOrders.begin(), m_buildOrders.end(), [](const S_buildOrder &lhs, const S_buildOrder &rhs) {
            return lhs.priority > rhs.priority;
        });

        log("addBuildOrder() - results into the following build orders:");

        int id = 0;
        for (auto &buildOrder : m_buildOrders) {
            std::string msg;
            if (buildOrder.buildType == eBuildType::UNIT) {
                msg = fmt::format("[{}] - type = UNIT, buildId = {} (={}), priority = {}, state = {}", id, buildOrder.buildId,
                        sUnitInfo[buildOrder.buildId].name, buildOrder.priority, eBuildOrderStateString(buildOrder.state));
            } else if (buildOrder.buildType == eBuildType::STRUCTURE) {
                msg = fmt::format("[{}] - type = STRUCTURE, buildId = {} (={}), priority = {}, place at {}, state = {}", id,
                        buildOrder.buildId, sStructureInfo[buildOrder.buildId].name, buildOrder.priority,
                        buildOrder.placeAt, eBuildOrderStateString(buildOrder.state));
            } else if (buildOrder.buildType == eBuildType::SPECIAL) {
                msg = fmt::format("[{}] - type = SPECIAL, buildId = {} (={}), priority = {}, state = {}", id, buildOrder.buildId,
                        sSpecialInfo[buildOrder.buildId].description, buildOrder.priority, eBuildOrderStateString(buildOrder.state));
            } else if (buildOrder.buildType == eBuildType::BULLET) {
                msg = fmt::format("[{}] - type = SPECIAL, buildId = {} (=NOT YET IMPLEMENTED), priority = {}, state = {}", id,
                        buildOrder.buildId, buildOrder.priority, eBuildOrderStateString(buildOrder.state));
            }
            log(msg);

            id++;
        }
    }

    void cPlayerBrainSkirmish::onNotifyGameEvent(const s_GameEvent &event) {
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
                    default:
                        break;
                }
            } else if (event.entityType == eBuildType::UNIT) {
                switch (event.eventType) {
                    case eGameEventType::GAME_EVENT_DAMAGED:
                        onMyUnitAttacked(event);
                        break;
                    default:
                        break;
                }
            }
        }

        if (event.eventType == eGameEventType::GAME_EVENT_DISCOVERED) {
            onEntityDiscoveredEvent(event);
        }

        // notify mission about any kind of event
        for (auto &mission : m_missions) {
            mission.onNotifyGameEvent(event);
        }
    }

    void cPlayerBrainSkirmish::onMyStructureCreated(const s_GameEvent &event) {
        // a structure was created, update our baseplan
        cAbstractStructure *pStructure = structure[event.entityID];

        if (event.entitySpecificType == PALACE) {
            // built a palace, create super weapon missions asap!
            m_TIMER_produceMissionCooldown = 0;
        }

        int placedAtCell = pStructure->getCell();
        bool foundExistingStructureInBase = false;
        for (auto &structurePosition : m_myBase) {
            if (!structurePosition.isDestroyed) continue; // not destroyed, hence cannot be rebuilt

            if (structurePosition.cell == placedAtCell) {
                if (structurePosition.type == event.entitySpecificType) {
                    // same structure type, at same place
                    structurePosition.structureId = event.entityID;
                    structurePosition.isDestroyed = false; // no longer destroyed!
                    foundExistingStructureInBase = true;
                } else {
                    // different structure type, but same place! So our base layout no longer matches up.
                    structurePosition.structureId = event.entityID;
                    structurePosition.type = structurePosition.type; // overwrite type
                    structurePosition.isDestroyed = false; // overwrite type
                }
            }
        }

        if (!foundExistingStructureInBase) {
            log(fmt::format("cPlayerBrainSkirmish::onNotifyGameEvent() - concluded to add structure {} to base register:",
                            pStructure->getStructureInfo().name));

            // new structure placed, update base register
            S_structurePosition position = {
                    .cell = pStructure->getCell(),
                    .type = pStructure->getType(),
                    .structureId = pStructure->getStructureId(),
                    .isDestroyed = pStructure->isDead()
            };

            if (pStructure->getType() == CONSTYARD) {
                m_centerOfBaseCell = pStructure->getCell();
            }

            m_myBase.push_back(position);
        }
    }

    void cPlayerBrainSkirmish::onMyStructureDestroyed(const s_GameEvent &event) {
        // a structure got destroyed, figure out which one it is in my base plan, and update its state
        for (auto &structurePosition : m_myBase) {
            if (structurePosition.structureId == event.entityID) {
                // this structure got destroyed, so mark it as destroyed in my base plan
                structurePosition.isDestroyed = true;
                // and add order to rebuild it
                addBuildOrder(S_buildOrder{
                        .buildType = STRUCTURE,
                        .priority = 1,
                        .buildId = structurePosition.type,
                        .placeAt = structurePosition.cell,
                        .state = buildOrder::PROCESSME,
                });
            }
        }
    }

    void cPlayerBrainSkirmish::onMyStructureAttacked(const s_GameEvent &event) {
        if (player->hasEnoughCreditsFor(50)) {
            cAbstractStructure *pStructure = structure[event.entityID];
            if (!pStructure->isRepairing()) {
                s_StructureInfo &sStructures = sStructureInfo[event.entitySpecificType];
                if (pStructure->getHitPoints() < sStructures.hp * 0.75) {
                    pStructure->startRepairing();
                }
            }
        }

        int unitIdThatAttacks = event.originId;
        if (unitIdThatAttacks > -1) {
            // respond to something that attacks us
            cUnit originUnit = unit[unitIdThatAttacks];
            if (originUnit.getPlayer()->isSameTeamAs(player)) {
                // friendly fire, ignore
                log(fmt::format("Unit {} who damaged my structure is from friendly player, ignoring.", unitIdThatAttacks).c_str());
                return;
            }

            int cell = originUnit.getCell();

            respondToThreat(&originUnit, nullptr, cell, 2 + rnd(4));
        }
    }

    void cPlayerBrainSkirmish::respondToThreat(cUnit *threat, cUnit *victim, int cellOriginOfThreat, int maxUnitsToOrder) {
        // when we pass in a cell here by accident, it gets easily > 20
        assert(maxUnitsToOrder < 20 && "Are you sure you used the correct parameter here?");

        cRespondToThreatAction(this->player, threat, victim, cellOriginOfThreat, maxUnitsToOrder).execute();
    }

    void cPlayerBrainSkirmish::onMyStructureDecayed(const s_GameEvent &event) {
        if (player->hasEnoughCreditsFor(50)) {
            cAbstractStructure *pStructure = structure[event.entityID];
            if (!pStructure->isRepairing()) {
                s_StructureInfo &sStructures = sStructureInfo[event.entitySpecificType];
                if (pStructure->getHitPoints() < sStructures.hp * 0.75) {
                    pStructure->startRepairing();
                }
            }
        }
    }

    void cPlayerBrainSkirmish::thinkState_Base() {
        log("thinkState_ScanBase()");

        // structure placement is done in thinkState_ProcessBuildOrders() !

        if (player->hasEnoughCreditsFor(500) && m_economyState == PLAYERBRAIN_ECONOMY_STATE_NORMAL) {
            // we have money to do a (unit) upgrade, structure sUpgradeInfo are done via the "thinkAboutNextStructure" thing
            // which will enforce an upgrade via that path
            if (player->startUpgradingForUnitIfPossible(QUAD)) {
                return;
            }

            if (player->startUpgradingForUnitIfPossible(TROOPERS)) {
                return;
            }

            if (player->startUpgradingForUnitIfPossible(INFANTRY)) {
                return;
            }

            if (player->startUpgradingForUnitIfPossible(MCV)) {
                return;
            }

            if (player->startUpgradingForUnitIfPossible(LAUNCHER)) {
                return;
            }

            if (player->startUpgradingForUnitIfPossible(SIEGETANK)) {
                return;
            }

            if (player->startUpgradingForUnitIfPossible(ORNITHOPTER)) {
                return;
            }
        }

        if (player->hasAtleastOneStructure(CONSTYARD)) {
            if (!player->isBuildingStructure() &&
                !player->isUpgradingConstyard() &&
                !player->isBuildingStructureAwaitingPlacement() &&
                !hasBuildOrderQueuedForStructure()) {
                // think about what structure to build AND where to place it

                const s_SkirmishPlayer_PlaceForStructure &result = thinkAboutNextStructureToBuildAndPlace();
                // add it to the build queue

                if (result.structureType > -1 && result.cell > -1) {
                    addBuildOrder(S_buildOrder{
                            .buildType = STRUCTURE,
                            .priority = 1,
                            .buildId = result.structureType,
                            .placeAt = result.cell,
                            .state = buildOrder::PROCESSME,
                    });
                }
            }
        }

        // reset timer (for the next time we end up here)
        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_MISSIONS);
    }

    void cPlayerBrainSkirmish::thinkState_Missions() {
        log("thinkState_Missions()");

        if (game.isDebugMode()) {
            log("Missions - before deleting");
            logMissions();
        }

        // delete any missions which are ended
        m_missions.erase(
                std::remove_if(
                        m_missions.begin(),
                        m_missions.end(),
                        [](cPlayerBrainMission m) { return m.isEnded(); }),
                m_missions.end()
        );

        if (game.isDebugMode()) {
            log("Missions - after deleting - before produceMissions()");
            logMissions();
        }

        produceMissions();

        if (game.isDebugMode()) {
            log("Missions - after produceMissions()");
            logMissions();
        }

        if (m_TIMER_ai > MOMENT_PRODUCE_ADDITIONAL_UNITS) {
            if (m_economyState == PLAYERBRAIN_ECONOMY_STATE_BAD) {
                // build additional harvesters and carryalls
                if (player->getAmountOfUnitsForType(HARVESTER) < 3) {
                    buildUnitIfICanAndNotAlreadyQueued(HARVESTER);
                }
                if (player->getAmountOfUnitsForType(CARRYALL) < 1) {
                    buildUnitIfICanAndNotAlreadyQueued(CARRYALL);
                }
            } else if (m_economyState == PLAYERBRAIN_ECONOMY_STATE_IMPROVE) {
                // build additional harvesters and carryalls
                if (player->getAmountOfUnitsForType(HARVESTER) < 6) {
                    buildUnitIfICanAndNotAlreadyQueued(HARVESTER);
                }
                if (player->getAmountOfUnitsForType(CARRYALL) < 3) {
                    buildUnitIfICanAndNotAlreadyQueued(CARRYALL);
                }
            } else {
                if (allMissionsAreDoneGatheringResources()) {
                    int chance = m_economyState == PLAYERBRAIN_ECONOMY_STATE_GOOD ? 25 : 10;
                    // build units, as long as we have some money on the bank.
                    // these units are produced without a mission.
                    if (rnd(100) < chance) {
                        // when the player has it, it will be build
                        buildUnitIfICanAndNotAlreadyQueued(SONICTANK);
                        buildUnitIfICanAndNotAlreadyQueued(DEVIATOR);
                        buildUnitIfICanAndNotAlreadyQueued(DEVASTATOR);
                    }
                    if (rnd(100) < chance) {
                        if (player->getAmountOfUnitsForType(SIEGETANK) < 8) {
                            buildUnitIfICanAndNotAlreadyQueued(SIEGETANK);
                        }
                    }
                    if (rnd(100) < chance) {
                        if (player->getAmountOfUnitsForType(LAUNCHER) < 6) {
                            buildUnitIfICanAndNotAlreadyQueued(LAUNCHER);
                        }
                    }
                    if (rnd(100) < chance) {
                        if (player->getAmountOfUnitsForType(QUAD) < 5) {
                            buildUnitIfICanAndNotAlreadyQueued(QUAD);
                        }
                    }
                    if (rnd(100) < chance) {
                        if (player->getAmountOfUnitsForType(TANK) < 4) {
                            buildUnitIfICanAndNotAlreadyQueued(TANK);
                        }
                    }
                    if (rnd(100) < chance) {
                        if (player->getAmountOfUnitsForType(ORNITHOPTER) < 3) {
                            buildUnitIfICanAndNotAlreadyQueued(ORNITHOPTER);
                        }
                    }
                    if (rnd(100) < chance) {
                        if (player->getAmountOfUnitsForType(HARVESTER) < 6) {
                            buildUnitIfICanAndNotAlreadyQueued(HARVESTER);
                        }
                    }
                    if (rnd(100) < chance) {
                        if (player->getAmountOfUnitsForType(CARRYALL) < 3) {
                            buildUnitIfICanAndNotAlreadyQueued(CARRYALL);
                        }
                    }
                }
            }
        }

        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_PROCESS_BUILDORDERS);
    }

    void cPlayerBrainSkirmish::logMissions() {
        for (auto &mission : m_missions) {
            mission.log("Exists");
        }
    }

    void cPlayerBrainSkirmish::buildUnitIfICanAndNotAlreadyQueued(int type) {
        if (player->canBuildUnitBool(type) && !hasBuildOrderQueuedForUnit(type)) {
            addBuildOrderForUnit(type);
        }
    }

    void cPlayerBrainSkirmish::addBuildOrderForUnit(int type) {
        addBuildOrder(S_buildOrder{
            .buildType = UNIT,
            .priority = 0,
            .buildId = type,
            .placeAt = -1,
            .state = buildOrder::PROCESSME,
        });
    }

    void cPlayerBrainSkirmish::produceMissions() {
        // super weapon missions are required to be active at all times.
        produceSuperWeaponMissionsWhenApplicable();

        // if cooldown is set, do that, so we don't spam missions in a very short amount of time
        if (m_TIMER_produceMissionCooldown > 0) {
            log(fmt::format("m_TIMER_produceMissionCooldown [{}] is in effect.", m_TIMER_produceMissionCooldown));
            m_TIMER_produceMissionCooldown--;
            return;
        }

        int scoutingUnitType = player->getScoutingUnitType();

        if (m_economyState == ePlayerBrainSkirmishEconomyState::PLAYERBRAIN_ECONOMY_STATE_IMPROVE) {
            produceEconomyImprovingMissions();
            return; // bail, because we don't want to build additional things that might worsen economy at this point
        }

        if (m_economyState == PLAYERBRAIN_ECONOMY_STATE_BAD) {
            log("produceMissions() - aborting because economy state is BAD");
            return; // do nothing for now
        }

        if (m_state == ePlayerBrainState::PLAYERBRAIN_PEACEFUL) {
            produceMissionsDuringPeacetime(scoutingUnitType);
        } else if (m_state == ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED) {
            produceAttackingMissions();
        }
    }

    void cPlayerBrainSkirmish::produceEconomyImprovingMissions() {
        int idealAmountHarvesters = player->getAmountOfStructuresForType(REFINERY) * 2;
        int amountOfHarvesters = player->getAmountOfUnitsForType(HARVESTER);
        if (player->hasAtleastOneStructure(HEAVYFACTORY)) {
            if (amountOfHarvesters < idealAmountHarvesters) {
                if (!hasMission(MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_HARVESTER)) {
                    std::vector<S_groupKind> group = std::vector<S_groupKind>();
                    group.push_back(S_groupKind{
                            .buildType = UNIT,
                            .type = HARVESTER,
                            .required = 1,
                            .ordered = 0,
                            .produced = 0,
                    });
                    addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(15), MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_HARVESTER);
                }
            }
        }

        // TODO: make this even smarter (ie don't build when also building harvester, etc)
        if (player->hasAtleastOneStructure(HIGHTECH)) {
            int idealAmountOfCarryAlls = (amountOfHarvesters/2)+1;
            if (player->getAmountOfUnitsForType(CARRYALL) < idealAmountOfCarryAlls) {
                if (!hasMission(MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_CARRYALL)) {
                    std::vector<S_groupKind> group = std::vector<S_groupKind>();
                    group.push_back(S_groupKind{
                            .buildType = UNIT,
                            .type = CARRYALL,
                            .required = 1,
                            .ordered = 0,
                            .produced = 0,
                    });
                    addMission(PLAYERBRAINMISSION_IMPROVE_ECONOMY, group, rnd(15), MISSION_IMPROVE_ECONOMY_BUILD_ADDITIONAL_CARRYALL);
                }
            }
        }
        // focus on economy first, before anything else
    }

    void cPlayerBrainSkirmish::produceSuperWeaponMissionsWhenApplicable() {
        if (player->couldBuildSpecial(SPECIAL_DEATHHAND) && !hasMission(SPECIAL_MISSION1)) {
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            // no need to add resources to mission, they are auto-produced.
            // TODO: Think of a way to make this script/configurable
            addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND, group, rnd(10), SPECIAL_MISSION1);
        }

        if (player->couldBuildSpecial(SPECIAL_SABOTEUR) && !hasMission(SPECIAL_MISSION2)) {
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            // no need to add resources to mission, they are auto-produced.
            // TODO: Think of a way to make this script/configurable
            addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR, group, rnd(10), SPECIAL_MISSION2);
        }

        if (player->couldBuildSpecial(SPECIAL_FREMEN) && !hasMission(SPECIAL_MISSION3)) {
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            // no need to add resources to mission, they are auto-produced.
            // TODO: Think of a way to make this script/configurable
            addMission(PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN, group, rnd(10), SPECIAL_MISSION3);
        }
    }

    void cPlayerBrainSkirmish::produceAttackingMissions() {
        if (!hasMission(MISSION_ATTACK1)) {
            produceSkirmishGroundAttackMission(MISSION_ATTACK1);
        } else if (!hasMission(MISSION_ATTACK2)) {
            produceSkirmishGroundAttackMission(MISSION_ATTACK2);
        } else if (!hasMission(MISSION_ATTACK3)) {
            produceSkirmishGroundAttackMission(MISSION_ATTACK3);
        } else if (!hasMission(MISSION_ATTACK4)) {
            produceSkirmishGroundAttackMission(MISSION_ATTACK4);
        }

        // separate mission for air attacks
        if (!hasMission(MISSION_ATTACK5)) {
            if (player->canBuildUnitBool(ORNITHOPTER)) {
                std::vector<S_groupKind> group = std::vector<S_groupKind>();
                group.push_back(S_groupKind{
                        .buildType = UNIT,
                        .type = ORNITHOPTER,
                        .required = 1 + rnd(3),
                        .ordered = 0,
                        .produced = 0,
                });
                addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15),
                           MISSION_ATTACK5);
            }
        }
    }

    void cPlayerBrainSkirmish::produceMissionsDuringPeacetime(int scoutingUnitType) {
        if (player->canBuildUnitBool(scoutingUnitType)) {
            if (!hasMission(MISSION_SCOUT1)) {
                // add scouting mission
                std::vector<S_groupKind> group = std::vector<S_groupKind>();
                group.push_back(S_groupKind{
                        .buildType = UNIT,
                        .type = scoutingUnitType,
                        .required = 1,
                        .ordered = 0,
                        .produced = 0,
                });

                addMission(PLAYERBRAINMISSION_KIND_EXPLORE, group, rnd(15),
                           MISSION_SCOUT1);
            } else if (!hasMission(MISSION_SCOUT2)) {
                // add scouting mission
                std::vector<S_groupKind> group = std::vector<S_groupKind>();
                group.push_back(S_groupKind{
                        .buildType = UNIT,
                        .type = scoutingUnitType,
                        .required = 1,
                        .ordered = 0,
                        .produced = 0,
                });

                addMission(PLAYERBRAINMISSION_KIND_EXPLORE, group, rnd(15),
                           MISSION_SCOUT2);
            } else if (!hasMission(MISSION_SCOUT3)) {
                // add scouting mission
                std::vector<S_groupKind> group = std::vector<S_groupKind>();
                group.push_back(S_groupKind{
                        .buildType = UNIT,
                        .type = scoutingUnitType,
                        .required = 1,
                        .ordered = 0,
                        .produced = 0,
                });

                addMission(PLAYERBRAINMISSION_KIND_EXPLORE, group, rnd(15), MISSION_SCOUT3);
            }
        }

        if (!hasMission(MISSION_GUARDFORCE) && player->canBuildUnitBool(QUAD) && player->canBuildUnitBool(TANK)) {
            // add defending force
            std::vector<S_groupKind> group = std::vector<S_groupKind>();
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = QUAD,
                    .required = 1,
                    .ordered = 0,
                    .produced = 0,
            });
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = TANK,
                    .required = 2,
                    .ordered = 0,
                    .produced = 0,
            });
            addMission(PLAYERBRAINMISSION_KIND_DEFEND, group, rnd(15), MISSION_GUARDFORCE);
        }
    }

    void cPlayerBrainSkirmish::produceSkirmishGroundAttackMission(int missionId) {
        log(fmt::format("produceSkirmishGroundAttackMission for mission id %d - called", missionId));
        std::vector<S_groupKind> group = std::vector<S_groupKind>();
        int smallChance = 15;
        int normalChance = 50;
        int bigChance = 75;

        if (player->getCredits() > 2500) {
            // we can afford a bit more, so up the chances
            smallChance *= 1.5;
            normalChance *= 1.5;
            bigChance *= 1.5;
        } else if (player->getCredits() > 1500) {
            // we can afford a bit more, so up the chances
            smallChance *= 1.25;
            normalChance *= 1.25;
            bigChance *= 1.25;
        }

        if (rnd(100) < normalChance && player->canBuildUnitBool(SIEGETANK)) {
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = SIEGETANK,
                    .required = 1+rnd(4),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        if (rnd(100) < smallChance && player->hasAtleastOneStructure(WOR)) {
            int trooperUnit = TROOPER;
            if (player->canBuildUnitBool(TROOPERS)) {
                trooperUnit = TROOPERS;
            }
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = trooperUnit,
                    .required = 1 + rnd(2),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        if (rnd(100) < smallChance && player->hasAtleastOneStructure(BARRACKS)) {
            int infantryUnit = INFANTRY;
            if (player->canBuildUnitBool(INFANTRY)) {
                infantryUnit = INFANTRY;
            }
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = infantryUnit,
                    .required = 1 + rnd(2),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        if (rnd(100) < bigChance && player->canBuildUnitBool(LAUNCHER)) {
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = LAUNCHER,
                    .required = 1+rnd(3),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        if (rnd(100) < bigChance && player->canBuildUnitBool(TANK)) {
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = TANK,
                    .required = 1+rnd(4),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        int scoutingUnitType = player->getScoutingUnitType();
        if (rnd(100) < smallChance && player->canBuildUnitBool(scoutingUnitType)) {
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = scoutingUnitType,
                    .required = 1+rnd(2),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        if (QUAD != scoutingUnitType && player->canBuildUnitBool(QUAD) && rnd(100) < normalChance) {
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = QUAD,
                    .required = 1+rnd(3),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        int specialType = player->getSpecialUnitType();
        if (rnd(100) < normalChance && player->canBuildUnitBool(specialType)) {
            group.push_back(S_groupKind{
                    .buildType = UNIT,
                    .type = specialType,
                    .required = 1+rnd(2),
                    .ordered = 0,
                    .produced = 0,
            });
        }

        addMission(PLAYERBRAINMISSION_KIND_ATTACK, group, rnd(15), missionId);
    }

    bool cPlayerBrainSkirmish::hasMission(const int id) {
        auto position = std::find_if(m_missions.begin(), m_missions.end(), [&id](const cPlayerBrainMission & mission){ return mission.getUniqueId() == id; });
        bool hasMission = position != m_missions.end();
        return hasMission;
    }

    void cPlayerBrainSkirmish::addMission(ePlayerBrainMissionKind kind, std::vector<S_groupKind> &group,
                                          int initialDelay,
                                          int id) {

        for (auto &item : group) {
            if (item.buildType == eBuildType::UNIT) {
                if (!player->canBuildUnitBool(item.type)) {
                    item.required = 0; // set it to required 0, so it won't be built
                    log(fmt::format("addMission - cannot build unit [{}] so setting required to 0, for mission kind [{}].",
                                    toStringBuildTypeSpecificType(eBuildType::UNIT, item.type),
                                    ePlayerBrainMissionKindString(kind))
                        );
                }
            }
        }

        cPlayerBrainMission someMission(player, kind, this, group, initialDelay, id);
        m_missions.push_back(someMission);

        // do note the cooldown is within the whole cylce of the AI thinking process. So this means the cooldown
        // here is the amount of 'iterations'. Since 1 iteration has a RestTime to wait, this means a cooldown of 10
        // is cooling down in 10 seconds.
        int cooldown = cPlayerBrain::RestTime;
        m_TIMER_produceMissionCooldown += cooldown;
        log(fmt::format("addMission - upping cooldown with {} to a total of {}", cooldown, m_TIMER_produceMissionCooldown));
        // rest for a few seconds before producing a new mission)

    }

    void cPlayerBrainSkirmish::thinkState_Evaluate() {
        log(fmt::format("thinkState_Evaluate() : credits [{}], m_economyScore [{}], m_economyState [{}]",
                        player->getCredits(),
                        m_economyScore,
                        ePlayerBrainSkirmishEconomyStateString(m_economyState))
            );

        if (player->getAmountOfStructuresForType(CONSTYARD) == 0) {
            // no constyards, endgame
            changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_ENDGAME);
            return;
        }

        // Re-iterate over all (pending) m_buildOrders - and check if we are still going to do that or we might want to
        // up/downplay some priorities and re-sort?
        // Example: Money is short, Harvester is in build queue, but not high in priority?
        if (player->hasAtleastOneStructure(REFINERY)) {
            evaluateEconomyState();
        }

        // take a little rest, before going into a new loop again?
        m_TIMER_rest = cPlayerBrain::RestTime;
        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_REST);
    }

    void cPlayerBrainSkirmish::evaluateEconomyState() {
        ePlayerBrainSkirmishEconomyState state = determineEconomyState();
        log(fmt::format("evaluateEconomyState: {} credits - {} score --> {}", player->getCredits(), m_economyScore, ePlayerBrainSkirmishEconomyStateString(m_economyState)));
        changeEconomyStateTo(state);
    }

    ePlayerBrainSkirmishEconomyState cPlayerBrainSkirmish::determineEconomyState() {
        // positive development...
        int credits = player->getCredits();

        int idealMoneyCount = 1000;
        int economyStateBasedOnMoney = ((credits / (float)idealMoneyCount)*100);
        if (economyStateBasedOnMoney > 100) {
            economyStateBasedOnMoney = 100;
        }

        // at first, decrease badness of economy by default
        if (m_economyScore < economyStateBasedOnMoney) {
            m_economyScore += 3;
        }
        if (m_economyScore > economyStateBasedOnMoney) {
            m_economyScore--;
        }

        log(fmt::format("determineEconomyState -> ideal = {}, current = {}, moneyScore = {}, m_economyScore = {}",
                        idealMoneyCount, credits, economyStateBasedOnMoney, m_economyScore));

        // Evaluate economy score
        if (m_economyScore < 15) {
            return PLAYERBRAIN_ECONOMY_STATE_BAD;
        }
        if (m_economyScore < 30) {
            return PLAYERBRAIN_ECONOMY_STATE_IMPROVE;
        }
        if (m_economyScore > 70) {
            return PLAYERBRAIN_ECONOMY_STATE_GOOD;
        }

        return PLAYERBRAIN_ECONOMY_STATE_NORMAL;
    }

    void cPlayerBrainSkirmish::thinkState_EndGame() {
        // ...
//    char msg[255];
//    sprintf(msg, "cPlayerBrainSkirmish::thinkState_EndGame(), for player [%d]", player->getId());
//    logbook(msg);

        bool foundIdleUnit = false;
        std::vector<int> ids = player->getAllMyUnits();
        for (auto &id : ids) {
            cUnit &cUnit = unit[id];
            if (cUnit.isIdle()) {
                foundIdleUnit = true;
                break;
            }
        }

        if (!foundIdleUnit) {
            return;
        }

        int cellToAttack = -1;
        if (rnd(100) < 50) {
            for (int i = 0; i < MAX_UNITS; i++) {
                cUnit &cUnit = unit[i];
                if (!cUnit.isValid()) continue;
                if (cUnit.getPlayer()->isSameTeamAs(player)) continue; // skip allies and self
                // enemy structure
                cellToAttack = cUnit.getCell();
                if (rnd(100) < 5) {
                    break; // this way we kind of have randomly another target...
                }
            }
        } else {
            for (int i = 0; i < MAX_STRUCTURES; i++) {
                cAbstractStructure *theStructure = structure[i];
                if (!theStructure) continue;
                if (!theStructure->isValid()) continue;
                if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies and self
                // enemy structure
                cellToAttack = theStructure->getCell();
                if (rnd(100) < 10) {
                    break; // this way we kind of have randomly another target...
                }
            }
        }

        if (cellToAttack > -1) {
            for (auto &id : ids) {
                cUnit &pUnit = unit[id];
                if (pUnit.isIdle()) {
                    pUnit.attackAt(cellToAttack);
                }
            }
        }
    }

    void cPlayerBrainSkirmish::thinkState_ProcessBuildOrders() {
        log("thinkState_ProcessBuildOrders()");

        // check if we can find a similar build order
        for (auto &buildOrder : m_buildOrders) {
            if (buildOrder.state != buildOrder::eBuildOrderState::PROCESSME)
                continue; // only process those which are marked

            assert(buildOrder.buildId > -1 && "(cPlayerBrainSkirmish) A build order with no buildId got in the m_buildOrders list, which is not allowed!");

            if (buildOrder.buildType == eBuildType::STRUCTURE) {
                if (player->canBuildStructure(buildOrder.buildId) == eCantBuildReason::NONE) {
                    if (player->startBuildingStructure(buildOrder.buildId)) {
                        buildOrder.state = buildOrder::eBuildOrderState::BUILDING;
                    }
                } else {
                    buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                }
            } else if (buildOrder.buildType == eBuildType::UNIT) {
                eCantBuildReason reason = player->canBuildUnit(buildOrder.buildId, false);
                if (reason == eCantBuildReason::NONE) {
                    if (player->startBuildingUnit(buildOrder.buildId)) {
                        buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                    }
                } else if (reason == eCantBuildReason::REQUIRES_UPGRADE) {
                    // first check if there is an upgrade available directly granting the unit we want (ie LAUNCHER?)
                    cBuildingListItem *pItem = player->isUpgradeAvailableToGrantUnit(buildOrder.buildId);

                    if (buildOrder.buildId != ORNITHOPTER) {
                        if (!pItem) {
                            // not found, first check if MCV upgrade exists. It is required before anything else.
                            pItem = player->isUpgradeAvailableToGrantUnit(MCV);
                        }

                        if (!pItem) {
                            if (buildOrder.buildId == SIEGETANK) {
                                // still not found, if we want to build a SIEGETANK, we need to upgrade to LAUNCHER?
                                // check that too
                                pItem = player->isUpgradeAvailableToGrantUnit(LAUNCHER);
                            }
                        }
                    } else {
                        pItem = player->isUpgradeAvailableToGrantUnit(ORNITHOPTER);
                    }

                    if (pItem) {
                        if (!pItem->isBuilding()) {
                            bool b = player->startUpgrading(pItem->getBuildId());
                            if (!b) {
                                // not upgrading... for whatever reason, remove it from the list
                                buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                            }
                        } else {
                            // wait for upgrade to pass, so don't do anything.
                        }
                    }
                } else if (reason == eCantBuildReason::REQUIRES_ADDITIONAL_STRUCTURE || reason == eCantBuildReason::REQUIRES_STRUCTURE) {
                    // notify whoever is interested that the requested thing being built is not available for building...
                    s_GameEvent event {
                            .eventType = eGameEventType::GAME_EVENT_CANNOT_BUILD,
                            .entityType = buildOrder.buildType,
                            .entityID = -1,
                            .player = player,
                            .entitySpecificType = buildOrder.buildId,
                            .atCell = -1,
                            .isReinforce = false,
                            .buildingListItem = nullptr
                    };

                    game.onNotifyGameEvent(event);
                }
            } else if (buildOrder.buildType == eBuildType::SPECIAL) {
                if (player->startBuildingSpecial(buildOrder.buildId)) {
                    buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                }
            }
        }

        // delete any m_buildOrders which are flagged to remove
        m_buildOrders.erase(
                std::remove_if(
                        m_buildOrders.begin(),
                        m_buildOrders.end(),
                        [](const S_buildOrder &o) { return o.state == buildOrder::eBuildOrderState::REMOVEME; }),
                m_buildOrders.end()
        );

        // TODO: use an event "structure ready for placement" for this?
        if (player->isBuildingStructureAwaitingPlacement()) {
            int structureType = player->getStructureTypeBeingBuilt();

            S_buildOrder * matchingOrder = nullptr;
            for (auto &buildOrder : m_buildOrders) {
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
                const s_PlaceResult &placeResult = player->canPlaceStructureAt(iCll, pItem->getBuildId());

                if (placeResult.success) {
                    player->placeItem(iCll, pItem);
                    matchingOrder->state = buildOrder::eBuildOrderState::REMOVEME;
                } else {
                    findNewLocationOrMoveAnyBlockingUnitsOrCancelBuild(matchingOrder, pItem, placeResult);
                }
            } else {
                // whut? there is something in progress, but we are not aware of it? cancel build
                player->cancelStructureBuildingListItemBeingBuilt();
            }
        }

        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_EVALUATE);
    }

    bool cPlayerBrainSkirmish::findNewPlaceToPlaceStructureOrCancelBuild(S_buildOrder *pBuildOrder, cBuildingListItem *pItem) {
        // pick a new spot?
        int newCell = player->findCellToPlaceStructure(pItem->getBuildId());

        if (newCell < 0) {
            // cancel building this thing
            player->cancelBuildingListItem(pItem);
            pBuildOrder->state = buildOrder::REMOVEME;
            return false;
        }

        // update the cell, it will be retried in the next think iteration
        pBuildOrder->placeAt = newCell;
        return true;
    }

    void cPlayerBrainSkirmish::findNewLocationOrMoveAnyBlockingUnitsOrCancelBuild(S_buildOrder *pBuildOrder,
                                                                                  cBuildingListItem *pItem,
                                                                                  const s_PlaceResult &placeResult) {
        // unable to place it, we determined previously that we could, so what to do?
        if (placeResult.badTerrain) {
            findNewPlaceToPlaceStructureOrCancelBuild(pBuildOrder, pItem);
            return;
        }

        // find any unit that is from us, and move it away
        // if there is any enemy player, then find a new place.
        if (!placeResult.unitIds.empty()) {
            for (auto &unitId : placeResult.unitIds) {
                cUnit &aUnit = unit[unitId];
                if (!aUnit.isValid()) continue;
                if (aUnit.getPlayer() == player) {
                    if (aUnit.isUnableToMove()) {
                        // it can't move, so we have to think of another place position
                        findNewPlaceToPlaceStructureOrCancelBuild(pBuildOrder, pItem);
                        return;
                    }
                    // move it when idle, else don't do a thing
                    if (aUnit.isIdle()) {
                        aUnit.move_to(map.getRandomCellFrom(aUnit.getCell(), 3));
                    }
                } else {
                    // if (aUnit.getPlayer()->isSameTeamAs(player)) { // who knows, a teammate is willing to move away when it is AI?
                    findNewPlaceToPlaceStructureOrCancelBuild(pBuildOrder, pItem);
                    return;
                }
            }
        }

        // find any unit that is from us, and move it away
        // if there is any enemy player, then find a new place.
        if (!placeResult.structureIds.empty()) {
            findNewPlaceToPlaceStructureOrCancelBuild(pBuildOrder, pItem);
        }
    }

    void cPlayerBrainSkirmish::changeThinkStateTo(const ePlayerBrainSkirmishThinkState& newState) {
        log(fmt::format("changeThinkStateTo(), from {} to {}",
                        ePlayerBrainSkirmishThinkStateString(m_thinkState),
                        ePlayerBrainSkirmishThinkStateString(newState))
            );
        this->m_thinkState = newState;
    }

    void cPlayerBrainSkirmish::changeEconomyStateTo(const ePlayerBrainSkirmishEconomyState &newState) {
        if (newState != m_economyState) {
            log(fmt::format("cPlayerBrainSkirmish::changeEconomyStateTo(), from {} to {}",
                            ePlayerBrainSkirmishEconomyStateString(m_economyState),
                            ePlayerBrainSkirmishEconomyStateString(newState))
            );
            m_economyState = newState;
        }
    }

    void cPlayerBrainSkirmish::thinkState_Rest() {
        if (m_TIMER_rest > 0) {
            m_TIMER_rest--;
            log(fmt::format("cPlayerBrainSkirmish::thinkState_Rest(), rest {}", m_TIMER_rest));
            return;
        }

        // resting is done, now go into the base/missions/evaluate loop
        changeThinkStateTo(ePlayerBrainSkirmishThinkState::PLAYERBRAIN_SKIRMISH_STATE_BASE);
    }

    void cPlayerBrainSkirmish::onEntityDiscoveredEvent(const s_GameEvent &event) {
        bool wormsign = event.entityType == eBuildType::UNIT && event.entitySpecificType == SANDWORM;
        if (m_state == ePlayerBrainState::PLAYERBRAIN_PEACEFUL) {
            if (!wormsign) {
                if (event.player == player) {
                    // i discovered something
                    if (event.entityType == eBuildType::UNIT) {
                        cUnit &pUnit = unit[event.entityID];
                        if (pUnit.isValid() && !pUnit.getPlayer()->isSameTeamAs(player)) {
                            // found enemy unit
                            m_TIMER_produceMissionCooldown = 0;
                            m_state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                            m_TIMER_rest = 0; // if we were still 'resting' then stop this now.
                            m_discoveredEnemyAtCell.insert(event.atCell);

                            if (m_centerOfBaseCell > -1 && map.distance(m_centerOfBaseCell, event.atCell) < kScanRadius) {
                                respondToThreat(&pUnit, nullptr, event.atCell, 2 + rnd(4));
                            }
                        }
                    } else if (event.entityType == eBuildType::STRUCTURE) {
                        cAbstractStructure *pStructure = structure[event.entityID];
                        if (!pStructure->getPlayer()->isSameTeamAs(player)) {
                            // found enemy structure
                            m_TIMER_produceMissionCooldown = 0;
                            m_state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                            m_TIMER_rest = 0; // if we were still 'resting' then stop this now.
                            m_discoveredEnemyAtCell.insert(event.atCell);
                        }
                    }
                } else {
                    // event.player == player who discovered something
                    if (event.player == &players[AI_WORM]) {
                        // ignore anything that the WORM AI player detected.
                    } else if (!event.player->isSameTeamAs(player)) {
                        if (event.entityType == eBuildType::UNIT) {
                            cUnit &pUnit = unit[event.entityID];
                            // the other player discovered a unit of mine
                            if (pUnit.isValid() && pUnit.getPlayer() == player) {
                                // found my unit
                                m_TIMER_produceMissionCooldown = 0;
                                m_state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                                m_TIMER_rest = 0; // if we were still 'resting' then stop this now.
                                m_discoveredEnemyAtCell.insert(pUnit.getCell());
                            }
                        } else if (event.entityType == eBuildType::STRUCTURE) {
                            cAbstractStructure *pStructure = structure[event.entityID];
                            // the other player discovered a structure of mine
                            if (pStructure->getPlayer() == player) {
                                // found my structure
                                m_TIMER_produceMissionCooldown = 0;
                                m_state = ePlayerBrainState::PLAYERBRAIN_ENEMY_DETECTED;
                                m_TIMER_rest = 0; // if we were still 'resting' then stop this now.
//                                m_discoveredEnemyAtCell.insert(pStructure.getCell());
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
            if (!wormsign) {
                if (event.player == player) {
                    // i discovered something
                    if (event.entityType == eBuildType::UNIT) {
                        cUnit &pUnit = unit[event.entityID];
                        if (pUnit.isValid() && !pUnit.getPlayer()->isSameTeamAs(player)) {
                            if (m_centerOfBaseCell > -1 && map.distance(m_centerOfBaseCell, event.atCell) < kScanRadius) {
                                respondToThreat(&pUnit, nullptr, event.atCell, 2 + rnd(4));
                            }
                        }
                    }
                }
            }
        }
    }

    s_SkirmishPlayer_PlaceForStructure cPlayerBrainSkirmish::thinkAboutNextStructureToBuildAndPlace() {
        s_SkirmishPlayer_PlaceForStructure result;

        // think about structure type first
        int structureTypeToBuild = getStructureToBuild();

        // then where to place it
        int cellToPlaceStructureAt = -1;
        if (structureTypeToBuild > -1) {
            cellToPlaceStructureAt = player->findCellToPlaceStructure(structureTypeToBuild);
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

        // nothing the AI wants to build right now...
        if (structureIWantToBuild < 0) {
            return -1;
        }

        // determine if we have enough power for the thing we want to build, if not, build a windtrap first...
        if (!player->hasEnoughPowerFor(structureIWantToBuild)) {
            return WINDTRAP;
        }

        // can build it?
        eCantBuildReason reason = player->canBuildStructure(structureIWantToBuild);
        switch (reason) {
            case NONE:
                // We can build. Carry on.
                break;
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
            case REQUIRES_ADDITIONAL_STRUCTURE:
                break;
            case NOT_ENOUGH_MONEY:
                // economy is bad? set state for that
                break;
            case ALREADY_BUILDING:
                // already building something, so don't return any id to build
                return -1;
            case NOT_AVAILABLE:
                // The building is not available
                return -1;
        }

        return structureIWantToBuild;
    }

    int cPlayerBrainSkirmish::getStructureIdToBuildWithoutConsideringPowerUsage() const {
        player->logStructures();

        if (!player->hasAtleastOneStructure(REFINERY))      return REFINERY;
        if (m_economyState == PLAYERBRAIN_ECONOMY_STATE_IMPROVE || m_economyState == PLAYERBRAIN_ECONOMY_STATE_BAD) {
            if (player->getAmountOfStructuresForType(REFINERY) < 3) {
                return REFINERY;
            }
        }

        // don't build anything else for now
        if (m_economyState == PLAYERBRAIN_ECONOMY_STATE_BAD) {
            return -1;
        }

        if (m_TIMER_ai > MOMENT_CONSIDER_ADDITIONAL_REFINERY) {
            // time to think about an additional refinery
            if (rnd(100) < 50 && player->getAmountOfStructuresForType(REFINERY) < 3) {
                // build one
                return REFINERY;
            }
        }

        if (player->hasAlmostReachMaxSpiceStorageCapacity() && rnd(100) < 15) {
            return SILO;
        }

        // play around with the order... (depending on difficulty, or type of ai, etc?)
        if (!player->hasAtleastOneStructure(LIGHTFACTORY))  return LIGHTFACTORY;

        // randomize the order a bit, but do make sure that we have both before continueing
        if (!player->hasAtleastOneStructure(RADAR) && !player->hasAtleastOneStructure(HEAVYFACTORY)) {
            if (rnd(100) < 50) {
                return RADAR;
            } else {
                return HEAVYFACTORY;
            }
        } else {
            if (!player->hasAtleastOneStructure(RADAR)) return RADAR;
            if (!player->hasAtleastOneStructure(HEAVYFACTORY)) return HEAVYFACTORY;
        }

        if (rnd(100) < 15) {
            if (player->isStructureTypeAvailableForConstruction(WOR)) {
                if (!player->hasAtleastOneStructure(WOR)) return WOR;
            }
            if (player->isStructureTypeAvailableForConstruction(BARRACKS)) {
                if (!player->hasAtleastOneStructure(BARRACKS)) return BARRACKS;
            }
        }

        // low chance on early palace (could be a specific "build profile" property?)
        if (rnd(100) < 5) {
            if (!player->hasAtleastOneStructure(PALACE)) return PALACE;
        }

        if (rnd(100) < 35) {
            if (player->getAmountOfStructuresForType(TURRET) < 2) {
                return TURRET;
            }
        }

        if (rnd(100) < 40) {
            if (player->isStructureTypeAvailableForConstruction(HIGHTECH)) {
                if (!player->hasAtleastOneStructure(HIGHTECH))  return HIGHTECH;
            }
        }

        int chanceToBuildRocketTurrets = 60 - (player->getAmountOfStructuresForType(RTURRET) * 10);
        if (rnd(100) < chanceToBuildRocketTurrets) {
            return RTURRET;
        }

        int chanceToBuildTurrets = 40 - (player->getAmountOfStructuresForType(TURRET) * 5);
        if (rnd(100) < chanceToBuildTurrets)  {
            return TURRET;
        }

        if (!player->hasAtleastOneStructure(HIGHTECH))  return HIGHTECH;
        if (!player->hasAtleastOneStructure(STARPORT))  return STARPORT;

        if (rnd(100) < 15) {
            if (!player->hasAtleastOneStructure(PALACE)) return PALACE;
        }

        if (!player->hasAtleastOneStructure(IX))  return IX;

        // take a shot at building the palace
        if (rnd(100) < 15) {
            if (!player->hasAtleastOneStructure(PALACE)) return PALACE;
        }

        if (player->getAmountOfStructuresForType(RTURRET) < 8)  return RTURRET;

        // take another shot at building the palace
        if (rnd(100) < 15) {
            if (!player->hasAtleastOneStructure(PALACE)) return PALACE;
        }

        if (!player->hasAtleastOneStructure(REPAIR))  return REPAIR;

        if (m_economyState == PLAYERBRAIN_ECONOMY_STATE_NORMAL) {
            if (rnd(100) < 30) {
                if (player->getAmountOfStructuresForType(HEAVYFACTORY) < 2) return HEAVYFACTORY;
            }

            if (!player->hasAtleastOneStructure(PALACE)) return PALACE;

            if (player->getAmountOfStructuresForType(HEAVYFACTORY) < 2) return HEAVYFACTORY;

            if (player->getAmountOfStructuresForType(LIGHTFACTORY) < 2) return LIGHTFACTORY;

            if (player->isStructureTypeAvailableForConstruction(WOR)) {
                if (player->getAmountOfStructuresForType(WOR) < 2) return WOR;
            }

            if (player->isStructureTypeAvailableForConstruction(BARRACKS)) {
                if (player->getAmountOfStructuresForType(BARRACKS) < 2) return BARRACKS;
            }
        }

        // already has everything, this makes more sense now
        if (player->hasAlmostReachMaxSpiceStorageCapacity()) {
            return SILO;
        }

        // nothing to build
        return -1;
    }

    bool cPlayerBrainSkirmish::hasBuildOrderQueuedForStructure() {
        for (auto &buildOrder : m_buildOrders) {
            if (buildOrder.buildType == eBuildType::STRUCTURE && buildOrder.state != buildOrder::REMOVEME) {
                return true;
            }
        }
        return false;
    }

    bool cPlayerBrainSkirmish::hasBuildOrderQueuedForUnit(int buildId) {
        for (auto &buildOrder : m_buildOrders) {
            if (buildOrder.buildType == eBuildType::UNIT && buildOrder.state != buildOrder::REMOVEME && buildOrder.buildId == buildId) {
                return true;
            }
        }
        return false;
    }

    bool cPlayerBrainSkirmish::allMissionsAreDoneGatheringResources() {
        for (auto &mission : m_missions) {
            if (!mission.isDoneGatheringResources()) {
                return false;
            }
        }
        return true;
    }

    void cPlayerBrainSkirmish::thinkFast() {
        for (auto &mission : m_missions) {
            mission.think();
        }
    }

    void cPlayerBrainSkirmish::log(const std::string & txt) {
        player->log(fmt::format(
                "cPlayerBrainSkirmish [state={}, m_thinkState={}, m_economyState={}, m_TIMER_rest={}, m_TIMER_ai={}, m_economyScore={}] | {}",
                ePlayerBrainStateString(m_state),
                ePlayerBrainSkirmishThinkStateString(m_thinkState),
                ePlayerBrainSkirmishEconomyStateString(m_economyState),
                this->m_TIMER_rest,
                this->m_TIMER_ai,
                this->m_economyScore,
                txt)
        );
    }

    void cPlayerBrainSkirmish::onMyUnitAttacked(const s_GameEvent &event) {
        if (event.originType == eBuildType::UNKNOWN) {
            // don't know who attacked us; ignore
            return;
        }

        cUnit *threat = nullptr;
        if (event.originType == eBuildType::UNIT) {
            assert(event.originId > -1);
            threat = &unit[event.originId];
        }

        cUnit &victim = unit[event.entityID];
        if (victim.isHarvester()) {
            respondToThreat(threat, &victim, event.atCell, 2 + rnd(4));
        }
    }

}