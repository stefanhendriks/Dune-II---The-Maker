/*
 * cBuildingListUpgrader.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cBuildingListUpdater::cBuildingListUpdater(cPlayer *thePlayer) {
	assert(thePlayer);
	player = thePlayer;
}

void cBuildingListUpdater::onStructureCreated(int structureType) {
	cLogger::getInstance()->logCommentLine("onStructureCreated - begin");

	// activate/deactivate any lists if needed
	cBuildingList *listConstYard = player->getSideBar()->getList(LIST_CONSTYARD);
    cBuildingList *listFootUnits = player->getSideBar()->getList(LIST_FOOT_UNITS);
    cBuildingList *listUnits = player->getSideBar()->getList(LIST_UNITS);

	int house = player->getHouse();
	int techLevel = player->getTechLevel();

    char msg[255];
    sprintf(msg, "onStructureCreated - for player [%d], structureType [%d], techlevel [%d], house [%d]", player->getId(), structureType, techLevel, house);
    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureCreated", msg);

	assert(listConstYard);
	assert(listFootUnits);
	assert(listConstYard);

	if (structureType == CONSTYARD) {
        // add items
        listConstYard->addStructureToList(SLAB1, 0);
        cLogger::getInstance()->logCommentLine("onStructureCreated - added SLAB1 to list");

        if (techLevel >= 2) {
            if (house == ATREIDES || house == ORDOS) {
                listConstYard->addStructureToList(BARRACKS, 0);
                cLogger::getInstance()->logCommentLine("onStructureCreated - added BARRACKS to list");
            }
            if (house == HARKONNEN) {
                listConstYard->addStructureToList(WOR, 0);
                cLogger::getInstance()->logCommentLine("onStructureCreated - added WOR to list");
            }
        }

        if (techLevel >= 4) {
            //list->addItemToList(new cBuildingListItem(SLAB4, structures[SLAB4])); // only available after upgrading
            listConstYard->addStructureToList(WALL, 0);
            cLogger::getInstance()->logCommentLine("onStructureCreated - added WALL to list");
        }

        listConstYard->addStructureToList(WINDTRAP, 0);
        cLogger::getInstance()->logCommentLine("onStructureCreated - added WINDTRAP to list");
    }

	if (structureType == WINDTRAP) {
		listConstYard->addStructureToList(REFINERY, 0);
		cLogger::getInstance()->logCommentLine("onStructureCreated - added REFINARY to list");
	}

	if (structureType == REFINERY) {
		if (techLevel >= 2) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added LIGHTFACTORY to list");
			listConstYard->addStructureToList(LIGHTFACTORY,0);

			if (house == ATREIDES ||
				house == ORDOS ||
				house == FREMEN) {
				cLogger::getInstance()->logCommentLine("onStructureCreated - added BARRACKS to list");
				listConstYard->addStructureToList(BARRACKS, 0);

				if (house == ORDOS && techLevel >= 5) {
					cLogger::getInstance()->logCommentLine("onStructureCreated - added WOR to list");
					listConstYard->addStructureToList(WOR, 0);
				}
			} else if (
					house == HARKONNEN ||
					house == SARDAUKAR ||
					house == FREMEN ||
					house == MERCENARY) {
				cLogger::getInstance()->logCommentLine("onStructureCreated - added WOR to list");
				listConstYard->addStructureToList(WOR, 0);
			}
		}

		if (techLevel >= 3) {
			listConstYard->addStructureToList(RADAR, 0);
		}

		cLogger::getInstance()->logCommentLine("onStructureCreated - added SILO to list");
		listConstYard->addStructureToList(SILO, 0);
	}


	if (structureType == RADAR) {
		if (techLevel >= 5) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added TURRET to list");
			listConstYard->addStructureToList(TURRET, 0);
		}

		if (techLevel >= 8) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added PALACE to list");
			listConstYard->addStructureToList(PALACE, 0);
		}
	}

	if (structureType == LIGHTFACTORY)
	{
		if (techLevel >=4) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added HEAVYFACTORY to list");
			listConstYard->addStructureToList(HEAVYFACTORY, 0);
		}

		if (techLevel >=5) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added HIGHTECH to list");
			cLogger::getInstance()->logCommentLine("onStructureCreated - added REPAIR to list");
			listConstYard->addStructureToList(HIGHTECH, 0);
			listConstYard->addStructureToList(REPAIR, 0);
		}

		if (techLevel >= 6) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added STARPORT to list");
			listConstYard->addStructureToList(STARPORT, 0);
		}

        if (house == ATREIDES) {
            listUnits->addUnitToList(TRIKE, SUBLIST_LIGHTFCTRY);
        } else if (house == ORDOS) {
            listUnits->addUnitToList(RAIDER, SUBLIST_LIGHTFCTRY);
        } else if (house == HARKONNEN) {
            listUnits->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
        } else {
            listUnits->addUnitToList(TRIKE, SUBLIST_LIGHTFCTRY);
            listUnits->addUnitToList(RAIDER, SUBLIST_LIGHTFCTRY);
            listUnits->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
        }
	}

	// Heavyfactory
	if (structureType == HEAVYFACTORY)
	{
		if (techLevel >= 7) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added IX to list");
			listConstYard->addStructureToList(IX, 0);
		}

        listUnits->addUnitToList(TANK, SUBLIST_HEAVYFCTRY);
        listUnits->addUnitToList(HARVESTER, SUBLIST_HEAVYFCTRY);
	}

	///////////////////////////////////
	// ADJUSTMENTS TO INFANTRY LIST
	///////////////////////////////////
	if (structureType == BARRACKS) {
        listFootUnits->addUnitToList(SOLDIER, SUBLIST_INFANTRY);
    } else if (structureType == WOR) {
        listFootUnits->addUnitToList(TROOPER, SUBLIST_TROOPERS);
    }

	///////////////////////////////////
	// ADJUSTMENTS TO HEAVY FACTORY LIST
	///////////////////////////////////

	// Heavyfactory
	if (structureType == IX) {
		if (techLevel >= 7) {
			if (player->getHouse() == ATREIDES) {
                listUnits->addUnitToList(SONICTANK, SUBLIST_HEAVYFCTRY);
			} else if (player->getHouse() == HARKONNEN) {
                listUnits->addUnitToList(DEVASTATOR, SUBLIST_HEAVYFCTRY);
			} else if (player->getHouse() == ORDOS) {
                listUnits->addUnitToList(DEVIATOR, SUBLIST_HEAVYFCTRY);
			}
		}
	}

	if (structureType == HIGHTECH) {
        listUnits->addUnitToList(CARRYALL, SUBLIST_HIGHTECH);
    }

	// do something
	cLogger::getInstance()->logCommentLine("onStructureCreated - end");
}

// structure destroyed..
void cBuildingListUpdater::onStructureDestroyed(int structureType) {
	cLogger::getInstance()->logCommentLine("onStructureDestroyed - begin");

	// do something
    int house = player->getHouse();
    int techLevel = player->getTechLevel();

    char msg[255];
    sprintf(msg, "onStructureDestroyed - for player [%d], structureType [%d], techlevel [%d], house [%d]", player->getId(), structureType, techLevel, house);
    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureDestroyed", msg);


    cLogger::getInstance()->logCommentLine("onStructureDestroyed - end");
}


/**
 * this method will update any list given, with phase given.
 */
