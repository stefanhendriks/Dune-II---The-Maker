#include "d2tmh.h"

cUnitUtils *cUnitUtils::instance = NULL;

cUnitUtils::cUnitUtils() {	
}


cUnitUtils *cUnitUtils::getInstance() {
	if (instance == NULL) {
		instance = new cUnitUtils();
	}

	return instance;
}

// find the first unit of type belonging to player Id.
int cUnitUtils::findUnit(int type, int iPlayerId) {
	return findUnit(type, iPlayerId, -1);
}

// find the first unit of type belonging to player Id, and is not the same Id as iIgnoreUnitId.
int cUnitUtils::findUnit(int type, int iPlayerId, int iIgnoreUnitId) {
	for (int i=0; i < MAX_UNITS; i++) {
		if (i != iIgnoreUnitId) {
			if (unit[i].isValid()) {
				if (unit[i].iPlayer == iPlayerId) {
					return i;
				}
			}
		}
	}
	return -1;
}
