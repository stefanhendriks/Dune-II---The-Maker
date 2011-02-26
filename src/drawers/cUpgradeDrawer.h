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

		void drawUpgradeButtonForSelectedListIfNeeded(cPlayer * thePlayer, cBuildingList * theSelectedList);
		bool shouldDrawButtonForSelectedList(cPlayer * thePlayer, cBuildingList * theSelectedList);

		void drawUpgradeButton(cPlayer * thePlayer, cBuildingList * theSelectedList);
		void drawUpgradeProgress(cPlayer * thePlayer, cBuildingList * theSelectedList);

	protected:

	private:
};

#endif /* CUPGRADEDRAWER_H_ */
