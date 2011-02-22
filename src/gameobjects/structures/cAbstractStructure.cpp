/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "../../include/d2tmh.h"


// "default" Constructor
cAbstractStructure::cAbstractStructure() {
    iHitPoints=-1;      // default = no hitpoints
    iCell=-1;

    armor = 1;

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

cAbstractStructure::~cAbstractStructure()
{
    // destructor
}

// X drawing position
int cAbstractStructure::iDrawX()
{
  return ( (( iCellGiveX(iCell) * 32 ) - (mapCamera->getX()*32)));
}

// Y drawing position
int cAbstractStructure::iDrawY()
{
  return (( (( iCellGiveY(iCell) * 32 ) - (mapCamera->getY()*32)))+42);
}

BITMAP * cAbstractStructure::getBitmap() {
	s_Structures structureType = getS_StructuresType();
	return structureType.bmp;
}

BITMAP * cAbstractStructure::getShadowBitmap() {
	s_Structures structureType = getS_StructuresType();
	return structureType.shadow;
}

cPlayer * cAbstractStructure::getPlayer() {
	assert(iPlayer >= HUMAN);
	assert(iPlayer < MAX_PLAYERS);
	return &player[iPlayer];
}

int cAbstractStructure::getMaxHP() {
	int type = getType();
	return structures[type].hp;
}

int cAbstractStructure::getSight() {
	int type = getType();
	return structures[type].sight;
}

int cAbstractStructure::getRange() {
	int type = getType();
	return structures[type].sight;
}


// this structure dies
void cAbstractStructure::die()
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
        logbook("cAbstractStructure(): Could not die");
        return;
    }

    // selected structure
    if (game.selected_structure == iIndex) {
        game.selected_structure = -1;
    }

	// remove from array
    structure[iIndex]=NULL;

    // Destroy structure, take stuff in effect for the player
    player[iPlayer].iStructures[getType()]--; // remove from player building indexes

    // fix up power usage
    player[iPlayer].use_power -= structures[getType()].power_drain;

    // less power
    player[iPlayer].has_power -= structures[getType()].power_give;

	if (getType() == SILO) {
		player[iPlayer].max_credits -= 1000;
	}

	if (getType() == REFINERY) {
		player[iPlayer].max_credits -= 1500;
	}

    // UnitID > -1, means the unit inside will die too
    if (iUnitID > -1) {
        unit[iUnitID].init(iUnitID); // die here... softly
    }

	int iCll=iCell;
	int iCX=iCellGiveX(iCll);
	int iCY=iCellGiveY(iCll);

    // create destroy particles
    for (int w=0; w < iWidth; w++)
    {
        for (int h=0; h < iHeight; h++)
        {
			iCll=iCellMake(iCX+w, iCY+h);

			map->cell[iCll].terrainTypeGfxDataIndex = TERRAIN_ROCK;
			mapEditor.smoothAroundCell(iCll);

			PARTICLE_CREATE(iDrawX() + (mapCamera->getX()*32) + (w*32) + 16,
				iDrawY() + (mapCamera->getY()*32) + (h*32) + 16, OBJECT_BOOM01, -1, -1);


            for (int i=0; i < 3; i++)
            {
				map->smudge_increase(SMUDGE_ROCK, iCll);

                // create particle
                PARTICLE_CREATE(iDrawX() + (mapCamera->getX()*32) + (w*32) + 16 + (-8 + rnd(16)),
                                iDrawY() + (mapCamera->getY()*32) + (h*32) + 16 + (-8 + rnd(16)), EXPLOSION_STRUCTURE01 + rnd(2), -1, -1);

                // create smoke
                if (rnd(100) < 7)
                    PARTICLE_CREATE(iDrawX() + (mapCamera->getX()*32) + (w*32) + 16 + (-8 + rnd(16)),
                                    iDrawY() + (mapCamera->getY()*32) + (h*32) + 16 + (-8 + rnd(16)), OBJECT_SMOKE, -1, -1);

                // create fire
                if (rnd(100) < 5)
                    PARTICLE_CREATE(iDrawX() + (mapCamera->getX()*32) + (w*32) + 16 + (-8 + rnd(16)),
                                    iDrawY() + (mapCamera->getY()*32) + (h*32) + 16 + (-8 + rnd(16)), EXPLOSION_FIRE, -1, -1);

            }
        }
    }

    // play sound
    play_sound_id(SOUND_CRUMBLE01 + rnd(2), iCellOnScreen(iCell));

    // remove from the playground
    map->remove_id(iIndex, MAPID_STRUCTURES);

    // screen shaking
    game.TIMER_shake = (iWidth * iHeight) * 20;

    // eventually die
    cStructureFactory::getInstance()->deleteStructureInstance(this);
}


void cAbstractStructure::think_prebuild()
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
int cAbstractStructure::iFreeAround()
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

			if (map->occupied(cll) == false) {
				return cll;
			}
		}

	return -1; // fail
}

