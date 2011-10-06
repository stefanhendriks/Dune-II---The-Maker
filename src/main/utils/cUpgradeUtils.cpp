/*
 * cUpgradeUtils.cpp
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cUpgradeUtils::cUpgradeUtils() {
	rectangle = new Rectangle(29, 2, 187, 31);
	upgradeButtonGuiShape = new GuiShape(rectangle);
}

cUpgradeUtils::~cUpgradeUtils() {
	delete upgradeButtonGuiShape;
	delete rectangle;
	upgradeButtonGuiShape = NULL;
	rectangle = NULL;
}

bool cUpgradeUtils::canUpgradeList(cPlayer * thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	int costToUpgrade = getPriceToUpgradeList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);
	return costToUpgrade > -1;
}

int cUpgradeUtils::getPriceToUpgradeList(cPlayer * thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(thePlayer);

	cListUpgrade * upgrade = getListUpgradeForList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);

	if (upgrade) {
		int totalPrice = upgrade->getTotalPrice();
		delete upgrade; // delete, as it was temporarily created
		upgrade = NULL;
		return totalPrice;
	}

	return -1;
}

cListUpgrade * cUpgradeUtils::getListUpgradeForList(cPlayer * thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(thePlayer);
	cBuildingList *list = thePlayer->getSideBar()->getList(listTypeId);

	if (listTypeId == LIST_CONSTYARD) {
		// upgrade for 4SLAB
		if (techLevel >= 4 && currentUpgradeLevelOfList < 1) {
			return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(SLAB4, structures[SLAB4], list));
		}
		// upgrade for RTURRET
		if (techLevel >= 6 && thePlayer->iStructures[RADAR] > 0 && currentUpgradeLevelOfList < 2) {
			return new cListUpgrade(100, 200, UPGRADE_TWO, new cBuildingListItem(RTURRET, structures[RTURRET], list));
		}
	}

	if (listTypeId == LIST_LIGHTFC) {
		if (thePlayer->getHouse() != HARKONNEN) {
			// upgrade for Quads
			if (techLevel >= 3 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(QUAD, units[QUAD], list));
			}
		}
	}

	if (listTypeId == LIST_HEAVYFC) {
		if (thePlayer->getHouse() == HARKONNEN || thePlayer->getHouse() == ATREIDES) {
			// upgrade to MCV
			if (techLevel >= 4 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(MCV, units[MCV], list));
			}
			// upgrade to Launcher
			if (techLevel >= 5 && currentUpgradeLevelOfList < 2) {
				return new cListUpgrade(100, 200, UPGRADE_TWO, new cBuildingListItem(LAUNCHER, units[LAUNCHER], list));
			}
			// upgrade to Siege Tank
			if (techLevel >= 6 && currentUpgradeLevelOfList < 3) {
				return new cListUpgrade(100, 200, UPGRADE_THREE, new cBuildingListItem(SIEGETANK, units[SIEGETANK], list));
			}
		} else if (thePlayer->getHouse() == ORDOS) {
			// upgrade to MCV
			if (techLevel >= 4 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(MCV, units[MCV], list));
			}
			// upgrade to Siege tank
			if (techLevel >= 7 && currentUpgradeLevelOfList < 2) {
				return new cListUpgrade(100, 200, UPGRADE_TWO, new cBuildingListItem(SIEGETANK, units[SIEGETANK], list));
			}
		}
	}

	if (listTypeId == LIST_ORNI) {
		if (thePlayer->getHouse() != HARKONNEN) {
			// upgrade for Ornithopter
			if (techLevel >= 7 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(100, 200, UPGRADE_ONE, new cBuildingListItem(ORNITHOPTER, units[ORNITHOPTER], list));
			}
		}
	}

	if (listTypeId == LIST_INFANTRY) {
		if (thePlayer->getHouse() == HARKONNEN) {
			// upgrade for troopers
			if (techLevel >= 3 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(125, 300, UPGRADE_ONE, new cBuildingListItem(TROOPERS, units[TROOPERS], list));
			}
		} else if (thePlayer->getHouse() == ATREIDES) {
			// upgrade for infantry
			if (techLevel >= 3 && currentUpgradeLevelOfList < 1) {
				return new cListUpgrade(75, 300, UPGRADE_ONE, new cBuildingListItem(INFANTRY, units[INFANTRY], list));
			}
		} else if (thePlayer->getHouse() == ORDOS) {
			// upgrade for infantry
			if (techLevel >= 3) {
				bool hasTroopers = list->hasItemType(TROOPERS);
				bool hasInfantry = list->hasItemType(INFANTRY);
				bool hasWor = thePlayer->iStructures[WOR] > 0;
				bool hasBarracks = thePlayer->iStructures[BARRACKS] > 0;

				if (currentUpgradeLevelOfList < 2) {
					if (!hasInfantry && hasBarracks) {
						return new cListUpgrade(75, 300, UPGRADE_ONE, new cBuildingListItem(INFANTRY, units[INFANTRY], list));
					} else if (!hasTroopers && hasWor) {
						return new cListUpgrade(75, 300, UPGRADE_TWO, new cBuildingListItem(TROOPERS, units[TROOPERS], list));
					} else {
						// do nothing
					}
				}
			}
		}
	}

	return NULL;
}

bool cUpgradeUtils::isUpgradeApplicableForPlayerAndList(cPlayer *thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(thePlayer);
	assert(listTypeId >= LIST_CONSTYARD);
	assert(techLevel >= 0);
	assert(currentUpgradeLevelOfList >= 0);
	cBuildingList *list = thePlayer->getSideBar()->getList(listTypeId);
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

bool cUpgradeUtils::canPlayerPayForUpgradeForList(cPlayer *thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList) {
	assert(thePlayer);
	int credits = (int) thePlayer->credits;
	int price = getPriceToUpgradeList(thePlayer, listTypeId, techLevel, currentUpgradeLevelOfList);

	// it is not available, so we cant pay
	if (price < 0) {
		return false;
	}

	return credits >= price;
}

bool cUpgradeUtils::isMouseOverUpgradeButton(int mouseX, int mouseY) {
	return upgradeButtonGuiShape->isMouseOverShape();
}
