#include "cUnitFactory.h"

cUnitFactory::cUnitFactory() {
}

cUnitFactory& cUnitFactory::getInstance() {
  static cUnitFactory unitFactory;
	return unitFactory;
}

cAbstractUnit *cUnitFactory::createUnit(int /*type*/) {
	// return new c<bla>Unit (extends cAbstractUnit)
	return nullptr;
}

void cUnitFactory::deleteUnit(cAbstractUnit */*unit*/) {
	// delete memory that was aquired
    /*if (unit->iType == CONSTYARD)
        delete (cConstYard *)unit;
    else
        delete unit;*/
}
