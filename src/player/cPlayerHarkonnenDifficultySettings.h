#ifndef CPLAYERHARKONNENDIFFICULTYSETTINGS_H_
#define CPLAYERHARKONNENDIFFICULTYSETTINGS_H_

#include "cPlayerDifficultySettings.h"

class cPlayerHarkonnenDifficultySettings : public cPlayerDifficultySettings {

	public:
		cPlayerHarkonnenDifficultySettings();
		~cPlayerHarkonnenDifficultySettings();

		int getMoveSpeed(int iUnitType); // get move speed of a unit

		int getBuildSpeed(int iSpeed); // get build speed

		int getHarvestSpeed(int iSpeed); // get harvesting speed

		int getDumpSpeed(int iSpeed); // get harvester 'dumping spice in refinery' speed

		int getInflictDamage(int iDamageInflicted); // get damage inflicted by this house

};

#endif // CPLAYERHARKONNENDIFFICULTYSETTINGS_H_
