/*
 * cUpgradeUtils.h
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 */

#ifndef CUPGRADEUTILS_H_
#define CUPGRADEUTILS_H_

class cUpgradeUtils {
	public:
		cUpgradeUtils();
		~cUpgradeUtils();

		bool canUpgradeList(int listTypeId, int techlevel, int currentUpgradeLevelOfList);

		int getPriceToUpgradeList(int listTypeId, int techlevel, int currentUpgradeLevelOfList);

		bool canPlayerPayForUpgradeForList(cPlayer *thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList);
	protected:

	private:

};

#endif /* CUPGRADEUTILS_H_ */
