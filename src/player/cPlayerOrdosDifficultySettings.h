#ifndef CPLAYERORDOSDIFFICULTYSETTINGS_H_
#define CPLAYERORDOSDIFFICULTYSETTINGS_H_

#include "cPlayerDifficultySettings.h"

class cPlayerOrdosDifficultySettings : public cPlayerDifficultySettings {

public:
	cPlayerOrdosDifficultySettings();
	~cPlayerOrdosDifficultySettings();

	int getMoveSpeed(int iUnitType);	// get move speed of a unit

	int getBuildSpeed(int iSpeed);		// get build speed

	int getHarvestSpeed(int iSpeed);	// get harvesting speed

	int getDumpSpeed(int iSpeed);		// get harvester 'dumping spice in refinery' speed

	int getInflictDamage(int iDamageInflicted);	// get damage inflicted by this house

};

#endif
