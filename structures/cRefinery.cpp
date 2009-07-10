#include "../d2tmh.h"

// Constructor
cRefinery::cRefinery()
{


 // other variables (class specific)
 
}

int cRefinery::getType() {
	return REFINERY;
}

cRefinery::~cRefinery()
{

}


/*  OVERLOADED FUNCTIONS  */

// Specific Construction Yard thinking
void cRefinery::think()
{

     // When a refinery, and expecting a unit
     if (iUnitID > -1)
        {
			int iMyID=-1;
			for (int i=0; i < MAX_STRUCTURES; i++)
				if (structure[i] == this)
				{
					iMyID=i;
					break;
				}
            
            // when the unit somehow does not go to us anymore, stop animating
            if (unit[iUnitID].isValid() == false)
            {
                iUnitID=-1;
				bAnimate=false;
                return;
            }

			if (unit[iUnitID].iStructureID != iMyID)
			{
				iUnitID=-1;
				bAnimate=false;
				return;
			}                
            
            // the unit id is filled in, that means the unit is IN this structure
            // the TIMER_harvest of the unit will be used to dump the harvest in the
            // refinery

            unit[iUnitID].TIMER_harvest++;
            
            if (unit[iUnitID].TIMER_harvest > player[iPlayer].iDumpSpeed(10))
            {
                unit[iUnitID].TIMER_harvest = 0;

                // dump credits
                if (unit[iUnitID].iCredits > 0)
                {                    
                    int iAmount = 5;

                    // cap at max
                    if (unit[iUnitID].iCredits > units[unit[iUnitID].iType].credit_capacity)
                        unit[iUnitID].iCredits = units[unit[iUnitID].iType].credit_capacity;


                    if ((unit[iUnitID].iCredits - iAmount) < 0)
                    {
                        iAmount = unit[iUnitID].iCredits;
                    }

					if (player[unit[iUnitID].iPlayer].credits < player[unit[iUnitID].iPlayer].max_credits)
                    {
						player[unit[iUnitID].iPlayer].credits += iAmount;
                        
                        // add
                        player[unit[iUnitID].iPlayer].lHarvested += iAmount;
                    }

                    unit[iUnitID].iCredits -= iAmount; 
                }
                else
                {
					int iNewCell = iFreeAround();

					if (iNewCell > -1)
					{
						unit[iUnitID].iCell = iNewCell;						
						
                        // let player know...
						if (unit[iUnitID].iPlayer==0)
							play_voice(SOUND_VOICE_02_ATR);

						// place back on map now
						//map.cell[iCell].id[MAPID_UNITS] = iUnitID;
					}
					else
					{
						logbook("Could not find space for this unit");

						// TODO: make carryall pick this up
						return;
					}

                    // done & restore unit
                    unit[iUnitID].iCredits = 0;
                    unit[iUnitID].iStructureID = -1;
                    unit[iUnitID].iHitPoints = unit[iUnitID].iTempHitPoints;
                    unit[iUnitID].iTempHitPoints = -1;                    
                    unit[iUnitID].iGoalCell = unit[iUnitID].iCell;
					unit[iUnitID].iPathIndex = -1;
					
					unit[iUnitID].TIMER_movewait = 0;
					unit[iUnitID].TIMER_thinkwait = 0;
                    
                    if (DEBUGGING)
                        assert(iUnitID > -1);

                    map.cell[unit[iUnitID].iCell].id[MAPID_UNITS] = iUnitID;

					// perhaps we can find a carryall to help us out
					int iHarvestCell = UNIT_find_harvest_spot(iUnitID);

					if (iHarvestCell)
					{
						int iCarry = CARRYALL_TRANSFER(iUnitID, iHarvestCell);

						if (iCarry < 0)
						{
							unit[iUnitID].iGoalCell = iHarvestCell;							
						}
						else
						{
							unit[iUnitID].TIMER_movewait = 100;
							unit[iUnitID].TIMER_thinkwait = 100;	
						}
					}

					iUnitID=-1;
                }
               
                
            }            
            
            
        }   

	// think like base class
	cStructure::think();

}

// Specific Animation thinking (flag animation OR its deploy animation)
void cRefinery::think_animation()
{
    // show (common) prebuild animation
    if (iBuildFase < 10)
        think_prebuild();
       
    if (bAnimate == false)
    {
        // FLAG animation
        TIMER_flag++;

        if (TIMER_flag > 70)
        {
            iFrame++;

            if (iFrame > 1)
                iFrame=0;

            TIMER_flag=0;            
        }
    }
    else
    {
        // harvester stuff
        if (iFrame < 0) 
            iFrame = 1;

        TIMER_flag++;                
        
        if (TIMER_flag > 70)
        {            
            TIMER_flag = 0;

            iFrame++;
            if (iFrame > 4) iFrame = 1;
        }
    }
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

void cRefinery::think_guard()
{

}

// Draw function to draw this structure()
void cRefinery::draw(int iStage)
{   
    // Select proper palette
    select_palette(player[iPlayer].pal);

    // iStage <= 1 -> Draw structure
    // iStage >  1 -> Draw structure repair icon (fading)
    
    // Draw structure
	if (iStage <= 1)
	{
        // Refinery has content, then the frame is a bit different    
        int FRAME = iFrame;
		if (iUnitID > -1)
            FRAME += 5;

		int iSourceY = structures[getType()].bmp_height * FRAME;
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


