/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"

// TODO: constructor/destructors

// cAIPlayer functions
void cAIPlayer::init(int iID) {
    ID = iID;

	// SKIRMISH RELATED SETTINGS
	// TODO: move this out of here, this basically ensures there is one playable AI at start (when setting up
	// skirmish game)
	if (iID > 1) {
        bPlaying = false;
    } else {
        bPlaying = true;
    }

	// same here, this initially sets 3 units
	iUnits=3;

    iCheckingPlaceStructure=-1;

	// -- END

    TIMER_attack = (700 + rnd(400));

    TIMER_BuildUnits = 300; // give m_Player advantage to build his stuff first, before computer grows his army
//    TIMER_blooms = 200;
    TIMER_blooms = 50;
    TIMER_repair = 500;
}

void cAIPlayer::BUILD_STRUCTURE(int iStrucType) {
    if (isBuildingStructure()) {
        char msg[255];
        sprintf(msg, "BUILD_STRUCTURE AI[%d] wants to build structure type [%d(=%s)] but it is still building a structure - ABORTING", ID, iStrucType, structures[iStrucType].name);
        logbook(msg);
        return;
    }

    if (isBuildingStructureAwaitingPlacement()) {
        char msg[255];
        sprintf(msg, "BUILD_STRUCTURE AI[%d] wants to build structure type [%d(=%s)] but it is still awaiting placement of a structure", ID, iStrucType, structures[iStrucType].name);
        logbook(msg);
        return;
    }

	// not building & busy placing

	// check if its allowed at all
    cPlayer &cPlayer = player[ID];

    if (!cPlayer.hasAtleastOneStructure(CONSTYARD)) {
        char msg[255];
        sprintf(msg, "BUILD_STRUCTURE AI[%d] wants to build structure type [%d(=%s)] but can't because no CONSTYARD present.", ID, iStrucType, structures[iStrucType].name);
        logbook(msg);
        return;
    }

    startBuildingStructure(iStrucType);
}

void cAIPlayer::BUILD_UNIT(int iUnitType) {

    // Fix up house mixtures
    const cPlayer &cPlayer = player[ID];

    if (cPlayer.getHouse() == HARKONNEN || cPlayer.getHouse() == SARDAUKAR) {
        if (iUnitType == INFANTRY) iUnitType = TROOPERS;
        if (iUnitType == SOLDIER) iUnitType = TROOPER;
        if (iUnitType == TRIKE) iUnitType = QUAD;
    }

    // In mission 9 there are only WOR's, so make a special hack hack for that ;)
    if (game.iMission >= 8) {
        if (iUnitType == INFANTRY) iUnitType = TROOPERS;
        if (iUnitType == SOLDIER) iUnitType = TROOPER;
    }

    if (cPlayer.getHouse() == ORDOS) {
        if (iUnitType == TRIKE) iUnitType = RAIDER;
    }

    bool bAllowed = canAIBuildUnit(ID, iUnitType);

    if (!bAllowed) {
        return; // do not go further
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
        startBuildingUnit(iUnitType);
    } else {
        char msg[255];
        sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d] - but already building.", units[iUnitType].name, iUnitType);
        logbook(msg);
    }
}

void cAIPlayer::startBuildingUnit(int iUnitType) const {
    const cPlayer &cPlayer = player[ID];

    // Duplicated logic - for now - determining lists by unit id.
    int listId = LIST_UNITS;

    if (iUnitType == SOLDIER || iUnitType == INFANTRY) {
        listId = LIST_FOOT_UNITS;
    } else if (iUnitType == TROOPER || iUnitType == TROOPERS) {
        listId = LIST_FOOT_UNITS;
    }

    bool startedBuilding = cPlayer.getSideBar()->startBuildingItemIfOk(listId, iUnitType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listId[%d] - SUCCESS", units[iUnitType].name, iUnitType, listId);
        } else {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listId[%d] - FAILED", units[iUnitType].name, iUnitType, listId);
        }
        logbook(msg);
    }
}

