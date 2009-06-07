/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"

// "default" Constructor
cStructure::cStructure()
{
    iHitPoints=-1;      // default = no hitpoints
    iCell=-1;

    fConcrete=0.0f;

    iPlayer=-1;
    
    iFrame=-1;    

    bRepair=false;
    iRepairX=0;
    iRepairY=0;
    iRepairAlpha=255;
    
    bAnimate=false;
    
    iRallyPoint=-1;
    
    iBuildFase=-1;

    iType=-1;               // Type of structure (defined by derived class constructor)

    iUnitID=-1;

    iWidth=-1;
    iHeight=-1;

    // TIMERS
    TIMER_repair=-1;
    TIMER_repairanimation=-1;
    TIMER_flag=-1;
    TIMER_fade=-1;     

    TIMER_damage=0;   // damaging stuff
    TIMER_prebuild=0;
}

cStructure::~cStructure()
{
    // destructor
}

// X drawing position
int cStructure::iDrawX()
{
  return ( (( iCellGiveX(iCell) * 32 ) - (map.scroll_x*32)));
}

// Y drawing position
int cStructure::iDrawY()
{
  return (( (( iCellGiveY(iCell) * 32 ) - (map.scroll_y*32)))+42);
}


// this structure dies
void cStructure::die()
{
    // find myself and set to zero
    int iIndex=-1;
    for (int i=0; i < MAX_STRUCTURES; i++)
        if (structure[i] == this)
        {
            iIndex=i;
            break;
        }

    if (iIndex < 0)
    {
        logbook("cStructure(): Could not die");
        return;
    }

    // selected structure
    if (game.selected_structure == iIndex)
        game.selected_structure = -1;

    structure[iIndex]=NULL;

    // Destroy structure, take stuff in effect for the player    
    player[iPlayer].iStructures[iType]--; // remove from player building indexes

    // fix up power usage
    player[iPlayer].use_power -= structures[iType].power_drain;
    
    // less power
    player[iPlayer].has_power -= structures[iType].power_give;

	if (iType == SILO)
		player[iPlayer].max_credits -= 1000;

	if (iType == REFINERY)
		player[iPlayer].max_credits -= 1500;

    
    // killed
    player[iPlayer].iLost[INDEX_KILLS_STRUCTURES]++;

    // UnitID > -1, means the unit inside will die too
    if (iUnitID > -1)
        unit[iUnitID].init(iUnitID); // die here... softly

	int iCll=iCell;
	int iCX=iCellGiveX(iCll);
	int iCY=iCellGiveY(iCll);

    // create destroy particles
    for (int w=0; w < iWidth; w++)
    {
        for (int h=0; h < iHeight; h++)
        {
			iCll=iCellMake(iCX+w, iCY+h);

			map.cell[iCll].type = TERRAIN_ROCK;
			map.smooth_cell(iCll);
			
			PARTICLE_CREATE(iDrawX() + (map.scroll_x*32) + (w*32) + 16, 
				iDrawY() + (map.scroll_y*32) + (h*32) + 16, OBJECT_BOOM01, -1, -1);


            for (int i=0; i < 3; i++)
            {				
				map.smudge_increase(SMUDGE_ROCK, iCll);

                // create particle
                PARTICLE_CREATE(iDrawX() + (map.scroll_x*32) + (w*32) + 16 + (-8 + rnd(16)), 
                                iDrawY() + (map.scroll_y*32) + (h*32) + 16 + (-8 + rnd(16)), EXPLOSION_STRUCTURE01 + rnd(2), -1, -1);

                // create smoke
                if (rnd(100) < 10)
                    PARTICLE_CREATE(iDrawX() + (map.scroll_x*32) + (w*32) + 16 + (-8 + rnd(16)), 
                                    iDrawY() + (map.scroll_y*32) + (h*32) + 16 + (-8 + rnd(16)), OBJECT_SMOKE, -1, -1);

                // create fire
                if (rnd(100) < 5)
                    PARTICLE_CREATE(iDrawX() + (map.scroll_x*32) + (w*32) + 16 + (-8 + rnd(16)), 
                                    iDrawY() + (map.scroll_y*32) + (h*32) + 16 + (-8 + rnd(16)), EXPLOSION_FIRE, -1, -1);

            }
        }
    }

    // play sound
    play_sound_id(SOUND_CRUMBLE01 + rnd(2), iCellOnScreen(iCell));

    // remove from the playground
    map.remove_id(iIndex, MAPID_STRUCTURES);

    // screen shaking
    game.TIMER_shake=(iWidth*iHeight)*20;

    // eventually die
    delete this;
}