void cAbstractStructure::think_guard() {
 // filled in by derived class
}

// This method thinks about basic animation
void cAbstractStructure::think_animation() {
// show (common) prebuild animation
	if (iBuildFase < 10) {
        think_prebuild();
	}

    // Repair blink
    if (bRepair) {
		TIMER_repairanimation++;

		// when there is still enough
		if (TIMER_repairanimation > 1 &&
			player[0].credits > 2) {

			TIMER_repairanimation=0;
			// decrease alpha (make it fade out)
			iRepairAlpha -= 7;

			// when faded out completely, choose new location
			if (iRepairAlpha < 1) {
				iRepairX = -8 + rnd((structures[getType()].bmp_width + 8));
				iRepairY = -16 + rnd((structures[getType()].bmp_height + 16));
				iRepairAlpha = 255;
			} else {
				iRepairY--;
				// raise the repair icon.
			}
		}
     }
}

void cAbstractStructure::think_flag() {
	if (isAnimating()) return; // do no flag animation when animating

	TIMER_flag++;

    if (TIMER_flag > 70) {
        iFrame++;

		// switch between 0 and 1.
		if (iFrame > 1) {
            iFrame=0;
		}

        TIMER_flag=0;
    }
}

void cAbstractStructure::think_damage()
{
    TIMER_damage--;
    if (TIMER_damage < 0) {
        TIMER_damage = rnd(500)+250;

        // chance based (so it does not decay all the time)
        if (rnd(100) < getPercentageNotPaved()) {
			if (iHitPoints > (structures[getType()].hp / 2)) {
				iHitPoints -= 1;
			}
        }

        // AI reacting to this damage
        // TODO: remove here
		if (iPlayer != HUMAN && player[iPlayer].credits > 50) {
			if (iHitPoints < ((structures[getType()].hp / 4)*3)) { // lower than 75%
                bRepair=true;
			}
		}
    }
}

void cAbstractStructure::setWidth(int width) {
	assert(width > 0);
	assert(width < 4);
	iWidth = width;
}

void cAbstractStructure::setHeight(int height) {
	assert(height > 0);
	assert(height < 4);
	iHeight = height;
}

void cAbstractStructure::setRallyPoint(int cell) {
	assert(cell > -2); // -1 is allowed (means disable);
	assert(cell < MAX_CELLS);
	iRallyPoint = cell;
}

void cAbstractStructure::setAnimating(bool value) {
	bAnimate = value;
}

void cAbstractStructure::setFrame(int frame) {
	iFrame = frame;
}


/**
	Damage structure by amount of hp. The amount passed to this method
	must be > 0. When it is < 0, it will be wrapped to > 0 anyway and
	an error is written in the log.

	When the hitpoints drop below 1, the structure will die. The actual call
	to the die method will be done by the abstract think function. So it is
	still safe to refer to this structure even if it is declared dead (ie hp < 1).
**/
void cAbstractStructure::damage(int hp) {
	int damage = hp;
	if (damage < 0) {
		logbook("cAbstractStructuredamage() got negative parameter, wrapped");
		damage *= -1; // - * - = +
	}

	iHitPoints -= damage; // do damage

	// do not die here, that is not the responsibility of this method to determine
}

/**
 * Set HP of structure, caps it at its maximum.
 */
void cAbstractStructure::setHitPoints(int hp) {
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

void cAbstractStructure::setCell(int cell) {
	iCell = cell;
}

void cAbstractStructure::setOwner(int player) {
	iPlayer = player;
}

/**
	Think actions like any other structure would have.
**/
void cAbstractStructure::think() {
	// AI
    if (iPlayer > 0) {
    	aiplayer[iPlayer].think_repair_structure(this);
    }

    // Other
    think_damage();
    think_repair();

	// die when out of hp
	if (iHitPoints < 1) {
		die();
	}
}

void cAbstractStructure::think_repair()
{
    // REPAIRING
    if (bRepair) {
		if (player[iPlayer].credits > 1.0f) {
			TIMER_repair++;

			if (TIMER_repair > 7)
			{
				TIMER_repair=0;
				iHitPoints += structures[getType()].fixhp;
				player[iPlayer].credits--;
				if (player[iPlayer].credits < 0.0f) {
					player[iPlayer].credits = 0.0f;
				}
			}

			// done repairing
			if (iHitPoints >= getMaxHP()) {
				iHitPoints = getMaxHP();
				bRepair=false;
			}
		}
		assert(iHitPoints <= structures[getType()].hp);
	}
}

s_Structures cAbstractStructure::getS_StructuresType() {
	return structures[getType()];
}

int cAbstractStructure::getPercentageNotPaved() {
	return fConcrete * 100;
}

bool cAbstractStructure::isPrimary() {
	cPlayer * thePlayer = getPlayer();
	return thePlayer->iPrimaryBuilding[getType()] == id;
}

int cAbstractStructure::getPowerUsage() {
	s_Structures structure = getS_StructuresType();
	return structure.power_drain;
}
