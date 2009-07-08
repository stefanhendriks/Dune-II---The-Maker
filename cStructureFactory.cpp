#include "d2tmh.h"

cStructureFactory *cStructureFactory::instance = NULL;

cStructureFactory::cStructureFactory() {	
}

cStructureFactory *cStructureFactory::getInstance() {
	if (instance == NULL) {
		instance = new cStructureFactory();
	}

	return instance;
}
 
cStructure *cStructureFactory::createStructureInstance(int type) {
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

void cStructureFactory::deleteStructureInstance(cStructure *structure) {
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