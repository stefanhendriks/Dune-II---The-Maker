#include "../d2tmh.h"

cStructureFactory *cStructureFactory::instance = NULL;

cStructureFactory::cStructureFactory() {
}

cStructureFactory *cStructureFactory::getInstance() {
	if (instance == NULL) {
		instance = new cStructureFactory();
	}

	return instance;
}

cAbstractStructure *cStructureFactory::createStructureInstance(int type) {
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

void cStructureFactory::deleteStructureInstance(cAbstractStructure *structure) {
	// delete memory that was aquired
	/*
    if (structure->getType() == CONSTYARD)
        delete (cConstYard *)structure;
    else if (structure->getType() == STARPORT)
        delete (cStarPort *)structure;
    else if (structure->getType() == WINDTRAP)
        delete (cWindTrap *)structure;
    else if (structure->getType() == SILO)
        delete (cSpiceSilo *)structure;
    else if (structure->getType() == RADAR)
        delete (cOutPost *)structure;
    else if (structure->getType() == HIGHTECH)
        delete (cHighTech *)structure;
    else if (structure->getType() == LIGHTFACTORY)
        delete (cLightFactory *)structure;
    else if (structure->getType() == HEAVYFACTORY)
        delete (cHeavyFactory *)structure;
    else if (structure->getType() == PALACE)
        delete (cPalace *)structure;
    else if (structure->getType() == TURRET)
        delete (cGunTurret *)structure;
    else if (structure->getType() == RTURRET)
        delete (cRocketTurret *)structure;
    else if (structure->getType() == REPAIR)
        delete (cRepairFacility *)structure;
    else if (structure->getType() == BARRACKS)
        delete (cBarracks *)structure;
    else if (structure->getType() == WOR)
        delete (cWor *)structure;
    else if (structure->getType() == IX)
        delete (cIx *)structure;
    else
		*/
        delete structure;
}


/**
	Shorter version, creates structure at full health.
**/
cAbstractStructure* cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer) {
	return createStructure(iCell, iStructureType, iPlayer, 100);
}

/**
	Create a structure, place it and return a reference to this created class.

	This method will return NULL when either an error occured, or the creation
	of a non-structure (ie SLAB/WALL) is done.
**/
cAbstractStructure* cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer, int iPercent) {
    int iNewId = getFreeSlot();

	assert(iPercent < 200); // percentages may not really exceed 200, above is weird behavior
	assert(iPlayer >= 0);
	assert(iPlayer <= MAX_PLAYERS);

	// fail
    if (iNewId < 0) {
		logbook("cStructureFactory::createStructure -> cannot create structure: No free slot available, returning NULL");
        return NULL;
    }

	// When 100% of the structure is blocked, this method is never called
	// therefore we can assume that SLAB4 can be placed always partially
	// when here.
	int result = getSlabStatus(iCell, iStructureType, -1);

	// we may not place it, GUI messed up
    if (result < -1 && iStructureType != SLAB4) {
		logbook("cStructureFactory::createStructure -> cannot create structure: slab status < -1, and type != SLAB4, returning NULL");
       return NULL;
    }

	float fPercent = iPercent;
	fPercent /=100;				// devide by 100 (to make it 0.x)

	// calculate actual health
	cHitpointCalculator *calc = new cHitpointCalculator();
	int hp = structures[iStructureType].hp;
	assert(hp > 0);
	float fHealth = calc->getByPercent(hp, fPercent);

	char msg2[255];
	sprintf(msg2, "Structure with id [%d] has [%d] hp , fhealth is [%d]", iStructureType, hp, fHealth);
	logbook(msg2);

	placeStructure(iCell, iStructureType, iPlayer);
	clearFogForStructureType(iCell, iStructureType, 2, iPlayer);

	// SLAB and WALL is not a real structure. The terrain will be manipulated
	// therefore quit here.
    if (iStructureType == SLAB1 || iStructureType == SLAB4 || iStructureType == WALL) {
		return NULL;
	}

	cAbstractStructure *str = createStructureInstance(iStructureType);

	if (str == NULL) {
		return NULL; // fail
	}

	// assign to array
	structure[iNewId] = str;

    // Now set it up for location & player
    str->setCell(iCell);
    str->setOwner(iPlayer);
    str->iBuildFase = 1; // prebuild
    str->TIMER_prebuild = 250; // prebuild timer
    str->TIMER_damage = rnd(1000)+100;
    str->fConcrete = (1 - fPercent);
	str->setHitPoints((int)fHealth);
    str->setFrame(rnd(1)); // random start frame (flag)
    str->setStructureId(iNewId);

    // fix up power usage
    player[iPlayer].use_power += structures[iStructureType].power_drain;

    // fix up power supply
    player[iPlayer].has_power += structures[iStructureType].power_give;

    // fix up spice storage stuff
    if (iStructureType == SILO)	    player[iPlayer].max_credits += 1000;
	if (iStructureType == REFINERY)   player[iPlayer].max_credits += 1500;

	str->setWidth(structures[str->getType()].bmp_width/32);
	str->setHeight(structures[str->getType()].bmp_height/32);


 	// clear fog around structure
	clearFogForStructureType(iCell, str);

	// additional forces: (UNITS)
	if (iStructureType == REFINERY) {
		REINFORCE(iPlayer, HARVESTER, iCell+2, iCell+2);
	}

    // Use power
	powerUp(iStructureType, iPlayer);

	// deletion of objects used
	delete (cHitpointCalculator *)calc;

    return str;
}


