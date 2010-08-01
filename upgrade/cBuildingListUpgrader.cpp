/*
 * cBuildingListUpgrader.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cBuildingListUpgrader::cBuildingListUpgrader(cBuildingList *theList, int theTypeOfList) {
	listToUpgrade = theList;
	typeOfList = theTypeOfList;
}

void cBuildingListUpgrader::upgradeList(int stage) {
	assert(stage >= UPGRADE_STAGE_ONE);
	assert(stage <= UPGRADE_STAGE_FOUR);
	switch (typeOfList) {
		case LIST_CONSTYARD:
			upgradeListConstYard(stage); break;
		case LIST_HEAVYFC:
			upgradeListHeavyFactory(stage); break;
		case LIST_LIGHTFC:
			upgradeListLightFactory(stage); break;
		case LIST_PALACE:
			upgradeListPalace(stage); break;
		case LIST_ORNI:
			upgradeListAirUnits(stage); break;
		case LIST_INFANTRY:
			upgradeListInfantryOrTroopers(stage); break;
		default:
			assert(false); break;
	}
}

// concrete implementations of upgrades of lists

void cBuildingListUpgrader::upgradeListConstYard(int stage) {

}

void cBuildingListUpgrader::upgradeListHeavyFactory(int stage) {

}

void cBuildingListUpgrader::upgradeListLightFactory(int stage) {

}

void cBuildingListUpgrader::upgradeListPalace(int stage) {

}

void cBuildingListUpgrader::upgradeListAirUnits(int stage) {

}

void cBuildingListUpgrader::upgradeListInfantryOrTroopers(int stage) {

}