void cStructure::think_prebuild()
{
        // not yet done prebuilding
        // Buildfase 1, 3, 5, 7, 9 are all 'prebuilds'
        TIMER_prebuild--;

        if (TIMER_prebuild < 0)
        {
            iBuildFase++;

            TIMER_prebuild = (240 / iBuildFase);
        }    
}

// Free around structure
int cStructure::iFreeAround()
{

	//int iType = structure[iStructure].iType;

    //int w = structures[iType].bmp_width/32;
    //int h = structures[iType].bmp_height/32;

	int iStartX = iCellGiveX(iCell);
	int iStartY = iCellGiveY(iCell);

	int iEndX = (iStartX + iWidth) + 1;
	int iEndY = (iStartY + iHeight) + 1;

	iStartX--;
	iStartY--;

	int iCx=0;
	int iCy=0;

	for (int x = iStartX; x < iEndX; x++)
		for (int y = iStartY; y < iEndY; y++)
		{
			iCx=x; 
			iCy=y;

			FIX_BORDER_POS(iCx, iCy);
			
			int cll = iCellMake(iCx, iCy);
			
            if (map.occupied(cll) == false)
				return cll;
		}

	return -1; // fail
}

void cStructure::think_guard()
{
 // filled in by derived class
}

void cStructure::think_damage()
{
    TIMER_damage--;

    if (TIMER_damage < 0)
    {
        TIMER_damage = rnd(1000)+100;

        // damage done is 5 to building, but using percentage
        float fDamage = fConcrete * 5;

        if (iHitPoints > (structures[iType].hp / 2))
            iHitPoints -= (int)fDamage;

        // AI reacting to this damage
        if (iPlayer != 0 && player[iPlayer].credits > 50)
            if (iHitPoints < ((structures[iType].hp / 4)*3))
                bRepair=true;
    }
}

void cStructure::think_repair()
{
    // REPAIRING
    if (bRepair)
    {
		if (player[iPlayer].credits > 1)
			TIMER_repair++;

		if (TIMER_repair > 7)
		{
			TIMER_repair=0;
			iHitPoints += structures[iType].fixhp;
			player[iPlayer].credits--;
		}

		// done repairing
		if (iHitPoints >= structures[iType].hp)
		{
			iHitPoints = structures[iType].hp;
			bRepair=false;
		}
	}
}

void cStructure::draw(int iStage)
{
   return;

   /*
   
		
    
		// Palace tops fade    
		if (iType == PALACE || iType == REFINERY)    
		{
        //int swap_color = houses[player[iPlayer].iHouse].swap_color;
        RGB c = player[iPlayer].pal[144 + iFade];
        set_color(239, &c);    
		}
    
		

		
   
   */

   /*
  
*/

}

int STRUCTURE_FREE_TYPE(int iPlyr, int iCll, int iTpe)
{
    if (iPlyr < 0)
        return -1;

    if (iCll < 0 || iCll >= MAX_CELLS)
        return -1;

    int iX = iCellGiveX(iCll);
    int iY = iCellGiveY(iCll);
    int iTheID=-1;
    long distance=9999;

    for (int i=0; i < MAX_STRUCTURES; i++)
        if (structure[i])                     // valid
            if (structure[i]->iPlayer == iPlyr)         // same player
                if (structure[i]->iType == iTpe)     // refinery
                    if (structure[i]->iUnitID < 0)       // no unit took this
                    {                       
                        long d = ABS_length(iX, iY, iCellGiveX(structure[i]->iCell), iCellGiveY(structure[i]->iCell));

                        if (d < distance)
                        {
                            iTheID=i;
                            distance=d;
                        }
                    }

    // eventually return value
    return iTheID;
}


