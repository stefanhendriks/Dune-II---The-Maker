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
	Shorter version, creates structure at full health.
**/
cStructure* cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer) {
	return createStructure(iCell, iStructureType, iPlayer, 100);
}

/**
	Create a structure, place it and return a reference to this created class.

	This method will return NULL when either an error occured, or the creation
	of a non-structure (ie SLAB/WALL) is done.
**/
cStructure* cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer, int iPercent) {
    int iNewId = getFreeSlot();

	// fail
    if (iNewId < 0) {
		logbook("cStructureFactory::createStructure -> No free slot available, returning NULL");
        return NULL;
    }

	// When 100% of the structure is blocked, this method is never called
	// therefore we can assume that SLAB4 can be placed always partially
	// when here.
	int result = cStructureFactory::getInstance()->getSlabStatus(iCell, iStructureType, -1);
   
	// we may not place it, GUI messed up
    if (result < -1 && iStructureType != SLAB4) {
	   logbook("cStructureFactory::createStructure -> slab status < -1, and type != SLAB4");
       return NULL;
    }

	float fPercent = iPercent;
	fPercent /=100;				// devide by 100 (to make it 0.x)

	// calculate actual health
	cHitpointCalculator *calc = new cHitpointCalculator();
	float fHealth = calc->getByPercent(structures[iStructureType].hp, fPercent);

	placeStructure(iCell, iStructureType, iPlayer);
	clearFogForStructureType(iCell, iStructureType, 2, iPlayer);

	// SLAB and WALL is not a real structure. The terrain will be manipulated
	// therefore quit here.
    if (iStructureType == SLAB1 || iStructureType == SLAB4 || iStructureType == WALL) {		
		return NULL;
	}

	cStructure *str = cStructureFactory::getInstance()->createStructureInstance(iStructureType);

	if (str == NULL) {
		return NULL; // fail
	}

	// assign to array
	structure[iNewId] = str;

    // Now set it up for location & player
    structure[iNewId]->iCell   = iCell;
    structure[iNewId]->iPlayer = iPlayer;
    structure[iNewId]->iBuildFase = 1; // prebuild
    structure[iNewId]->TIMER_prebuild = 250; // prebuild timer
    structure[iNewId]->TIMER_damage = rnd(1000)+100;
    structure[iNewId]->fConcrete = (1 - fPercent);


    // fix up power usage
    player[iPlayer].use_power += structures[iStructureType].power_drain;

    // fix up power supply
    player[iPlayer].has_power += structures[iStructureType].power_give;

    // fix up spice storage stuff
    if (iStructureType == SILO)	    player[iPlayer].max_credits += 1000;
	if (iStructureType == REFINERY)   player[iPlayer].max_credits += 1500;
	
	structure[iNewId]->iHitPoints = structures[iStructureType].hp;

    // Animation set up
    structure[iNewId]->iFrame = rnd(1); // random frame
  
    //else if (iTpe == PALACE)
      //  structure[iNewId]->iFade = rnd(5); // its a palace
    //else
      //  structure[iNewId]->iFade = 0; // no fading

   // structure[iNewId].TIMER_flag = rnd(10); // random

	// clear fog around structure
	clearFogForStructureType(iCell, str);

	// additional forces: (UNITS)
	if (iStructureType == REFINERY) {
		REINFORCE(iPlayer, HARVESTER, iCell+2, iCell+2);
	}
		

    // Use power
	powerUp(iStructureType, iPlayer);
   
    // And finally handle upgrades if any...
	if (iPlayer == 0) {
		upgradeTechTree(iPlayer, iStructureType);
	}

	// deletion of objects used
	delete calc;

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

    if (iCell == WALL) {
		map.create_spot(iCell, TERRAIN_WALL, 0);            
		// change surrounding walls here
        map.smooth_spot(iCell);

		return;
    }

}



/**
	Clear fog around structure, using a cStructure class.
**/
void cStructureFactory::clearFogForStructureType(int iCell, cStructure *str) {
	if (str == NULL) {
		return; 
	}

	clearFogForStructureType(iCell, str->iType, structures[str->iType].sight, str->iPlayer);
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


// Handle logic for upgrades and such (when building placed)
// Only applies to PLAYER 0 (human), since AI needs special
// routines
void upgradeTechTree(int iPlayer, int iStructureType) {
	
	if (iPlayer != 0) {
		logbook("tried to update tech tree for a non-human player");
		return;
	}

	logbook("upgrading tech tree");

	int iHouse = player[iPlayer].house;
	int iMission = game.iMission; // MISSION IS OK NOW, IS CORRECTLY SET AT LOADING

    // Upgrade/update lists
    if (iStructureType == WINDTRAP) {
		game.list_new_item(LIST_CONSTYARD, ICON_STR_REFINERY, structures[REFINERY].cost, REFINERY, -1);
    }

    // refinery built
    if (iStructureType == REFINERY)
    {		
		if (iMission >= 2)		
		{

			game.list_new_item(LIST_CONSTYARD, ICON_STR_LIGHTFACTORY, structures[LIGHTFACTORY].cost, LIGHTFACTORY, -1);

			if (iHouse == ATREIDES || iHouse == ORDOS || iHouse == FREMEN)
				game.list_new_item(LIST_CONSTYARD, ICON_STR_BARRACKS, structures[BARRACKS].cost, BARRACKS, -1);
				
			if (iHouse == HARKONNEN || iHouse == SARDAUKAR || iHouse == FREMEN || iHouse == MERCENARY)
				game.list_new_item(LIST_CONSTYARD, ICON_STR_WOR, structures[WOR].cost, WOR, -1);				
		}

		if (iMission >= 3)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_RADAR, structures[RADAR].cost, RADAR, -1);			
        
		
		// add silo
		game.list_new_item(LIST_CONSTYARD, ICON_STR_SILO, structures[SILO].cost, SILO, -1);					
        
    }

    // Lightfactory built
    if (iStructureType == RADAR)
    {
		if (iMission >= 5)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_TURRET, structures[TURRET].cost, TURRET, -1);	

		if (iMission >= 8)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_PALACE, structures[PALACE].cost, PALACE, -1);
    }
    
    // Lightfactory built
    if (iStructureType == LIGHTFACTORY)
    {
		if (iMission >=4)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_HEAVYFACTORY, structures[HEAVYFACTORY].cost, HEAVYFACTORY, -1);

		if (iMission >=5)
		{
			game.list_new_item(LIST_CONSTYARD, ICON_STR_HIGHTECH, structures[HIGHTECH].cost, HIGHTECH, -1);
			game.list_new_item(LIST_CONSTYARD, ICON_STR_REPAIR, structures[REPAIR].cost, REPAIR, -1);
		}

		if (iMission >= 6)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_STARPORT, structures[STARPORT].cost, STARPORT, -1);
			
    }

    // Heavyfactory
    if (iStructureType == HEAVYFACTORY)
    {
		if (iMission >= 7)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_IX, structures[IX].cost, IX, -1);			

		//if (iMission >= 5)
		//	game.list_new_item(LIST_CONSTYARD, ICON_STR_REPAIR, structures[REPAIR].cost, REPAIR, -1);			
    }
}