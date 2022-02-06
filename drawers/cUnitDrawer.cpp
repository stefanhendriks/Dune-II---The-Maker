#include "cUnitDrawer.h"

cUnitDrawer::cUnitDrawer() {
}

const cUnitDrawer& cUnitDrawer::getInstance() {
    static cUnitDrawer unitDrawer{};
    return unitDrawer;
}

void cUnitDrawer::draw(cAbstractUnit *) const {
	// int drawX = getDrawX(unit);
	// int drawY = getDrawY(unit);
}

/**
	Calculate the X drawing coordinate from a unit
**/
int cUnitDrawer::getDrawX(cAbstractUnit *) const {
//	int iCellX = unit->getCellX();
//	int iOffsetX = unit->getOffsetX();
//	return ( (( iCellX * 32 ) - (mapCamera->getX()*32)) + iOffsetX);
return 0;
}

/**
	Calculate the Y drawing coordinate from a unit
**/
int cUnitDrawer::getDrawY(cAbstractUnit *) const {
//	int iCellY = unit->getCellY();
//	int iOffsetY = unit->getOffsetY();
//	return ( (( iCellY * 32 ) - (mapCamera->getY()*32)) + iOffsetY) + 42; // 42 = the options bar height
return 0;
}