// Returns:
// -1 = ERROR
// 0  = PERFECT (all green)
// > 0= GOOD but it is not slabbed all (so not perfect)
int STRUCTURE_VALID_LOCATION(int iCll, int iTpe, int iUnitID)
{
    // checks if this structure can be placed on this cell
    int w = structures[iTpe].bmp_width/32;
    int h = structures[iTpe].bmp_height/32;

    
    int slabs=0;
    int total=w*h;
    int x = iCellGiveX(iCll);
    int y = iCellGiveY(iCll);
    
    for (int cx=0; cx < w; cx++)
        for (int cy=0; cy < h; cy++)
        {
            int cll=iCellMake(cx+x, cy+y);
            
            if (map.cell[cll].id[MAPID_STRUCTURES] > -1)
            {                
                return -2;
            }
            
            if (map.cell[cll].id[MAPID_UNITS] > -1)
            {
                if (iUnitID > -1)
                {
                    if (map.cell[cll].id[MAPID_UNITS] == iUnitID)
                    {
                        // ok! Same unit
                    }
                    else
                        return -2;
                }
                else
                    return -2;            
            }

            if (map.cell[cll].type != TERRAIN_ROCK &&
                map.cell[cll].type != TERRAIN_SLAB)
            {
                logbook("euh?");
                return -2; // cannot place on sand
            }


            // still valid, check if its slabbed
            if (map.cell[cll].type == TERRAIN_SLAB)
                slabs++;
        }


    if (slabs >= total)
        return -1; // perfect

    return slabs;
}

// return new valid ID
int STRUCTURE_NEW() {
	for (int i=0; i < MAX_STRUCTURES; i++) {
        if (structure[i] == NULL) return i;
	}

    return -1; // NONE
}


// Handle power usage (on creation)
void STRUCTURE_POWER(int iID, int iTpe, int iPlyer)
{
	
    // fix up power usage
    player[iPlyer].use_power += structures[iTpe].power_drain;

    // fix up power supply
    player[iPlyer].has_power += structures[iTpe].power_give;

	if (DEBUGGING)
	{
	char msg[255];
	sprintf(msg, "Drain=%d, Give=%d - PLAYER USE =%d, PLAYER HAS = %d", structures[iTpe].power_drain,structures[iTpe].power_give, player[iPlyer].use_power, player[iPlyer].has_power);
	logbook(msg);
	}

	if (iTpe == SILO)
		player[iPlyer].max_credits += 1000;

	if (iTpe == REFINERY)
		player[iPlyer].max_credits += 1500;


}

// Handle power usage on destruction
void STRUCTURE_DEPOWER(int iID, int iTpe, int iPlyer)
{
    // fix up power usage
    player[iPlyer].use_power -= structures[iTpe].power_drain;
    
    // less power
    player[iPlyer].has_power -= structures[iTpe].power_give;

	if (iTpe == SILO)
		player[iPlyer].max_credits -= 1000;

	if (iTpe == REFINERY)
		player[iPlyer].max_credits -= 1500;
}

// Handle logic for upgrades and such (when building placed)
// Only applies to PLAYER 0 (human), since AI needs special
// routines
void STRUCTURE_UPGRADE(int iID, int iTpe)
{
	
	int iHouse = player[structure[iID]->iPlayer].house;
	int iMission = game.iMission; // MISSION IS OK NOW, IS CORRECTLY SET AT LOADING

    // Upgrade/update lists
    if (iTpe == WINDTRAP)
    {
		game.list_new_item(LIST_CONSTYARD, ICON_STR_REFINERY, structures[REFINERY].cost, REFINERY, -1);
    }

    // refinery built
    if (iTpe == REFINERY)
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
    if (iTpe == RADAR)
    {
		if (iMission >= 5)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_TURRET, structures[TURRET].cost, TURRET, -1);	

		if (iMission >= 8)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_PALACE, structures[PALACE].cost, PALACE, -1);
    }
    
    // Lightfactory built
    if (iTpe == LIGHTFACTORY)
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
    if (iTpe == HEAVYFACTORY)
    {
		if (iMission >= 7)
			game.list_new_item(LIST_CONSTYARD, ICON_STR_IX, structures[IX].cost, IX, -1);			

		//if (iMission >= 5)
		//	game.list_new_item(LIST_CONSTYARD, ICON_STR_REPAIR, structures[REPAIR].cost, REPAIR, -1);			
    }
}

