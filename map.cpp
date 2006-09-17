/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"
#include <math.h>

#define WATCH_PLAYER 0

void cMap::init()
{
    INIT_REINFORCEMENT();
   
	// remember, dune 2 has 64x64 maps, but they use 62x62!
	scroll_x=scroll_y=1;
    
    // clear out all cells
    for (int c=0; c < MAX_CELLS; c++)
    {   
        cell[c].credits = cell[c].health = 0;
        cell[c].passable = true;
        cell[c].tile = 0;
        cell[c].type = TERRAIN_SAND;    // refers to gfxdata!

        cell[c].smudgetile = -1;
        cell[c].smudgetype = -1;

        // clear out the ID stuff
        memset(cell[c].id, -1, sizeof(cell[c].id));


		for (int i=0; i < MAX_PLAYERS; i++)
			iVisible[c][i] = false;
    }

    // set visibility   
	
    //memset(iVisible, MAP_FOG, sizeof(iVisible));

    iStaticFrame = STAT14;               //
    iStatus = -1;                       // 0 = show minimap , -1 = no minimap 
                                        // (static animation, of going down)
                                        // 1 = static animation 'getting up'...

    iTrans = 0;

    for (int i=0; i < MAX_STRUCTURES; i++)
    {   
        // clear out all structures
        if (structure[i])
        {
            // delete memory that was aquired
            if (structure[i]->iType == CONSTYARD) 
                delete (cConstYard *)structure[i];
            else if (structure[i]->iType == STARPORT)
                delete (cStarPort *)structure[i];
            else if (structure[i]->iType == WINDTRAP)
                delete (cWindTrap *)structure[i];
            else if (structure[i]->iType == SILO)
                delete (cSpiceSilo *)structure[i];
            else if (structure[i]->iType == RADAR)
                delete (cOutPost *)structure[i];
            else if (structure[i]->iType == HIGHTECH)
                delete (cHighTech *)structure[i];
            else if (structure[i]->iType == LIGHTFACTORY)
                delete (cLightFactory *)structure[i];
            else if (structure[i]->iType == HEAVYFACTORY)
                delete (cHeavyFactory *)structure[i];
            else if (structure[i]->iType == PALACE)
                delete (cPalace *)structure[i];
            else if (structure[i]->iType == TURRET)
                delete (cGunTurret *)structure[i];
            else if (structure[i]->iType == RTURRET)
                delete (cRocketTurret *)structure[i];
            else if (structure[i]->iType == REPAIR)
                delete (cRepairFacility *)structure[i];
            else if (structure[i]->iType == BARRACKS)
                delete (cBarracks *)structure[i];
            else if (structure[i]->iType == WOR)
                delete (cWor *)structure[i];
            else if (structure[i]->iType == IX)
                delete (cIx *)structure[i];
            else
                delete structure[i];
        }  
        
        // clear pointer
        structure[i] = NULL;
    }

	for (int i=0; i < MAX_BULLETS; i++) {
		bullet[i].init();
	}
        
	for (int i=0; i < MAX_PARTICLES; i++) {
        particle[i].init();
	}

	for (int i=0; i < MAX_UNITS; i++) {
		unit[i].init(i);
	}
	
}

void cMap::smudge_increase(int iType, int iCell)
{
    if (cell[iCell].smudgetype < 0)
        cell[iCell].smudgetype = iType;

    if (cell[iCell].smudgetype == SMUDGE_WALL)
        cell[iCell].smudgetile = 0;

    if (cell[iCell].smudgetype == SMUDGE_ROCK)
    {
        if (cell[iCell].smudgetile < 0)
            cell[iCell].smudgetile = rnd(2);
        else if (cell[iCell].smudgetile + 2 < 6)
            cell[iCell].smudgetile += 2;        
    }

    if (cell[iCell].smudgetype == SMUDGE_SAND)
    {
        if (cell[iCell].smudgetile < 0)
            cell[iCell].smudgetile = rnd(2);
        else if (cell[iCell].smudgetile + 2 < 6)
            cell[iCell].smudgetile += 2;        
    }
}

bool cMap::occupied(int iCll)
{
    bool bResult=false;

    if (iCll < 0)
	{
		if (DEBUGGING)
			Logger.print("No valid cell specified in cMap::Occupied(int)");

        return true;
	}

    if (map.cell[iCll].id[MAPID_UNITS] > -1)
        bResult=true;

	// air units also count
    if (map.cell[iCll].id[MAPID_AIR] > -1)
        bResult=true;

    // TODO: when unit wants to enter a structure... 

    if (map.cell[iCll].id[MAPID_STRUCTURES] > -1 )
        bResult=true;

    // walls block as do mountains
    if (map.cell[iCll].type == TERRAIN_WALL)
        bResult=true;

    // mountains only block infantry
    if (map.cell[iCll].type == TERRAIN_MOUNTAIN)
		bResult=true;

	return bResult;

}

bool cMap::occupied(int iCll, int iUnitID)
{
    bool bResult=false;

    if (iCll < 0 || iUnitID < 0)
        return true;

    if (map.cell[iCll].id[MAPID_UNITS] > -1 && 
		map.cell[iCll].id[MAPID_UNITS] != iUnitID)
        bResult=true;

    // TODO: when unit wants to enter a structure... 

    if (map.cell[iCll].id[MAPID_STRUCTURES] > -1 )
	{
		// we are on top of a structure we do NOT want to enter...
		if (unit[iUnitID].iStructureID > -1)
		{
			if (map.cell[iCll].id[MAPID_STRUCTURES] != unit[iUnitID].iStructureID)
			bResult=true;
		}
		else
			bResult=true;
	}

    // walls block as do mountains
    if (map.cell[iCll].type == TERRAIN_WALL)
        bResult=true;

    // mountains only block infantry
    if (units[unit[iUnitID].iType].infantry == false)
        if (map.cell[iCll].type == TERRAIN_MOUNTAIN)
           bResult=true;

	

    return bResult;    
}

void cMap::draw_structures_health()
{
    int iDrawHealthStructure = game.hover_structure;

    // DRAW HEALTH
    if (iDrawHealthStructure > -1)
    {
        int i = iDrawHealthStructure;

        // Draw structure health
        int draw_x = structure[i]->iDrawX()-1;
        int draw_y = structure[i]->iDrawY()-5;
        int width_x = structures[structure[i]->iType].bmp_width-1;
        int height_y = 4;

        if (draw_y < 30) draw_y = 30;

        int w = health_structure(i, structures[structure[i]->iType].bmp_width);
        
        int step = (255/structures[structure[i]->iType].bmp_width);
        int r = 255-(w*step);
        int g = w*step;
        
        if (g > 255)
            g = 255;
        if (r < 0) r= 0;
        

        // shadow
       // rectfill(bmp_screen, draw_x+2, draw_y+2, draw_x + width_x + 2, draw_y + height_y + 2, makecol(0,0,0));
        
        // bar itself
        rectfill(bmp_screen, draw_x, draw_y, draw_x + width_x+1, draw_y + height_y+1, makecol(0,0,0));
        rectfill(bmp_screen, draw_x, draw_y, draw_x + (w-1), draw_y + height_y, makecol(r,g,32));
        
        // bar around it
        rect(bmp_screen, draw_x, draw_y, draw_x + width_x, draw_y + height_y, makecol(255, 255, 255));

    }

}

