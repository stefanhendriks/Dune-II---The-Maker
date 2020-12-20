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

    // iBuildingUnit[TRIKE] > 0 = its building a trike (progress!)
    memset(iBuildingUnit, -1, sizeof(iBuildingUnit));
	memset(iBuildingStructure, -1, sizeof(iBuildingStructure));
    memset(TIMER_BuildUnit, -1, sizeof(TIMER_BuildUnit));
	memset(TIMER_BuildStructure, -1, sizeof(TIMER_BuildStructure));

    TIMER_attack = (700 + rnd(400));

    TIMER_BuildUnits = 500; // give m_Player advantage to build his stuff first, before computer grows his army
    TIMER_blooms = 200;
    TIMER_repair = 500;
}

void cAIPlayer::BUILD_STRUCTURE(int iStrucType) {
	for (int i=0; i < MAX_STRUCTURETYPES; i++) {
        if (iBuildingStructure[i] > -1) {
            // already building some structure
            return;
        }
    }

	// not building

	// check if its allowed at all
    cPlayer &cPlayer = player[ID];

    if (!cPlayer.hasAtleastOneStructure(CONSTYARD))
		return;

	if (!cPlayer.hasEnoughCreditsFor(structures[iStrucType].cost))
		return; // cannot buy

	iBuildingStructure[iStrucType]=0;
	TIMER_BuildStructure[iStrucType]=0; // start building

    if (DEBUGGING) {
        logbook("Building STRUCTURE: ");
        logbook(structures[iStrucType].name);
    }

	cPlayer.substractCredits(structures[iStrucType].cost);
}

void cAIPlayer::BUILD_UNIT(int iUnitType) {

    // Fix up house mixtures
    cPlayer &cPlayer = player[ID];

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

    bool bAllowed = AI_UNITSTRUCTURETYPE(ID, iUnitType);

    if (bAllowed == false)
        return; // do not go further

    // when building a tank, etc, check if we do not already build
    bool bAlreadyBuilding = false;
    for (int i = 0; i < MAX_UNITTYPES; i++) {

        // when building a quad
        if (iUnitType == QUAD || iUnitType == TRIKE || iUnitType == RAIDER) {
            // the same
            if (i == iUnitType)
                if (iBuildingUnit[i] > -1)
                    bAlreadyBuilding = true;
        }

        // when building a tank or something
        if (iUnitType == TANK || iUnitType == LAUNCHER || iUnitType == SIEGETANK ||
            iUnitType == SONICTANK || iUnitType == DEVASTATOR || iUnitType == HARVESTER) {

            if (i == iUnitType)
                if (iBuildingUnit[i] > -1)
                    bAlreadyBuilding = true;
        }

        // when building a carryall
        if (iUnitType == CARRYALL || iUnitType == ORNITHOPTER) {
            if (i == iUnitType)
                if (iBuildingUnit[i] > -1)
                    bAlreadyBuilding = true;
        }
    }

    if (!bAlreadyBuilding) {
        // Now build it
        iBuildingUnit[iUnitType] = 0;                  // start building!
        cPlayer.credits -= units[iUnitType].cost; // pay for it
        if (DEBUGGING) {
            logbook("Building UNIT: ");
            logbook(units[iUnitType].name);
        }
    } else {
        if (DEBUGGING) {
//            char msg[255];
//            sprintf(msg, "Attempted to build unit %s but something similar is already being built")
            std::string unitName = units[iUnitType].name;
            std::string message =
                    "Attempted to build unit " + unitName + " but something similar is already being built";
            cLogger::getInstance()->log(eLogLevel::LOG_TRACE, eLogComponent::COMP_AI, "BUILD_UNIT", message,
                                        eLogOutcome::OUTC_FAILED, ID, cPlayer.getHouse());
        }
    }
}


