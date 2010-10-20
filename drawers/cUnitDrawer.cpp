#include "../d2tmh.h"

cUnitDrawer *cUnitDrawer::instance = NULL;

cUnitDrawer::cUnitDrawer() {
}

cUnitDrawer *cUnitDrawer::getInstance() {
	if (instance == NULL) {
		instance = new cUnitDrawer();
	}

	return instance;
}

void cUnitDrawer::draw(cAbstractUnit *unit) {
	int drawX = getDrawX(unit);
	int drawY = getDrawY(unit);
}

/**
	Calculate the X drawing coordinate from a unit
**/
int cUnitDrawer::getDrawX(cAbstractUnit *unit) {
	int iCellX = unit->getCellX();
	int iOffsetX = unit->getOffsetX();
	return ( (( iCellX * 32 ) - (mapCamera->getX()*32)) + iOffsetX);
}

/**
	Calculate the Y drawing coordinate from a unit
**/
int cUnitDrawer::getDrawY(cAbstractUnit *unit) {
	int iCellY = unit->getCellY();
	int iOffsetY = unit->getOffsetY();
	return ( (( iCellY * 32 ) - (mapCamera->getY()*32)) + iOffsetY) + 42; // 42 = the options bar height
}
