/*
 * cUpgradeUtils.cpp
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cUpgradeUtils::cUpgradeUtils() {

}

cUpgradeUtils::~cUpgradeUtils() {
}


bool cUpgradeUtils::canUpgradeList(int listTypeId, int techlevel, int currentUpgradeLevelOfList) {
	int costToUpgrade = getPriceToUpgradeList(listTypeId, techlevel, currentUpgradeLevelOfList);
	return costToUpgrade > -1;
}

int cUpgradeUtils::getPriceToUpgradeList(int listTypeId, int techlevel, int currentUpgradeLevelOfList) {

	if (listTypeId == LIST_CONSTYARD) {
		// upgrade for 4SLAB
		if (techlevel >= 4 && currentUpgradeLevelOfList < 1) {
			return 200;
		}
		// upgrade for RTURRET
		if (techlevel >= 6 && currentUpgradeLevelOfList < 2) {
			return 400;
		}
	}

	return -1;
}

cListUpgrade * cUpgradeUtils::getListUpgradeForList(int listTypeId, int techlevel, int currentUpgradeLevelOfList) {
	if (listTypeId == LIST_CONSTYARD) {
		// upgrade for 4SLAB
		if (techlevel >= 4 && currentUpgradeLevelOfList < 1) {
			return new cListUpgrade(50, 200, UPGRADE_ONE);
		}
		// upgrade for RTURRET
		if (techlevel >= 6 && currentUpgradeLevelOfList < 2) {
			return new cListUpgrade(200, 400, UPGRADE_TWO);
		}
	}

	return NULL;
}

bool cUpgradeUtils::isUpgradeApplicableForPlayerAndList(cPlayer *thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList) {
	assert(thePlayer);
	assert(listTypeId >= LIST_CONSTYARD);
	assert(techlevel >= 0);
	assert(currentUpgradeLevelOfList >= 0);
	cBuildingList *list = thePlayer->getSideBar()->getList(listTypeId);
	assert(list);

	bool isUpgrading = list->isUpgrading();

	// when already upgrading, a new upgrade is not applicable
	if (isUpgrading) {
		return false;
	}

	bool canUpgradeListResult = canUpgradeList(listTypeId, techlevel, currentUpgradeLevelOfList);

	// if we cannot upgrade list, return false
	if (!canUpgradeListResult) {
		return false;
	}

	// if we cannot pay for the upgrade, return false
	bool canPayForUpgrade = canPlayerPayForUpgradeForList(thePlayer, listTypeId, techlevel, currentUpgradeLevelOfList);

	return canPayForUpgrade;
}

bool cUpgradeUtils::canPlayerPayForUpgradeForList(cPlayer *thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList) {
	assert(thePlayer);
	int credits = (int)thePlayer->credits;
	int price = getPriceToUpgradeList(listTypeId, techlevel, currentUpgradeLevelOfList);

	// it is not available, so we cant pay
	if (price < 0) {
		return false;
	}

	return credits > price;
}

bool cUpgradeUtils::isMouseOverUpgradeButton(int mouseX, int mouseY) {
	// determine if mouse is over the button..
	if (  (mouseX > 29 && mouseX < 187) && (mouseY > 2 && mouseY < 31)) {
		return true;
	}
	return false;
}
