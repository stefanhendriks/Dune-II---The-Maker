#include "../d2tmh.h"

// Constructor
cWindTrap::cWindTrap() {

 // other variables (class specific)
 iFade=rnd(63);
 bFadeDir=true;

 // Timers
 TIMER_fade=0;
}

int cWindTrap::getType() {
	return WINDTRAP;
}

cWindTrap::~cWindTrap()
{

}


void cWindTrap::think()
{
	// think like base class
	cStructure::think();
     
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cWindTrap::think_animation()
{
    // show (common) prebuild animation
    if (iBuildFase < 10)
        think_prebuild();        

    TIMER_fade++;

    int iTime;
    
    // depending on fade direction, fade in slower/faster
    if (bFadeDir)
        iTime = 2;
    else
        iTime = 3;
    
    // timer based
    if (TIMER_fade > iTime)
    {
        if (bFadeDir)
        {
            iFade++;
            
            if (iFade > 254)
                bFadeDir=false;
/*
            if (iFade > 62)
                bFadeDir=false;*/
        }
        else
        {
            iFade--;
            if (iFade < 1)
                bFadeDir=true;
        }

        TIMER_fade=0;
    }

    // FLAG animation
    TIMER_flag++;

     if (TIMER_flag > 70)
     {
         iFrame++;

         if (iFrame > 1)
             iFrame=0;
         
         TIMER_flag=0;            
     }

     // Repair blink
     if (bRepair)
     {
		TIMER_repairanimation++;

        // if possible to repair
		if (TIMER_repairanimation > 1 && player[iPlayer].credits > 2)
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

void cWindTrap::think_guard()
{

}

// Draw function to draw this structure()
void cWindTrap::draw(int iStage)
{   
    // Select proper palette
    select_palette(player[iPlayer].pal);

    // iStage <= 1 -> Draw structure
    // iStage >  1 -> Draw structure repair icon (fading)
        
    // Draw structure
	if (iStage <= 1)
	{
        // Mess up the palette for windtraps:
//		RGB c = { 0 , 0, iFade };
//        set_color(239, &c);

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
            BITMAP *wind=create_bitmap(structures[getType()].bmp_width, structures[getType()].bmp_height);

            // Only for Construction Yard
            clear(temp);			
            clear_to_color(wind, makecol(255,0,255));

            clear_to_color(temp_shadow, makecol(255,0,255));
            blit(structures[getType()].bmp, temp, 0, iSourceY, 0, 0, structures[getType()].bmp_width, structures[getType()].bmp_height);		
            
            
            // in case shadow
            if (structures[getType()].shadow)
                blit(structures[getType()].shadow, temp_shadow, 0, iSourceY, 0, 0, structures[getType()].bmp_width, structures[getType()].bmp_height);
            
            //draw_sprite(bmp_screen, temp, iDrawX(), iDrawY());
            draw_sprite(wind, temp, 0, 0);
            
            lit_windtrap_color(wind, makecol(0,0,iFade));

            draw_sprite(bmp_screen, wind, iDrawX(), iDrawY());
            destroy_bitmap(wind);

            // in case shadow
            if (structures[getType()].shadow)
            {
                set_trans_blender(0,0,0,128);
                draw_trans_sprite(bmp_screen, temp_shadow, iDrawX(), iDrawY());

                //fblend_trans(temp_shadow, bmp_screen, iDrawX(), iDrawY(), 128);
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

    if (iUnitID > -1 && getType() == REPAIR)
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