void cAIPlayer::think_building() {
	if (ID == HUMAN)
		return; // human m_Player does not think

    /*
		structure building;

		when building completed, search for a spot and place it!
	*/
    cPlayer &cPlayer = player[ID];
    for (int structureType=0; structureType < MAX_STRUCTURETYPES; structureType++) {
		if (iBuildingStructure[structureType] > -1) {
			int iTimerCap=35; // was 35

			int structureCount = cPlayer.getAmountOfStructuresForType(CONSTYARD);
//			char msg[255];
//			sprintf(msg, "AI [%d] - StructureCount for CONSTYARD = %d", ID, structureCount);
//			logbook(msg);

			if (structureCount > 0) {
                iTimerCap /= structureCount;
			} else {
			    // no more const yards, abort construction
                iBuildingStructure[structureType]=-1;
                TIMER_BuildStructure[structureType]=-1;
                continue;
			}

            cPlayerDifficultySettings *difficultySettings = cPlayer.getDifficultySettings();
            iTimerCap = difficultySettings->getBuildSpeed(iTimerCap);

			TIMER_BuildStructure[structureType]++;

            if (TIMER_BuildStructure[structureType] > iTimerCap) {
                iBuildingStructure[structureType]++;
                TIMER_BuildStructure[structureType] = 0;
            }

            if (iBuildingStructure[structureType] >= structures[structureType].build_time) {
				// find place to place structure
                if (game.bSkirmish) {
					int iCll= findCellToPlaceStructure(structureType);

                    iBuildingStructure[structureType]=-1;
                    TIMER_BuildStructure[structureType]=-1;

                    if (iCll > -1) {
						cStructureFactory::getInstance()->createStructure(iCll, structureType, ID, 100);
                    } else {
						// cannot place structure now, this sucks big time. return money
                        cPlayer.credits+= structures[structureType].cost;
					}
				}
			}

			// now break the loop, as we may only do one building at a time!
			break;
		}
	}

	/*
		unit building ---> TODO: use the itemBuilder logic instead, this is duplicate logic
	*/
    for (int unitType=0; unitType < MAX_UNITTYPES; unitType++) {
        if (iBuildingUnit[unitType] > -1) {
            int iStrucType = AI_STRUCTYPE(unitType);

            TIMER_BuildUnit[unitType]++;

            int iTimerCap=35;
            int structureCount = cPlayer.getAmountOfStructuresForType(iStrucType);
//            char msg[255];
//            sprintf(msg, "AI [%d] - StructureCount for %s = %d", ID, structures[iStrucType].name, structureCount);
//            logbook(msg);

            if (structureCount > 0) {
                iTimerCap /= structureCount;
            } else {
                // structure got destroyed while building!
                iBuildingUnit[unitType] = -1;
                TIMER_BuildUnit[unitType]=0;
                continue;
            }

            cPlayerDifficultySettings *difficultySettings = cPlayer.getDifficultySettings();
            iTimerCap = difficultySettings->getBuildSpeed(iTimerCap);

            if (TIMER_BuildUnit[unitType] > iTimerCap) {
                iBuildingUnit[unitType]++;
                TIMER_BuildUnit[unitType]=0; // set to 0 again
            }

            if (iBuildingUnit[unitType] >= units[unitType].build_time) {
                //logbook("DONE BUILDING");

                // produce
                iBuildingUnit[unitType] = -1;

                // TODO: Remove duplication, which also exists in cItemBuilder::think()

                // produce now
                int structureToDeployUnit = structureUtils.findStructureToDeployUnit(&cPlayer, iStrucType);
                if (structureToDeployUnit > -1) {
                    // TODO: Remove duplication, which also exists in cItemBuilder::think()
                    cAbstractStructure *pStructureToDeploy = structure[structureToDeployUnit];
                    int cell = pStructureToDeploy->getNonOccupiedCellAroundStructure();

                    if (cell > -1) {
                        pStructureToDeploy->setAnimating(true); // animate
                        int iProducedUnit = UNIT_CREATE(cell, unitType, ID, false);
                        // Assign to team (for AI attack purposes)
                        unit[iProducedUnit].iGroup = rnd(3) + 1;
                    } else {
                        logbook("AI: huh? I was promised that this structure would have some place to deploy unit at!?");
                    }
                } else {
                    // TODO: Remove duplication, which also exists in cItemBuilder::think()
                    structureToDeployUnit = cPlayer.getPrimaryStructureForStructureType(iStrucType);
                    if (structureToDeployUnit < 0) {
                        // find any structure of type (regardless if we can deploy or not)
                        for (int i = 0; i < MAX_STRUCTURES; i++) {
                            cAbstractStructure *pStructure = structure[i];
                            if (pStructure &&
                                pStructure->isValid() &&
                                pStructure->belongsTo(ID) &&
                                pStructure->getType() == iStrucType) {
                                structureToDeployUnit = i;
                                break;
                            }
                        }
                    }
                    // TODO: Remove duplication, which also exists in cItemBuilder::think()

                    if (structureToDeployUnit > -1) {
                        // deliver unit by carryall
                        REINFORCE(ID, unitType, structure[structureToDeployUnit]->getCell(), -1);
                    } else {
                        logbook("ERROR: Unable to find structure to deploy unit!");
                    }

                }
            }

        }
    }

//    char msg2[255];
//    sprintf(msg2, "AI [%d] think_building() - end", ID);
//    logbook(msg2);

	// END OF THINK BUILDING
}