void cMap::draw_structures(int iStage)
{   
    // draw all structures
    for (int i=0; i < MAX_STRUCTURES; i++)
    {
        if (structure[i])
        {
            // put this thing on the map
            for (int w=0; w < structure[i]->iWidth; w++)
                for (int h=0; h < structure[i]->iHeight; h++)
                {
                    int cll = iCellMake((iCellGiveX(structure[i]->iCell)+w), 
                        (iCellGiveY(structure[i]->iCell)+h));

                    cell[cll].id[MAPID_STRUCTURES] = i;
                }

            if (((structure[i]->iDrawX()+structures[structure[i]->iType].bmp_width) > 0 && structure[i]->iDrawX() < game.screen_x) &&
                ((structure[i]->iDrawY()+structures[structure[i]->iType].bmp_height) > 0 && structure[i]->iDrawY() < game.screen_y))
            {
                // draw               
                structure[i]->draw(iStage);

				if (player[0].iPrimaryBuilding[structure[i]->iType] == i)
				{
					alfont_textprintf(bmp_screen, game_font, structure[i]->iDrawX()-1, structure[i]->iDrawY()-1, makecol(0,0,0), "P");
					alfont_textprintf(bmp_screen, game_font, structure[i]->iDrawX(), structure[i]->iDrawY(), makecol(255,255,255), "P");
				}


                // When mouse hovers over this structure
                if (((mouse_x >= structure[i]->iDrawX() && mouse_x <= (structure[i]->iDrawX() + structures[structure[i]->iType].bmp_width))) &&
                    ((mouse_y >= structure[i]->iDrawY() && mouse_y <= (structure[i]->iDrawY() + structures[structure[i]->iType].bmp_height))))
                {
                    // draw some cool id of it
					if (key[KEY_D] && key[KEY_TAB])
                    {
						alfont_textprintf(bmp_screen, game_font, structure[i]->iDrawX(),structure[i]->iDrawY(), makecol(255,255,255), "%d", i);
                        alfont_textprintf(bmp_screen, game_font, structure[i]->iDrawX(),structure[i]->iDrawY()-16, makecol(255,255,255), "%s", structures[structure[i]->iType].name);
                    }


					game.hover_structure=i;
                }

                if (i == game.selected_structure)
                {
                    // draw rectangle around it fading, like dune 2
                    int draw_x = structure[i]->iDrawX();
                    int draw_y = structure[i]->iDrawY();
                    int width_x = structures[structure[i]->iType].bmp_width-1;
                    int height_y = structures[structure[i]->iType].bmp_height-1;

                    rect(bmp_screen, draw_x, draw_y, draw_x + width_x, draw_y + height_y, makecol(game.fade_select, game.fade_select, game.fade_select));
                }

				
            }

        }

    }


	rectfill(bmp_screen, (game.screen_x-160), 0, game.screen_x, game.screen_y, makecol(0,0,0));

}

void cMap::draw_shroud()
{   
    set_trans_blender(0,0,0,128);

    BITMAP *temp = create_bitmap(32,32);


    int iEndX=scroll_x + ((game.screen_x-160)/32); // width of sidebar is 160
    int iEndY=scroll_y + ((game.screen_y-42)/32)+1;  // height of upper bar is 42

    int iDrawX=0;
    int iDrawY=42;

    int cll=-1;
    int tile=-1;

	int iPl = WATCH_PLAYER;

    for (int iStartX=scroll_x; iStartX < iEndX; iStartX++)
    {
        iDrawY=42;
        tile=-1;
        // new row
        for (int iStartY=scroll_y; iStartY < iEndY; iStartY++)
        {
            cll = iCellMake(iStartX, iStartY);

			if (DEBUGGING)
			{
				if (iVisible[cll][iPl])
				{
					// do nothing
				}
				else
				{
					rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(0,0,0));	
				}	

			}
			else
			{
            

            if (iVisible[cll][iPl])
            {               
            // Visible stuff, now check for not visible stuff. When found, assign the proper border
            // of shroud to it.
            int above = CELL_ABOVE(cll);
            int under = CELL_UNDER(cll);
            int left  = CELL_LEFT(cll);
            int right = CELL_RIGHT(cll);
            
            bool a, u, l, r;
            a=u=l=r=true;

            if (above > -1)
			{
                if (iVisible[above][iPl])
                    a = false;  // visible
			}
			else a = false;
                
            if (under > -1)
			{
                if (iVisible[under][iPl])
                    u = false;  // visible
			}
			else u = false;
             
            if (left > -1) 
			{
                if (iVisible[left][iPl])
                    l = false;  // visible
			}
			else l = false;
                    
            if (right > -1)
			{
                if (iVisible[right][iPl])
                    r = false;  // visible
			}
			else r = false;
                /*        
            // Upper left/right stuff
            bool Ul, Ur, Ll, Lr;
            Ul=Ur=Ll=Lr=true;

            if (CELL_L_LEFT(cll) > -1)
                if (iVisible[CELL_L_LEFT(cll)][0])
                    Ll = false; // visible
            
            if (CELL_L_RIGHT(cll) > -1)
                if (iVisible[CELL_L_RIGHT(cll)][0])
                    Lr = false; // visible

            if (CELL_U_LEFT(cll) > -1)
                if (iVisible[CELL_U_LEFT(cll)][0])
                    Ul = false; // visible

            if (CELL_U_RIGHT(cll) > -1)
                if (iVisible[CELL_U_RIGHT(cll)][0])              
                    Ur = false; // visible*/

                int t=-1;

                // when above is not visible then change this border tile
                if (a == true && u == false && l == false && r == false)
                    t = 3;
                
                if (a == false && u == true && l == false && r == false)
                    t = 7;
                
                if (a == false && u == false && l == true && r == false)
                    t = 9;
                
                if (a == false && u == false && l == false && r == true)
                    t = 5;
                        // corners
                
                if (a == true && u == false && l == true && r == false)
                    t = 2;

                if (a == true && u == false && l == false && r == true)
                    t = 4;

                if (a == false && u == true && l == true && r == false)
                    t = 8;

                if (a == false && u == true && l == false && r == true)
                    t = 6;

                // 3 connections
                if (a == true && u == true && l == true && r == false) 
                    t = 10;

                if (a == true && u == true && l == false && r == true) 
                    t = 12;

                if (a == true && u == false && l == true && r == true) 
                    t = 11;

                if (a == false && u == true && l == true && r == true) 
                    t = 13;

                if (a == true && u == true && l == true && r == true)
                    t=1;

                tile = t - 1;

                if (tile > -1)
                {
					
					// Draw cell
                    masked_blit((BITMAP *)gfxdata[SHROUD].dat, bmp_screen, tile * 32, 0, iDrawX, iDrawY, 32, 32);

					 clear_to_color(temp, makecol(255,0,255));

                        masked_blit((BITMAP *)gfxdata[SHROUD_SHADOW].dat, temp, tile * 32, 0, 0, 0, 32, 32);
                        draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);


				//	alfont_textprintf(bmp_screen, game_font, iDrawX,iDrawY, makecol(255,255,255), "%d", tile);
               }
            
              }
              else
              {

				  // NOT VISIBLE, DO NOT DRAW A THING THEN!
 				//	alfont_textprintf(bmp_screen, game_font, iDrawX,iDrawY, makecol(255,255,255), "%d", tile);
 
				  // Except when there is a building here, that should not be visible ;)
				  // if (map.cell[cll].id[1] > -1 || map.cell[cll].id[0] > -1 || map.cell[cll].id[2] > -1 || map.cell[cll].id[3] > -1)
					  masked_blit((BITMAP *)gfxdata[SHROUD].dat, bmp_screen, 0, 0, iDrawX, iDrawY, 32, 32);
              }

			  }
            iDrawY+=32;
        }
        iDrawX+=32;
    }

    destroy_bitmap(temp); 
}

