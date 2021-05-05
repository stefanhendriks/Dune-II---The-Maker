/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */
#include "include/d2tmh.h"
#include <vector>
#include <algorithm>
#include <random>

// TODO: constructor/destructors

// cAIPlayer functions
void cAIPlayer::init(cPlayer *thePlayer) {
    _player = thePlayer;

	// SKIRMISH RELATED SETTINGS
	// TODO: move this out of here, this basically ensures there is one playable AI at start (when setting up
	// skirmish game)
	if (_player->getId() > 1) {
        bPlaying = false;
    } else {
        bPlaying = true;
    }

	// same here, this initially sets 3 units
	iUnits=3;

    DELAY_buildbase = 0;
    TIMER_think=rnd(10);        // timer for thinking itself (calling main routine)

    iCheckingPlaceStructure=-1;

	// -- END

    TIMER_attack = (700 + rnd(400));

    TIMER_Upgrades = 50;
    TIMER_BuildUnits = 300; // give m_Player advantage to build his stuff first, before computer grows his army
    TIMER_blooms = 200;
    TIMER_repair = 500;
}

void cAIPlayer::buildStructureIfAllowed(int iStrucType) {
    if (isBuildingStructure()) {
        char msg[255];
        sprintf(msg, "buildStructureIfAllowed AI[%d] wants to build structure type [%d(=%s)] but it is still building a structure - ABORTING", _player->getId(), iStrucType, structures[iStrucType].name);
        logbook(msg);
        return;
    }

    if (isBuildingStructureAwaitingPlacement()) {
        char msg[255];
        sprintf(msg, "buildStructureIfAllowed AI[%d] wants to build structure type [%d(=%s)] but it is still awaiting placement of a structure", _player->getId(), iStrucType, structures[iStrucType].name);
        logbook(msg);
        return;
    }

	// not building & busy placing

	// check if its allowed at all
    if (!_player->hasAtleastOneStructure(CONSTYARD)) {
        char msg[255];
        sprintf(msg, "buildStructureIfAllowed AI[%d] wants to build structure type [%d(=%s)] but can't because no CONSTYARD present.", _player->getId(), iStrucType, structures[iStrucType].name);
        logbook(msg);
        return;
    }

    startBuildingStructure(iStrucType);
}

bool cAIPlayer::BUILD_UNIT(int iUnitType) {
    // Fix up house mixtures
    if (_player->getHouse() == HARKONNEN || _player->getHouse() == SARDAUKAR) {
        if (iUnitType == INFANTRY) iUnitType = TROOPERS;
        if (iUnitType == SOLDIER) iUnitType = TROOPER;
        if (iUnitType == TRIKE) iUnitType = QUAD;
    }

    // In mission 9 there are only WOR's, so make a special hack hack for that ;)
    if (!game.bSkirmish && game.iMission >= 8) {
        if (iUnitType == INFANTRY) iUnitType = TROOPERS;
        if (iUnitType == SOLDIER) iUnitType = TROOPER;
    }

    if (_player->getHouse() == ORDOS) {
        if (iUnitType == TRIKE) iUnitType = RAIDER;
    }

    cantBuildReason reason = canBuildUnit(iUnitType);

    if (reason != cantBuildReason::NONE) {
        if (reason == cantBuildReason::REQUIRES_UPGRADE) {
            // try to build upgrade instead?
            cBuildingListItem *upgrade = isUpgradeAvailableToGrantUnit(iUnitType);
            if (upgrade != nullptr) {
                if (upgrade->isAvailable() && _player->hasEnoughCreditsFor(upgrade->getCosts())) {
                    char msg[255];
                    sprintf(msg, "BUILD_UNIT: Cannot build %s because I need to upgrade; starting upgrade now.",
                            units[iUnitType].name);
                    logbook(msg);
                    startUpgrading(upgrade->getBuildId());
                } else {
                    char msg[255];
                    sprintf(msg, "BUILD_UNIT: Cannot build %s; upgrade available, but either cant upgrade or is too expensive.",
                            units[iUnitType].name);
                    logbook(msg);
                }
            } else {
                char msg[255];
                sprintf(msg, "BUILD_UNIT: Cannot build %s , nor an upgrade available.",
                        units[iUnitType].name);
                logbook(msg);
            }
        }
        return false; // do not go further
    }

    bool bAlreadyBuilding = isBuildingUnitType(iUnitType);

    // the current unitType is not being built, check if similar types are being built
    if (!bAlreadyBuilding) {
        if (iUnitType == QUAD ||
            iUnitType == TRIKE ||
            iUnitType == RAIDER) {
            bAlreadyBuilding =  isBuildingUnitType(QUAD) ||
                                isBuildingUnitType(TRIKE) ||
                                isBuildingUnitType(RAIDER);
        }

        if (iUnitType == TANK ||
            iUnitType == LAUNCHER ||
            iUnitType == SIEGETANK ||
            iUnitType == SONICTANK ||
            iUnitType == DEVASTATOR ||
            iUnitType == HARVESTER) {
            bAlreadyBuilding =  isBuildingUnitType(TANK) ||
                                isBuildingUnitType(LAUNCHER) ||
                                isBuildingUnitType(SIEGETANK) ||
                                isBuildingUnitType(SONICTANK) ||
                                isBuildingUnitType(DEVASTATOR) ||
                                isBuildingUnitType(HARVESTER);
        }

        if (iUnitType == CARRYALL ||
            iUnitType == ORNITHOPTER) {
            bAlreadyBuilding =  isBuildingUnitType(CARRYALL) ||
                                isBuildingUnitType(ORNITHOPTER);
        }
    }

    if (!bAlreadyBuilding) {
        bool result = startBuildingUnit(iUnitType);
        if (result) {
            int delay = 25; // increase delay with 'weaker' ai ?
            TIMER_BuildUnits = units[iUnitType].build_time + delay;
        } else {
            TIMER_BuildUnits = 25;
        }
        return result;
    } else {
        char msg[255];
        sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d] - but already building.", units[iUnitType].name, iUnitType);
        logbook(msg);
    }
    return false;
}

