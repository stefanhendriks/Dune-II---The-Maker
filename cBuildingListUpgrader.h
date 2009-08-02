/*
 * cBuildingListUpgrader.h
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#ifndef CBUILDINGLISTUPGRADER_H_
#define CBUILDINGLISTUPGRADER_H_

#define UPGRADE_STAGE_ONE 250		// ie, MCV
#define UPGRADE_STAGE_TWO 251		// ie ROCKET LAUNCHER
#define UPGRADE_STAGE_THREE 252		// ie, SIEGE TANK
#define UPGRADE_STAGE_FOUR 253		// ie, SPECIAL TANK (DEVASTATOR)


class cBuildingListUpgrader {
	public:
		cBuildingListUpgrader(cBuildingList *theList, int theTypeOfList);

		void upgradeList(int stage);

	private:
		int typeOfList;
		cBuildingList *listToUpgrade;

		void upgradeListConstYard(int stage);
		void upgradeListHeavyFactory(int stage);
		void upgradeListLightFactory(int stage);
		void upgradeListAirUnits(int stage);
		void upgradeListInfantryOrTroopers(int stage);
		void upgradeListPalace(int stage);
};

#endif /* CBUILDINGLISTUPGRADER_H_ */