void cMap::draw_minimap()
{
	int iCll=0;

	int iDrawX=511;
	int iDrawY=351;

	int iColor=makecol(0,0,0);

 for (int x=0; x < (game.map_width); x++)
 {

	 iDrawY=351; // reset every time ;)
	 for (int y=0; y < (game.map_height); y++)
	 {
		iColor=makecol(0,0,0);
		iCll=iCellMake(x,y);

		if (iVisible[iCll][0])
		{
            if (player[0].iStructures[RADAR] > 0 && player[0].bEnoughPower())
            {
			// change color
			if (cell[iCll].type == TERRAIN_ROCK)
				iColor = makecol(80,80,60);

			if (cell[iCll].type == TERRAIN_SPICE)
				iColor = makecol(186,93,32);

			if (cell[iCll].type == TERRAIN_SPICEHILL)
				iColor = makecol(180,90,25); // bit darker

			if (cell[iCll].type == TERRAIN_HILL)
				iColor = makecol(188, 115, 50);

			if (cell[iCll].type == TERRAIN_MOUNTAIN)
				 iColor = makecol(48, 48, 36);

			if (cell[iCll].type == TERRAIN_SAND)
				iColor = makecol(194, 125, 60);

			if (cell[iCll].type == TERRAIN_WALL)
				iColor = makecol(192, 192, 192);

			if (cell[iCll].type == TERRAIN_SLAB)
				iColor = makecol(80,80,80);

			if (cell[iCll].type == TERRAIN_BLOOM)
				iColor = makecol(214,145,100);

            }

            	if (cell[iCll].id[MAPID_STRUCTURES] > -1)
			{
               
				int iPlr=structure[cell[iCll].id[MAPID_STRUCTURES]]->iPlayer;
				// get house color from the player of this structure
                if (iPlr ==0 || (player[0].iStructures[RADAR] > 0 && player[0].bEnoughPower()))
                {
				if (player[iPlr].house == ATREIDES) iColor = makecol(0,0,255);
				if (player[iPlr].house == HARKONNEN) iColor = makecol(255,0,0);
				if (player[iPlr].house == ORDOS) iColor = makecol(0,255,0);
				if (player[iPlr].house == SARDAUKAR) iColor = makecol(255,0,255);
                }
			}

			if (cell[iCll].id[MAPID_UNITS] > -1)
			{
				int iPlr=unit[cell[iCll].id[MAPID_UNITS]].iPlayer;
				// get house color from the player of this structure
                if (iPlr == 0 || (player[0].iStructures[RADAR] > 0 && player[0].bEnoughPower()))
                {
				if (player[iPlr].house == ATREIDES) iColor = makecol(0,0,255);
				if (player[iPlr].house == HARKONNEN) iColor = makecol(255,0,0);
				if (player[iPlr].house == ORDOS) iColor = makecol(0,255,0);
				if (player[iPlr].house == SARDAUKAR) iColor = makecol(255,0,255);
                }
			}

            if (cell[iCll].id[MAPID_WORMS] > -1)
			{				
				iColor = makecol(game.fade_select,game.fade_select,game.fade_select);				
			}

		

		}

        

		// do not show the helper border ;)
		if (x == 0 || y == 0)
			iColor = makecol(0,0,0);

		if (x == 63 || y == 63)
			iColor = makecol(0,0,0);


		putpixel(bmp_screen, iDrawX+x, iDrawY+y, iColor);
		putpixel(bmp_screen, iDrawX+x+1, iDrawY+y, iColor);
		putpixel(bmp_screen, iDrawX+x+1, iDrawY+y+1, iColor);
		putpixel(bmp_screen, iDrawX+x, iDrawY+y+1, iColor);

		iDrawY+=1;
	 }
	 iDrawX+=1;
 }


 // Draw the magic rectangle
 int iWidth=((game.screen_x-160)/32);
 int iHeight=((game.screen_y-42)/32)+1;

 iWidth--;
 iHeight--;

 rect(bmp_screen, 511+(scroll_x*2), 351+(scroll_y*2), ((511+(scroll_x*2))+iWidth*2)+1, (351+(scroll_y*2)+iHeight*2)+1, makecol(255,255,255));

 // Mouse reaction
 if (mouse_x >= 511 && mouse_y >= 351)
 {
	 if (Mouse.btnLeft && mouse_co_x1 < 0 && mouse_co_y1 < 0)
	{
		// change scroll positions and such :)
		scroll_x = (((mouse_x-(iWidth)) - 511) / 2);
		scroll_y = (((mouse_y-(iHeight)) - 351) / 2);

		if (scroll_x < 1)
			scroll_x = 1;
		if (scroll_y < 1)
			scroll_y = 1;
		if (scroll_x >= (62-iWidth))
			scroll_x = (62-iWidth);
		if (scroll_y >= (62-iHeight))
			scroll_y = (62-iHeight);

	}


 }

 if (player[0].iStructures[RADAR] > 0 &&
     player[0].bEnoughPower())
 {
     if (iStatus < 0)
     {
         play_sound_id(SOUND_RADAR,-1);
         play_voice(SOUND_VOICE_03_ATR);
     }

     iStatus = 0;

 }
 else 
 {
     if (iStatus > -1)
         play_voice(SOUND_VOICE_04_ATR);

     iStatus = -1;
 }

 // Draw static info
 if (iStatus < 0)
 {
     
     draw_sprite(bmp_screen, (BITMAP *)gfxinter[iStaticFrame].dat, 511, 350);

 }
 else
 {
     

     if (iStaticFrame < STAT10)
         iTrans = 255 - health_bar(192, (STAT12-iStaticFrame), 12); 
     else
         iTrans=255;

     if (iStaticFrame != STAT01)
     {
         set_trans_blender(0,0,0,iTrans);

         draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[iStaticFrame].dat, 511, 350);
         // reset the trans blender
         set_trans_blender(0,0,0,128);
     }

 }


}

// do the static info thinking
void cMap::think_minimap()
{
 // Draw static info
 if (iStatus < 0)
 {
     
     draw_sprite(bmp_screen, (BITMAP *)gfxinter[iStaticFrame].dat, 511, 350);

     if (iStaticFrame < STAT21)
         iStaticFrame++;
 }
 else
 {
     

     // transparancy is calculated actulaly
     if (iStaticFrame > STAT01)
         iStaticFrame--;

 }


}

