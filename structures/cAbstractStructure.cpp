/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "../d2tmh.h"

// "default" Constructor
cAbstractStructure::cAbstractStructure() {
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

cPlayer * cAbstractStructure::getPlayer() {
	assert(iPlayer >= HUMAN);
	assert(iPlayer < MAX_PLAYERS);
	return &player[iPlayer];
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

	if (getType() == SILO)
		player[iPlayer].max_credits -= 1000;

	if (getType() == REFINERY)
		player[iPlayer].max_credits -= 1500;


    // killed
	// TODO: update statistics
//    player[iPlayer].iLost[INDEX_KILLS_STRUCTURES]++;

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

			map.cell[iCll].type = TERRAIN_ROCK;
			map.smooth_cell(iCll);

			PARTICLE_CREATE(iDrawX() + (mapCamera->getX()*32) + (w*32) + 16,
				iDrawY() + (mapCamera->getY()*32) + (h*32) + 16, OBJECT_BOOM01, -1, -1);


            for (int i=0; i < 3; i++)
            {
				map.smudge_increase(SMUDGE_ROCK, iCll);

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
    map.remove_id(iIndex, MAPID_STRUCTURES);

    // screen shaking
    game.TIMER_shake=(iWidth*iHeight)*20;

    // eventually die
    delete this;
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

			if (map.occupied(cll) == false) {
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

void cAbstractStructure::setWidth(int width) {
	assert(width > 0);
	assert(width < 10);
	iWidth = width;
}

void cAbstractStructure::setHeight(int height) {
	assert(height > 0);
	assert(height < 10);
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

	// do not die right here; it will cause havoc. Instead do that in the think method.
}

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
    if (iPlayer > 0) aiplayer[iPlayer].think_repair_structure(this);

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

/**
	Basic drawing, used for every structure. When not animating that is.
**/
void cAbstractStructure::draw(int iStage) {

	// always select proper palette (of owner)
    select_palette(player[getOwner()].pal);

	// when stage is <= 1 the building is just being placed. The prebuild
	// animation should be be drawn or, the normal drawing is shown (ie the
	// structure is not in action, like deploying harvester etc).

	// when stage == 2, it means only to draw the repair animation above the structure
	// this is done after all the structures have been drawn with stage 1 or lower. Causing
	// the repair icons to always overlap other structures. This is ugly, the repair icons
	// should be 'particles' (like smoke etc) instead of being hacked here!

	if (iStage <= 1) {
        int iSourceY = structures[getType()].bmp_height * iFrame;
		int iDrawPreBuild=-1;


        // prebuild
        if (iBuildFase == 1 ||
            iBuildFase == 3 ||
            iBuildFase == 5 ||
            iBuildFase == 7 ||
            iBuildFase == 9) {

            // determine what kind of prebuild picture should be used.
			if (iWidth == 1 && iHeight == 1) {
				iDrawPreBuild = BUILD_PRE_1X1;
			}

			if (iWidth == 2 && iHeight == 2) {
                iDrawPreBuild = BUILD_PRE_2X2;
			}

			if (iWidth == 3 && iHeight == 2) {
                iDrawPreBuild = BUILD_PRE_3X2;
			}

			if (iWidth == 3 && iHeight == 3) {
                iDrawPreBuild = BUILD_PRE_3X3;
			}
        }

        // if no prebuild picture is selected, than we should draw the building
		// itself. The reason why the above if uses only buildfase 1, 3, 5, 7 and 9 means
		// that it will cause the switching between pre-build/building state as if the
		// building is being 'readied' after placement.
        if (iDrawPreBuild < 0) {
            // Fix this up, since NEMA now posted a structure which somehow needs transculency
            // and does not work. Sloppy work Stefan! Fixed @ 13-04-2005

            BITMAP *temp=create_bitmap_ex(8, structures[getType()].bmp_width, structures[getType()].bmp_height);
            BITMAP *temp_shadow=create_bitmap(structures[getType()].bmp_width, structures[getType()].bmp_height);

            // Only for Construction Yard
            clear(temp);

            clear_to_color(temp_shadow, makecol(255,0,255));
            blit(structures[getType()].bmp, temp, 0, iSourceY, 0, 0, structures[getType()].bmp_width, structures[getType()].bmp_height);

            // in case shadow, prepare shadow bitmap in memory
			if (structures[getType()].shadow) {
                blit(structures[getType()].shadow, temp_shadow, 0, iSourceY, 0, 0, structures[getType()].bmp_width, structures[getType()].bmp_height);
			}

			// draw normal structure
            draw_sprite(bmp_screen, temp, iDrawX(), iDrawY());

            // in case shadow, draw shadow now using fBlend.
            if (structures[getType()].shadow) {
                //set_trans_blender(0,0,0,128);
                fblend_trans(temp_shadow, bmp_screen, iDrawX(), iDrawY(), 128);
                //draw_trans_sprite(bmp_screen, temp_shadow, iDrawX(), iDrawY());
            }

			// destroy used bitmaps
            destroy_bitmap(temp);
            destroy_bitmap(temp_shadow);
        } else {
            // Draw prebuild
            draw_sprite(bmp_screen, (BITMAP *)gfxdata[iDrawPreBuild].dat, iDrawX(), iDrawY());

            // Draw shadow of the prebuild animation
            if (iDrawPreBuild != BUILD_PRE_CONST)
            {
                set_trans_blender(0,0,0,128);
                draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[iDrawPreBuild+1].dat, iDrawX(), iDrawY());
            }
        }
	}
	else if (iStage == 2) {
		// TODO: create particles for this
		// now draw the repair alpha when repairing
		if (bRepair) {
			if (iRepairAlpha > -1) {
				set_trans_blender(0, 0, 0, iRepairAlpha);
				draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_REPAIR].dat, iDrawX()+iRepairX, iDrawY() + iRepairY);
			} else {
				iRepairAlpha = rnd(255);
			}
		}
	}
}
