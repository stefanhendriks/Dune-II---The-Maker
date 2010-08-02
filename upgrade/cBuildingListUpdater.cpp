/*
 * cBuildingListUpgrader.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cBuildingListUpdater::cBuildingListUpdater(cPlayer *thePlayer) {
	assert(thePlayer);
	player = thePlayer;
}

void cBuildingListUpdater::updateStructureCreated(int structureType) {
	cLogger::getInstance()->logCommentLine("updateStructureCreated - begin");

	int house = player->getHouse();
	int techLevel = player->getTechLevel();

	cBuildingList *list = player->getSideBar()->getList(LIST_CONSTYARD);

	assert(list);

	if (structureType == WINDTRAP) {
		list->addItemToList(new cBuildingListItem(REFINERY, structures[REFINERY], list));
		return;
	}

	if (structureType == REFINERY) {
		if (techLevel >= 2) {
			list->addItemToList(new cBuildingListItem(LIGHTFACTORY, structures[LIGHTFACTORY], list));

			if (house == ATREIDES ||
				house == ORDOS ||
				house == FREMEN) {
				list->addItemToList(new cBuildingListItem(BARRACKS, structures[BARRACKS], list));
			} else if (
					house == HARKONNEN ||
					house == SARDAUKAR ||
					house == FREMEN ||
					house == MERCENARY)
				list->addItemToList(new cBuildingListItem(WOR, structures[WOR], list));
		}

		list->addItemToList(new cBuildingListItem(SILO, structures[SILO], list));
	}

	if (techLevel >= 3) {
		list->addItemToList(new cBuildingListItem(RADAR, structures[RADAR], list));
	}


	if (structureType == RADAR) {
		if (techLevel >= 5) {
			list->addItemToList(new cBuildingListItem(TURRET, structures[TURRET], list));
		}

		if (techLevel >= 8) {
			list->addItemToList(new cBuildingListItem(PALACE, structures[PALACE], list));
		}
	}

	if (structureType == LIGHTFACTORY)
	{
		if (techLevel >=4) {
			list->addItemToList(new cBuildingListItem(HEAVYFACTORY, structures[HEAVYFACTORY], list));
		}

		if (techLevel >=5) {
			list->addItemToList(new cBuildingListItem(HIGHTECH, structures[HIGHTECH], list));
			list->addItemToList(new cBuildingListItem(REPAIR, structures[REPAIR], list));
		}

		if (techLevel >= 6) {
			list->addItemToList(new cBuildingListItem(STARPORT, structures[STARPORT], list));
		}

	}

	// Heavyfactory
	if (structureType == HEAVYFACTORY)
	{
		if (techLevel >= 7) {
			list->addItemToList(new cBuildingListItem(IX, structures[IX], list));
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

// upgrade completed, like add 4slab
void cBuildingListUpdater::updateUpgradeCompleted(int upgradeType) {
	cLogger::getInstance()->logCommentLine("updateUpgradeCompleted - begin");

	// do something

	cLogger::getInstance()->logCommentLine("updateUpgradeCompleted - end");
}

//void upgradeTechTree(int iPlayer, int iStructureType) {
//
//	if (iPlayer != 0) {
//		logbook("tried to update tech tree for a non-human player");
//		return;
//	}
//
//	logbook("upgrading tech tree");
//
//	int iHouse = player[iPlayer].getHouse();
//	int iMission = game.iMission; // MISSION IS OK NOW, IS CORRECTLY SET AT LOADING
//
//
//

//}
