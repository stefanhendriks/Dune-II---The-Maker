/*
 * cUpgradeBuilder.cpp
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cUpgradeBuilder::cUpgradeBuilder(cPlayer * thePlayer) {
	assert(thePlayer);
	memset(upgrades, 0, sizeof(upgrades));
	player = thePlayer;
}

cUpgradeBuilder::~cUpgradeBuilder() {
}

void cUpgradeBuilder::addUpgrade(int listId, cListUpgrade * listUpgrade) {
	assert(listUpgrade);
	assert(listId >= 0);
	assert(listId <= LIST_MAX);

	if (isUpgrading(listId)) {
		return;
	}

	upgrades[listId] = listUpgrade;
	player->getSideBar()->getList(listId)->setUpgrading(true);
}

bool cUpgradeBuilder::isUpgrading(int listId) {
	assert(listId >= 0);
	assert(listId < LIST_MAX);
	return player->getSideBar()->getList(listId)->isUpgrading();
}

// timer based method that processes upgrades
void cUpgradeBuilder::processUpgrades() {
	for (int i = 0; i < LIST_MAX; i++) {
		cListUpgrade * upgrade = upgrades[i];
		if (upgrade) {
			upgrade->setTimerProgress(upgrade->getTimerProgress()+1);

			if (upgrade->getTimerProgress() > 10) {
				upgrade->setTimerProgress(0);

				upgrade->setProgress(upgrade->getProgress() + 1);

				// pay for each slice of upgrade
				player->credits -= upgrade->getPricePerTimeUnit();

				if (upgrade->getProgress() >= upgrade->getProgressLimit()) {
					cBuildingList *listToUpgrade = player->getSideBar()->getList(i);
					player->getBuildingListUpdater()->updateUpgradeCompleted(listToUpgrade);
					upgrades[i] = NULL;
					listToUpgrade->setUpgrading(false);
					delete upgrade;
				}

			}
		}
	}
}