bool cAIPlayer::startBuildingUnit(int iUnitType) const {
    s_UnitP &unitType = units[iUnitType];
    int listId = unitType.listId;
    bool startedBuilding = _player->getSideBar()->startBuildingItemIfOk(listId, iUnitType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listId[%d] - SUCCESS", unitType.name, iUnitType, listId);
        } else {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listId[%d] - FAILED", unitType.name, iUnitType, listId);
        }
        logbook(msg);
    }
    return startedBuilding;
}

void cAIPlayer::startBuildingStructure(int iStructureType) const {
    // Duplicated logic - for now - determining lists by unit id.
    int listId = LIST_CONSTYARD;

    bool startedBuilding = _player->getSideBar()->startBuildingItemIfOk(listId, iStructureType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build structure [%s] iStructureType = [%d], with listId[%d] - SUCCESS", structures[iStructureType].name, iStructureType, listId);
        } else {
            sprintf(msg, "Wanting to build structure [%s] iStructureType = [%d], with listId[%d] - FAILED", structures[iStructureType].name, iStructureType, listId);
        }
        logbook(msg);
    }
}

void cAIPlayer::startUpgrading(int iUpgradeType) const {
    int listId = LIST_UPGRADES;
    bool startedBuilding = _player->getSideBar()->startBuildingItemIfOk(listId, iUpgradeType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to start upgrade [%s] iUpgradeType = [%d], with listId[%d] - SUCCESS", upgrades[iUpgradeType].description, iUpgradeType, listId);
        } else {
            sprintf(msg, "Wanting to start upgrade [%s] iUpgradeType = [%d], with listId[%d] - FAILED", upgrades[iUpgradeType].description, iUpgradeType, listId);
        }
        logbook(msg);
    }
}

bool cAIPlayer::isBuildingUnitType(int iUnitType) const {
    cItemBuilder *pItemBuilder = _player->getItemBuilder();

    int listId = units[iUnitType].listId;
    int subListId = units[iUnitType].subListId;

    return pItemBuilder->isAnythingBeingBuiltForListId(listId, subListId);
}

/**
 * Returns true if anything is built from ConstYard
 */
cBuildingListItem *cAIPlayer::isBuildingStructure() const {
    cItemBuilder *pItemBuilder = _player->getItemBuilder();
    return pItemBuilder->getListItemWhichIsBuilding(LIST_CONSTYARD, 0);
}

/**
 * Returns true if anything is built from ConstYard
 */
bool cAIPlayer::isBuildingStructureAwaitingPlacement() const {
    cItemBuilder *pItemBuilder = _player->getItemBuilder();

    return pItemBuilder->isAnythingBeingBuiltForListIdAwaitingPlacement(LIST_CONSTYARD, 0);
}

int cAIPlayer::getStructureTypeBeingBuilt() const {
    cBuildingListItem *pItem = getStructureBuildingListItemBeingBuilt();
    if (pItem) {
        return pItem->getBuildId();
    }
    return -1;
}

cBuildingListItem *cAIPlayer::getStructureBuildingListItemBeingBuilt() const {
    cItemBuilder *pItemBuilder = _player->getItemBuilder();

    return pItemBuilder->getListItemWhichIsBuilding(LIST_CONSTYARD, 0);
}


bool cAIPlayer::think_buildingplacement() {
    /*
		structure building;

		when building completed, search for a spot and place it!
	*/
    bool buildingStructureAwaitingPlacement = isBuildingStructureAwaitingPlacement();

    if (game.bSkirmish) {
        if (isBuildingStructure()) {
            // still building, so do nothing, unless we await placement
            if (!buildingStructureAwaitingPlacement) {
                return false;
            }
        }

        // find place to place structure
        if (buildingStructureAwaitingPlacement) {
            int structureType = getStructureTypeBeingBuilt();
            int iCll = findCellToPlaceStructure(structureType);

            if (iCll > -1) {
                int iHealthPercent = 50; // the minimum is 50% (with no slabs)

                // TODO: This logic is duplicated in cPlaceItDrawer, this should be somewhere
                // generic, unrelated to player in places (ie, the health determination?)
//                if (iTotalRocks > 0) {
//                    iHealthPercent += health_bar(50, iTotalRocks, iTotalBlocks);
//                }

                cBuildingListItem *pItem = getStructureBuildingListItemBeingBuilt();
                _player->getStructurePlacer()->placeStructure(iCll, structureType, iHealthPercent);
                _player->getBuildingListUpdater()->onBuildItemCompleted(pItem);

                pItem->decreaseTimesToBuild();
                pItem->setPlaceIt(false);
                pItem->setIsBuilding(false);
                pItem->resetProgress();
                if (pItem->getTimesToBuild() < 1) {
                    _player->getItemBuilder()->removeItemFromList(pItem);
                }
                // delay base build thinking a bit, so the new structure will be taken into account when deciding
                // which structure to build next
                return true;
            } else {
                // unable to place?!
                logbook("CANNOT PLACE STRUCTURE");
            }
        }
    }

    return false;
}

void cAIPlayer::think_spiceBlooms() {
    if (TIMER_blooms > 0) {
        TIMER_blooms--;
        return;
    }
    logbook("think_spiceBlooms - START");

//    TIMER_blooms = 200;
    TIMER_blooms = 50;

    int totalSpiceBloomsCount = map.getTotalCountCellType(TERRAIN_BLOOM);

    // When no blooms are detected, we must 'spawn' one
    int amountOfSpiceBloomsInMap = 3;
    if (totalSpiceBloomsCount < amountOfSpiceBloomsInMap) {
        // randomly create a new spice bloom somewhere on the map
        int iCll = -1;
        for (int i = 0; i < 10; i++) {
            int cell = map.getRandomCell();
            if (map.getCellType(cell) == TERRAIN_SAND) {
                iCll = cell;
                break;
            }
        }
        // create bloom (can deal with < -1 cell)
        mapEditor.createCell(iCll, TERRAIN_BLOOM, 0);
    } else {
        if (rnd(100) < 15) {
            logbook("AI: think_spiceBlooms - Going to find (or build) an infantry unit to blow up a spice bloom");
            // if we got a unit to spare (something light), we can blow it up  by walking over it
            // with a soldier or something
            int iUnit = -1;
            int iDist = 9999;
            int iBloom = CLOSE_SPICE_BLOOM(_player->getFocusCell());

            // TODO: Move this to a unit repository thingy which can query the units
            for (int i = 0; i < MAX_UNITS; i++) {
                cUnit &cUnit = unit[i];
                if (!cUnit.isValid()) continue;
                if (cUnit.iPlayer == HUMAN) continue; // skip human units
                if (cUnit.iPlayer != _player->getId()) continue; // skip units which are not my own
                if (cUnit.iAction != ACTION_GUARD) continue; // skip units which are doing something else
                if (!cUnit.isInfantryUnit()) continue; // skip non-infantry units

                int c = cUnit.getCell();
                int c1 = cUnit.getCell();
                int d = ABS_length(map.getCellX(iBloom),
                                   map.getCellY(iBloom),
                                   map.getCellX(c1),
                                   map.getCellY(c));

                if (d < iDist) {
                    iUnit = i;
                    iDist = d;
                }
            }

            if (iUnit > -1) {
                logbook("AI: think_spiceBlooms - Found unit to attack spice bloom");
                // shoot spice bloom
                UNIT_ORDER_ATTACK(iUnit, iBloom, -1, -1, iBloom);
            } else {
                logbook("AI: think_spiceBlooms - No unit found to attack spice bloom, building...");
                // try to build a unit to shoot the spice bloom
                BUILD_UNIT(SOLDIER);
            }
        }
    }
    logbook("think_spiceBlooms - END");
}

