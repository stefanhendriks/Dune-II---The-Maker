#include "../include/d2tmh.h"

cPlayerAtreidesDifficultySettings::cPlayerAtreidesDifficultySettings() {

}

cPlayerAtreidesDifficultySettings::~cPlayerAtreidesDifficultySettings() {

}

float cPlayerAtreidesDifficultySettings::getBuildSpeed(int iSpeed) {
	return iSpeed;
}

float cPlayerAtreidesDifficultySettings::getDumpSpeed(int iSpeed) {
	return iSpeed;
}

float cPlayerAtreidesDifficultySettings::getHarvestSpeed(int iSpeed) {
	return iSpeed;
}

float cPlayerAtreidesDifficultySettings::getInflictDamage(int iDamageInflicted) {
	return iDamageInflicted;
}

float cPlayerAtreidesDifficultySettings::getMoveSpeed(int iUnitType, int terrainSlowDown) {
	return (int)(units[iUnitType].speed) + terrainSlowDown;
}