void cAIPlayer::startBuildingStructure(int iStructureType) const {
    const cPlayer &cPlayer = player[ID];

    // Duplicated logic - for now - determining lists by unit id.
    int listId = LIST_CONSTYARD;

    bool startedBuilding = cPlayer.getSideBar()->startBuildingItemIfOk(listId, iStructureType);

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
    const cPlayer &cPlayer = player[ID];
    int listId = LIST_UPGRADES;
    bool startedBuilding = cPlayer.getSideBar()->startBuildingItemIfOk(listId, iUpgradeType);

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
    const cPlayer &cPlayer = player[ID];
    cItemBuilder *pItemBuilder = cPlayer.getItemBuilder();

    // here we hard-code a transition from unitType to LIST_ * and sublist id, need to think how we can shove this
    // into the game.ini and such. This logic is taken from the cBuildingListUpdater::onStructureCreated.
    // Duplicated logic - for now - determining lists by unit id.
    int listId = LIST_UNITS;
    int sublistId = SUBLIST_HEAVYFCTRY; // default

    if (iUnitType == SOLDIER || iUnitType == INFANTRY) {
        listId = LIST_FOOT_UNITS;
        sublistId = SUBLIST_INFANTRY;
    } else if (iUnitType == TROOPER || iUnitType == TROOPERS) {
        listId = LIST_FOOT_UNITS;
        sublistId = SUBLIST_TROOPERS;
    } else if (iUnitType == QUAD || iUnitType == TRIKE || iUnitType == RAIDER) {
        sublistId = SUBLIST_LIGHTFCTRY;
    } else  if (iUnitType == CARRYALL || iUnitType == ORNITHOPTER) {
        sublistId = SUBLIST_HIGHTECH;
    }

    return pItemBuilder->isAnythingBeingBuiltForListId(listId, sublistId);
}

/**
 * Returns true if anything is built from ConstYard
 */
cBuildingListItem *cAIPlayer::isBuildingStructure() const {
    const cPlayer &cPlayer = player[ID];
    cItemBuilder *pItemBuilder = cPlayer.getItemBuilder();

    return pItemBuilder->getListItemWhichIsBuilding(LIST_CONSTYARD, 0);
}

/**
 * Returns true if anything is built from ConstYard
 */
bool cAIPlayer::isBuildingStructureAwaitingPlacement() const {
    const cPlayer &cPlayer = player[ID];
    cItemBuilder *pItemBuilder = cPlayer.getItemBuilder();

    return pItemBuilder->isAnythingBeingBuiltForListIdAwaitingPlacement(LIST_CONSTYARD, 0);
}

int cAIPlayer::getStructureTypeBeingBuilt() {
    cBuildingListItem *pItem = getStructureBuildingListItemBeingBuilt();
    if (pItem) {
        return pItem->getBuildId();
    }
    return -1;
}

cBuildingListItem *cAIPlayer::getStructureBuildingListItemBeingBuilt() const {
    const cPlayer &cPlayer = player[ID];
    cItemBuilder *pItemBuilder = cPlayer.getItemBuilder();

    return pItemBuilder->getListItemWhichIsBuilding(LIST_CONSTYARD, 0);
}