void cBuildingListUpdater::updateUpgradeCompleted(cBuildingList *listToUpgrade) {
	assert(listToUpgrade);
	cLogger::getInstance()->logCommentLine("updateUpgradeCompleted - begin");

	int currentLevel = listToUpgrade->getUpgradeLevel();
	int newLevel = currentLevel + 1;
	// up the upgrade level
	listToUpgrade->setUpgradeLevel(newLevel);

	// constyard list upgrades two times
	char msg[255];
	sprintf(msg, "currentLevel = %d, newLevel = %d , listId = %d", currentLevel, newLevel, listToUpgrade->getType());
	cLogger::getInstance()->logCommentLine(msg);

	if (listToUpgrade->getType() == LIST_CONSTYARD) {
		if (listToUpgrade->getUpgradeLevel() == 1) {
			listToUpgrade->addStructureToList(SLAB4, 0);
		} else if (listToUpgrade->getUpgradeLevel() == 2) {
			listToUpgrade->addStructureToList(RTURRET, 0);
		}
	}

	if (listToUpgrade->getType() == LIST_UNITS) {
		if (listToUpgrade->getUpgradeLevel() == 1) {
			listToUpgrade->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
		}
	}

	cLogger::getInstance()->logCommentLine("updateUpgradeCompleted - end");
}