void cAIPlayer::think() {
    if (_player->getId() < 0 || _player->getId() > AI_WORM) {
        logbook("ERROR!!! AI has invalid ID to think with!?");
        return;
    }

    if (game.bDisableAI) return; // do nothing

    if (_player->getId() == HUMAN) {
        return; // AI is human / skip
    }

    if (game.bSkirmish) {
        if (!bPlaying) return; // skip non playing AI?
    }

//    if (ID == 1) {
//        char msg[255];
//        sprintf(msg, "AI[%d]: TIMER_think [%d]", _player->getId(), TIMER_think);
//        logbook(msg);
//    }

    // not time yet to think
    if (TIMER_think > 0) {
        TIMER_think--;
        return;
    }

    TIMER_think = 25;

    // think about fair harvester stuff
    think_spiceBlooms();

    think_buildbase();
    think_upgrades();
    think_buildarmy();
    think_attack();
    think_repair();
}

void cAIPlayer::think_repair() {
    if (TIMER_repair > 0) {
        TIMER_repair--;
        return;
    }

    TIMER_repair=1000; // next timed interval to think about this...

    // check if we must repair, only if we have a repair structure ofcourse
    // and we have some money to spare for repairs
    if (_player->hasAtleastOneStructure(REPAIR) && _player->hasEnoughCreditsFor(250)) {
        // yes, we can repair
        for (int i=0; i < MAX_UNITS; i++) {
            cUnit &pUnit = unit[i];
            if (pUnit.isValid()) {
                if (pUnit.iPlayer == _player->getId()) {
                    if (pUnit.isDamaged())
                    {
                        // head off to repair
                    	int iNewID = structureUtils.findClosestStructureTypeWhereNoUnitIsHeadingToComparedToCell(
                                pUnit.getCell(), REPAIR, _player);

						if (iNewID > -1) {
							int iCarry = CARRYALL_TRANSFER(i, structure[iNewID]->getCell() + 2);

							if (iCarry > -1) {
								// yehaw we will be picked up!
								pUnit.TIMER_movewait = 100;
                                pUnit.TIMER_thinkwait = 100;
							} else {
							    pUnit.move_to(structure[iNewID]->getCell());
							}

                            pUnit.iStructureID = iNewID;
                            pUnit.iGoalCell = structure[iNewID]->getCell();
						}
                    }
                }
            }
        } // FOR
    }
    // check if any structures where breaking down due decay
}


