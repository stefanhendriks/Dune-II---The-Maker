#include "../include/d2tmh.h"

UnitDrawer *UnitDrawer::instance = NULL;

UnitDrawer::UnitDrawer() {
}

UnitDrawer *UnitDrawer::getInstance() {
	if (instance == NULL) {
		instance = new UnitDrawer();
	}

	return instance;
}

void UnitDrawer::draw(cAbstractUnit *unit) {
//	int drawX = getDrawX(unit);
//	int drawY = getDrawY(unit);
}

/**
 Calculate the X drawing coordinate from a unit
 **/
int UnitDrawer::getDrawX(cAbstractUnit *unit) {
	int iCellX = unit->getCellX();
	int iOffsetX = unit->getOffsetX();
	return (((iCellX * 32) - (mapCamera->getX() * 32)) + iOffsetX);
}

/**
 Calculate the Y drawing coordinate from a unit
 **/
int UnitDrawer::getDrawY(cAbstractUnit *unit) {
	int iCellY = unit->getCellY();
	int iOffsetY = unit->getOffsetY();
	return (((iCellY * 32) - (mapCamera->getY() * 32)) + iOffsetY) + 42; // 42 = the options bar height
}
