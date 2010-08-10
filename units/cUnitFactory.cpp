#include "../d2tmh.h"

cUnitFactory *cUnitFactory::instance = NULL;

cUnitFactory::cUnitFactory() {
}


cUnitFactory *cUnitFactory::getInstance() {
	if (instance == NULL) {
		instance = new cUnitFactory();
	}

	return instance;
}

cAbstractUnit *cUnitFactory::createUnit(int type) {
	// return new c<bla>Unit (extends cAbstractUnit)
	return NULL;
}



void cUnitFactory::deleteUnit(cAbstractUnit *unit) {
	// delete memory that was aquired
    /*if (unit->iType == CONSTYARD)
        delete (cConstYard *)unit;
    else
        delete unit;*/
}