void cMap::draw()
{
    // draw only what is visible

    // Scrolling:
    // - is X,Y position of map, not smoothed!
    // - all we need is the end of the 'to be drawn area' , which is:
    //   WIDTH OF SCREEN / 32
   

    int iEndX=scroll_x + ((game.screen_x-160)/32); // width of sidebar is 160
    int iEndY=scroll_y + ((game.screen_y-42)/32)+1;  // height of upper bar is 42

    int iDrawX=0;
    int iDrawY=42;

    int cll=-1;

	int iPl = WATCH_PLAYER;

    for (int iStartX=scroll_x; iStartX < iEndX; iStartX++)
    {
        iDrawY=42;
        
		

        // new row
        for (int iStartY=scroll_y; iStartY < iEndY; iStartY++)
        {
            cll = iCellMake(iStartX, iStartY);

			if (iStartX==0 || iStartY == 0 || iStartX==63 || iStartY == 63)
			{
				cell[cll].type = TERRAIN_MOUNTAIN;
				cell[cll].tile = 0;
			}

            if (iVisible[cll][iPl] == false)
            {               
                iDrawY+=32;
                continue; // do not draw this one
            }
            
            // Draw cell
//            blit((BITMAP *)gfxdata[cell[cll].type].dat, bmp_screen, 
  //               cell[cll].tile * 32, 0, iDrawX, iDrawY, 32, 32);

            blit((BITMAP *)gfxdata[cell[cll].type].dat, bmp_screen, 
                 cell[cll].tile * 32, 0, iDrawX, iDrawY, 32, 32);


            // draw smudge if nescesary
            if (cell[cll].smudgetype > -1 && cell[cll].smudgetile > -1)
            {
                /*
                masked_blit((BITMAP *)gfxdata[SMUDGE].dat, bmp_screen, 
                 cell[cll].smudgetile * 32, cell[cll].smudgetype * 32, iDrawX, iDrawY, 32, 32);*/
                
                masked_blit((BITMAP *)gfxdata[SMUDGE].dat, bmp_screen, 
                 cell[cll].smudgetile * 32, cell[cll].smudgetype * 32, iDrawX, iDrawY, 32, 32);

            }

			if (DEBUGGING)
			{
				if (map.mouse_cell() > -1)					
				{
					int mc = map.mouse_cell();
					if (iCellGiveX(mc) == iStartX && iCellGiveY(mc) == iStartY)
						rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(64,64,64));

				}

				rect(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(128,128,128));
			}

            if (game.selected_structure > -1)
            {
                // show draw a target on this cell so we know this is the rally point.
                if (structure[game.selected_structure]->iRallyPoint > -1)
                    if (structure[game.selected_structure]->iRallyPoint == cll)
                    {
                        // draw this thing ...
                        set_trans_blender(0,0,0,128);
                        draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_MOVE].dat, iDrawX, iDrawY);
                    }
            }

			
			if (key[KEY_D] && key[KEY_TAB])
			{
				int iClr=makecol(255,0,0);

				bool bDraw=false;

				if (cell[cll].passable == false)
					bDraw=true;

				if (map.cell[cll].id[MAPID_STRUCTURES] > -1)
				{
					iClr=makecol(0,255,0);
		            bDraw=true;
				}
	
			    if (map.cell[cll].id[MAPID_UNITS] > -1)
				{
					iClr=makecol(0,0,255);
				    bDraw=true;
				}

				if (bDraw)
				{
					//draw_sprite(bmp_screen, (BITMAP *)gfxdata[PLACE_BAD].dat, iDrawX, iDrawY);
					rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, iClr);
				}

			}
            
            iDrawY+=32;
        }
        iDrawX+=32;
    }
    
}

void cMap::draw_bullets()
{

    // Loop through all units, check if they should be drawn, and if so, draw them  
  for (int i=0; i < MAX_BULLETS; i++)
  {    
    if (bullet[i].bAlive)
    {      
      if (bullet[i].draw_x() > -32 && 
          bullet[i].draw_x() < game.screen_x &&
          bullet[i].draw_y() > -32 &&
          bullet[i].draw_y() < game.screen_y)      
          bullet[i].draw();
    }
  }  

}


void cMap::draw_particles(int layer)
{
    for (int i=0; i < MAX_PARTICLES; i++)
        if (particle[i].isValid() && particle[i].layer == layer )
            particle[i].draw();    
}



void cMap::clear_all()
{
    for (int c=0; c < MAX_CELLS; c++)
        iVisible[c][0] = true;
}

// The given CELL
// or the X, Y
// give the 'center' of this window. This is the 'focus cell'
void cMap::set_pos(int x, int y, int cell)
{
    int px, py;
    px = py = -1;

    // Get our variables straight
    if (cell > -1)
    {
        px = iCellGiveX(cell);
        py = iCellGiveY(cell);
    }
    else
    {
        if (x > -1 && y > -1)
        {
            px = x;
            py = y;
        }
        else
            return;
            
    }
    
    // determine the half of our screen

    // Whole...
    int iX = ((game.screen_x-160)/32); 
    int iY = ((game.screen_y-42)/32)+1;

    // Half ...
    int iHalfX = iX/2;
    int iHalfY = iY/2;
    
    scroll_x = (px-iHalfX);
    scroll_y = (py-iHalfY);

    int iEndX=scroll_x + ((game.screen_x-160)/32);
    int iEndY=scroll_y + ((game.screen_y-42)/32)+1;

	// 10/01 - HACK HACK : Correct the Y position
	iEndY++;

     // make sure the X,Y position is NOT set 'out of the borders'
    if (iEndX >= (game.map_width-2))
        scroll_x = game.map_width-(iX+1);

    if (iEndY >= ( game.map_height-2))
        scroll_y = game.map_height-(iY+1);    

    // in case its a tiny map
    if (scroll_x < 1) scroll_x = 1;
    if (scroll_y < 1) scroll_y = 1;
    
}


void cMap::clear_spot(int c, int size, int player)
{
  // Get the x and y and make a circle around it of 16xR, then calculate of every step the cell and
  // clear it

  int cx = iCellGiveX(c); /*cell[c].x(c);*/
  int cy = iCellGiveY(c); /*cell[c].y(c);*/

  // fail
  if (cx < 0 || cy < 0)
      return;

  map.iVisible[c][player]=true;

#define TILE_SIZE_PIXELS 32

  // go around 360 fDegrees and calculate new stuff
  for (int dr=1; dr < size; dr++)
  {
    for (double d=0; d < 360; d++)  
    {
      int x = cx, y = cy;

      // when scrolling, compensate
      x -= scroll_x;
      y -= scroll_y;
      
      // convert to pixels (*32)
      x *= TILE_SIZE_PIXELS;
      y *= TILE_SIZE_PIXELS;
     
      y += 42;
      
	  // center on unit
      x+= 16;
      y+= 16;

	  
      int dx, dy;
      dx = x;
      dy = y;

       x = (x + (cos(d)*((dr*32))));
       y = (y + (sin(d)*((dr*32))));

     // DEBUG
	   
      //line (bmp_screen, dx, dy, x, y, makecol(255,255,255));
      //if (DEBUGGING)
		//  putpixel(bmp_screen, x, y, makecol(255,255,0));

      //putpixel(bmp_screen, dx, dy, makecol(0,255,0));
     // DEBUG

      // convert back
      
       int cell_x, cell_y;             

       cell_y  = y;       
	   cell_y -= 42;       	   
	   cell_y += (scroll_y*32);
	   cell_y = cell_y/32;
	   
	   cell_x  = x;
	   cell_x += (scroll_x*32);
       cell_x  = cell_x/32;

	  
	   /*
	   if (DEBUGGING)
	   {
	   
	   int iDrawX = (cell_x-scroll_x)*32;
	   int iDrawY = (((cell_y-scroll_y)*32)+42);

	   rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(0,32,0));

	   }*/


       FIX_POS(cell_x, cell_y);



	   //draw the cells
		

	   
	   /*
	   if (game.state == GAME_PLAYING)
	   {
	   char msg[255];
	   sprintf(msg, "X %d, Y %d -> CX %d, CY = %d", x, y, cell_x, cell_y);
	   Logger.print(msg);
	   }*/
       
       int cl = iCellMake(cell_x, cell_y);
       
	       

       if (iVisible[cl][player] == false)
       {
       
       iVisible[cl][player] = true;     // make visible
           // human unit detected enemy, now be scared and play some neat music
       if (player == 0)
       {
            if (cell[cl].id[MAPID_UNITS] > -1)
            {
                int id = cell[cl].id[MAPID_UNITS];
                
                if (unit[id].iPlayer != 0) // NOT friend
                {
                    // when state of music is not attacking, do attacking stuff and say "Warning enemy unit approaching
                    
                    if (game.iMusicType == MUSIC_PEACE)
                    {
                        play_music(MUSIC_ATTACK);

                        // warning... bla bla
                        if (unit[id].iType == SANDWORM)
                            play_voice(SOUND_VOICE_10_ATR);  // omg a sandworm, RUN!
                        else
                            play_voice(SOUND_VOICE_09_ATR);  // enemy unit
                    }

                }
            }
       }

       } // make visible

	   
       
    }
  }


}