// Attack logic, do note this is used for Skirmish and Campaign gameplay
void cAIPlayer::think_attack() {
    if (TIMER_attack > 0) {
        TIMER_attack--;
        return;
    }

    char msg[255];
    sprintf(msg, "AI [%d] think_attack() - start", _player->getId());
    logbook(msg);

    TIMER_attack = (rnd(900) + 100);

    int armySize = 3 + rnd(8); // size of units to attack with
    int iTarget = -1;
    bool isAttackingUnit = false; // do we attack a unit? (if false, it means we attack a structure)

    bool bInfantryOnly = false;

    int iAttackPlayer = 0; // HUMAN


    if (game.bSkirmish) {
        // skirmish games will make the ai a bit more agressive!
        armySize += 3 + rnd(7);

        // check what players are playing
        int iPl[5];
        int iPlID = 0;
        memset(iPl, -1, sizeof(iPl));

        for (int p = 0; p < AI_WORM; p++) {
            if (p != _player->getId()) {
                bool bOk = false;
                if (p == 0) bOk = true;
                if (p > 1 && aiplayer[p].bPlaying) bOk = true;

                cPlayer &pPlayer = player[p];
                // skip same team
                //.iTeam != _player->iTeam
                if (pPlayer.isSameTeamAs(_player)) {
                    iPl[iPlID] = p;
                    iPlID++;
                }
            }
        }

        iAttackPlayer = iPl[rnd(iPlID)];
        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "AI (id=%d) - Attacking player id %d", this->_player->getId(), iAttackPlayer);
            logbook(msg);
        }
    }

    // only when ai has a wor/barracks to regenerate his troops, send off infantry sometimes.
    bool hasHeavyOrLightFactory = _player->hasAtleastOneStructure(LIGHTFACTORY) || _player->hasAtleastOneStructure(HEAVYFACTORY);
    bool hasInfantryStructures = _player->hasAtleastOneStructure(WOR) || _player->hasAtleastOneStructure(BARRACKS);
    if (hasInfantryStructures && !hasHeavyOrLightFactory) {
        bInfantryOnly = true;
    }

    int unitsAvailable = 0;
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != _player->getId()) continue; // skip all units but my own
        if (cUnit.isHarvester()) continue; // skip harvesters
        if (cUnit.isAirbornUnit()) continue; // skip airborn
        if (!cUnit.isIdle()) continue; // skip non-idle units
        unitsAvailable++;
    }

    if (unitsAvailable < (armySize / 2)) {
        char msg[255];
        sprintf(msg, "AI: (ID=%d) skipping attack, because we have %d units available for attacking which is less than 50 percent of requested army size %d", _player->getId(), unitsAvailable, armySize);
        logbook(msg);
        return;
    }

    // flip coin, either attack a random unit or structure
    if (rnd(100) < 50) {
        iTarget = findRandomStructureTarget(iAttackPlayer);

        if (iTarget < 0) {
            iTarget = findRandomUnitTarget(iAttackPlayer);
            isAttackingUnit = true;
        }
    } else {
        iTarget = findRandomUnitTarget(iAttackPlayer);
        isAttackingUnit = true;

        if (iTarget < 0) {
            iTarget = findRandomStructureTarget(iAttackPlayer);
            isAttackingUnit = false;
        }
    }

    if (iTarget < 0) {
        logbook("Could not determine target to attack.");
        return;
    }

    // make up the balance
    int iWe = 0;
    int iThem = 0;

    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.isAirbornUnit()) continue; // do not count airborn units
        if (cUnit.isHarvester()) continue;

        if (cUnit.iPlayer == iAttackPlayer) {
            iThem++;
        } else {
            iWe++;
        }
    }

    // When WE do not even match THEM, we wait until we have build more
    bool scared = rnd(100) < 30;
    if (iWe <= iThem && scared) {
        logbook("AI: Decided to wait a bit because 'our' army is as big as theirs...");

        TIMER_attack = 250;
        return;
    }

    int unitsOrderedToAttack = 0;

    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != _player->getId()) continue; // skip all units but my own
        if (cUnit.isHarvester()) continue; // skip harvesters
        if (cUnit.isAirbornUnit()) continue; // skip airborn
        if (!cUnit.isIdle()) continue; // skip non-idle units

        if (isAttackingUnit)
            UNIT_ORDER_ATTACK(i, unit[iTarget].getCell(), iTarget, -1, -1);
        else
            UNIT_ORDER_ATTACK(i, structure[iTarget]->getCell(), -1, iTarget, -1);

        unitsOrderedToAttack++;
        if (unitsOrderedToAttack >= armySize)
            break;
    }

    if (DEBUGGING) {
        char msg[255];
        if (isAttackingUnit) {
            sprintf(msg, "AI: I (id=%d) ordered %d units to attack player %d (armySize=%d), UNIT %d", _player->getId(), unitsOrderedToAttack, iAttackPlayer, armySize, iTarget);
        } else {
            sprintf(msg, "AI: I (id=%d) ordered %d units to attack player %d (armySize=%d), STRUCTURE %d", _player->getId(), unitsOrderedToAttack, iAttackPlayer, armySize, iTarget);
        }
        logbook(msg);
    }

    char msg2[255];
    sprintf(msg2, "AI [%d] think_attack() - end", _player->getId());
    logbook(msg2);
}


void cAIPlayer::think_upgrades() {
    // prevent human m_Player thinking
    if (_player->getId() == HUMAN) {
        return; // do not build for human! :)
    }

    if (TIMER_Upgrades > 0) {
        TIMER_Upgrades--;
        return;
    }

    TIMER_Upgrades = 20;

    // logic for remaining upgrades? (ie super weapons?)
    if (_player->hasAtleastOneStructure(HEAVYFACTORY)) {
        // check if there is an upgrade for MCV available
        cBuildingListItem *upgrade = isUpgradeAvailableToGrantUnit(MCV);
        if (upgrade != nullptr && upgrade->isAvailable() && _player->hasEnoughCreditsFor(upgrade->getCosts())) {
            startUpgrading(upgrade->getBuildId());
        }
    }
}