/**
	Actually place the structure on the map now. Assign to the array of structures.
**/
void cStructureFactory::placeStructure(int iCell, int iStructureType, int iPlayer) {
	// add this structure to the array of the player (for some score management)
	player[iPlayer].iStructures[iStructureType]++;

	if (iStructureType == SLAB1) {
		map.create_spot(iCell, TERRAIN_SLAB, 0);
		return; // done
	}

    if (iStructureType == SLAB4)   {

		if (map.occupied(iCell) == false) {
			if (map.cell[iCell].type == TERRAIN_ROCK) {
                map.create_spot(iCell, TERRAIN_SLAB, 0);
			}
		}

		if (map.occupied(iCell+1) == false) {
			if (map.cell[iCell+1].type == TERRAIN_ROCK) {
                map.create_spot(iCell+1, TERRAIN_SLAB, 0);
			}
		}

		if (map.occupied(iCell+MAP_W_MAX) == false) {
			if (map.cell[iCell+MAP_W_MAX].type == TERRAIN_ROCK) {
                map.create_spot(iCell+MAP_W_MAX, TERRAIN_SLAB, 0);
			}
		}

		if (map.occupied(iCell+MAP_W_MAX+1) == false) {
			if (map.cell[iCell+MAP_W_MAX+1].type == TERRAIN_ROCK) {
                map.create_spot(iCell+MAP_W_MAX+1, TERRAIN_SLAB, 0);
			}
		}

		return;
    }

    if (iStructureType == WALL) {
		map.create_spot(iCell, TERRAIN_WALL, 0);
		// change surrounding walls here
        map.smooth_spot(iCell);
		return;
    }

}



/**
	Clear fog around structure, using a cAbstractStructure class.
**/
void cStructureFactory::clearFogForStructureType(int iCell, cAbstractStructure *str) {
	if (str == NULL) {
		return;
	}

	clearFogForStructureType(iCell, str->getType(), structures[str->getType()].sight, str->getOwner());
}

/**
	Clear the cells of a structure on location of iCell, the size that is cleared is
	determined from the iStructureType
**/
void cStructureFactory::clearFogForStructureType(int iCell, int iStructureType, int iSight, int iPlayer) {
	int iWidth = structures[iStructureType].bmp_width / 32;;
	int iHeight = structures[iStructureType].bmp_height / 32;

	int iCellX = iCellGiveX(iCell);
	int iCellY = iCellGiveY(iCell);
	int iCellXMax = iCellX + iWidth;
	int iCellYMax = iCellY + iHeight;

	for (int x = iCellX; x < iCellXMax; x++) {
		for (int y = iCellY; y < iCellYMax; y++) {
			map.clear_spot(iCellMake(x, y), iSight, iPlayer);
		}
	}
}

/**
	return free slot in array of structures.
**/
int cStructureFactory::getFreeSlot() {
	for (int i=0; i < MAX_STRUCTURES; i++) {
		if (structure[i] == NULL) {
			return i;
		}
	}

    return -1; // NONE
}