// Each index is a mapdata field holding indexes of the map layout 
// 
void cMap::remove_id(int iIndex, int iIDType)
{
    // Search through the entire map and remove it
    // Using X,Y stuff, since we do not want to have unnescesary slow-downs
    // when using small maps... and going through a 512x512 map
    
	/*

	int x, y, cll;
    x = y = cll = 0;
    
	

    for (x=0; x < game.map_width; x++)
        for (y=0; y < game.map_height; y++)
        {
            cll = iCellMake(x,y);
            if (cell[cll].id[iIDType] == iIndex)
			{
                cell[cll].id[iIDType] = -1; // remove it				
			}
        }*/

	for (int iCell=0; iCell < MAX_CELLS; iCell++)
		if (cell[iCell].id[iIDType] == iIndex)
		{
			// remove
			cell[iCell].id[iIDType] = -1;
		}
    
    
    
}


void cMap::randommap()
{
    // create random map
    init();
  
  // <> is MAP_WIDTH+MAP_HEIGHT / <a number>

  int a_spice=rnd((game.iSkirmishStartPoints * 2))+(game.iSkirmishStartPoints * 2);
  int a_rock=6+rnd(game.iSkirmishStartPoints);
  int a_hill=2+rnd(8);

  

  // rock terrain is placed not near centre, also, we want 4 plateaus be
  // placed not too near to each other
  int iSpotRock[6]; // first 4
  memset(iSpotRock, -1, sizeof(iSpotRock));
  
  int iSpot=0;  
  int iFails=0;

  memset(PreviewMap[0].iStartCell, -1, sizeof(PreviewMap[0].iStartCell));

  int iDistance=16;
  
  if (game.iSkirmishStartPoints == 2)  iDistance=32;
  if (game.iSkirmishStartPoints == 3)  iDistance=26;
  if (game.iSkirmishStartPoints == 4)  iDistance=22;

  int iProgress=0;
  
  // x = 160
  // y = 180
  draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GENERATING].dat, 160, 180);
  
  // draw  

	  while (a_rock > 0)
	  {
		  int iCll=iCellMake(4+rnd(game.map_width-8), 4+rnd(game.map_height-8));

		  bool bOk=true;
		  if (iSpot < 6)
		  {
			  for (int s=0; s < 6; s++)
			  {
				  if (iSpotRock[s] > -1)
				  {	
					  if (ABS_length(iCellGiveX(iCll), iCellGiveY(iCll), iCellGiveX(iSpotRock[s]), iCellGiveY(iSpotRock[s])) < iDistance)
						  bOk=false;
					  else
						  iFails++;
				  }
			  }
		  }		  

		  if (iFails > 10)
		  {
			  iFails=0;
			  bOk=true;
		  }

		  if (bOk)
		  {
			  iProgress+=5;
			  create_field(TERRAIN_ROCK, iCll, 5500+rnd(1500));

			  if (iSpot < game.iSkirmishStartPoints)
			  {
				  PreviewMap[0].iStartCell[iSpot] = iCll;
			  }
			  else
			  {
				  create_field(TERRAIN_MOUNTAIN, iCll, 5+rnd(15));
			  }

			  if (iSpot < 6)
				  iSpotRock[iSpot]=iCll;

			  iSpot++;
			  a_rock--;

			  // blit on screen
			  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
			  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

		  }


		  
		  // take screenshot
		  if (key[KEY_F11])
		  {
			  char filename[25];       
			  sprintf(filename, "%dx%d_%d.bmp", game.screen_x, game.screen_y, game.screenshot);
			  save_bmp(filename, bmp_screen, general_palette);
			  game.screenshot++;
		  }

	  }

	  // soft out rock a bit
  soft();
  
  // blit on screen
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  soft();

  // blit on screen
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  soft();

  // blit on screen
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);
  
  while (a_spice > 0)
  {
	  int iCll=iCellMake(rnd(game.map_width), rnd(game.map_height));
      create_field(TERRAIN_SPICE, iCll, 2500);	  
      a_spice--;
  	  // blit on screen
	  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  }

  while (a_hill > 0)
  {
      create_field(TERRAIN_HILL, iCellMake(rnd(game.map_width), rnd(game.map_height)), 500+rnd(500));
      a_hill--;
	  iProgress+=10;
	  // blit on screen
	  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);
  }

  
  // end of map creation

  smooth();

  // blit on screen
  iProgress+=25;
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  clear_to_color(PreviewMap[0].terrain, makecol(0,0,0));



  // now put in previewmap 0
  for (int x=0; x < 64; x++)
	  for (int y=0; y < 64; y++)
	  {		  
        
        if (iProgress < 210 && rnd(100) < 3)
        {
            rectfill(screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
        }
		  
		int cll=iCellMake(x,y);		
		int iColor=makecol(194, 125, 60);

		// rock
		if (cell[cll].type == TERRAIN_ROCK) iColor = makecol(80,80,60);
		if (cell[cll].type == TERRAIN_MOUNTAIN) iColor = makecol(48, 48, 36);
		if (cell[cll].type == TERRAIN_SPICEHILL) iColor = makecol(180,90,25); // bit darker
		if (cell[cll].type == TERRAIN_SPICE) iColor = makecol(186,93,32);
		if (cell[cll].type == TERRAIN_HILL) iColor = makecol(188, 115, 50);

        PreviewMap[0].mapdata[cll] = cell[cll].type;

		for (int s=0; s < 4; s++)
		{
            if (PreviewMap[0].iStartCell[s] > -1)
            {
			int sx = iCellGiveX(PreviewMap[0].iStartCell[s]);
			int sy = iCellGiveY(PreviewMap[0].iStartCell[s]);

			if (sx == x && sy == y)
				iColor = makecol(255,255,255);
            }
		}

		putpixel(PreviewMap[0].terrain, (x*2), (y*2), iColor);
		putpixel(PreviewMap[0].terrain, (x*2)+1, (y*2), iColor);
		putpixel(PreviewMap[0].terrain, (x*2)+1, (y*2)+1, iColor);
		putpixel(PreviewMap[0].terrain, (x*2), (y*2)+1, iColor);
	  }	  

  // blit on screen
  iProgress=211;
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);  
}