// Creates a new structure
int STRUCTURE_CREATE(int iCll, int iTpe, int iHP, int iPlyer)
{
    int iNewId = STRUCTURE_NEW();

    if (iNewId < 0)
    {
        return -1;
    }

    // Now check if we may place this structure
    int result = STRUCTURE_VALID_LOCATION(iCll, iTpe, -1);
   
        if (result < -1 && iTpe != SLAB4)
        {
            if (iPlyer == 0)
            {
				if (DEBUGGING)
				{
                char msg[256];
                sprintf(msg, "Cannot place %s on specified area", structures[iTpe].name);
				logbook(msg);
				}
		    }

           return -1;
        }

    // special type of structures:
    // SLABS
    // WALLS
    if (iTpe == SLAB1 ||
        iTpe == SLAB4 ||
        iTpe == WALL)
    {
		// last fase is to add this structure to the array of the player
		player[iPlyer].iStructures[iTpe]++;

        if (iTpe == SLAB1)
        {
            map.create_spot(iCll, TERRAIN_SLAB, 0);
        }
        

        if (iTpe == SLAB4)
        {
            if (map.occupied(iCll) == false)	
                if (map.cell[iCll].type == TERRAIN_ROCK)
                    map.create_spot(iCll, TERRAIN_SLAB, 0);

			if (map.occupied(iCll+1) == false)    
                if (map.cell[iCll+1].type == TERRAIN_ROCK)
                    map.create_spot(iCll+1, TERRAIN_SLAB, 0);

			if (map.occupied(iCll+MAP_W_MAX) == false)    
                if (map.cell[iCll+MAP_W_MAX].type == TERRAIN_ROCK)
                    map.create_spot(iCll+MAP_W_MAX, TERRAIN_SLAB, 0);

			if (map.occupied(iCll+MAP_W_MAX+1) == false)    
                if (map.cell[iCll+MAP_W_MAX+1].type == TERRAIN_ROCK)
                    map.create_spot(iCll+MAP_W_MAX+1, TERRAIN_SLAB, 0);
          
            map.clear_spot(iCll+1, 2, 0);
            map.clear_spot(iCll+MAP_W_MAX, 2, 0);
            map.clear_spot(iCll+MAP_W_MAX+1, 2, 0);
        }

        if (iTpe == WALL)
        {
			map.create_spot(iCll, TERRAIN_WALL, 0);            
            map.smooth_spot(iCll);
        }

        map.clear_spot(iCll, 2, 0);
        

        return -1;
    }
    
	structure[iNewId] = cStructureFactory::getInstance()->createStructure(iTpe);

    // Double check
    if (structure[iNewId] == NULL) {
        logbook("FAILED: Could not create structure (class missing to create)");
        return -1;
    }

    // Now set it up for location & player
    structure[iNewId]->iCell   = iCll;
    structure[iNewId]->iPlayer = iPlyer;
    structure[iNewId]->iBuildFase = 1; // prebuild
    structure[iNewId]->TIMER_prebuild = 250; // prebuild timer
    structure[iNewId]->TIMER_damage = rnd(1000)+100;

    // fix up power usage
    player[iPlyer].use_power += structures[iTpe].power_drain;

    // fix up power supply
    player[iPlyer].has_power += structures[iTpe].power_give;

    // fix up spice storage stuff
    if (iTpe == SILO)	    player[iPlyer].max_credits += 1000;
	if (iTpe == REFINERY)   player[iPlyer].max_credits += 1500;

    
	if (iHP < 0)
		structure[iNewId]->iHitPoints = structures[iTpe].hp;
	else
		structure[iNewId]->iHitPoints = iHP;

    // Animation set up
    structure[iNewId]->iFrame = rnd(1); // random frame
  
    //else if (iTpe == PALACE)
      //  structure[iNewId]->iFade = rnd(5); // its a palace
    //else
      //  structure[iNewId]->iFade = 0; // no fading

   // structure[iNewId].TIMER_flag = rnd(10); // random


    // Clear shroud now:
    int iSize = structures[iTpe].sight;

    for (int w=0; w < structure[iNewId]->iWidth; w++)
        for (int h=0; h < structure[iNewId]->iHeight; h++)
        {
            int sx=iCellGiveX(iCll);
            int sy=iCellGiveY(iCll);

            sx+=w;
            sy+=h;
            
            map.clear_spot(iCellMake(sx, sy), iSize, iPlyer);        

			// when AI , place slabs underneath
			if (iPlyer != 0 && iTpe != CONSTYARD)
				map.create_spot(iCellMake(sx, sy), TERRAIN_SLAB, 0);
        }
		
	// additional forces: (UNITS)
	if (iTpe == REFINERY)
		REINFORCE(iPlyer, HARVESTER, iCll+2, iCll+2);
		

    // Use power
    STRUCTURE_POWER(iNewId, iTpe, iPlyer);

    // And finally handle upgrades if any...
    if (iPlyer == 0)
        STRUCTURE_UPGRADE(iNewId, iTpe);
    

	// last fase is to add this structure to the array of the player
	player[iPlyer].iStructures[iTpe]++;

    return iNewId;
}


