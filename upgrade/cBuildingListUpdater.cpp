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
    char msg[255];
    sprintf(msg, "onStructureCreated - for player [%d], structureType [%d]", player->getId(), structureType);
    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureCreated", msg);

	// activate/deactivate any lists if needed
	cBuildingList *listConstYard = player->getSideBar()->getList(LIST_CONSTYARD);

	int house = player->getHouse();
	int techLevel = player->getTechLevel();

	assert(listConstYard);

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

	}

	// Heavyfactory
	if (structureType == HEAVYFACTORY)
	{
		if (techLevel >= 7) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added IX to list");
			listConstYard->addStructureToList(IX, 0);
		}
	}

	///////////////////////////////////
	// ADJUSTMENTS TO INFANTRY LIST
	///////////////////////////////////
    cBuildingList *listFootUnits = player->getSideBar()->getList(LIST_FOOT_UNITS);
	if (structureType == BARRACKS) {
        listFootUnits->addUnitToList(SOLDIER, SUBLIST_INFANTRY);
    } else if (structureType == WOR) {
        listFootUnits->addUnitToList(TROOPER, SUBLIST_TROOPERS);
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
                listUnits->addUnitToList(SONICTANK, SUBLIST_HEAVYFCTRY);
			} else if (player->getHouse() == HARKONNEN) {
                listUnits->addUnitToList(DEVASTATOR, SUBLIST_HEAVYFCTRY);
			} else if (player->getHouse() == ORDOS) {
                listUnits->addUnitToList(DEVIATOR, SUBLIST_HEAVYFCTRY);
			}
		}
	}

	// do something
	cLogger::getInstance()->logCommentLine("onStructureCreated - end");
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