void cMap::create_field(int type, int cll, int size)
{

  int x = iCellGiveX(cll);
  int y = iCellGiveY(cll);

  if (x < 0)
    x = rnd(game.map_width);

  if (y < 0)
    y = rnd(game.map_height);
  
  if (type == TERRAIN_ROCK && size < 0)
    size=150;

  if (type == TERRAIN_MOUNTAIN && size < 0)
    size=25;  // very small mountainious spots

  int iDist=0;

  int iOrgX=x;
  int iOrgY=y;

  for (int i = 0; i < size; i++)
  {
    int c = iCellMake(x, y);
	iDist = ABS_length(x, y, iOrgX, iOrgY);
    
	if (iDist > rnd(4)+4)
		if (rnd(100) < 5)
			create_field(type, c, 100);

    if (c > -1)
    {
      // if NOT a rock tile, then place spice on it (if we are placing spice)
      if (type == TERRAIN_SPICE)
      {
          if ((cell[c].type != TERRAIN_ROCK) && (cell[c].type != TERRAIN_SLAB) && (cell[c].type != TERRAIN_MOUNTAIN) &&
              (cell[c].type != TERRAIN_WALL))
              create_spot(c, TERRAIN_SPICE, 0);  
      }      
      // create hill
      else if (type == TERRAIN_HILL)
      {        
        if (cell[c].type == TERRAIN_SAND)
        {
          create_spot(c, TERRAIN_HILL, 0);          
        }
        else if (cell[c].type == TERRAIN_SPICE)
        {
          create_spot(c, TERRAIN_SPICE, 0);
        }
                
      }
      else
      {        
        create_spot(c, type, 0);            
       
        if (type == TERRAIN_MOUNTAIN)
        {
            cell[c].passable=false;
        }
        else
            cell[c].passable=true;            
        
      }
    }

	if (rnd(100) < 25)
	{
		x = iCellGiveX(cll);
		y = iCellGiveY(cll);
	}

 
    switch(rnd(4))
    {
    case 0: x++; break;
    case 1: y++; break;
    case 2: x--; break;
    case 3: y--; break;
    } 

    FIX_POS(x, y);
  }

  smooth();

  /*

  // find any gaps with this type and fill it up
  for (int cx = 0; cx < MAP_W_MAX; cx++)
    for (int cy = 0; cy < MAP_H_MAX; cy++)
    {
      int c = iCellMake(cx,cy);

      if (c > -1)
        if (cell[c].type == TERRAIN_SAND)
        {
        
         int above = CELL_ABOVE(c);
         int under = CELL_UNDER(c);
         int left  = CELL_LEFT(c);
         int right = CELL_RIGHT(c);

         int a = 0; // amount 'near', 2 means 50/50 (random fill), > 2 means fill < 2 means no fill
         if (above > -1)
             if (cell[above].type == type) a++;

         if (under > -1)
             if (cell[under].type == type) a++;

         if (left > -1)
             if (cell[left].type == type) a++;

         if (right > -1)
             if (cell[right].type == type) a++;

         // 
         if (a > 2)
         {
           cell[c].type = type;           
           cell[c].tile = 0;
           if (cell[c].type != TERRAIN_MOUNTAIN)
               cell[c].passable=true;
           else
               cell[c].passable=false;
         }
         else if (a == 2)
           if (rnd(100) < 50) 
           {        
               cell[c].type = type;
               cell[c].tile = 0;
               if (cell[c].type != TERRAIN_MOUNTAIN)
                   cell[c].passable=true;
               else
                   cell[c].passable=false;
               
           }
       
        }
    }
  */

}
void cMap::draw_units()
{
    set_trans_blender(0, 0, 0, 160);

    // draw all worms first
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {
			// DEBUG MODE: DRAW PATHS
			if (DEBUGGING)
				unit[i].draw_path();

            if (unit[i].iType == SANDWORM)
            {
           
            int drawx = unit[i].draw_x();
            int drawy = unit[i].draw_y();
                        
            if (((drawx+units[unit[i].iType].bmp_width) > 0 && drawx < (game.screen_x-160)) &&
                ((drawy+units[unit[i].iType].bmp_height) > 42 && drawy < game.screen_y))
            {
                // draw
                unit[i].draw();              
            }
            }

        }

    }

    // draw all units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {
           
            if (unit[i].iType == CARRYALL ||
                unit[i].iType == ORNITHOPTER ||
                unit[i].iType == FRIGATE ||
                unit[i].iType == SANDWORM ||
                units[unit[i].iType].infantry == false)
                continue; // do not draw aircraft

            int drawx = unit[i].draw_x();
            int drawy = unit[i].draw_y();
            
			//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(255,255,255));

            if (((drawx+units[unit[i].iType].bmp_width) > 0 && drawx < (game.screen_x-160)) &&
                ((drawy+units[unit[i].iType].bmp_height) > 42 && drawy < game.screen_y))
            {

                // draw
                unit[i].draw();              

				//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(0,255,255));

				if (key[KEY_D] && key[KEY_TAB])
					alfont_textprintf(bmp_screen, game_font, unit[i].draw_x(),unit[i].draw_y(), makecol(255,255,255), "%d", i);

            }

        }

    }

    // draw all units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {
           
            if (unit[i].iType == CARRYALL ||
                unit[i].iType == ORNITHOPTER ||
                unit[i].iType == FRIGATE ||
                unit[i].iType == SANDWORM ||
                units[unit[i].iType].infantry)
                continue; // do not draw aircraft

            int drawx = unit[i].draw_x();
            int drawy = unit[i].draw_y();
            
			//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(255,255,255));

            if (((drawx+units[unit[i].iType].bmp_width) > 0 && drawx < (game.screen_x-160)) &&
                ((drawy+units[unit[i].iType].bmp_height) > 42 && drawy < game.screen_y))
            {

                // draw
                unit[i].draw();              

				//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(0,255,255));

				if (key[KEY_D] && key[KEY_TAB])
					alfont_textprintf(bmp_screen, game_font, unit[i].draw_x(),unit[i].draw_y(), makecol(255,255,255), "%d", i);

            }

        }

    }

    int mc = map.mouse_cell();
    if (mc > -1)
    {
        if (map.cell[mc].id[MAPID_UNITS] > -1)
        {
            int iUnitId = map.cell[mc].id[MAPID_UNITS];
            
            if (unit[iUnitId].iTempHitPoints < 0)
                game.hover_unit = iUnitId;
        }
        else if (map.cell[mc].id[MAPID_WORMS] > -1)
        {
            int iUnitId = map.cell[mc].id[MAPID_WORMS];
            game.hover_unit = iUnitId;
        }
    }

}

void cMap::draw_units_2nd()
{
    set_trans_blender(0, 0, 0, 160);

    // draw unit power
    if (game.hover_unit > -1)
    {
		if (unit[game.hover_unit].iType == HARVESTER)
			unit[game.hover_unit].draw_spice();

        unit[game.hover_unit].draw_health();
		unit[game.hover_unit].draw_experience();
				
    }

    // draw health of units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {
            if (unit[i].bSelected)
			{
               unit[i].draw_health();
			   unit[i].draw_experience();
			}
        }

    }

        // draw all units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {
            
            if (((unit[i].draw_x()+units[unit[i].iType].bmp_width) > 0 && unit[i].draw_x() < game.screen_x) &&
                ((unit[i].draw_y()+units[unit[i].iType].bmp_height) > 0 && unit[i].draw_y() < game.screen_y))
            {
                // Draw aircraft here
                if (unit[i].iType == CARRYALL ||
                    unit[i].iType == ORNITHOPTER ||
                    unit[i].iType == FRIGATE)
                {
                    unit[i].draw();
                }                           
            }
        }

    }

    set_trans_blender(0, 0, 0, 128);
     

}

