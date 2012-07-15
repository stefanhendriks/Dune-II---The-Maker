#ifndef CPLAYERATREIDESDIFFICULTYSETTINGS_H_
#define CPLAYERATREIDESDIFFICULTYSETTINGS_H_

#include "cPlayerDifficultySettings.h"

class cPlayerAtreidesDifficultySettings : public cPlayerDifficultySettings {

	public:
		cPlayerAtreidesDifficultySettings();
		~cPlayerAtreidesDifficultySettings();

		int getMoveSpeed(int iUnitType); // get move speed of a unit

		int getBuildSpeed(int iSpeed); // get build speed

		int getHarvestSpeed(int iSpeed); // get harvesting speed

		int getDumpSpeed(int iSpeed); // get harvester 'dumping spice in refinery' speed

		int getInflictDamage(int iDamageInflicted); // get damage inflicted by this house

};

#endif
