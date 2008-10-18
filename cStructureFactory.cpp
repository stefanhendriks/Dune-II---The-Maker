#include "d2tmh.h"

cStructureFactory::cStructureFactory() {
	instance = NULL;
}

cStructureFactory *cStructureFactory::getInstance() {
	if (instance == NULL) {
		instance = new cStructureFactory();
	}

	return instance;
}

cStructure *cStructureFactory::createStructure(int type) {
	 // Depending on type, create proper derived class. The constructor
    // will take care of the rest
    if (type == CONSTYARD)		return new cConstYard;
    if (type == STARPORT)		return new cStarPort;
    if (type == WINDTRAP)		return new cWindTrap;
    if (type == SILO)			return new cSpiceSilo;
    if (type == REFINERY)		return new cRefinery;
    if (type == RADAR)			return new cOutPost;
    if (type == PALACE)			return new cPalace;
    if (type == HIGHTECH)		return new cHighTech;
    if (type == LIGHTFACTORY)      return new cLightFactory;
    if (type == HEAVYFACTORY)      return new cHeavyFactory;
    if (type == TURRET)      return new cGunTurret;
    if (type == RTURRET)      return new cRocketTurret;
    if (type == REPAIR)      return new cRepairFacility;
    if (type == IX)      return new cIx;
    if (type == WOR)      return new cWor;
    if (type == BARRACKS)      return new cBarracks;
	return NULL;
}

void cStructureFactory::deleteStructure(cStructure *structure) {
	// delete memory that was aquired
    if (structure->iType == CONSTYARD) 
        delete (cConstYard *)structure;
    else if (structure->iType == STARPORT)
        delete (cStarPort *)structure;
    else if (structure->iType == WINDTRAP)
        delete (cWindTrap *)structure;
    else if (structure->iType == SILO)
        delete (cSpiceSilo *)structure;
    else if (structure->iType == RADAR)
        delete (cOutPost *)structure;
    else if (structure->iType == HIGHTECH)
        delete (cHighTech *)structure;
    else if (structure->iType == LIGHTFACTORY)
        delete (cLightFactory *)structure;
    else if (structure->iType == HEAVYFACTORY)
        delete (cHeavyFactory *)structure;
    else if (structure->iType == PALACE)
        delete (cPalace *)structure;
    else if (structure->iType == TURRET)
        delete (cGunTurret *)structure;
    else if (structure->iType == RTURRET)
        delete (cRocketTurret *)structure;
    else if (structure->iType == REPAIR)
        delete (cRepairFacility *)structure;
    else if (structure->iType == BARRACKS)
        delete (cBarracks *)structure;
    else if (structure->iType == WOR)
        delete (cWor *)structure;
    else if (structure->iType == IX)
        delete (cIx *)structure;
    else
        delete structure;
}