int cMap::mouse_cell()
{    
    // Determine where the mouse is, devide by 32
    if (mouse_y < 42)
        return -1; // at this cool bar (upbar)

    if (mouse_x > (game.screen_x - 128))
        if (mouse_y > (game.screen_y - 128))
            return -2 ; // on minimap

	if (mouse_x > (game.screen_x - 160))
		return -3 ; // on sidebar
   

    int iMouseX = mouse_x/32;
    int iMouseY = (mouse_y-42)/32;

    iMouseX += scroll_x;
    iMouseY += scroll_y;

    return iCellMake(iMouseX, iMouseY);
}

// Create a TYPE of terrain with TILE (when specified)
// at CELL
void cMap::create_spot(int c, int type, int tile)
{
    // set to 0
    if (tile < 0)
        tile = 0;

    // boundries
    if (c < 0 || c > MAX_CELLS)
    {
		if (DEBUGGING)
			Logger.print("ERROR: create_spot()");

        return;
    }

    // Set
    cell[c].type = type;
    cell[c].tile = tile;    
    cell[c].credits = 0;
    cell[c].passable = true;

    cell[c].smudgetile=-1;
    cell[c].smudgetype=-1;

    // when spice
    if (type == TERRAIN_SPICE || type == TERRAIN_SPICEHILL)
        cell[c].credits = 150 + rnd(75);

    if (type == TERRAIN_MOUNTAIN)
        cell[c].passable = false;

	if (type == TERRAIN_WALL)
	{
		cell[c].health = 100;
		cell[c].passable = false;
	}

}

void cMap::smooth_spot(int c)
{

    int above = CELL_ABOVE(c);
    int under = CELL_UNDER(c);
    int left  = CELL_LEFT(c);
    int right = CELL_RIGHT(c);

    if (above > -1)
        smooth_cell(above);

    if (under > -1) 
        smooth_cell(under);

    if (left > -1)
        smooth_cell(left);

    if (right > -1)
        smooth_cell(right);

    if (c > -1)
        smooth_cell(c);
}

// smooths out ROCK/SAND/SPICE/HILL/SPICEHILL stuff
void cMap::smooth()
{
    // loop through all cells and smooth
    for (int c=0; c < MAX_CELLS; c++)
        smooth_cell(c);

}

void cMap::soft()
{
	// soft out rocky spots!
	for (int x=1; x < 63; x++)
		for (int y=1; y< 63; y++)
		{
			int cll=iCellMake(x,y);
			if (cell[cll].type == TERRAIN_ROCK)
			{
				int iC=0;

				if (cell[iCellMake(x-1, y)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x, y+1)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x+1, y)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x, y-1)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x-1, y)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (cell[iCellMake(x, y+1)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (cell[iCellMake(x+1, y)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (cell[iCellMake(x, y-1)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (iC < 2)
				{
					cell[cll].type = TERRAIN_SAND;
					cell[cll].tile = 0;
				}
			}
		}
}

// 
void cMap::smooth_cell(int c)
{
    // smooth cell
    
    int above = CELL_ABOVE(c);
    int under = CELL_UNDER(c);
    int left  = CELL_LEFT(c);
    int right = CELL_RIGHT(c);

    int above_type = -1;
    int under_type = -1;
    int left_type  = -1;
    int right_type = -1;

    // checks
    if (above > -1)
      above_type = cell[above].type;
    if (under > -1)
      under_type = cell[under].type;
    if (left > -1)
      left_type  = cell[left].type;
    if (right > -1)
      right_type = cell[right].type;

    // Rock smoothing (is this a rocky tile?)
    if (cell[c].type == TERRAIN_ROCK)
    {

      bool a, u, l, r;
    
      // rock above?
      if ((above_type == TERRAIN_ROCK) || (above_type == TERRAIN_SLAB))
        a = true; 
      else
        a = false;

      // rock under?
      if ((under_type == TERRAIN_ROCK) || (under_type == TERRAIN_SLAB))
        u = true; 
      else
        u = false;

      // rock left?
      if ((left_type == TERRAIN_ROCK) ||  (left_type == TERRAIN_SLAB))
        l = true; 
      else
        l = false;

      // rock right?
      if ((right_type == TERRAIN_ROCK) || (right_type == TERRAIN_SLAB))
        r = true; 
      else
        r = false;
   
      // extra for mountains
      // rock above?
      if (above_type == TERRAIN_MOUNTAIN)
        a = true; 
      
      // rock under?
      if (under_type == TERRAIN_MOUNTAIN)
        u = true; 
      
      // rock left?
      if (left_type == TERRAIN_MOUNTAIN)
        l = true; 
      
      // rock right?
      if (right_type == TERRAIN_MOUNTAIN)
        r = true; 


      // extra for walls
      // rock above?
      if (above_type == TERRAIN_WALL)
        a = true; 
      
      // rock under?
      if (under_type == TERRAIN_WALL)
        u = true; 
      
      // rock left?
      if (left_type == TERRAIN_WALL)
        l = true; 
      
      // rock right?
      if (right_type == TERRAIN_WALL)
        r = true; 

/*
      // Extra for debris! (broken stuff)
      if ((above_type >= STR_B_BOTTOM && above_type <= STR_TURRET))
        a = true; 

      // rock under?
      if ((under_type >= STR_B_BOTTOM && under_type <= STR_TURRET))
        u = true; 
      
      // rock left?
      if ((left_type >= STR_B_BOTTOM && left_type <= STR_TURRET))
        l = true; 
      
      // rock right?
      if ((right_type >= STR_B_BOTTOM && right_type <= STR_TURRET))
        r = true; */

            
      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile
      
      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only
      
      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)
      
      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)
      
      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner
      
      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner
      
      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner
      
      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner
      
      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT
      
      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT     

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER
      
      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE
      

      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile -= 1; // because 0 is actually 1, etc
    }


    // Mountain smoothing    
    if (cell[c].type == TERRAIN_MOUNTAIN)
    {

      bool a, u, l, r;
    
      // rock above?
      if ((above_type == TERRAIN_MOUNTAIN))
        a = true; 
      else
        a = false;

      // rock under?
      if ((under_type == TERRAIN_MOUNTAIN))
        u = true; 
      else
        u = false;

      // rock left?
      if ((left_type == TERRAIN_MOUNTAIN) )
        l = true; 
      else
        l = false;

      // rock right?
      if ((right_type == TERRAIN_MOUNTAIN))
        r = true; 
      else
        r = false;

      /*
      if ((above_type >= ROCK1 && above_type <= ROCK9))
        a = true; 
      
      // rock under?
      if ((under_type >= ROCK1 && under_type <= ROCK9))
        u = true; 
      
      // rock left?
      if ((left_type >= ROCK1 && left_type <= ROCK9))
        l = true; 
      
      // rock right?
      if ((right_type >= ROCK1 && right_type <= ROCK9))
        r = true; 
        */
      
      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile
      
      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only
      
      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)
      
      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)
      
      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile =7; // corner
      
      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner
      
      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner
      
      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner
      
      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT
      
      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT     

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER
      
      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE
      

      // accompanied MOUNTAIN (all sides MOUNTAIN too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely MOUNTAIN

      cell[c].tile -= 1;
    }

    
    // Spice smoothing
    if (cell[c].type == TERRAIN_SPICE)
    {

      bool a, u, l, r;
    
      // spice above?
      if (above_type == TERRAIN_SPICE)
        a = true; 
      else
        a = false;

      // spice under?
      if (under_type == TERRAIN_SPICE)
        u = true; 
      else
        u = false;

      // spice left?
      if (left_type == TERRAIN_SPICE)
        l = true; 
      else
        l = false;

      // spice right?
      if (right_type == TERRAIN_SPICE)
        r = true; 
      else
        r = false;
      

      // For spice smoothing (normal spice) we simply check if anything 'spice hill' is near
      // and then we count that as spice aswell.

      // spice above?
      if (above_type == TERRAIN_SPICEHILL)
        a = true; 

      // spice under?
      if (under_type == TERRAIN_SPICEHILL)
        u = true; 

      // spice left?
      if (left_type == TERRAIN_SPICEHILL)
        l = true; 

      // spice right?
      if (right_type == TERRAIN_SPICEHILL)
        r = true; 


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile
      
      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only
      
      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)
      
      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)
      
      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner
      
      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner
      
      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner
      
      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner
      
      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT
      
      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT     

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER
      
      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE
      

      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile--;

    }


    // Spice HILL smoothing
    if (cell[c].type == TERRAIN_SPICEHILL)
    {

      bool a, u, l, r;
    
      // Now we only check the hills
      
      // spice above?
      if (above_type == TERRAIN_SPICEHILL)
        a = true; 
      else
        a = false;

      // spice under?
      if (under_type == TERRAIN_SPICEHILL)
        u = true; 
      else
        u = false;

      // spice left?
      if (left_type == TERRAIN_SPICEHILL)
        l = true; 
      else
        l = false;

      // spice right?
      if (right_type == TERRAIN_SPICEHILL)
        r = true; 
      else
        r = false;


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile
      
      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only
      
      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)
      
      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)
      
      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner
      
      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner
      
      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner
      
      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner
      
      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT
      
      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT     

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER
      
      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE
      

      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile--;

    }

    // Normal hills smoothing
    if (cell[c].type == TERRAIN_HILL)
    {

      bool a, u, l, r;
    
      // Now we only check the hills
      
      // spice above?
      if (above_type == TERRAIN_HILL)
        a = true; 
      else
        a = false;

      // spice under?
      if (under_type == TERRAIN_HILL)
        u = true; 
      else
        u = false;

      // spice left?
      if (left_type == TERRAIN_HILL)
        l = true; 
      else
        l = false;

      // spice right?
      if (right_type == TERRAIN_HILL)
        r = true; 
      else
        r = false;


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile
      
      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only
      
      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)
      
      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)
      
      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner
      
      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner
      
      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner
      
      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner
      
      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT
      
      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT     

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER
      
      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE
      

      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile--;

    }

    
    // Walls smoothing
    if (cell[c].type == TERRAIN_WALL)
    {

      bool a, u, l, r;
    
      // rock above?
      if ((above_type == TERRAIN_WALL))
        a = true; 
      else
        a = false;

      // WALL under?
      if ((under_type == TERRAIN_WALL))
        u = true; 
      else
        u = false;

      // WALL left?
      if ((left_type == TERRAIN_WALL))
        l = true; 
      else
        l = false;

      // WALL right?
      if ((right_type == TERRAIN_WALL))
        r = true; 
      else
        r = false;   
      
      /*
      // Extra for debris! (broken stuff)
      if ((above_type >= STR_B_BOTTOM && above_type <= STR_TURRET))
        a = true; 

      // rock under?
      if ((under_type >= STR_B_BOTTOM && under_type <= STR_TURRET))
        u = true; 
      
      // rock left?
      if ((left_type >= STR_B_BOTTOM && left_type <= STR_TURRET))
        l = true; 
      
      // rock right?
      if ((right_type >= STR_B_BOTTOM && right_type <= STR_TURRET))
        r = true; */

            
      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 12; // lonely tile
      
      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 10; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 10; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 12; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 8; // at the bottom only
      
      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 10; // one way connection  (left/right)
      
      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 8; // one way connection  (up/down)
      
      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 3; // corner
      
      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 11; // corner
      
      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 2; // corner
      
      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 1; // corner
      
      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 5; // border LEFT
      
      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 4; // border RIGHT     

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 7; // border UNDER
      
      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 6; // border ABOVE
      

      // accompanied WALL (all sides WALL too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 9; // not so lonely WALL

      cell[c].tile--;

    }



}

