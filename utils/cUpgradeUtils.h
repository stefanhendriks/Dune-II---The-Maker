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

		bool canUpgradeList(cPlayer * thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList);

		int getPriceToUpgradeList(cPlayer * thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList);

		bool canPlayerPayForUpgradeForList(cPlayer *thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList);

		bool isMouseOverUpgradeButton(int mouseX, int mouseY);

		bool isUpgradeApplicableForPlayerAndList(cPlayer *thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList);

		cListUpgrade * getListUpgradeForList(cPlayer * thePlayer, int listTypeId, int techlevel, int currentUpgradeLevelOfList);

	protected:

	private:

};

#endif /* CUPGRADEUTILS_H_ */
