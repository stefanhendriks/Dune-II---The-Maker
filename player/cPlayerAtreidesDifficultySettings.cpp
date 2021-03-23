#include "../include/d2tmh.h"

cPlayerAtreidesDifficultySettings::cPlayerAtreidesDifficultySettings() {

}

cPlayerAtreidesDifficultySettings::~cPlayerAtreidesDifficultySettings() {

}

int cPlayerAtreidesDifficultySettings::getBuildSpeed(int iSpeed) {
	return iSpeed;
}

int cPlayerAtreidesDifficultySettings::getDumpSpeed(int iSpeed) {
	return iSpeed;
}

int cPlayerAtreidesDifficultySettings::getHarvestSpeed(int iSpeed) {
	return iSpeed;
}

int cPlayerAtreidesDifficultySettings::getInflictDamage(int iDamageInflicted) {
	return iDamageInflicted;
}

int cPlayerAtreidesDifficultySettings::getMoveSpeed(int iUnitType, int terrainSlowDown) {
	return (int)(units[iUnitType].speed) + terrainSlowDown;
}


