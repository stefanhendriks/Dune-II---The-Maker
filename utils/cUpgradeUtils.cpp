/*
 * cUpgradeUtils.cpp
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"


cUpgradeUtils::cUpgradeUtils() {

}

cUpgradeUtils::~cUpgradeUtils() {
}


bool cUpgradeUtils::canUpgradeList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	int costToUpgrade = getPriceToUpgradeList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);
	return costToUpgrade > -1;
}

int cUpgradeUtils::getPriceToUpgradeList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(&thePlayer);

	cListUpgrade * upgrade = getListUpgradeForList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);

	if (!upgrade) return -1;

    int totalPrice = upgrade->getTotalPrice();
    delete upgrade; // delete, as it was temporarily created
    return totalPrice;
}

cListUpgrade * cUpgradeUtils::getListUpgradeForList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(&thePlayer);
	cBuildingList *list = thePlayer.getSideBar()->getList(listTypeId);

	if (listTypeId == LIST_CONSTYARD) {
		// upgrade for 4SLAB
		if (techLevel >= 4 && currentUpgradeLevelOfList < 1) {
			return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(SLAB4, structures[SLAB4], list, 0));
		}
		// upgrade for RTURRET
		if (techLevel >= 6 && thePlayer.iStructures[RADAR] > 0 && currentUpgradeLevelOfList < 2) {
			return new cListUpgrade(100, 200, UPGRADE_TWO, new cBuildingListItem(RTURRET, structures[RTURRET], list, 0));
		}
	}

	if (listTypeId == LIST_UNITS) {
		if (thePlayer.getHouse() != HARKONNEN) {
			// upgrade for Quads
			if (techLevel >= 3 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(QUAD, units[QUAD], list, 0));
			}
		}

		if (thePlayer.getHouse() == HARKONNEN || thePlayer.getHouse() == ATREIDES) {
			// upgrade to MCV
			if (techLevel >= 4 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(MCV, units[MCV], list, 0));
			}
			// upgrade to Launcher
			if (techLevel >= 5 && currentUpgradeLevelOfList < 2) {
				return new cListUpgrade(100, 200, UPGRADE_TWO, new cBuildingListItem(LAUNCHER, units[LAUNCHER], list, 0));
			}
			// upgrade to Siege Tank
			if (techLevel >= 6 && currentUpgradeLevelOfList < 3) {
				return new cListUpgrade(100, 200, UPGRADE_THREE, new cBuildingListItem(SIEGETANK, units[SIEGETANK], list, 0));
			}
		} else if (thePlayer.getHouse() == ORDOS) {
			// upgrade to MCV
			if (techLevel >= 4 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(MCV, units[MCV], list, 0));
			}
			// upgrade to Siege tank
			if (techLevel >= 7 && currentUpgradeLevelOfList < 2) {
				return new cListUpgrade(100, 200, UPGRADE_TWO, new cBuildingListItem(SIEGETANK, units[SIEGETANK], list, 0));
			}
		}

		if (thePlayer.getHouse() != HARKONNEN) {
			// upgrade for Ornithopter
			if (techLevel >= 7 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(ORNITHOPTER, units[ORNITHOPTER], list, 0));
			}
		}
	}

	if (listTypeId == LIST_FOOT_UNITS) {
		if (thePlayer.getHouse() == HARKONNEN) {
			// upgrade for troopers
			if (techLevel >= 3 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(125, 300, UPGRADE_ONE, new cBuildingListItem(TROOPERS, units[TROOPERS], list, 1));
			}
		} else if (thePlayer.getHouse() == ATREIDES) {
			// upgrade for infantry
			if (techLevel >= 3 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(75, 300, UPGRADE_ONE, new cBuildingListItem(INFANTRY, units[INFANTRY], list, 0));
			}
		} else if (thePlayer.getHouse() == ORDOS) {
			// upgrade for infantry
			if (techLevel >= 3) {
				bool hasTroopers = list->hasItemType(TROOPERS);
				bool hasInfantry = list->hasItemType(INFANTRY);

				bool hasWor = thePlayer.hasWor();
				bool hasBarracks = thePlayer.hasBarracks();

				if (currentUpgradeLevelOfList < 2) {
					if (!hasInfantry && hasBarracks) {
						return new cListUpgrade(75, 300, UPGRADE_ONE, new cBuildingListItem(INFANTRY, units[INFANTRY], list, 0));
					} else if (!hasTroopers && hasWor) {
						return new cListUpgrade(75, 300, UPGRADE_TWO, new cBuildingListItem(TROOPERS, units[TROOPERS], list, 1));
					} else {
						// do nothing
					}
				}
			}
		}
	}

	return NULL;
}

bool cUpgradeUtils::isUpgradeApplicableForPlayerAndList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(&thePlayer);
	assert(listTypeId >= LIST_CONSTYARD);
	assert(techLevel >= 0);
	assert(currentUpgradeLevelOfList >= 0);
	cBuildingList *list = thePlayer.getSideBar()->getList(listTypeId);
	assert(list);

	bool isUpgrading = list->isUpgrading();

	// when already upgrading, a new upgrade is not applicable
	if (isUpgrading) {
		return false;
	}

	bool canUpgradeListResult = canUpgradeList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);

	// if we cannot upgrade list, return false
	if (!canUpgradeListResult) {
		return false;
	}

	// if we cannot pay for the upgrade, return false
	return canPlayerPayForUpgradeForList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);
}

bool cUpgradeUtils::canPlayerPayForUpgradeForList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(&thePlayer);
	int credits = (int)thePlayer.credits;
	int price = getPriceToUpgradeList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);

	// it is not available, so we cant pay
	if (price < 0) {
		return false;
	}

	return credits >= price;
}

bool cUpgradeUtils::isMouseOverUpgradeButton(int mouseX, int mouseY) {
    return cMouse::isOverRectangle(29, 2, 158, 29);
}
