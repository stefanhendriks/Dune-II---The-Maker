/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  -----------------------------------------------
  Game menu items
  -----------------------------------------------
*/

#include "d2tmh.h"

// Fading between menu items
void cGame::FADE_OUT()
{
    iFadeAction = 1; // fade out
    draw_sprite(bmp_fadeout, bmp_screen, 0, 0);
}

// Drawing of any movie/scene loaded
void cGame::draw_movie(int iType)
{
    if (gfxmovie != NULL && iMovieFrame > -1)
    {    

        // drawing only, circulating is done in think function
        draw_sprite(bmp_screen, (BITMAP *)gfxmovie[iMovieFrame].dat, 256, 120);

    }
}

// draw the message
void cGame::draw_message()
{
	if (iMessageAlpha > -1)
	{
		set_trans_blender(0,0,0,iMessageAlpha);
		BITMAP *temp = create_bitmap(480,30);
		clear_bitmap(temp);
		rectfill(temp, 0,0,480,40, makecol(255,0,255));
		draw_sprite(temp, (BITMAP *)gfxinter[BMP_MESSAGEBAR].dat, 0,0);

		// draw message
//		alfont_textprintf(temp, game_font, 13,7, makecol(0,0,0), cMessage);
		alfont_textprintf(temp, game_font, 13,21, makecol(0,0,0), cMessage);
		
		//alfont_textprintf_aa(temp, game_font, 13,21, makecol(1,1,1), cMessage);
		
		
		// draw temp
		draw_trans_sprite(bmp_screen, temp, 1, 42);

		destroy_bitmap(temp);
	}


}



void cGame::draw_order()
{
	// draw order button
	if (iActiveList != LIST_STARPORT)
		return;

	bool bMouseHover=false;

		// determine if mouse is over the button..
	if (  (mouse_x > 29 && mouse_x < 187) && (mouse_y > 2 && mouse_y < 31))		
			bMouseHover=true;

	bool bDrawOrder=false;
	
	// we ordered stuff
	for (int i = 0; i < MAX_ICONS; i++)
	{
		if (iconFrigate[i] > 0)		
			bDrawOrder=true;
	}

	if (TIMER_ordered > -1)
		bDrawOrder=false; // grey

	if (TIMER_mayorder > -1)
		bDrawOrder=false;

	if (bDrawOrder)
    {        
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_ORDER].dat, 29, 0);
    }
	else
    {
                    // clear
            BITMAP *bmp_trans=create_bitmap(((BITMAP *)gfxinter[BTN_ORDER].dat)->w,((BITMAP *)gfxinter[BTN_ORDER].dat)->h);
            clear_to_color(bmp_trans, makecol(255,0,255));
            
            // copy 
            draw_sprite(bmp_trans, (BITMAP *)gfxinter[BTN_ORDER].dat, 0, 0);

            // make black
            rectfill(bmp_screen, 29, 0, 187, 29, makecol(0,0,0));

            // set blender
            //set_trans_blender(0,0,0,128);
            
            // trans
            fblend_trans(bmp_trans, bmp_screen, 29, 0, 128);
            //draw_trans_sprite(bmp_screen, bmp_trans, 29, 0);
            
            // destroy - phew
            destroy_bitmap(bmp_trans);

//        rectfill(bmp_screen, 29, 0, 187, 29, makecol(0,0,0));
  //      set_trans_blender(0,0,0,128);
    //    draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_ORDER].dat, 29, 0);
		//draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_ORDER_GREY].dat, 29, 0);        
    }


	// react on mouse
	if (bMouseHover && bDrawOrder)
	{
		if (bMousePressedLeft)
		{
			TIMER_ordered=30; // 30 seconds
		}
	}
}

