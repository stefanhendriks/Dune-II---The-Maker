#include "../include/d2tmh.h"

cPlayerHarkonnenDifficultySettings::cPlayerHarkonnenDifficultySettings() {

}

cPlayerHarkonnenDifficultySettings::~cPlayerHarkonnenDifficultySettings() {

}

float cPlayerHarkonnenDifficultySettings::getBuildSpeed(int iSpeed) {
	return (int) (iSpeed * 1.2f);
}

float cPlayerHarkonnenDifficultySettings::getDumpSpeed(int iSpeed) {
	// harkonnen dumps slower
	return (int) (iSpeed * 1.2f);
}

float cPlayerHarkonnenDifficultySettings::getHarvestSpeed(int iSpeed) {
	return (int) (iSpeed * 1.2f);
}

// damage done by Harkonnen is 20% more in overall
float cPlayerHarkonnenDifficultySettings::getInflictDamage(int iDamageInflicted) {
	return (int) (iDamageInflicted * 1.2);
}

float cPlayerHarkonnenDifficultySettings::getMoveSpeed(int iUnitType, int terrainSlowDown) {
	return (int)(units[iUnitType].speed) + ((float)terrainSlowDown * 1.1);
}


