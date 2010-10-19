#include "../d2tmh.h"

// Constructor
cRocketTurret::cRocketTurret() {

 // other variables (class specific)
 iHeadFacing=FACE_UP;        // (for turrets only) what is this structure facing at?
 iShouldHeadFacing=FACE_UP;  // where should we look face at?
 iTargetID=-1;           // target id

 TIMER_fire=0;
 TIMER_turn=0;
 TIMER_guard=0;         // timed 'area scanning'

}

int cRocketTurret::getType() {
	return RTURRET;
}

cRocketTurret::~cRocketTurret()
{

}


void cRocketTurret::think()
{
    int iMyIndex=-1;

	for (int i=0; i < MAX_STRUCTURES; i++) {
        if (structure[i] == this)
        {
            iMyIndex=i;
            break;
        }
	}

    // this should not happen, but just in case
	if (iMyIndex < 0) {
        return;
	}

	if (player[getOwner()].bEnoughPower() == false) {
        return;  // do not fire a thing now
	}

    // turning & shooting
     if (iTargetID > -1)
        {
            if (unit[iTargetID].isValid())
            {
            // first make sure we face okay!
            int iCellX=iCellGiveX(getCell());
            int iCellY=iCellGiveY(getCell());

            int iTargetX = iCellGiveX(unit[iTargetID].iCell);
            int iTargetY = iCellGiveY(unit[iTargetID].iCell);

            int d = fDegrees(iCellX, iCellY, iTargetX, iTargetY);
            int f = face_angle(d); // get the angle

            // set facing
            iShouldHeadFacing = f;


            if (iShouldHeadFacing == iHeadFacing || (unit[iTargetID].iType == ORNITHOPTER))
            {

                TIMER_fire++;

                int iDistance=9999;
                int iSlowDown=250;

                if (getType() == RTURRET)
                    iSlowDown=450;

                if (unit[iTargetID].isValid())
                {
                    // calculate distance
                    iDistance = ABS_length(iCellX, iCellY, iTargetX, iTargetY);

                    if (iDistance > structures[getType()].sight)
                        iTargetID=-1;
                }
                else
                    iTargetID=-1;

                if (iTargetID < 0)
                    return;

                if (TIMER_fire > iSlowDown)
                {
                    int iTargetCell = unit[iTargetID].iCell;

                    int iBullet = BULLET_TURRET;

                    if (getType() == RTURRET && iDistance > 3)
                        iBullet = ROCKET_RTURRET;
                    else
                    {
                        int iShootX=(iDrawX() + 16 ) + (map.scroll_x*32);
                        int iShootY=(iDrawY() + 16 ) + (map.scroll_y*32);
                        int     bmp_head            = convert_angle(iHeadFacing);

                        PARTICLE_CREATE(iShootX, iShootY, OBJECT_TANKSHOOT, -1, bmp_head);

                    }

                    int iBull = create_bullet(iBullet, getCell(), iTargetCell, -1, iMyIndex);

                    if (unit[iTargetID].iType == ORNITHOPTER)
                    {
                        // it is a homing missile!
                        bullet[iBull].iHoming = iTargetID;
                        bullet[iBull].TIMER_homing = 200;


                    }

                    TIMER_fire=0;

                }



            }
            else
            {
                TIMER_turn++;

                int iSlowDown = 200;

                if (TIMER_turn > iSlowDown)
                {
                    TIMER_turn=0;

                    int d = 1;

                    int toleft = (iHeadFacing + 8) - iShouldHeadFacing;
                    if (toleft > 7) toleft -= 8;


                    int toright = abs(toleft-8);

                    if (toright == toleft) d = -1+(rnd(2));
                    if (toleft  > toright) d = 1;
                    if (toright > toleft)  d = -1;

                    iHeadFacing += d;

                    if (iHeadFacing < 0)
                        iHeadFacing = 7;

                    if (iHeadFacing > 7)
                        iHeadFacing = 0;
                } // turning

            }

            }
            else
                iTargetID=-1;
        }

	// think like base class
	cAbstractStructure::think();

}

void cRocketTurret::think_animation() {
	cAbstractStructure::think_animation();
}

void cRocketTurret::think_guard() {

	// no power = no defense
	if (player[getOwner()].bEnoughPower() == false) {
        return;
	}

    TIMER_guard++;

    if (TIMER_guard > 10) {
        int iCellX = iCellGiveX(getCell());
        int iCellY = iCellGiveY(getCell());

        int iDistance=9999; // closest distance

        int iAir=-1;        // aircraft (prioritized!)
        int iWorm=-1;       // worm lowest priority
        int iDanger=-1;     // danger id (unit to attack)

        iTargetID=-1;       // no target

        // scan area for units
        for (int i=0; i < MAX_UNITS; i++)
        {
            // is valid
            if (unit[i].isValid())
            {
				bool bAlly=player[getOwner()].iTeam == player[unit[i].iPlayer].iTeam;

                // not ours and its visible
                if (unit[i].iPlayer != getOwner() &&
                	mapUtils->isCellVisibleForPlayerId(getOwner(), unit[i].iCell) &&
					bAlly == false) {

                    int distance = ABS_length(iCellX, iCellY, iCellGiveX(unit[i].iCell), iCellGiveY(unit[i].iCell));

					// when worm or ornithopter is in range, they are not limited to the iDistance (closest
					// unit) range.
					if (distance <= structures[getType()].sight) {
						if (unit[i].iType == ORNITHOPTER) {
                            iAir=i;
	                    } else if (unit[i].iType == SANDWORM) {
                            iWorm=i;
                        }
                    }

					// when distance < closest range so far, this one is the most dangerous.
					if (distance <= structures[getType()].sight && distance < iDistance)
                    {
                        // ATTACK
                        iDistance = distance;
                        iDanger=i;
                    }
                }
            }
        }

        // set target
		if (iAir > -1) {
            iTargetID = iAir;
		} else if (iDanger > -1) {
            iTargetID = iDanger;
		} else if (iWorm > -1) {
            iTargetID = iWorm; // else pick worm
		}

        TIMER_guard=0-rnd(20); // redo

    }
}

// Draw function to draw this structure()
void cRocketTurret::draw(int iStage) {
	// When turret, frame = iHeadFacing
    setFrame(convert_angle(iHeadFacing));

    cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
