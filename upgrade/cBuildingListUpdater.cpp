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

void cBuildingListUpdater::updateStructureCreated(int structureType) {
	cLogger::getInstance()->logCommentLine("updateStructureCreated - begin");

	// activate/deactivate any lists if needed
	cBuildingList *listConstYard = player->getSideBar()->getList(LIST_CONSTYARD);

	int house = player->getHouse();
	int techLevel = player->getTechLevel();

	assert(listConstYard);

	if (structureType == WINDTRAP) {
		listConstYard->addItemToList(new cBuildingListItem(REFINERY, structures[REFINERY], listConstYard, 0));
		cLogger::getInstance()->logCommentLine("updateStructureCreated - added REFINARY to list");
	}

	if (structureType == REFINERY) {
		if (techLevel >= 2) {
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added LIGHTFACTORY to list");
			listConstYard->addItemToList(new cBuildingListItem(LIGHTFACTORY, structures[LIGHTFACTORY], listConstYard, 0));

			if (house == ATREIDES ||
				house == ORDOS ||
				house == FREMEN) {
				cLogger::getInstance()->logCommentLine("updateStructureCreated - added BARRACKS to list");
				listConstYard->addItemToList(new cBuildingListItem(BARRACKS, structures[BARRACKS], listConstYard, 0));

				if (house == ORDOS && techLevel >= 5) {
					cLogger::getInstance()->logCommentLine("updateStructureCreated - added WOR to list");
					listConstYard->addItemToList(new cBuildingListItem(WOR, structures[WOR], listConstYard, 0));
				}
			} else if (
					house == HARKONNEN ||
					house == SARDAUKAR ||
					house == FREMEN ||
					house == MERCENARY) {
				cLogger::getInstance()->logCommentLine("updateStructureCreated - added WOR to list");
				listConstYard->addItemToList(new cBuildingListItem(WOR, structures[WOR], listConstYard, 0));
			}
		}

		if (techLevel >= 3) {
			listConstYard->addItemToList(new cBuildingListItem(RADAR, structures[RADAR], listConstYard, 0));
		}

		cLogger::getInstance()->logCommentLine("updateStructureCreated - added SILO to list");
		listConstYard->addItemToList(new cBuildingListItem(SILO, structures[SILO], listConstYard, 0));
	}


	if (structureType == RADAR) {
		if (techLevel >= 5) {
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added TURRET to list");
			listConstYard->addItemToList(new cBuildingListItem(TURRET, structures[TURRET], listConstYard, 0));
		}

		if (techLevel >= 8) {
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added PALACE to list");
			listConstYard->addItemToList(new cBuildingListItem(PALACE, structures[PALACE], listConstYard, 0));
		}
	}

	if (structureType == LIGHTFACTORY)
	{
		if (techLevel >=4) {
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added HEAVYFACTORY to list");
			listConstYard->addItemToList(new cBuildingListItem(HEAVYFACTORY, structures[HEAVYFACTORY], listConstYard, 0));
		}

		if (techLevel >=5) {
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added HIGHTECH to list");
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added REPAIR to list");
			listConstYard->addItemToList(new cBuildingListItem(HIGHTECH, structures[HIGHTECH], listConstYard, 0));
			listConstYard->addItemToList(new cBuildingListItem(REPAIR, structures[REPAIR], listConstYard, 0));
		}

		if (techLevel >= 6) {
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added STARPORT to list");
			listConstYard->addItemToList(new cBuildingListItem(STARPORT, structures[STARPORT], listConstYard, 0));
		}

	}

	// Heavyfactory
	if (structureType == HEAVYFACTORY)
	{
		if (techLevel >= 7) {
			cLogger::getInstance()->logCommentLine("updateStructureCreated - added IX to list");
			listConstYard->addItemToList(new cBuildingListItem(IX, structures[IX], listConstYard, 0));
		}
	}

	///////////////////////////////////
	// ADJUSTMENTS TO INFANTRY LIST
	///////////////////////////////////
    cBuildingList *listFootUnits = player->getSideBar()->getList(LIST_FOOT_UNITS);
	if (structureType == BARRACKS) {
        listFootUnits->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], listFootUnits, 0));
    } else if (structureType == WOR) {
        listFootUnits->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], listFootUnits, 1));
    }

	///////////////////////////////////
	// ADJUSTMENTS TO HEAVY FACTORY LIST
	///////////////////////////////////
    cBuildingList *listUnits = player->getSideBar()->getList(LIST_UNITS);

	// Heavyfactory
	if (structureType == IX)
	{
		if (techLevel >= 7) {
			if (player->getHouse() == ATREIDES) {
                listUnits->addItemToList(new cBuildingListItem(SONICTANK, units[SONICTANK], listUnits, 0));
			} else if (player->getHouse() == HARKONNEN) {
                listUnits->addItemToList(new cBuildingListItem(DEVASTATOR, units[DEVASTATOR], listUnits, 0));
			} else if (player->getHouse() == ORDOS) {
                listUnits->addItemToList(new cBuildingListItem(DEVIATOR, units[DEVIATOR], listUnits, 0));
			}
		}
	}

	// do something
	cLogger::getInstance()->logCommentLine("updateStructureCreated - end");
}

// structure destroyed..
void cBuildingListUpdater::updateStructureDestroyed(int structureType) {
	cLogger::getInstance()->logCommentLine("updateStructureDestroyed - begin");

	// do something

	cLogger::getInstance()->logCommentLine("updateStructureDestroyed - end");
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
			listToUpgrade->addItemToList(new cBuildingListItem(SLAB4, structures[SLAB4], listToUpgrade, 0));
		} else if (listToUpgrade->getUpgradeLevel() == 2) {
			listToUpgrade->addItemToList(new cBuildingListItem(RTURRET, structures[RTURRET], listToUpgrade, 0));
		}
	}

	if (listToUpgrade->getType() == LIST_UNITS) {
		if (listToUpgrade->getUpgradeLevel() == 1) {
			listToUpgrade->addItemToList(new cBuildingListItem(QUAD, units[QUAD], listToUpgrade, SUBLIST_LIGHTFCTRY));
		}
	}

	cLogger::getInstance()->logCommentLine("updateUpgradeCompleted - end");
}
