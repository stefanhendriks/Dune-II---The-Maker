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