void cAIPlayer::think_buildingplacement() {
    /*
		structure building;

		when building completed, search for a spot and place it!
	*/
    bool buildingStructureAwaitingPlacement = isBuildingStructureAwaitingPlacement();

    if (game.bSkirmish) {
        if (isBuildingStructure()) {
            // still building, so do nothing, unless we await placement
            if (!buildingStructureAwaitingPlacement) {
                return;
            }
        }

        cPlayer &cPlayer = player[ID];

        // find place to place structure
        if (buildingStructureAwaitingPlacement) {
            int structureType = getStructureTypeBeingBuilt();
            int iCll = findCellToPlaceStructure(structureType);

            if (iCll > -1) {
//                cStructureFactory::getInstance()->createStructure(iCll, structureType, ID, 100);

                int iHealthPercent = 50; // the minimum is 50% (with no slabs)

                // TODO: This logic is duplicated in cPlaceItDrawer, this should be somewhere
                // generic, unrelated to player in places (ie, the health determination?)
//                if (iTotalRocks > 0) {
//                    iHealthPercent += health_bar(50, iTotalRocks, iTotalBlocks);
//                }

                cBuildingListItem *pItem = getStructureBuildingListItemBeingBuilt();
                cPlayer.getStructurePlacer()->placeStructure(iCll, structureType, iHealthPercent);
                cPlayer.getBuildingListUpdater()->onBuildItemCompleted(pItem);

                pItem->decreaseTimesToBuild();
                pItem->setPlaceIt(false);
                pItem->setIsBuilding(false);
                pItem->setProgress(0);
                if (pItem->getTimesToBuild() < 1) {
                    cPlayer.getItemBuilder()->removeItemFromList(pItem);
                }
            } else {
                // unable to place?!
                logbook("CANNOT PLACE STRUCTURE");
            }
        }
    }
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
            int cell = rnd(MAX_CELLS);
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
            cPlayer &cPlayer = player[ID];
            int iBloom = CLOSE_SPICE_BLOOM(cPlayer.focus_cell);

            // TODO: Move this to a unit repository thingy which can query the units
            for (int i = 0; i < MAX_UNITS; i++) {
                cUnit &cUnit = unit[i];
                if (!cUnit.isValid()) continue;
                if (cUnit.iPlayer == HUMAN) continue; // skip human units
                if (cUnit.iPlayer != ID) continue; // skip units which are not my own
                if (cUnit.iAction != ACTION_GUARD) continue; // skip units which are doing something else
                if (!cUnit.isInfantryUnit()) continue; // skip non-infantry units

                int d = ABS_length(iCellGiveX(iBloom),
                                   iCellGiveY(iBloom),
                                   iCellGiveX(cUnit.iCell),
                                   iCellGiveY(cUnit.iCell));

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
    if (ID < 0 || ID > AI_WORM) {
        logbook("ERROR!!! AI has invalid ID to think with!?");
        return;
    }

    if (game.bDisableAI) return; // do nothing

    if (ID == HUMAN) {
        return; // AI is not human / skip
    }

    // not time yet to think
    cPlayer &cPlayer = player[ID];
    if (cPlayer.TIMER_think > 0) {
        cPlayer.TIMER_think--;
        return;
    }

    cPlayer.TIMER_think = 10;

    // depening on m_Player, do thinking
    if (ID == AI_WORM) {
        if (rnd(100) < 25)
            think_worm();

        return;
    }

    // think about fair harvester stuff
    think_spiceBlooms();

    // Now think about building stuff etc
	think_buildbase();
//	think_buildarmy();
//    think_attack();
}

void cAIPlayer::think_repair() {
    if (TIMER_repair > 0) {
        TIMER_repair--;
        return;
    }

    TIMER_repair=1000; // next timed interval to think about this...

    // check if we must repair, only if we have a repair structure ofcourse
    // and we have some money to spare for repairs
    if (player[ID].hasAtleastOneStructure(REPAIR) && player[ID].credits > 250) {
        // yes, we can repair
        for (int i=0; i < MAX_UNITS; i++) {
            if (unit[i].isValid()) {
                if (unit[i].iPlayer == ID) {
                    if (unit[i].isDamaged())
                    {
                        // head off to repair
                    	int iNewID = structureUtils.findClosestStructureTypeWhereNoUnitIsHeadingToComparedToCell(
                                unit[i].iCell, REPAIR, &player[ID]);

						if (iNewID > -1) {
							int iCarry = CARRYALL_TRANSFER(i, structure[iNewID]->getCell() + 2);

							if (iCarry > -1) {
								// yehaw we will be picked up!
								unit[i].TIMER_movewait = 100;
								unit[i].TIMER_thinkwait = 100;
							} else {
								UNIT_ORDER_MOVE(i, structure[iNewID]->getCell());
							}

							unit[i].iStructureID = iNewID;
							unit[i].iGoalCell = structure[iNewID]->getCell();
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
        think_repair();
        return;
    }

    char msg[255];
    sprintf(msg, "AI [%d] think_attack() - start", ID);
    logbook(msg);

    TIMER_attack = (rnd(900) + 100);

    int armySize = 3 + rnd(8); // size of units to attack with
    int iTarget = -1;
    bool isAttackingUnit = false; // do we attack a unit? (if false, it means we attack a structure)

    bool bInfantryOnly = false;

    int iAttackPlayer = 0; // HUMAN

    cPlayer &cPlayer = player[ID];
    if (game.bSkirmish) {
        // skirmish games will make the ai a bit more agressive!
        armySize += 3 + rnd(7);

        // check what players are playing
        int iPl[5];
        int iPlID = 0;
        memset(iPl, -1, sizeof(iPl));

        for (int p = 0; p < AI_WORM; p++) {
            if (p != ID) {
                bool bOk = false;
                if (p == 0) bOk = true;
                if (p > 1 && aiplayer[p].bPlaying) bOk = true;

                // skip same team
                if (player[p].iTeam != cPlayer.iTeam) {
                    iPl[iPlID] = p;
                    iPlID++;
                }
            }
        }

        iAttackPlayer = iPl[rnd(iPlID)];
        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "AI (id=%d) - Attacking player id %d", this->ID, iAttackPlayer);
            logbook(msg);
        }
    }

    // only when ai has a wor/barracks to regenerate his troops, send off infantry sometimes.
    bool hasHeavyOrLightFactory = cPlayer.hasAtleastOneStructure(LIGHTFACTORY) || cPlayer.hasAtleastOneStructure(HEAVYFACTORY);
    bool hasInfantryStructures = cPlayer.hasAtleastOneStructure(WOR) || cPlayer.hasAtleastOneStructure(BARRACKS);
    if (hasInfantryStructures && !hasHeavyOrLightFactory) {
        bInfantryOnly = true;
    }

    int unitsAvailable = 0;
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != ID) continue; // skip all units but my own
        if (cUnit.isHarvester()) continue; // skip harvesters
        if (cUnit.isAirbornUnit()) continue; // skip airborn
        if (!cUnit.isIdle()) continue; // skip non-idle units
        unitsAvailable++;
    }

    if (unitsAvailable < (armySize / 2)) {
        char msg[255];
        sprintf(msg, "AI: (ID=%d) skipping attack, because we have %d units available for attacking which is less than 50 percent of requested army size %d", ID, unitsAvailable, armySize);
        logbook(msg);
    }

    // flip coin, either attack a random unit or structure
    if (rnd(100) < 50) {
        iTarget = AI_RANDOM_STRUCTURE_TARGET(ID, iAttackPlayer);

        if (iTarget < 0) {
            iTarget = AI_RANDOM_UNIT_TARGET(ID, iAttackPlayer);
            isAttackingUnit = true;
        }
    } else {
        iTarget = AI_RANDOM_UNIT_TARGET(ID, iAttackPlayer);
        isAttackingUnit = true;

        if (iTarget < 0) {
            iTarget = AI_RANDOM_STRUCTURE_TARGET(ID, iAttackPlayer);
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
        if (cUnit.iPlayer != ID) continue; // skip all units but my own
        if (cUnit.isHarvester()) continue; // skip harvesters
        if (cUnit.isAirbornUnit()) continue; // skip airborn
        if (!cUnit.isIdle()) continue; // skip non-idle units

        if (isAttackingUnit)
            UNIT_ORDER_ATTACK(i, unit[iTarget].iCell, iTarget, -1, -1);
        else
            UNIT_ORDER_ATTACK(i, structure[iTarget]->getCell(), -1, iTarget, -1);

        unitsOrderedToAttack++;
        if (unitsOrderedToAttack >= armySize)
            break;
    }

    if (DEBUGGING) {
        char msg[255];
        if (isAttackingUnit) {
            sprintf(msg, "AI: I (id=%d) ordered %d units to attack player %d (armySize=%d), UNIT %d", ID, unitsOrderedToAttack, iAttackPlayer, armySize, iTarget);
        } else {
            sprintf(msg, "AI: I (id=%d) ordered %d units to attack player %d (armySize=%d), STRUCTURE %d", ID, unitsOrderedToAttack, iAttackPlayer, armySize, iTarget);
        }
        logbook(msg);
    }

    char msg2[255];
    sprintf(msg2, "AI [%d] think_attack() - end", ID);
    logbook(msg2);

}

void cAIPlayer::think_buildarmy() {
    // prevent human m_Player thinking
    if (ID == 0)
        return; // do not build for human! :)

    if (TIMER_BuildUnits > 0) {
        TIMER_BuildUnits--;
        return;
    }

    TIMER_BuildUnits=5;

    // Depending on the mission/tech level, try to build stuff
    int iMission = game.iMission;
	int iChance = 10;

    cPlayer &cPlayer = player[ID];

    if (cPlayer.getHouse() == HARKONNEN ||
        cPlayer.getHouse() == SARDAUKAR) {
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

    // Skirmish override
    if (game.bSkirmish) {
        iChance=10;
    }

    if (iMission > 1 && rnd(100) < iChance)
    {
        if (cPlayer.credits > units[INFANTRY].cost)
        {
            BUILD_UNIT(INFANTRY); // (INFANTRY->TROOPERS CONVERSION IN FUNCTION)
        }
        else
            BUILD_UNIT(SOLDIER); // (SOLDIER->TROOPER CONVERSION IN FUNCTION)
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
        if (cPlayer.credits > units[QUAD].cost)
        {
			BUILD_UNIT(QUAD);
        }
        else if (cPlayer.credits > units[TRIKE].cost)
		{
			BUILD_UNIT(TRIKE);
		}
    }

    int harvesters = 0;     // harvesters
    if (iMission > 3) {
        // how many harvesters do we own?
        for (int i=0; i < MAX_UNITS; i++) {
            if (!unit[i].isValid()) continue;
            if (unit[i].iPlayer == ID && unit[i].iType == HARVESTER)
               harvesters++;
        }

        // 1 harvester for each refinery please
        if (harvesters < cPlayer.getAmountOfStructuresForType(REFINERY)) {
            if (cPlayer.hasEnoughCreditsFor(units[HARVESTER].cost)) {
                BUILD_UNIT(HARVESTER); // build harvester
            }
        }
        else if (harvesters >= cPlayer.getAmountOfStructuresForType(REFINERY)) {
            // enough harvesters , try to get ratio 2 harvesters for one refinery
            if (harvesters < (cPlayer.getAmountOfStructuresForType(REFINERY) * 2)) {
                if (rnd(100) < 15) {
                    BUILD_UNIT(HARVESTER);
                }
            }
        }
    } // Harvester buy logic when mission > 3

    // ability to build carryalls
    if (iMission >= 5) {
        int carryalls= 0;     // carryalls
        if (cPlayer.hasEnoughCreditsFor(units[CARRYALL].cost)) {

            for (int i=0; i < MAX_UNITS; i++) {
                if (!unit[i].isValid()) continue;
                if (unit[i].iPlayer == ID && unit[i].iType == CARRYALL)
                    carryalls++;
            }

            int optimalAmountCarryAlls = (harvesters + 1) / 2;

            if (carryalls < optimalAmountCarryAlls) {
                if (rnd(100) < 30) {
                    BUILD_UNIT(CARRYALL);
                }
            }
        }
    }

    if (iMission > 6) {
        if (cPlayer.getHouse() == ATREIDES) {
            if (cPlayer.credits > units[ORNITHOPTER].cost) {
                if (rnd(100) < 15) {
                    BUILD_UNIT(ORNITHOPTER);
                }
            }
        }
    }

	if (iMission >= 8 || game.bSkirmish) {
	    bool canBuySpecial = cPlayer.hasAtleastOneStructure(HEAVYFACTORY) && cPlayer.hasAtleastOneStructure(IX);
	    if (canBuySpecial) {
            int iSpecial = DEVASTATOR;

            if (cPlayer.getHouse() == ATREIDES) {
                iSpecial = SONICTANK;
            }

            if (cPlayer.getHouse() == ORDOS) {
                iSpecial = DEVIATOR;
            }

            if (cPlayer.credits > units[iSpecial].cost) {
                BUILD_UNIT(iSpecial);
            }
        }

        if (cPlayer.credits > units[SIEGETANK].cost)
        {
            // enough to buy launcher , tank
            int nr = rnd(100);
            if (nr < 30)
                BUILD_UNIT(LAUNCHER);
            else if (nr > 30 && nr < 60)
                BUILD_UNIT(TANK);
            else if (nr > 60)
                BUILD_UNIT(SIEGETANK);
        }
        else if (cPlayer.credits > units[LAUNCHER].cost)
        {
            if (rnd(100) < 50)
                BUILD_UNIT(LAUNCHER);
            else
                BUILD_UNIT(TANK);
        }
        else if (cPlayer.credits > units[TANK].cost)
        {
            BUILD_UNIT(TANK);
        }
    }


    if (iMission == 4 && rnd(100) < 70)
    {
        if (cPlayer.credits > units[TANK].cost)
            BUILD_UNIT(TANK);
    }

    if (iMission == 5)
    {
        if (cPlayer.credits > units[LAUNCHER].cost)
        {
            // enough to buy launcher , tank
            if (rnd(100) < 50)
                BUILD_UNIT(LAUNCHER);
            else
                BUILD_UNIT(TANK);
        }
        else if (cPlayer.credits > units[TANK].cost)
        {
            BUILD_UNIT(TANK);
        }
    }

    if (iMission == 6)
    {
		// when enough money, 50/50 on siege/launcher. Else just buy a tank or do nothing
		if (cPlayer.credits > units[SIEGETANK].cost)
		{
			if (rnd(100) < 50)
				BUILD_UNIT(SIEGETANK);
			else
				BUILD_UNIT(LAUNCHER);
		}
		else if (cPlayer.credits > units[TANK].cost)
		{
			if (rnd(100) < 30)
				BUILD_UNIT(TANK); // buy a normal tank in mission 6
		}
    }

    if (iMission == 7)
    {
        // when enough money, 50/50 on siege/launcher. Else just buy a tank or do nothing
		if (cPlayer.credits > units[SIEGETANK].cost)
		{
			if (rnd(100) < 50)
				BUILD_UNIT(SIEGETANK);
			else
				BUILD_UNIT(LAUNCHER);
		}
		else if (cPlayer.credits > units[TANK].cost)
		{
			if (rnd(100) < 30)
				BUILD_UNIT(TANK); // buy a normal tank in mission 6
		}
    }
}

void cAIPlayer::think_buildbase() {
    if (ID == HUMAN) {
        return; // human m_Player does not think for buildbase
    }

    if (game.bSkirmish) {
        cBuildingListItem *pUpgrade = isUpgradingConstyard();
        if (pUpgrade) {
            char msg[255];
            sprintf(msg, "AI[%d] think_buildbase() - upgrade [%s] in progress [%d/%d] for ConstYard.", ID, pUpgrade->getS_Upgrade().description, pUpgrade->getBuildTime(), pUpgrade->getTotalBuildTime());
            logbook(msg);
            return; // wait for upgrade to be completed
        }

        think_buildingplacement();

        cBuildingListItem *pBuildingStructure = isBuildingStructure();
        if (pBuildingStructure) {
            char msg[255];
            sprintf(msg, "AI[%d] think_buildbase() - building structure [%s]; [%d/%d] wait for that to complete.", ID, pBuildingStructure->getS_Structures().name, pBuildingStructure->getBuildTime(), pBuildingStructure->getTotalBuildTime());
            logbook(msg);
            return; // wait
        }

        cPlayer &cPlayer = player[ID];

        if (!cPlayer.hasAtleastOneStructure(CONSTYARD)) {
            // if player has HEAVY FACTORY, try to build MVC and then get new Const Yard.
//            char msg[255];
//            sprintf(msg, "AI[%d] think_buildbase() - No Constyard present, bail.", ID);
//            logbook(msg);
            return;
        }

        // when no windtrap, then build one (or when low power)
        if (!cPlayer.hasAtleastOneStructure(WINDTRAP) ||
            !cPlayer.bEnoughPower()) {
            BUILD_STRUCTURE(WINDTRAP);
            return;
        }

        // build refinery
        if (!cPlayer.hasAtleastOneStructure(REFINERY)) {
            BUILD_STRUCTURE(REFINERY);
            return;
        }

        // build wor / barracks
        if (cPlayer.getHouse() == ATREIDES) {
            if (!cPlayer.hasAtleastOneStructure(BARRACKS)) {
                BUILD_STRUCTURE(BARRACKS);
                return;
            }
        } else {
            if (!cPlayer.hasAtleastOneStructure(WOR)) {
                BUILD_STRUCTURE(WOR);
                return;
            }
        }

        if (!cPlayer.hasAtleastOneStructure(RADAR)) {
            BUILD_STRUCTURE(RADAR);
            return;
        }

        if (!cPlayer.hasAtleastOneStructure(LIGHTFACTORY)) {
            BUILD_STRUCTURE(LIGHTFACTORY);
            return;
        }

        // from here, we can build turrets
        if (!cPlayer.hasAtleastOneStructure(HEAVYFACTORY)) {
            BUILD_STRUCTURE(HEAVYFACTORY);
            return;
        }

        // when mission is lower then 5, build normal turrets
        if (game.iMission <= 5) {
            int amountOfTurretsWanted = 3;
            if (cPlayer.getAmountOfStructuresForType(TURRET) < amountOfTurretsWanted) {
                BUILD_STRUCTURE(TURRET);
                return;
            }
        }

        if (game.iMission >= 6) {
            // Requires an upgrade
            if (isStructureAvailableForBuilding(RTURRET)) {
                if (cPlayer.getAmountOfStructuresForType(RTURRET) < 3) {
                    BUILD_STRUCTURE(RTURRET);
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

        // build refinery
        if (cPlayer.getAmountOfStructuresForType(REFINERY) < 2)
        {
            BUILD_STRUCTURE(REFINERY);
            return;
        }

        if (!cPlayer.hasAtleastOneStructure(HIGHTECH))
        {
            BUILD_STRUCTURE(HIGHTECH);
            return;
        }

        if (cPlayer.hasAtleastOneStructure(REPAIR))
        {
            BUILD_STRUCTURE(REPAIR);
            return;
        }

        if (!cPlayer.hasAtleastOneStructure(PALACE))
        {
            BUILD_STRUCTURE(PALACE);
            return;
        }

        if (game.iMission >= 6)
        {
            if (cPlayer.getAmountOfStructuresForType(RTURRET) < 9)
            {
                BUILD_STRUCTURE(RTURRET);
                return;
            }
        }

        if (cPlayer.getAmountOfStructuresForType(STARPORT) < 1)
        {
            BUILD_STRUCTURE(STARPORT);
            return;
        }

	}
	else
	{
		// rebuild only destroyed stuff
	}

}

void cAIPlayer::think_worm() {
    // loop through all its worms and move them around
    for (int i=0; i < MAX_UNITS; i++) {
        if (!unit[i].isValid()) continue;
        if (unit[i].iType != SANDWORM) continue;
        if (unit[i].iPlayer != ID) continue;

        // when on guard
        if (unit[i].iAction == ACTION_GUARD) {

            // find new spot to go to
            for (int iTries = 0; iTries < 10; iTries++) {
                int iMoveTo = rnd(MAX_CELLS);

                if (map.getCellType(iMoveTo) == TERRAIN_SAND ||
                    map.getCellType(iMoveTo) == TERRAIN_HILL ||
                    map.getCellType(iMoveTo) == TERRAIN_SPICE ||
                    map.getCellType(iMoveTo) == TERRAIN_SPICEHILL) {
                    UNIT_ORDER_MOVE(i, iMoveTo);
                    break;
                }
            }
        }
    }

//    char msg2[255];
//    sprintf(msg2, "AI [%d] think_worm() - end", ID);
//    logbook(msg2);
}

/////////////////////////////////////////////////


/**
 * Returns what kind of structure will build this unit type
 * @param iUnitType
 * @return
 */
int AI_STRUCTYPE(int iUnitType) {
    // Default = heavyfactory, so do a check if its NOT.

    // light vehicles
    if (iUnitType == TRIKE || iUnitType == RAIDER || iUnitType == QUAD) return LIGHTFACTORY;

    // soldiers and troopers
    if (iUnitType == INFANTRY || iUnitType == SOLDIER) return BARRACKS;

    if (iUnitType == TROOPER || iUnitType == TROOPERS) return WOR;

    // airborn stuff
    if (iUnitType == CARRYALL || iUnitType == ORNITHOPTER) return HIGHTECH;

    return HEAVYFACTORY;
}

// This function will do a check what kind of structure is needed to build the unittype
// Basicly the function returns true when its valid to build the unittype, or false
// when its impossible (due no structure, money, etc)
bool canAIBuildUnit(int iPlayer, int iUnitType) {
    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.
    cPlayer &cPlayer = player[iPlayer];

    char msg[255];
    sprintf(msg, "canAIBuildUnit: Wanting to build iUnitType = [%d(=%s)] for player [%d(=%s)]; allowed?...", iUnitType, units[iUnitType].name, iPlayer, cPlayer.getHouseName().c_str());
    logbook(msg);

    // CHECK 1: Do we have the money?
    if (cPlayer.credits < units[iUnitType].cost) {
        char msg[255];
        sprintf(msg, "canAIBuildUnit: FALSE, because cost %d higher than credits %d", units[iUnitType].cost, cPlayer.credits);
        logbook(msg);
        return false; // NOPE
    }

    // CHECK 2: Aren't we building this already?
    if (aiplayer[iPlayer].isBuildingUnitType(iUnitType)) {
        char msg[255];
        sprintf(msg, "canAIBuildUnit: FALSE, because already building unitType");
        logbook(msg);
        return false;
    }

    int iStrucType = AI_STRUCTYPE(iUnitType);

    // Do the reality-check, do we have the building needed?
    if (!cPlayer.hasAtleastOneStructure(iStrucType)) {
        char msg[255];
        sprintf(msg, "canAIBuildUnit: FALSE, because we do not own the required structure type [%s] for this unit: [%s]", structures[iStrucType].name, units[iUnitType].name);
        logbook(msg);
        return false; // we do not have the building
    }

    logbook("canAIBuildUnit: ALLOWED");

    // WE MAY BUILD IT!
    return true;
}

int AI_RANDOM_UNIT_TARGET(int iPlayer, int playerIndexToAttack) {
    if (iPlayer < 0 || iPlayer >= MAX_PLAYERS) return -1;

    cPlayer &cPlayer = player[iPlayer];

    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));

    int maxTargets=0;

    for (int i=0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != playerIndexToAttack) continue;
        // unit belongs to player of the player we wish to attack

        bool isVisibleForPlayer = mapUtils->isCellVisible(&cPlayer, cUnit.iCell);

        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "AI %d (House %d) -> Visible = %d", iPlayer, cPlayer.getHouse(),
                    isVisibleForPlayer);
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

int AI_RANDOM_STRUCTURE_TARGET(int iPlayer, int iAttackPlayer)
{
    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));

    int iT=0;

    for (int i=0; i < MAX_STRUCTURES; i++)
        if (structure[i])
            if (structure[i]->getOwner() == iAttackPlayer)
				if (mapUtils->isCellVisibleForPlayerId(iPlayer, structure[i]->getCell()) ||
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
		if (player[struc->getOwner()].credits > 1000) {
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
        if (pStructure->getOwner() != ID) continue;

        // scan around
        int iStartX=iCellGiveX(pStructure->getCell());
        int iStartY=iCellGiveY(pStructure->getCell());

        int iEndX = iStartX + (pStructure->getWidthInPixels() / 32) + 1;
        int iEndY = iStartY + (pStructure->getHeightInPixels() / 32) + 1;

        iStartX -= iWidth;
        iStartY -= iHeight;

        // do not go out of boundries
        FIX_POS(iStartX, iStartY);
        FIX_POS(iEndX, iEndY);


        for (int sx = iStartX; sx < iEndX; sx++) {
            for (int sy = iStartY; sy < iEndY; sy++) {
                int cell = iCellMake(sx, sy);

                int r = pStructureFactory->getSlabStatus(cell, iStructureType, -1);

                if (r > -2) {
                    if (iStructureType == TURRET && iStructureType == RTURRET) {
                        // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                        int iDist = ABS_length(sx, sy, iCellGiveX(player[ID].focus_cell),
                                               iCellGiveY(player[ID].focus_cell));

                        if (iDist >= iDistance) {
                            iDistance = iDist;
                            iGoodCells[iGoodCellID] = cell;
                            iGoodCellID++;
                        }

                    } else {
                        // for turrets, the most far counts
                        iGoodCells[iGoodCellID] = cell;
                        iGoodCellID++;
                    }
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
    const cPlayer &cPlayer = player[ID];
    cBuildingListItem *pItem = cPlayer.getSideBar()->getBuildingListItem(LIST_CONSTYARD, iStructureType);
    return pItem != nullptr;
}

/**
 * Checks if the given structureType is available for upgrading. If so, returns the corresponding cBuildingListItem
 * which can be used later (ie, to execute an upgrade, or check other info)
 *
 * @param iStructureType
 * @return
 */
cBuildingListItem * cAIPlayer::isUpgradeAvailableToGrantStructure(int iStructureType) const {
    const cPlayer &cPlayer = player[ID];
    cBuildingList *pList = cPlayer.getSideBar()->getList(LIST_UPGRADES);
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
 * Checks if any Upgrade is in progress for the given listId/sublistId
 *
 * @param listId
 * @param sublistId
 * @return
 */
cBuildingListItem * cAIPlayer::isUpgradingList(int listId, int sublistId) const {
    const cPlayer &cPlayer = player[ID];
    cBuildingList *upgradesList = cPlayer.getSideBar()->getList(LIST_UPGRADES);
    if (upgradesList == nullptr) {
        char msg[255];
        sprintf(msg, "AI[%d] - isUpgradingList for listId [%d] and sublistId [%d], could not find upgradesList!? - will return FALSE.", ID, listId, sublistId);
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
        iCell = iCellMakeWhichCanReturnMinusOne(halfWidth, halfHeight);
        iDistance = map.getWidth();
    }

    int cx, cy;
    int closestBloomFoundSoFar=-1;
    int bloomsEvaluated = 0;

    cx = iCellGiveX(iCell);
    cy = iCellGiveY(iCell);

    for (int i=0; i < MAX_CELLS; i++) {
        int cellType = map.getCellType(i);
        if (cellType != TERRAIN_BLOOM) continue;
        bloomsEvaluated++;

        int d = ABS_length(cx, cy, iCellGiveX(i), iCellGiveY(i));

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

    for (int i=0; i < MAX_CELLS; i++) {
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