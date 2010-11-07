/*
 * cPlayerStatistics.cpp
 *
 *  Created on: 1-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cPlayerStatistics::cPlayerStatistics() {
	setEnemyStructuresDestroyed(0);
	setEnemyUnitsDestroyed(0);
	setSpiceMined(0);
	setStructuresConstructed(0);
	setStructuresDestroyed(0);
	setSuperWeaponsUsed(0);
	setUnitsConstructed(0);
	setUnitsEaten(0);
}

cPlayerStatistics::~cPlayerStatistics() {
	setEnemyStructuresDestroyed(0);
	setEnemyUnitsDestroyed(0);
	setSpiceMined(0);
	setStructuresConstructed(0);
	setStructuresDestroyed(0);
	setSuperWeaponsUsed(0);
	setUnitsConstructed(0);
	setUnitsEaten(0);
}