/**
	This function will check if at iCell (the upper left corner of a structure) a structure
	can be placed of type "iStructureType". If iUnitIDTOIgnore is > -1, then if any unit is
	supposidly 'blocking' this structure from placing, it will be ignored.

	Ie, you will use the iUnitIDToIgnore value when you want to create a Const Yard on the
	location of an MCV.

	Returns:
	-2  = ERROR / Cannot be placed at this location with the params given.
	-1  = PERFECT / Can be placed, and entire structure has pavement (slabs)
	>=0 = GOOD but it is not slabbed all (so not perfect)
**/
int cStructureFactory::getSlabStatus(int iCell, int iStructureType, int iUnitIDToIgnore) {
    // checks if this structure can be placed on this cell
    int w = structures[iStructureType].bmp_width/32;
    int h = structures[iStructureType].bmp_height/32;

    int slabs=0;
    int total=w*h;
    int x = iCellGiveX(iCell);
    int y = iCellGiveY(iCell);

    for (int cx=0; cx < w; cx++)
        for (int cy=0; cy < h; cy++)
        {
            int cll=iCellMake(cx+x, cy+y); // <-- some evil global thing that calculates the cell...

			// check if terrain allows it.
            if (map.cell[cll].type != TERRAIN_ROCK &&
                map.cell[cll].type != TERRAIN_SLAB) {
				logbook("getSlabStatus will return -2, reason: terrain is not rock or slab.");
                return -2; // cannot place on sand
            }

			// another structure found on this location, return -2 meaning "blocked"
            if (map.cell[cll].id[MAPID_STRUCTURES] > -1) {
				logbook("getSlabStatus will return -2, reason: another structure found on one of the cells");
                return -2;
            }

			// unit found on location where structure wants to be placed. Check if
			// it may be ignored, if not, return -2.
            if (map.cell[cll].id[MAPID_UNITS] > -1)
            {
                if (iUnitIDToIgnore > -1)
                {
                    if (map.cell[cll].id[MAPID_UNITS] == iUnitIDToIgnore) {
                        // ok; this may be ignored.
					} else {
						// not the unit to be ignored.
						logbook("getSlabStatus will return -2, reason: unit found that blocks placement; is not ignored");
						return -2;
					}
				} else {
					// no iUnitIDToIgnore given, this means always -2
					logbook("getSlabStatus will return -2, reason: unit found that blocks placement; no id to ignore");
                    return -2;
				}
            }

            // now check if the 'terrain' type is 'slab'. If that is true, increase value of found slabs.
			if (map.cell[cll].type == TERRAIN_SLAB) {
                slabs++;
			}
        }


		// if the amount of slabs equals the amount of total slabs possible, return a perfect status.
		if (slabs >= total) {
			return -1; // perfect
		}

    return slabs; // ranges from 0 to <max slabs possible of building (ie height * width in cells)
}

void cStructureFactory::powerUp(int iStructureType, int iPlayer) {
   // fix up power usage
    player[iPlayer].use_power += structures[iStructureType].power_drain;

    // fix up power supply
    player[iPlayer].has_power += structures[iStructureType].power_give;


	// TODO: move to other methods regarding spice management?
	if (iStructureType == SILO) {
		player[iPlayer].max_credits += 1000;
	}

	if (iStructureType == REFINERY) {
		player[iPlayer].max_credits += 1500;
	}

}

void cStructureFactory::powerDown(int iStructureType, int iPlayer) {
    // fix up power usage
    player[iPlayer].use_power -= structures[iStructureType].power_drain;

    // less power
    player[iPlayer].has_power -= structures[iStructureType].power_give;

	if (iStructureType == SILO) {
		player[iPlayer].max_credits -= 1000;
	}

	if (iStructureType == REFINERY) {
		player[iPlayer].max_credits -= 1500;
	}
}


void cStructureFactory::createSlabForStructureType(int iCell, int iStructureType) {
	assert(iCell > -1);
	cCellCalculator calculator;
	int height = structures[iStructureType].bmp_height / 32;
	int width = structures[iStructureType].bmp_width / 32;

	int cellX = calculator.getX(iCell);
	int cellY = calculator.getY(iCell);

	int endCellX = cellX + width;
	int endCellY = cellY + height;
	for (int x = cellX; x < endCellX; x++) {
		for (int y = cellY; y < endCellY; y++) {
			int cell = calculator.getCell(x, y);
			map.create_spot(cell, TERRAIN_SLAB, 0);
		}
	}
}

void cStructureFactory::clearAllStructures() {
	for (int i=0; i < MAX_STRUCTURES; i++) {
		// clear out all structures
		if (structure[i]) {
			cStructureFactory::getInstance()->deleteStructureInstance(structure[i]);
		}

		// clear pointer
		structure[i] = NULL;
	}
}
