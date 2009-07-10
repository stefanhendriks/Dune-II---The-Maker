/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

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
	for (int i=0; i < MAX_STRUCTURES; i++) {
        if (structure[i] == this)
        {
            iIndex=i;
            break;
        }
	}

    if (iIndex < 0) {
        logbook("cStructure(): Could not die");
        return;
    }

    // selected structure
    if (game.selected_structure == iIndex)
        game.selected_structure = -1;

    structure[iIndex]=NULL;

    // Destroy structure, take stuff in effect for the player    
    player[iPlayer].iStructures[getType()]--; // remove from player building indexes

    // fix up power usage
    player[iPlayer].use_power -= structures[getType()].power_drain;
    
    // less power
    player[iPlayer].has_power -= structures[getType()].power_give;

	if (getType() == SILO)
		player[iPlayer].max_credits -= 1000;

	if (getType() == REFINERY)
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
                if (rnd(100) < 7)
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

// Free around structure, return the first cell that is free.
int cStructure::iFreeAround()
{
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
			
			if (map.occupied(cll) == false) {
				return cll;
			}
		}

	return -1; // fail
}

void cStructure::think_guard() {
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

        if (iHitPoints > (structures[getType()].hp / 2))
            iHitPoints -= (int)fDamage;

        // AI reacting to this damage
		if (iPlayer != 0 && player[iPlayer].credits > 50) {
			if (iHitPoints < ((structures[getType()].hp / 4)*3)) { // lower than 75%
                bRepair=true;
				logbook("AI auto repair on decay");
			}
		}
    }
}

void cStructure::setWidth(int width) {
	iWidth = width;
}

void cStructure::setHeight(int height) {
	iHeight = height;
}

/**
	Damage structure by amount of hp. The amount passed to this method
	must be > 0. When it is < 0, it will be wrapped to > 0 anyway and 
	an error is written in the log.
**/
void cStructure::damage(int hp) {
	int damage = hp;
	if (damage < 0) {
		logbook("cStructure::damage() got negative parameter, wrapped");
		damage *= -1; // - * - = +
	}

	iHitPoints -= damage; // do damage

	// die when out of hp
	if (iHitPoints < 1) {
		die();
	}	
}

void cStructure::setHitPoints(int hp) {
	iHitPoints = hp;
	int maxHp = structures[getType()].hp;

	if (iHitPoints > maxHp) {
		char msg[256];
		sprintf(msg, "setHitpoints(%d) while max is %d; capped at max.", hp, maxHp);
		logbook(msg);

		// will fail (uncomment to let it be capped)
		assert(iHitPoints <= maxHp); // may never be more than the maximum of that structure

		iHitPoints = maxHp;
	}
	
}

void cStructure::think_repair()
{
    // REPAIRING
    if (bRepair) {
		if (player[iPlayer].credits > 1)
			TIMER_repair++;

		if (TIMER_repair > 7)
		{
			TIMER_repair=0;
			iHitPoints += structures[getType()].fixhp;
			player[iPlayer].credits--;
		}

		// done repairing
		if (iHitPoints >= structures[getType()].hp) {
			iHitPoints = structures[getType()].hp;
			bRepair=false;
		}

		assert(iHitPoints <= structures[getType()].hp);
	}
}

void cStructure::draw(int iStage) {
   return;
}

/**
	return free structure, closest to iCell of type iTpe
**/
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
                if (structure[i]->getType() == iTpe)     // refinery
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
