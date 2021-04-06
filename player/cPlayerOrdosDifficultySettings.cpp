#include "../include/d2tmh.h"


cPlayerOrdosDifficultySettings::cPlayerOrdosDifficultySettings() {

}

cPlayerOrdosDifficultySettings::~cPlayerOrdosDifficultySettings() {

}

float cPlayerOrdosDifficultySettings::getBuildSpeed(int iSpeed) {
	return (int) (iSpeed * 0.8f);
}

float cPlayerOrdosDifficultySettings::getDumpSpeed(int iSpeed) {
	return (int) (iSpeed * 0.8f);
}

float cPlayerOrdosDifficultySettings::getHarvestSpeed(int iSpeed) {
	return (int) (iSpeed * 0.8f);
}

float cPlayerOrdosDifficultySettings::getInflictDamage(int iDamageInflicted) {
	return (iDamageInflicted * 0.8f);
}

float cPlayerOrdosDifficultySettings::getMoveSpeed(int iUnitType, int terrainSlowDown) {
    if (iUnitType == SABOTEUR) {
        return units[iUnitType].speed + ((float)terrainSlowDown * 0.3f); // slow downs are much less influential for saboteur
    }

	return (int)((units[iUnitType].speed + terrainSlowDown) * 0.95f);
}