void cAIPlayer::think_buildarmy() {
    if (_player->getId() == HUMAN)
        return; // do not build for human! :)

    if (TIMER_BuildUnits > 0) {
        TIMER_BuildUnits--;
        return;
    }

    TIMER_BuildUnits=5;

    // Depending on the mission/tech level, try to build stuff
    int iMission = game.iMission;
	int iChance = 10;

    if (_player->getHouse() == HARKONNEN ||
        _player->getHouse() == SARDAUKAR) {
		if (iMission <= 2) {
			iChance=50;
		} else {
			iChance=10;
		}
	}
	else
	{
        // non trooper house(s)
		if (iMission <= 2) {
			iChance=20;
		}
	}

    char msg[255];
    sprintf(msg, "AI[%d] - think_buildarmy() : iMission = %d and iChance is %d", _player->getId(), iMission, iChance);
    logbook(msg);

    // Skirmish override
    if (game.bSkirmish) {
        // chance depends on ideal size of foot soldier army
        int groundUnits = _player->getAmountOfUnitsForType(std::vector<int>{TROOPERS, TROOPER, INFANTRY, SOLDIER});
        float idealSize = 7;
        iChance = (idealSize - groundUnits) * (100.0f / idealSize);
        char msg[255];
        sprintf(msg, "AI[%d] - think_buildarmy() : Determines chance to build infantry units. Counted %d. Ideal size %f, iChance is %d", _player->getId(), groundUnits, idealSize, iChance);
        logbook(msg);
    }

    if (iMission > 1 && rnd(100) < iChance) {
        if (_player->hasEnoughCreditsForUnit(INFANTRY) || _player->hasEnoughCreditsForUnit(TROOPERS)) {
            char msg[255];
            sprintf(msg, "AI[%d] - think_buildarmy() : Wants to build infantry/troopers.", _player->getId());
            logbook(msg);
            BUILD_UNIT(INFANTRY); // (INFANTRY->TROOPERS CONVERSION IN FUNCTION)
        } else {
            char msg[255];
            sprintf(msg, "AI[%d]: Wants to build soldier/trooper.", _player->getId());
            logbook(msg);
            BUILD_UNIT(SOLDIER); // (SOLDIER->TROOPER CONVERSION IN FUNCTION)
        }
    }


    // QUAD/TRIKE BUILDING
	iChance=50;

	// low chance on buying the higher the mission
	if (iMission > 4) iChance = 30;
	if (iMission > 6) iChance = 20;
	if (iMission > 7) iChance = 10;
    if (iMission > 8) iChance = 5;

    // Skirmish override
    if (game.bSkirmish)
        iChance=7;

	// build quads / trikes
    if (iMission > 2 && rnd(100) < iChance) {
        if (_player->hasEnoughCreditsForUnit(QUAD))
        {
            char msg[255];
            sprintf(msg, "AI[%d] - think_buildarmy() : Wants to build QUAD.", _player->getId());
            logbook(msg);

            BUILD_UNIT(QUAD);
        }
        else if (_player->hasEnoughCreditsForUnit(TRIKE))
		{
            char msg[255];
            sprintf(msg, "AI[%d] - think_buildarmy() : Wants to build TRIKE.", _player->getId());
            logbook(msg);
			BUILD_UNIT(TRIKE);
		}
    }

    int harvesters = 0;     // harvesters
    if (_player->hasAtleastOneStructure(HEAVYFACTORY)) {
        // how many harvesters do we own?
        for (int i=0; i < MAX_UNITS; i++) {
            if (!unit[i].isValid()) continue;
            if (unit[i].iPlayer == _player->getId() && unit[i].iType == HARVESTER)
               harvesters++;
        }

        // 1 harvester for each refinery please
        if (harvesters < _player->getAmountOfStructuresForType(REFINERY)) {
            if (_player->hasEnoughCreditsFor(units[HARVESTER].cost)) {
                BUILD_UNIT(HARVESTER); // build harvester
            }
        }
        else if (harvesters >= _player->getAmountOfStructuresForType(REFINERY)) {
            // enough harvesters , try to get ratio 2 harvesters for one refinery
            if (harvesters < (_player->getAmountOfStructuresForType(REFINERY) * 2)) {
                if (rnd(100) < 15) {
                    BUILD_UNIT(HARVESTER);
                }
            }
        }
    } // Harvester buy logic

    // ability to build carryalls
    if (_player->hasAtleastOneStructure(HIGHTECH)) {
        if (_player->hasEnoughCreditsForUnit(CARRYALL)) {
            int optimalAmountCarryAlls = (harvesters + 1) / 2;
            int carryalls = _player->getAmountOfUnitsForType(CARRYALL);     // carryalls

            if (carryalls < optimalAmountCarryAlls) {
                if (rnd(100) < 30) {
                    BUILD_UNIT(CARRYALL);
                }
            }
        }

        if (iMission > 6) {
            if (_player->getHouse() == ATREIDES) {
                if (_player->hasEnoughCreditsForUnit(ORNITHOPTER)) {
                    if (rnd(100) < 15) {
                        BUILD_UNIT(ORNITHOPTER);
                    }
                }
            }
        }
    }

    if (iMission >= 8 || game.bSkirmish) {
        bool canBuySpecial = _player->hasAtleastOneStructure(HEAVYFACTORY) && _player->hasAtleastOneStructure(IX);
        if (canBuySpecial) {
            int iSpecial = DEVASTATOR;

            if (_player->getHouse() == ATREIDES) {
                iSpecial = SONICTANK;
            }

            if (_player->getHouse() == ORDOS) {
                iSpecial = DEVIATOR;
            }

            if (_player->hasEnoughCreditsForUnit(iSpecial)) {
                BUILD_UNIT(iSpecial);
            }
        }

        if (_player->hasEnoughCreditsForUnit(SIEGETANK)) {
            // enough to buy launcher , tank
            int nr = rnd(100);
            if (nr < 30)
                BUILD_UNIT(LAUNCHER);
            else if (nr > 30 && nr < 60)
                BUILD_UNIT(TANK);
            else if (nr > 60)
                BUILD_UNIT(SIEGETANK);
        } else if (_player->hasEnoughCreditsForUnit(LAUNCHER)) {
            if (rnd(100) < 50)
                BUILD_UNIT(LAUNCHER);
            else
                BUILD_UNIT(TANK);
        } else if (_player->hasEnoughCreditsForUnit(TANK)) {
            BUILD_UNIT(TANK);
        }
    }


    if (iMission == 4 && rnd(100) < 70) {
        if (_player->hasEnoughCreditsForUnit(TANK)) {
            BUILD_UNIT(TANK);
        }
    }

    if (iMission == 5) {
        if (_player->hasEnoughCreditsForUnit(LAUNCHER)) {
            if (rnd(100) < 50) {
                BUILD_UNIT(LAUNCHER);
            } else {
                BUILD_UNIT(TANK);
            }
        } else if (_player->hasEnoughCreditsForUnit(TANK)) {
            BUILD_UNIT(TANK);
        }
    }

    if (iMission == 6) {
        // when enough money, 50/50 on siege/launcher. Else just buy a tank or do nothing
        if (_player->hasEnoughCreditsForUnit(SIEGETANK)) {
            if (rnd(100) < 50) {
                BUILD_UNIT(SIEGETANK);
            } else {
                BUILD_UNIT(LAUNCHER);
            }
        } else if (_player->hasEnoughCreditsForUnit(TANK)) {
            {
                if (rnd(100) < 30)
                    BUILD_UNIT(TANK); // buy a normal tank in mission 6
            }
        }
    }

    if (iMission == 7) {
        // when enough money, 50/50 on siege/launcher. Else just buy a tank or do nothing
        if (_player->hasEnoughCreditsForUnit(SIEGETANK)) {
            if (rnd(100) < 50)
                BUILD_UNIT(SIEGETANK);
            else
                BUILD_UNIT(LAUNCHER);
        } else if (_player->hasEnoughCreditsForUnit(TANK)) {
            if (rnd(100) < 30)
                BUILD_UNIT(TANK); // buy a normal tank in mission 6
        }
    }
}

void cAIPlayer::think_buildbase() {
    if (_player->getId() == HUMAN) {
        return; // human m_Player does not think for buildbase
    }

    char msg[255];
    sprintf(msg, "AI[%d]: DELAY_buildbase [%d]", _player->getId(), DELAY_buildbase);
    logbook(msg);
    if (DELAY_buildbase > 0) {
        DELAY_buildbase--;
        return; // skip
    }

    DELAY_buildbase = 0;

    if (game.bSkirmish) {
        think_skirmishBuildBase();
        return;
	}

    // non-skirmish game, rebuild destroyed structures (TODO)

}

