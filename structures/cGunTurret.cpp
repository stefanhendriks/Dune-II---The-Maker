#include "../d2tmh.h"

// Constructor
cGunTurret::cGunTurret() {
 // other variables (class specific)
 iHeadFacing=FACE_UP;        // (for turrets only) what is this structure facing at?
 iShouldHeadFacing=FACE_UP;  // where should we look face at? 
 iTargetID=-1;           // target id

 TIMER_fire=0;
 TIMER_turn=0;
 TIMER_guard=0;         // timed 'area scanning'
}


int cGunTurret::getType() {
	return TURRET;
}


cGunTurret::~cGunTurret()
{

}


/*  OVERLOADED FUNCTIONS  */

// Specific Construction Yard thinking
void cGunTurret::think()
{
    // AI
    if (iPlayer > 0) aiplayer[iPlayer].think_repair_structure(this);

    // Other
    think_damage();
    think_repair();  

    int iMyIndex=-1;

    for (int i=0; i < MAX_STRUCTURES; i++)
        if (structure[i] == this)
        {
            iMyIndex=i;
            break;
        }

    // this should not happen, but just in case
    if (iMyIndex < 0)
        return;

    
    if (player[iPlayer].bEnoughPower() == false)
        return; 


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
                int iSlowDown=200;

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

                int iSlowDown = 125;

                if (getType() == RTURRET)
                    iSlowDown = 200; // even more slow

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

}

// Specific Animation thinking (flag animation OR its deploy animation)
void cGunTurret::think_animation()
{
    // show (common) prebuild animation
    if (iBuildFase < 10)
        think_prebuild();
 

     // Repair blink
     if (bRepair)
     {
		TIMER_repairanimation++;

		if (TIMER_repairanimation > 1 && 
            player[0].credits > 2)
		{
			
			TIMER_repairanimation=0;
			iRepairAlpha -= 7;

			if (iRepairAlpha < 1)
			{
				iRepairX = rnd((structures[getType()].bmp_width-16));
				iRepairY = rnd((structures[getType()].bmp_height-32));
				iRepairAlpha = 255;
			}
			else
				iRepairY--;
			
		}
     }

    
}

void cGunTurret::think_guard()
{
 // TURRET CODE HERE

    if (player[iPlayer].bEnoughPower() == false)
        return; 

    TIMER_guard++;

    if (TIMER_guard > 10)
    {
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
				bool bAlly=false;

                if (player[iPlayer].iTeam == player[unit[i].iPlayer].iTeam)
                    bAlly=true;

				

                // not ours and its visible
                if (unit[i].iPlayer != iPlayer && map.iVisible[unit[i].iCell][iPlayer] && bAlly == false)
                {
                    if (getType() == TURRET)
                        if (units[unit[i].iType].airborn)
                            continue; // it was airborn, and normal turrets cannot hit this
                     
                    int distance = ABS_length(iCellX, iCellY, iCellGiveX(unit[i].iCell), iCellGiveY(unit[i].iCell));

                    if (unit[i].iType == ORNITHOPTER)
                    {
                        if (distance <= structures[getType()].sight)
                        {
                            iAir=i;
                        }
                    }
                    else if (unit[i].iType == SANDWORM)
                    {
                        if (distance <= structures[getType()].sight)
                        {
                            iWorm=i;
                        }
                    }
                    else if (distance <= structures[getType()].sight && distance < iDistance)
                    {
                        // ATTACK
                        iDistance = distance;
                        iDanger=i;
                    }

                }
            }
        }

        // set target
        if (iAir > -1)
            iTargetID = iAir;
        else if (iDanger > -1)
            iTargetID = iDanger;
        else if (iWorm > -1)
            iTargetID = iWorm; // else pick worm

        TIMER_guard=0-rnd(20); // redo

    }
}

// Draw function to draw this structure()
void cGunTurret::draw(int iStage)
{   
    // Select proper palette
    select_palette(player[iPlayer].pal);

    // iStage <= 1 -> Draw structure
    // iStage >  1 -> Draw structure repair icon (fading)
        
    // Draw structure
	if (iStage <= 1)
	{    
        // When turret, frame = iHeadFacing
        int iFrame = convert_angle(iHeadFacing);

        int iSourceY = structures[getType()].bmp_height * iFrame;
		int iDrawPreBuild=-1;

       
        // prebuild
        if (iBuildFase == 1 ||  
            iBuildFase == 3 ||
            iBuildFase == 5 ||
            iBuildFase == 7 ||
            iBuildFase == 9)
        {
            // Check width
            if (iWidth == 1 && iHeight == 1)
                iDrawPreBuild = BUILD_PRE_1X1;

            if (iWidth == 2 && iHeight == 2)
                iDrawPreBuild = BUILD_PRE_2X2;

            if (iWidth == 3 && iHeight == 2)
                iDrawPreBuild = BUILD_PRE_3X2;
            
            if (iWidth == 3 && iHeight == 3)
                iDrawPreBuild = BUILD_PRE_3X3;
           
        }

        // Draw structure itself
        if (iDrawPreBuild < 0)
        {
            // Fix this up, since NEMA now posted a structure which somehow needs transculency
            // and does not work. Sloppy work Stefan! Fixed @ 13-04-2005
            
            BITMAP *temp=create_bitmap_ex(8, structures[getType()].bmp_width, structures[getType()].bmp_height);
            BITMAP *temp_shadow=create_bitmap(structures[getType()].bmp_width, structures[getType()].bmp_height);

            // Only for Construction Yard
            clear(temp);			

            clear_to_color(temp_shadow, makecol(255,0,255));
            blit(structures[getType()].bmp, temp, 0, iSourceY, 0, 0, structures[getType()].bmp_width, structures[getType()].bmp_height);		
            
            // in case shadow
            if (structures[getType()].shadow)
                blit(structures[getType()].shadow, temp_shadow, 0, iSourceY, 0, 0, structures[getType()].bmp_width, structures[getType()].bmp_height);

            draw_sprite(bmp_screen, temp, iDrawX(), iDrawY());
            
            // in case shadow
            if (structures[getType()].shadow)
            {
                set_trans_blender(0,0,0,128);
                draw_trans_sprite(bmp_screen, temp_shadow, iDrawX(), iDrawY());
            }

            destroy_bitmap(temp);
            destroy_bitmap(temp_shadow);
        }
        // Draw prebuild
        else
        {
            // Draw prebuild
            draw_sprite(bmp_screen, (BITMAP *)gfxdata[iDrawPreBuild].dat, iDrawX(), iDrawY());    
            
            // Draw shadow of the prebuild animation
            if (iDrawPreBuild != BUILD_PRE_CONST)
            {
                set_trans_blender(0,0,0,128);
                draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[iDrawPreBuild+1].dat, iDrawX(), iDrawY());
            }
        }
} // stage <= 1
else if (iStage == 2)
{
	// now draw the repair alpha when repairing
	if (bRepair)
	{
		if (iRepairAlpha > -1)
		{
            set_trans_blender(0, 0, 0, iRepairAlpha);
            draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_REPAIR].dat, iDrawX()+iRepairX, iDrawY() + iRepairY);
		}
		else
			iRepairAlpha = rnd(255);
    }
}   
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */


