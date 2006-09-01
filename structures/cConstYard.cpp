#include "..\d2tmh.h"
#include "..\d2tmc.h"

// Constructor
cConstYard::cConstYard()
{
 iType = CONSTYARD;

 // From the iType get data and set the info that is in the base class
 iWidth     = structures[iType].bmp_width/32;
 iHeight    = structures[iType].bmp_height/32;
 iHitPoints = structures[iType].hp;

 // other variables (class specific)

}

cConstYard::~cConstYard()
{

}


/*  OVERLOADED FUNCTIONS  */

// Specific Construction Yard thinking
void cConstYard::think()
{
// AI
    if (iPlayer > 0) aiplayer[iPlayer].think_repair_structure(this);

    // Other
    think_damage();
    think_repair();  
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cConstYard::think_animation()
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
				iRepairX = rnd((structures[iType].bmp_width-16));
				iRepairY = rnd((structures[iType].bmp_height-32));
				iRepairAlpha = 255;
			}
			else
				iRepairY--;
			
		}
     }

}

void cConstYard::think_guard()
{

}

// Draw function to draw this structure()
void cConstYard::draw(int iStage)
{   
    // Select proper palette
    select_palette(player[iPlayer].pal);

    // iStage <= 1 -> Draw structure
    // iStage >  1 -> Draw structure repair icon (fading)
        
    // Draw structure
	if (iStage <= 1)
	{
		int iSourceY = structures[iType].bmp_height * iFrame;
		int iDrawPreBuild=-1;

        // prebuild
        if (iBuildFase == 1 ||  
            iBuildFase == 3 ||
            iBuildFase == 5 ||
            iBuildFase == 7 ||
            iBuildFase == 9)
        {
            // Construction Yard prebuild animation
            iDrawPreBuild = BUILD_PRE_CONST;
        }

        // Draw structure itself
        if (iDrawPreBuild < 0)
        {
            // Fix this up, since NEMA now posted a structure which somehow needs transculency
            // and does not work. Sloppy work Stefan! Fixed @ 13-04-2005
            
            BITMAP *temp=create_bitmap_ex(8, structures[iType].bmp_width, structures[iType].bmp_height);
            BITMAP *temp_shadow=create_bitmap(structures[iType].bmp_width, structures[iType].bmp_height);

            // Only for Construction Yard
            clear(temp);			

            clear_to_color(temp_shadow, makecol(255,0,255));
            blit(structures[iType].bmp, temp, 0, iSourceY, 0, 0, structures[iType].bmp_width, structures[iType].bmp_height);		
            
            // in case shadow
            if (structures[iType].shadow)
                blit(structures[iType].shadow, temp_shadow, 0, iSourceY, 0, 0, structures[iType].bmp_width, structures[iType].bmp_height);

            draw_sprite(bmp_screen, temp, iDrawX(), iDrawY());
            
            // in case shadow
            if (structures[iType].shadow)
            {
//              set_trans_blender(0,0,0,128);
//              draw_trans_sprite(bmp_screen, temp_shadow, iDrawX(), iDrawY());
                
                fblend_trans(temp_shadow, bmp_screen, iDrawX(), iDrawY(), 128);
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
                //set_trans_blender(0,0,0,128);
                //draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[iDrawPreBuild+1].dat, iDrawX(), iDrawY());
                fblend_trans((BITMAP *)gfxdata[iDrawPreBuild+1].dat, bmp_screen, iDrawX(), iDrawY(), 128);
            }
        }

    if (iUnitID > -1 && iType == REPAIR)
    {
        // draw icon on structure
        draw_sprite(bmp_screen, (BITMAP *)gfxinter[units[unit[iUnitID].iType].icon].dat, iDrawX()+16, iDrawY()+5);

        rectfill(bmp_screen, iDrawX()+18, iDrawY()+45, iDrawX()+78, iDrawY()+50, makecol(0,0,0));
        // now draw health (60 pixels)
        int iW = health_bar(58, unit[iUnitID].iTempHitPoints, units[unit[iUnitID].iType].hp);
        rectfill(bmp_screen, iDrawX()+19,iDrawY()+46, iDrawX()+19+iW, iDrawY()+49, makecol(0,255,0)); 

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

