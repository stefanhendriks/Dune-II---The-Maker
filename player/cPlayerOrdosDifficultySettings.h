#ifndef CPLAYERORDOSDIFFICULTYSETTINGS_H_
#define CPLAYERORDOSDIFFICULTYSETTINGS_H_

#include "cPlayerDifficultySettings.h"

class cPlayerOrdosDifficultySettings : public cPlayerDifficultySettings {

public:
	cPlayerOrdosDifficultySettings();
	~cPlayerOrdosDifficultySettings();

	float getMoveSpeed(int iUnitType, int terrainSlowDown);	// get move speed of a unit

	float getBuildSpeed(int iSpeed);		// get build speed

	float getHarvestSpeed(int iSpeed);	// get harvesting speed

	float getDumpSpeed(int iSpeed);		// get harvester 'dumping spice in refinery' speed

	float getInflictDamage(int iDamageInflicted);	// get damage inflicted by this house

};

#endif
