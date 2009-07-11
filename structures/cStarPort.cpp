#include "../d2tmh.h"

// Constructor
cStarPort::cStarPort()
{

 // other variables (class specific)
 TIMER_deploy = 1;
}

int cStarPort::getType() {
	return STARPORT;
}

cStarPort::~cStarPort()
{

}

void cStarPort::think()
{
	// think like base class
	cStructure::think();

}

// Specific Animation thinking (flag animation OR its deploy animation)
void cStarPort::think_animation()
{
// show (common) prebuild animation
    if (iBuildFase < 10)
        think_prebuild();

    if (bAnimate == false)
    {
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
        if (TIMER_deploy < 0)
        {
            if (iFrame < 0) 
                iFrame = 1;
            
            TIMER_flag++;                
            
            if (TIMER_flag > 70)
            {            
                TIMER_flag = 0;

                iFrame++;
                if (iFrame > 3) iFrame = 1;
            }
        }
        else
            iFrame = 4;
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

// called every second, to deploy a unit 
void cStarPort::think_deploy()
{
    logbook("think deploy");
    if (TIMER_deploy > -1)
	{
		TIMER_deploy--;
        logbook("timer deploy was > -1");

		if (TIMER_deploy == 0)
		{
            logbook("timer deploy == 0");
			bool bDeployed=false;

            // deploy stuff
            for (int i=0; i < MAX_ICONS; i++)
            {
                if (game.iconFrigate[i] > 0)
                {
                    int iID = game.iconlist[LIST_STARPORT][i].iUnitID;

                    // find free space
                    int iNewCell = iFreeAround();

					if (iNewCell > -1)
					{
						int id = UNIT_CREATE(iNewCell, iID, 0, true);
                        
                        if (iRallyPoint > -1)
                            unit[id].move_to(iRallyPoint, -1, -1);

						bDeployed=true;
						game.iconFrigate[i]--;
						play_voice(SOUND_VOICE_05_ATR); // unit deployed
						break;
					}
					else
					{
						
						// when nothing found now, it means the structure is the only
						// one. So, we cannot dump it. Send over a reinforcement
						int rX = (iCellGiveX(getCell()) - 5) + rnd(10);
						int rY = (iCellGiveY(getCell()) - 5) + rnd(10);
						
                        if (iRallyPoint > -1)
                        {
                         //   unit[id].move_to(iRallyPoint, -1, -1);
                          rX = (iCellGiveX(iRallyPoint) - 2) + rnd(4);
                          rY = (iCellGiveY(iRallyPoint) - 2) + rnd(4);
						}

						FIX_BORDER_POS(rX, rY);

						REINFORCE(0, iID, iCellMake(rX, rY), iCellMake(rX, rY));

						bDeployed=true;
						game.iconFrigate[i]--;
						break;
					}
			}
		}

            
		
            if (bDeployed)
            {
                logbook("Deployed a unit?");
                TIMER_deploy=2;
                game.TIMER_mayorder = 2;
            }
            else
            {
                // its empty, so we are done
                iFrame=0;
                bAnimate=false;
                TIMER_deploy=-1;
                game.TIMER_mayorder=-1;
            }
		} // deploy a unit
	}
}

void cStarPort::think_guard()
{
    return; // starport has no guard function
}

// Draw function to draw this structure()
void cStarPort::draw(int iStage)
{
  // Select proper palette
    select_palette(player[getOwner()].pal);

    // iStage <= 1 -> Draw structure
    // iStage >  1 -> Draw structure repair icon (fading)
        
    // Draw structure
	if (iStage <= 1)
	{
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