void cMap::draw_think()
{
   if (mouse_co_x1 > -1 &&
        mouse_co_y1 > -1)
        return;

	//if (game.bPlaceIt)
		//return; // do not scroll when placing

    int iEndX=scroll_x + ((game.screen_x-160)/32); // width of sidebar is 160
    int iEndY=scroll_y + ((game.screen_y-42)/32)+1;  // height of upper bar is 42

    // thinking for map (scrolling that is)
    if (mouse_x <= 1 || key[KEY_LEFT])    
        if (scroll_x > 1)
        {
            //scroll_x --;
            mouse_tile = MOUSE_LEFT;
        }
        
    
    if (mouse_y <= 1 || key[KEY_UP])
        if (scroll_y > 1)
        {
            //scroll_y --;
            mouse_tile = MOUSE_UP;
        }

    
    if (mouse_x >= (game.screen_x-2) || key[KEY_RIGHT])
        if ((iEndX) < (game.map_width-1))
        {
           // scroll_x ++;
            mouse_tile = MOUSE_RIGHT;
        }

    if (mouse_y >= (game.screen_y-2) || key[KEY_DOWN])
        if ((iEndY) < (game.map_height-1))
        {
         //   scroll_y ++;
            mouse_tile = MOUSE_DOWN;
        }


}

void cMap::think()
{
    if (mouse_co_x1 > -1 &&
        mouse_co_y1 > -1)
        return;

	//if (game.bPlaceIt)
		//return; // do not scroll when placing

    int iEndX=scroll_x + ((game.screen_x-160)/32); // width of sidebar is 160
    int iEndY=scroll_y + ((game.screen_y-42)/32)+1;  // height of upper bar is 42

    // thinking for map (scrolling that is)
    if (mouse_x <= 1 || key[KEY_LEFT])    
        if (scroll_x > 1)
        {
            scroll_x --;
            mouse_tile = MOUSE_LEFT;

        }
        
    
    if (mouse_y <= 1 || key[KEY_UP])
        if (scroll_y > 1)
        {
            scroll_y --;
            mouse_tile = MOUSE_UP;

        }

    
    if (mouse_x >= (game.screen_x-2) || key[KEY_RIGHT])
        if ((iEndX) < (game.map_width-1))
        {
            scroll_x ++;
            mouse_tile = MOUSE_RIGHT;

        }

    if (mouse_y >= (game.screen_y-2) || key[KEY_DOWN])
        if ((iEndY) < (game.map_height-1))
        {
            scroll_y ++;
            mouse_tile = MOUSE_DOWN;

        }

	
}



int cMap::mouse_draw_x()
{
	if (mouse_cell() > -1)
  return ( (( iCellGiveX(mouse_cell()) * 32 ) - (map.scroll_x*32)));
	else
		return -1;
}

int cMap::mouse_draw_y()
{
	if (mouse_cell() > -1)
  return (( (( iCellGiveY(mouse_cell()) * 32 ) - (map.scroll_y*32)))+42);
	else
		return -1;
}
