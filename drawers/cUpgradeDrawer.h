/*
 * cUpgradeDrawer.h
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 */

#ifndef CUPGRADEDRAWER_H_
#define CUPGRADEDRAWER_H_

class cUpgradeDrawer {
	public:
		cUpgradeDrawer();
		~cUpgradeDrawer();

		void drawUpgradeButtonForSelectedListIfNeeded(const cPlayer & thePlayer);
		bool shouldDrawButtonForSelectedList(const cPlayer & thePlayer, cBuildingList * theSelectedList);

		void drawUpgradeButton(const cPlayer & thePlayer, cBuildingList * theSelectedList);
		void drawUpgradeProgress(const cPlayer & thePlayer, cBuildingList * theSelectedList);


	protected:

	private:
        cUpgradeUtils upgradeUtils;

};

#endif /* CUPGRADEDRAWER_H_ */
