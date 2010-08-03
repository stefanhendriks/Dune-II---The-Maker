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
	}

	return -1;
}

bool cUpgradeUtils::canPlayerPayForUpgradeForList(cPlayer *thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList) {
	assert(thePlayer);
	int credits = thePlayer->credits;
	int price = getPriceToUpgradeList(listTypeId, techlevel, currentUpgradeLevelOfList);

	// it is not available, so we cant pay
	if (price < 0) {
		return false;
	}

	return credits > price;
}
