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

		bool canUpgradeList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList);

		int getPriceToUpgradeList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList);

		bool canPlayerPayForUpgradeForList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList);

		bool isMouseOverUpgradeButton(int mouseX, int mouseY);

		bool isUpgradeApplicableForPlayerAndList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList);

		cListUpgrade * getListUpgradeForList(const cPlayer & thePlayer, int listTypeId, int techLevel, int currentUpgradeLevelOfList);

	protected:

	private:

};

#endif /* CUPGRADEUTILS_H_ */
