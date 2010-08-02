#include "..\d2tmh.h"

cPlayerOrdosDifficultySettings::cPlayerOrdosDifficultySettings() {

}

cPlayerOrdosDifficultySettings::~cPlayerOrdosDifficultySettings() {

}

int cPlayerOrdosDifficultySettings::getBuildSpeed(int iSpeed) {
	return (int) (iSpeed * 0.8f);
}

int cPlayerOrdosDifficultySettings::getDumpSpeed(int iSpeed) {
	return (int) (iSpeed * 0.8f);
}

int cPlayerOrdosDifficultySettings::getHarvestSpeed(int iSpeed) {
	return (int) (iSpeed * 0.8f);
}

int cPlayerOrdosDifficultySettings::getInflictDamage(int iDamageInflicted) {
	return (iDamageInflicted * 0.8f);
}

int cPlayerOrdosDifficultySettings::getMoveSpeed(int iUnitType) {
	return (int)(units[iUnitType].speed);
}