void cAIPlayer::think_spiceBlooms() {
    if (TIMER_blooms > 0) {
        TIMER_blooms--;
        return;
    }

    TIMER_blooms = 200;

    // think about spice blooms
    int iBlooms = -1;

    for (int c = 0; c < MAX_CELLS; c++)
        if (map.getCellType(c) == TERRAIN_BLOOM)
            iBlooms++;

    // When no blooms are detected, we must 'spawn' one
    if (iBlooms < 3) {
        int iCll = rnd(MAX_CELLS);

        if (map.getCellType(iCll) == TERRAIN_SAND) {
            // create bloom
            mapEditor.createCell(iCll, TERRAIN_BLOOM, 0);
        }
    } else {
        // TODO: auto detonate spice bloom?

        if (rnd(100) < 15) {
            // if we got a unit to spare (something light), we can blow it up  by walking over it
            // with a soldier or something
            int iUnit = -1;
            int iDist = 9999;
            int iBloom = CLOSE_SPICE_BLOOM(-1);
            for (int i = 0; i < MAX_UNITS; i++) {
                if (unit[i].isValid()) {
                    if (unit[i].iPlayer > 0 &&
                        unit[i].iPlayer == ID && unit[i].iAction == ACTION_GUARD) {

                        if (units[unit[i].iType].infantry) {
                            int d = ABS_length(iCellGiveX(iBloom),
                                               iCellGiveY(iBloom),
                                               iCellGiveX(unit[i].iCell),
                                               iCellGiveY(unit[i].iCell));

                            if (d < iDist) {
                                iUnit = i;
                                iDist = d;
                            }
                        }
                    }
                }

            }

            if (iUnit > -1) {
                // shoot spice bloom
                UNIT_ORDER_ATTACK(iUnit, iBloom, -1, -1, iBloom);
            } else
                BUILD_UNIT(SOLDIER);
        }
    }
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

    // think about building stuff (yes this is before the timer_think stuff!)
    think_building();

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
	think_buildarmy();
    think_attack();
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

    /*
	if (game.bSkirmish)
	{
		if (m_Player[ID].credits < 300)
			return;
	}*/

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
    if (game.bSkirmish) {
        for (int i=0; i < MAX_STRUCTURETYPES; i++) {
            if (iBuildingStructure[i] > -1) {
                // already building
                return;
            }
        }

        if (player[ID].hasAtleastOneStructure(CONSTYARD)) {
            // already building

			// when no windtrap, then build one (or when low power)
			if (!player[ID].hasAtleastOneStructure(WINDTRAP) || !player[ID].bEnoughPower())
			{
				BUILD_STRUCTURE(WINDTRAP);
				return;
			}

			// build refinery
			if (!player[ID].hasAtleastOneStructure(REFINERY))
			{
				BUILD_STRUCTURE(REFINERY);
				return;
			}

			// build wor / barracks
			if (player[ID].getHouse() == ATREIDES)
			{
				if (!player[ID].hasAtleastOneStructure(BARRACKS))
				{
					BUILD_STRUCTURE(BARRACKS);
					return;
				}
			}
			else
			{
				if (!player[ID].hasAtleastOneStructure(WOR))
				{
					BUILD_STRUCTURE(WOR);
					return;
				}
			}

			if (!player[ID].hasAtleastOneStructure(RADAR))
			{
				BUILD_STRUCTURE(RADAR);
				return;
			}

			// from here, we can build turrets & rocket turrets

			if (!player[ID].hasAtleastOneStructure(LIGHTFACTORY))
			{
				BUILD_STRUCTURE(LIGHTFACTORY);
				return;
			}

			if (!player[ID].hasAtleastOneStructure(HEAVYFACTORY))
			{
				BUILD_STRUCTURE(HEAVYFACTORY);
				return;
			}

			// when mission is lower then 5, build normal turrets
			if (game.iMission <= 5)
			{
				if (player[ID].getAmountOfStructuresForType(TURRET) < 3)
				{
					BUILD_STRUCTURE(TURRET);
					return;
				}
			}

			if (game.iMission >= 6)
			{
				if (player[ID].getAmountOfStructuresForType(RTURRET) < 3)
				{
					BUILD_STRUCTURE(RTURRET);
					return;
				}
			}

            // build refinery
			if (player[ID].getAmountOfStructuresForType(REFINERY) < 2)
			{
				BUILD_STRUCTURE(REFINERY);
				return;
			}

			if (!player[ID].hasAtleastOneStructure(HIGHTECH))
			{
				BUILD_STRUCTURE(HIGHTECH);
				return;
			}

			if (player[ID].hasAtleastOneStructure(REPAIR))
			{
				BUILD_STRUCTURE(REPAIR);
				return;
			}

			if (!player[ID].hasAtleastOneStructure(PALACE))
			{
				BUILD_STRUCTURE(PALACE);
				return;
			}

            if (game.iMission >= 6)
			{
				if (player[ID].getAmountOfStructuresForType(RTURRET) < 9)
				{
					BUILD_STRUCTURE(RTURRET);
					return;
				}
			}

   			if (player[ID].getAmountOfStructuresForType(STARPORT) < 1)
			{
				BUILD_STRUCTURE(STARPORT);
				return;
			}

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

    char msg2[255];
    sprintf(msg2, "AI [%d] think_worm() - end", ID);
    logbook(msg2);
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

// Helper functions to keep fair play:
bool AI_UNITSTRUCTURETYPE(int iPlayer, int iUnitType) {
    // This function will do a check what kind of structure is needed to build the unittype
    // Basicly the function returns true when its valid to build the unittype, or false
    // when its impossible (due no structure, money, etc)

    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.

    // CHECK 1: Do we have the money?
    if (player[iPlayer].credits < units[iUnitType].cost)
        return false; // NOPE

    // CHECK 2: Aren't we building this already?
    if (aiplayer[iPlayer].iBuildingUnit[iUnitType] >= 0)
        return false;

    int iStrucType = AI_STRUCTYPE(iUnitType);

    // Do the reality-check, do we have the building needed?
    if (!player[iPlayer].hasAtleastOneStructure(iStrucType))
        return false; // we do not have the building

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

int cAIPlayer::findCellToPlaceStructure(int iType) {
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

	int iWidth=structures[iType].bmp_width/32;
	int iHeight=structures[iType].bmp_height/32;

	int iDistance=0;

	int iGoodCells[50]; // remember 50 possible locations
	int iGoodCellID=0;

	// clear out table of good locations
	memset(iGoodCells, -1, sizeof(iGoodCells));

	bool bGood=false;

	if (iCheckingPlaceStructure < 0)
		iCheckingPlaceStructure=0;

	if (iCheckingPlaceStructure >= MAX_STRUCTURES)
		iCheckingPlaceStructure=0;

	// loop through structures
	int i;
	for (i=iCheckingPlaceStructure; i < iCheckingPlaceStructure+2; i++)
	{
		// just in case
		if (i >= MAX_STRUCTURES)
			break;

		// valid
        cAbstractStructure *pStructure = structure[i];
        if (pStructure) {
			// same owner
            if (pStructure->getOwner() == ID) {
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


				for (int sx=iStartX; sx < iEndX; sx++)
				{
					for (int sy=iStartY; sy < iEndY; sy++)
					{
						int r =  cStructureFactory::getInstance()->getSlabStatus(iCellMake(sx, sy), iType, -1);

						if (r > -2)
						{
							if (iType != TURRET && iType != RTURRET)
							{
								// for turrets, the most far counts
								bGood=true;
								iGoodCells[iGoodCellID] = iCellMake(sx, sy);
								iGoodCellID++;

                                return iCellMake(sx, sy);
							}
							else
							{
								bGood=true;

								int iDist=ABS_length(sx, sy, iCellGiveX(player[ID].focus_cell),iCellGiveY(player[ID].focus_cell));

								if ((iDist > iDistance) || (iDist == iDistance))
								{
									iDistance=iDist;
									iGoodCells[iGoodCellID] = iCellMake(sx, sy);
									iGoodCellID++;
                                    return iCellMake(sx, sy);
								}
							}
						}
					}
				} // sx
			} // same m_Player
		} // valid structure
	}

	// not turret
	if (bGood)
	{
		//					STRUCTURE_CREATE(iGoodCells[rnd(iGoodCellID)], iType, structures[iType].hp, ID);
		logbook("FOUND CELL TO PLACE");
        iCheckingPlaceStructure=-1;
		return (iGoodCells[rnd(iGoodCellID)]);
	}

    iCheckingPlaceStructure=i;
    player[ID].TIMER_think = 5;

	return -1;
}