void cAIPlayer::think_skirmishBuildBase() {
    cBuildingListItem *pUpgrade = isUpgradingConstyard();
    if (pUpgrade) {
        char msg[255];
        sprintf(msg, "AI[%d] think_skirmishBuildBase() - upgrade [%s] in progress [%d/%d] for ConstYard.", _player->getId(), pUpgrade->getS_Upgrade().description, pUpgrade->getBuildTime(), pUpgrade->getTotalBuildTime());
        logbook(msg);
        return; // wait for upgrade to be completed
    }

    bool result = think_buildingplacement();
    if (result) {
        char msg[255];
        sprintf(msg, "AI[%d] think_skirmishBuildBase() - think_buildingplacement returns true, delaying!", _player->getId());
        DELAY_buildbase = 5;
        return;
    }

    cBuildingListItem *pBuildingStructure = isBuildingStructure();
    if (pBuildingStructure) {
        char msg[255];
        sprintf(msg, "AI[%d] think_skirmishBuildBase() - building structure [%s]; [%d/%d] wait for that to complete.", _player->getId(), pBuildingStructure->getS_Structures().name, pBuildingStructure->getProgress(), pBuildingStructure->getTotalBuildTime());
        logbook(msg);
        return; // wait
    }

    if (!_player->hasAtleastOneStructure(CONSTYARD)) {
        // if player has HEAVY FACTORY, try to build MVC and then get new Const Yard.
//            char msg[255];
//            sprintf(msg, "AI[%d] think_buildbase() - No Constyard present, bail.", ID);
//            logbook(msg);
        return;
    }

    bool isLowOnCash = !_player->hasEnoughCreditsFor(300);

    // when no windtrap, then build one regardless of low on cash
    if (!_player->hasAtleastOneStructure(WINDTRAP) ||
        !_player->bEnoughPower() &&
        _player->hasEnoughCreditsForStructure(WINDTRAP)) {
        buildStructureIfAllowed(WINDTRAP);
        return;
    }

    // build refinery
    if (!_player->hasAtleastOneStructure(REFINERY) &&
        _player->hasEnoughCreditsForStructure(REFINERY)) {
        buildStructureIfAllowed(REFINERY);
        return;
    }

    if (isLowOnCash) {
        return; // wait a bit
    }

    // build wor / barracks
    if (_player->getHouse() == ATREIDES) {
        if (!_player->hasAtleastOneStructure(BARRACKS)) {
            buildStructureIfAllowed(BARRACKS);
            return;
        }
    } else {
        if (!_player->hasAtleastOneStructure(WOR)) {
            buildStructureIfAllowed(WOR);
            return;
        }
    }

    if (!_player->hasAtleastOneStructure(RADAR)) {
        buildStructureIfAllowed(RADAR);
        return;
    }

    if (!_player->hasAtleastOneStructure(LIGHTFACTORY)) {
        buildStructureIfAllowed(LIGHTFACTORY);
        return;
    }

    // from here, we can build turrets
    if (!_player->hasAtleastOneStructure(HEAVYFACTORY)) {
        buildStructureIfAllowed(HEAVYFACTORY);
        return;
    }

    bool hasEnoughMoneyToAlsoBuildArmy = _player->hasEnoughCreditsFor(600);
    if (!hasEnoughMoneyToAlsoBuildArmy) {
        return; // skip, do not build defenses if we can't even build some units as well
    }

    // build refinery
    if (_player->getAmountOfStructuresForType(REFINERY) < 2)
    {
        buildStructureIfAllowed(REFINERY);
        return;
    }

    // when mission is lower then 5, build normal turrets
    if (game.iMission <= 5) {
        int amountOfTurretsWanted = 3;
        if (_player->getAmountOfStructuresForType(TURRET) < amountOfTurretsWanted) {
            buildStructureIfAllowed(TURRET);
            return;
        }
    }


    if (game.iMission >= 6) {
        // Requires an upgrade
        if (isStructureAvailableForBuilding(RTURRET)) {
            if (_player->getAmountOfStructuresForType(RTURRET) < 3) {
                buildStructureIfAllowed(RTURRET);
                return;
            }
        } else {
            // Requires an upgrade
            cBuildingListItem *upgrade = isUpgradeAvailableToGrantStructure(RTURRET);
            if (upgrade) {
                logbook("think_buildbase: Cannot build RTURRET because I need to upgrade; starting upgrade now.");
                startUpgrading(upgrade->getBuildId());
                return;
            } else {
                logbook("think_buildbase: Cannot build RTURRET because I need to upgrade; but no RTURRET upgrade available.");
                // no rturret upgrade available - check if 4slab upgrade is still there
                cBuildingListItem *upgrade = isUpgradeAvailableToGrantStructure(SLAB4);
                if (upgrade) {
                    logbook("think_buildbase: SLAB4 upgrade available, starting upgrade now.");
                    startUpgrading(upgrade->getBuildId());
                }
            }
        }
    }

    if (!_player->hasAtleastOneStructure(HIGHTECH)) {
        buildStructureIfAllowed(HIGHTECH);
        return;
    }

    if (_player->hasAtleastOneStructure(REPAIR))
    {
        buildStructureIfAllowed(REPAIR);
        return;
    }

    if (!_player->hasAtleastOneStructure(PALACE))
    {
        buildStructureIfAllowed(PALACE);
        return;
    }

    if (game.iMission >= 6)
    {
        if (_player->getAmountOfStructuresForType(RTURRET) < 9)
        {
            buildStructureIfAllowed(RTURRET);
            return;
        }
    }

    if (_player->getAmountOfStructuresForType(STARPORT) < 1) {
        buildStructureIfAllowed(STARPORT);
        return;
    }
}

/////////////////////////////////////////////////