void cGame::draw_upgrade()
{

	bool bDrawUpgradeButton=false;
	bool bMouseHover=false;
	int iLimit=0;
	int iType=-1;
	int iPrice=-1;

	int iDrawXLimit=-1;
	
	if (iActiveList == LIST_NONE)
		return; // nothing selected, so nothing to upgrade

	// determine if mouse is over the button..
	if (  (mouse_x > 29 && mouse_x < 187) && (mouse_y > 2 && mouse_y < 31))		
		if (iconbuilding[iActiveList] < 0) // not building, then we may do this
			bMouseHover=true;

	

	// determine what list is selected

	////////////////////////
	/// CONSTRUCTION YARD UPGRADING
	////////////////////////
	if (iActiveList == LIST_CONSTYARD)
	{
		iType=CONSTYARD;

		// first upgrade
		if (iStructureUpgrade[CONSTYARD] == 0 && iMission >= 4)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[CONSTYARD] < 0)
			{
			// when mission 4 or higher, show upgrade button

			// * upgrade is for 4 slabs
			iLimit=10;
			iPrice=200;
			}
		}

		// second upgrade
		// * Upgrade for Rocket Turret, but only when we have radar
		if (iStructureUpgrade[CONSTYARD] == 1 && iMission >= 6 &&
			player[0].iStructures[RADAR] > 0)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[CONSTYARD] < 0)
			{
			// when mission 4 or higher, show upgrade button

			// * upgrade is for 4 slabs
			iLimit=50;
			iPrice=400;
			}
		}

	}




	////////////////////////
	/// LIGHTFACTORY UPGRADING
	////////////////////////
	if (iActiveList == LIST_LIGHTFC)
	{
		
		// Lightfactory upgrading is done only for house Atreides and Ordos

		iType=LIGHTFACTORY;

		// first upgrade
		if (iStructureUpgrade[LIGHTFACTORY] == 0 && iMission >= 2)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[LIGHTFACTORY] < 0)
			{
			
			// * upgrade for Quad
			iLimit=50;
			iPrice=500;
			}
		}

	}


  	////////////////////////
	/// BARRACKS/WOR UPGRADING
	////////////////////////
	if (iActiveList == LIST_INFANTRY)
	{
		
		// Lightfactory upgrading is done only for house Atreides and Ordos

        if (player[0].house == ATREIDES ||
            player[0].house == ORDOS)
            iType=BARRACKS;         // Note: ordos uses also wor, so this is also for wor!!
        else
            iType=WOR;

		// first upgrade
		if (iStructureUpgrade[iType] == 0 && iMission >= 2)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[iType] < 0)
			{
                // * upgrade for TROOPERS
                iLimit=30;
                iPrice=150;

                if (iHouse == ORDOS)
                {
                    iLimit=60;
                    iPrice=300; // double the price
                }
			}
		}

	}


    	////////////////////////
	/// ORNI UPGRADING
	////////////////////////
	if (iActiveList == LIST_ORNI && player[0].house != HARKONNEN)
	{
		
		// Lightfactory upgrading is done only for house Atreides and Ordos

		iType=HIGHTECH;

		// first upgrade
		if (iStructureUpgrade[HIGHTECH] == 0 && iMission >= 6)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[HIGHTECH] < 0)
			{
			
			// * upgrade for ORNI
			iLimit=100;
			iPrice=700;
			}

		
		}

    }


	////////////////////////
	/// HEAVYFACTORY UPGRADING
	////////////////////////
	if (iActiveList == LIST_HEAVYFC)
	{
		
		// Lightfactory upgrading is done only for house Atreides and Ordos

		iType=HEAVYFACTORY;

		// first upgrade
		if (iStructureUpgrade[HEAVYFACTORY] == 0 && iMission >= 4)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[HEAVYFACTORY] < 0)
			{
			
			// * upgrade for MCV
			iLimit=60;
			iPrice=500;
			}

		
		}


		if (iStructureUpgrade[HEAVYFACTORY] == 1 && iMission >= 5)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[HEAVYFACTORY] < 0)
			{
			
			// * upgrade for LAUNCHER
			iLimit=80;
			iPrice=500;
			}


		
		}

		if (iStructureUpgrade[HEAVYFACTORY] == 2 && iMission >= 6)
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[HEAVYFACTORY] < 0)
			{
			
			// * upgrade for SIEGE
			iLimit=100;
			iPrice=700;
			}


		
		}

		if (iStructureUpgrade[HEAVYFACTORY] == 3 && iMission >= 7 &&
			player[0].iStructures[IX] > 0) // AND we do own a house of IX
		{			
			bDrawUpgradeButton=true;
			if (iUpgradeProgress[HEAVYFACTORY] < 0)
			{
			
			// * upgrade for SPECIAL WEAPON
			iLimit=120;
			iPrice=1000;
			}


		
		}
	}

	if (bMouseHover && bDrawUpgradeButton && iPrice > -1)
	{
		char msg[255];
		sprintf(msg, "$%d | Upgrade (Shortkey 'U')", iPrice);
		set_message(msg);
	}

	// Start upgrading sequence when pressing button and such
	if (bDrawUpgradeButton)
    	if (iconbuilding[iActiveList] < 0 || player[0].credits < iPrice)
		{
			draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_UPGRADE].dat, 29, 0);

            bool bUpgrade=false;

            if (bMousePressedLeft && bMouseHover)
                bUpgrade=true;

            if (key[KEY_U] && iUpgradeProgressLimit[iType] <= 0)
                bUpgrade=true;

			if (bUpgrade) // able to press button, valid and enough money
				if (iType > -1 && player[0].credits >= iPrice)
				{
					iUpgradeTIMER[iType]=0;
					iUpgradeProgress[iType]=0;
					iUpgradeProgressLimit[iType]=iLimit;
					player[0].credits -= iPrice;					
				}
		}
		else
        {
            // clear
            BITMAP *bmp_trans=create_bitmap(((BITMAP *)gfxinter[BTN_UPGRADE].dat)->w,((BITMAP *)gfxinter[BTN_UPGRADE].dat)->h);
            clear_to_color(bmp_trans, makecol(255,0,255));
            
            // copy 
            draw_sprite(bmp_trans, (BITMAP *)gfxinter[BTN_UPGRADE].dat, 0, 0);

            // make black
            rectfill(bmp_screen, 29, 0, 187, 29, makecol(0,0,0));

            // set blender
            set_trans_blender(0,0,0,128);
            
            // trans
            draw_trans_sprite(bmp_screen, bmp_trans, 29, 0);
            
            // destroy - phew
            destroy_bitmap(bmp_trans);
        }


	// Draw progress bar
    if (iType > -1 && iUpgradeProgress[iType] > -1)
    {
        iDrawXLimit = health_bar(157, iUpgradeProgress[iType], iUpgradeProgressLimit[iType]);

        if (iDrawXLimit > -1)
        {
            int iColor=makecol(255,255,255);
            BITMAP *temp = create_bitmap(157, 28);
            clear_to_color(temp, makecol(255,0,255));

            
            if (player[0].house == ATREIDES) iColor = makecol(0,0,255);
            if (player[0].house == HARKONNEN) iColor = makecol(255,0,0);
            if (player[0].house == ORDOS) iColor = makecol(0,255,0);
            if (player[0].house == SARDAUKAR) iColor = makecol(255,0,255);
            
            rectfill(temp, 0, 0, (157-iDrawXLimit), 30, iColor);
            
            draw_trans_sprite(bmp_screen, temp, 30, 1);
            destroy_bitmap(temp);
        }
    }
} // end of function