// This function will do a check what kind of structure is needed to build the unittype
// Basicly the function returns true when its valid to build the unittype, or false
// when its impossible (due no structure, money, etc)
cantBuildReason cAIPlayer::canBuildUnit(int iUnitType) {
    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.
    char msg[255];
    sprintf(msg, "canBuildUnit: Wanting to build iUnitType = [%d(=%s)] for player [%d(=%s)]; allowed?...", iUnitType, units[iUnitType].name, _player->getId(), _player->getHouseName().c_str());
    logbook(msg);

    // CHECK 1: Do we have the money?
    if (!_player->hasEnoughCreditsForUnit(iUnitType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because cost %d higher than credits %d", units[iUnitType].cost, _player->getCredits());
        logbook(msg);
        return cantBuildReason::NOT_ENOUGH_MONEY; // NOPE
    }

    // CHECK 2: Aren't we building this already?
    if (isBuildingUnitType(iUnitType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because already building unitType");
        logbook(msg);
        return cantBuildReason::ALREADY_BUILDING;
    }

    int iStrucType = structureUtils.getStructureTypeByUnitBuildId(iUnitType);

    // Do the reality-check, do we have the building needed?
    if (!_player->hasAtleastOneStructure(iStrucType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because we do not own the required structure type [%s] for this unit: [%s]", structures[iStrucType].name, units[iUnitType].name);
        logbook(msg);
        return cantBuildReason::REQUIRES_STRUCTURE;
    }

    if (!isUnitAvailableForBuilding(iUnitType)) {
        // not available to build (not in list)
        // assume it requires an upgrade?
        return cantBuildReason::REQUIRES_UPGRADE;
    }

    logbook("canBuildUnit: ALLOWED");

    return cantBuildReason::NONE;
}

int cAIPlayer::findRandomUnitTarget(int playerIndexToAttack) {
    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));

    int maxTargets=0;

    for (int i=0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != playerIndexToAttack) continue;
        // unit belongs to player of the player we wish to attack

        bool isVisibleForPlayer = map.isVisible(cUnit.getCell(), _player);

        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "AI %d (House %d) -> Visible = %d", _player->getId(), _player->getHouse(), isVisibleForPlayer);
            logbook(msg);
        }

        // HACK HACK: the AI player does not need to discover an enemy player yet
        if (isVisibleForPlayer || game.bSkirmish) {
            iTargets[maxTargets] = i;
            maxTargets++;

            if (maxTargets > 99)
                break;
        }
    }

	if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "Targets %d", maxTargets);
        logbook(msg);
	}

    return iTargets[rnd(maxTargets)];
}

int cAIPlayer::findRandomStructureTarget(int iAttackPlayer) {
    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));

    int iT=0;

    for (int i=0; i < MAX_STRUCTURES; i++)
        if (structure[i])
            if (structure[i]->getOwner() == iAttackPlayer)
				if (map.isVisible(structure[i]->getCell(), _player->getId()) ||
					game.bSkirmish)
				{
					iTargets[iT] = i;

					iT++;

					if (iT > 99)
						break;
				}


	if (DEBUGGING)
	{
	char msg[255];
	sprintf(msg, "STR] Targets %d", iT);
	logbook(msg);
	}


    return (iTargets[rnd(iT)]);
}

void cAIPlayer::think_repair_structure(cAbstractStructure *struc)
{
	// think of repairing, only when it is not being repaired yet.
	if (!struc->isRepairing()) {
		// when ai has a lot of money, repair even faster
		if (player[struc->getOwner()].hasEnoughCreditsFor(1000)) {
			if (struc->getHitPoints() < (struc->getS_StructuresType().hp))  {
				struc->setRepairing(true);
			}
		} else {
			// AUTO-REPAIR BY AI
			if (struc->getHitPoints() < (struc->getS_StructuresType().hp/2)) {
				struc->setRepairing(true);
			}
		}
	}
}

int cAIPlayer::findCellToPlaceStructure(int iStructureType) {
	// loop through all structures, and try to place structure
	// next to them:
	//         ww
	//           ss
	//           ss

	// s = structure:
	// w = start point (+ width of structure).

	// so starting at : x - width , y - height.
	// ending at      : x + width of structure + width of type
	// ...            : y + height of s + height of type

	if (iStructureType < 0) return -1;

	int iWidth= structures[iStructureType].bmp_width / 32;
	int iHeight= structures[iStructureType].bmp_height / 32;

	int iDistance=0;

	int iGoodCells[50]; // remember 50 possible locations
	int iGoodCellID=0;

	// clear out table of good locations
	memset(iGoodCells, -1, sizeof(iGoodCells));

	bool bGood=false;

    cStructureFactory *pStructureFactory = cStructureFactory::getInstance();

    // loop through structures
    for (int i=0; i < MAX_STRUCTURES; i++)	{

        // valid
        cAbstractStructure *pStructure = structure[i];
        if (pStructure == nullptr) continue;
        if (pStructure->getOwner() != _player->getId()) continue;

        // scan around
        int c1 = pStructure->getCell();
        int iStartX= map.getCellX(c1);
        int c = pStructure->getCell();
        int iStartY= map.getCellY(c);

        int iEndX = iStartX + pStructure->getWidth() + 1;
        int iEndY = iStartY + pStructure->getHeight()  + 1;

        int topLeftX = iStartX - iWidth;
        int topLeftY = iStartY - iHeight;

//        // check above structure if it can place
        for (int sx = topLeftX; sx < iEndX; sx++) {
            int sy = iStartY;
            int cell = map.getCellWithMapDimensions(sx, sy);

            int r = pStructureFactory->getSlabStatus(cell, iStructureType, -1);

            if (r > -2) {
                if (iStructureType == TURRET && iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), _player->getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        int underNeathStructureY = iStartY + pStructure->getHeight();
        // check underneath structure
        for (int sx = topLeftX; sx < iEndX; sx++) {
            int sy = underNeathStructureY;
            int cell = map.getCellWithMapDimensions(sx, sy);

            int r = pStructureFactory->getSlabStatus(cell, iStructureType, -1);

            if (r > -2) {
                if (iStructureType == TURRET && iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), _player->getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        // check left side
        for (int sy = topLeftY; sy < iEndY; sy++) {
            int sx = topLeftX;
            int cell = map.getCellWithMapDimensions(sx, sy);

            int r = pStructureFactory->getSlabStatus(cell, iStructureType, -1);

            if (r > -2) {
                if (iStructureType == TURRET && iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), _player->getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        // check right side
        int rightOfStructure = iStartX + pStructure->getWidth();
        for (int sy = topLeftY; sy < iEndY; sy++) {
            int sx = rightOfStructure;
            int cell = map.getCellWithMapDimensions(sx, sy);

            int r = pStructureFactory->getSlabStatus(cell, iStructureType, -1);

            if (r > -2) {
                if (iStructureType == TURRET && iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), _player->getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        if (iGoodCellID > 10) {
            break; // found 10 good spots
        }
    }

    if (iGoodCellID > 0) {
        char msg[255];
        sprintf(msg, "Found %d possible cells to place structureType [%d(=%s)]", iGoodCellID, iStructureType, structures[iStructureType].name);
        logbook(msg);
        return (iGoodCells[rnd(iGoodCellID)]);
    }

	return -1;
}

/**
 * Checks if the given structureType is available for producing.
 *
 * @param iStructureType
 * @return
 */
bool cAIPlayer::isStructureAvailableForBuilding(int iStructureType) const {
    cBuildingListItem *pItem = _player->getSideBar()->getBuildingListItem(LIST_CONSTYARD, iStructureType);
    return pItem != nullptr;
}

/**
 * Checks if the given unitType is available for producing.
 *
 * @param iUnitType
 * @return
 */
bool cAIPlayer::isUnitAvailableForBuilding(int iUnitType) const {
    int listId = units[iUnitType].listId;
    cBuildingListItem *pItem = _player->getSideBar()->getBuildingListItem(listId, iUnitType);
    char msg[255];
    bool result = pItem != nullptr;
    sprintf(msg, "AI[%d] isUnitAvailableForBuilding(unitType=%d) -> %s", _player->getId(), iUnitType, result ? "TRUE" : "FALSE");
    logbook(msg);
    return result;
}

/**
 * Checks if the given structureType is available for upgrading. If so, returns the corresponding cBuildingListItem
 * which can be used later (ie, to execute an upgrade, or check other info)
 *
 * @param iStructureType
 * @return
 */
cBuildingListItem * cAIPlayer::isUpgradeAvailableToGrantStructure(int iStructureType) const {
    cBuildingList *pList = _player->getSideBar()->getList(LIST_UPGRADES);
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = pList->getItem(i);
        if (pItem == nullptr) continue;
        const s_Upgrade &theUpgrade = pItem->getS_Upgrade();
        if (theUpgrade.providesType != STRUCTURE) continue;
        if (theUpgrade.providesTypeId == iStructureType) {
            return pItem;
        }
    }
    return nullptr;
}


/**
 * Checks if the given unitType is available for upgrading. If so, returns the corresponding cBuildingListItem
 * which can be used later (ie, to execute an upgrade, or check other info)
 *
 * @param iUnitTYpe
 * @return
 */
cBuildingListItem * cAIPlayer::isUpgradeAvailableToGrantUnit(int iUnitType) const {
    cBuildingList *pList = _player->getSideBar()->getList(LIST_UPGRADES);
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = pList->getItem(i);
        if (pItem == nullptr) continue;
        const s_Upgrade &theUpgrade = pItem->getS_Upgrade();
        if (theUpgrade.providesType != UNIT) continue;
        if (theUpgrade.providesTypeId == iUnitType) {
            return pItem;
        }
    }
    return nullptr;
}



/**
 * Checks if any Upgrade is in progress for the given listId/sublistId
 *
 * @param listId
 * @param sublistId
 * @return
 */
cBuildingListItem * cAIPlayer::isUpgradingList(int listId, int sublistId) const {
    cBuildingList *upgradesList = _player->getSideBar()->getList(LIST_UPGRADES);
    if (upgradesList == nullptr) {
        char msg[255];
        sprintf(msg, "AI[%d] - isUpgradingList for listId [%d] and sublistId [%d], could not find upgradesList!? - will return FALSE.", _player->getId(), listId, sublistId);
        logbook(msg);
        assert(false);
        return nullptr;
    }
    
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = upgradesList->getItem(i);
        if (pItem == nullptr) continue;
        const s_Upgrade &theUpgrade = pItem->getS_Upgrade();
        // is this upgrade applicable to the listId/sublistId we're interested in?
        if (theUpgrade.providesTypeList == listId && theUpgrade.providesTypeSubList == sublistId) {
            if (pItem->isBuilding()) {
                // it is in progress, so yes!
                return pItem;
            }
        }
    }
    return nullptr;
}

cBuildingListItem * cAIPlayer::isUpgradingConstyard() const {
    return isUpgradingList(LIST_CONSTYARD, 0);
}

/**
 * Find the closest spice bloom compared to iCell. If iCell < 0 then use middle of map. If no close cell is found
 * a fall back of 'select any spice bloom randomly' is performed.
 * <br/>
 * @param iCell
 * @return > -1 if found or < 0 upon failure
 */
int CLOSE_SPICE_BLOOM(int iCell) {
    int quarterOfMap = map.getWidth() / 4;
    int iDistance = quarterOfMap;
    int halfWidth = map.getWidth() / 2;
    int halfHeight = map.getHeight() / 2;

    if (iCell < 0) {
        // use cell at center
        iCell = map.getCellWithMapDimensions(halfWidth, halfHeight);
        iDistance = map.getWidth();
    }

    int cx, cy;
    int closestBloomFoundSoFar=-1;
    int bloomsEvaluated = 0;

    cx = map.getCellX(iCell);
    cy = map.getCellY(iCell);

    for (int i=0; i < map.getMaxCells(); i++) {
        int cellType = map.getCellType(i);
        if (cellType != TERRAIN_BLOOM) continue;
        bloomsEvaluated++;

        int d = ABS_length(cx, cy, map.getCellX(i), map.getCellY(i));

        if (d < iDistance) {
            closestBloomFoundSoFar = i;
            iDistance = d;
        }
    }

    // found a close spice bloom
    if (closestBloomFoundSoFar > 0) {
        return closestBloomFoundSoFar;
    }

    // no spice blooms evaluated, abort
    if (bloomsEvaluated < 0) {
        return -1;
    }

    // randomly pick one
    int iTargets[10];
    memset(iTargets, -1, sizeof(iTargets));
    int iT=0;

    for (int i=0; i < map.getMaxCells(); i++) {
        int cellType = map.getCellType(i);
        if (cellType == TERRAIN_BLOOM) {
            iTargets[iT] = i;
            iT++;
            if (iT >= 10) break;
        }
    }

    // when finished, return bloom
    return iTargets[rnd(iT)];
}