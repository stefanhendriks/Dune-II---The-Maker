/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

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

		// draw temp
		draw_trans_sprite(bmp_screen, temp, 1, 42);

		destroy_bitmap(temp);
	}


}

void cGame::draw_placeit()
{
	// this is only done when bPlaceIt=true
	if (player[HUMAN].getSideBar() == NULL) {
		return;
	}

	int iMouseCell = map.mouse_cell();

	if (iMouseCell < 0) {
		return;
	}

	cBuildingListItem *itemToPlace = player[HUMAN].getSideBar()->getList(LIST_CONSTYARD)->getItemToPlace();
	int iStructureID = itemToPlace->getBuildId();
	int iWidth = structures[iStructureID].bmp_width/32;
	int iHeight = structures[iStructureID].bmp_height/32;

	// Draw rectangle
	//int iDrawX = (iCellGiveX(iMouseCell)-map.scroll_x) * 32;
	//int iDrawY = 42 + (iCellGiveY(iMouseCell)-map.scroll_y) * 32;

	int iDrawX = map.mouse_draw_x();
	int iDrawY = map.mouse_draw_y();
	int iCellX = iCellGiveX(iMouseCell);
	int iCellY = iCellGiveY(iMouseCell);

	if (iStructureID == SLAB1)
		draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[PLACE_SLAB1].dat, iDrawX, iDrawY);
	else if (iStructureID == SLAB4)
		draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[PLACE_SLAB4].dat, iDrawX, iDrawY);
	else if (iStructureID == WALL)
		draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[PLACE_WALL].dat, iDrawX, iDrawY);
	else
	{
		BITMAP *temp;
		temp = create_bitmap(structures[iStructureID].bmp_width, structures[iStructureID].bmp_height);
		clear_bitmap(temp);

		//draw_sprite(temp, structures[iStructureID].bmp, 0, 0);
		blit(structures[iStructureID].bmp, temp, 0,0, 0,0, structures[iStructureID].bmp_width, structures[iStructureID].bmp_height);

		draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

		destroy_bitmap(temp);
	}

	bool bOutOfBorder=true;
	bool bMayPlace=true;

	int iTile = PLACE_ROCK;	// rocky placement = ok, but bad for power

	BITMAP *temp;
	temp = create_bitmap(structures[iStructureID].bmp_width, structures[iStructureID].bmp_height);
	clear_bitmap(temp);

	int iTotalBlocks=iWidth*iHeight;
	int iTotalRocks=0.0f;


	int iW = structures[iStructureID].bmp_width/32;
	int iH = structures[iStructureID].bmp_height/32;

#define SCANWIDTH	1

	// check
	int iStartX = iCellX-SCANWIDTH;
	int iStartY = iCellY-SCANWIDTH;

	int iEndX = iCellX + SCANWIDTH + iW;
	int iEndY = iCellY + SCANWIDTH + iH;

	// Fix up the boundries
	FIX_POS(iStartX, iStartY);
	FIX_POS(iEndX, iEndY);


	for (int iX=iStartX; iX < iEndX; iX++)
		for (int iY=iStartY; iY < iEndY; iY++)
		{
			int iCll=iCellMake(iX, iY);
			if (map.cell[iCll].id[MAPID_STRUCTURES] > -1)
			{
				int iID = map.cell[iCll].id[MAPID_STRUCTURES];

				if (structure[iID]->getOwner() == 0)
					bOutOfBorder=false; // connection!
                else
                    bMayPlace=false;
			}

			if (map.cell[iCll].type == TERRAIN_WALL ||
				map.cell[iCll].type == TERRAIN_SLAB)
            {
				bOutOfBorder=false;
                // here we should actually find out if the slab is ours or not...
            }
		}

	if (bOutOfBorder) {
		bMayPlace=false;
	}

	/*


	// Find closest building to X,Y, position.
	for (int i=0; i < MAX_STRUCTURES; i++)
		if (structure[i].isValid())
			if (structure[i].iPlayer == 0)
			{
				int iCell=structure[i].iCell;
				int x, y;

				x=iCellGiveX(iCell);
				y=iCellGiveY(iCell);

				int iDist = ABS_length(iCellX, iCellY, x, y);




						if (iDist < iDistanceToBuilding)
						{
							iClosestBuilding=i;
							iDistanceToBuilding=iDist;
						}
			}

			/*
	char msg[255];
	sprintf(msg, "The closest building is %s, distance %d", structures[structure[iClosestBuilding].iType].name, iDistanceToBuilding);
	logbook(msg);*/


	//if (iClosestBuilding < 0 || iDistanceToBuilding > 2)
	//	bOutOfBorder=true;


	// Draw over it the mask for good/bad placing
	for (int iX=0; iX < iWidth; iX++)
		for (int iY=0; iY < iHeight; iY++)
		{
			iTile = PLACE_ROCK;

			if ((iCellX+iX > 62) || (iCellY + iY > 62))
			{
				bOutOfBorder=true;
				bMayPlace=false;
				break;
			}

			int iCll=iCellMake((iCellX+iX), (iCellY+ iY));

			if (map.cell[iCll].passable == false)
				iTile = PLACE_BAD;

			if (map.cell[iCll].type != TERRAIN_ROCK)
				iTile = PLACE_BAD;

			if (map.cell[iCll].type == TERRAIN_SLAB)
				iTile = PLACE_GOOD;

			// occupied by units or structures
			if (map.cell[iCll].id[MAPID_STRUCTURES] > -1)
				iTile = PLACE_BAD;

			int unitIdOnMap = map.cell[iCll].id[MAPID_UNITS];
			if (unitIdOnMap > -1) {
				if (!unit[unitIdOnMap].bPickedUp) // only when not picked up, take this in (fixes carryall carrying this unit bug)
					iTile = PLACE_BAD;
			}


			// DRAWING & RULER
			if (iTile == PLACE_BAD && iStructureID != SLAB4)
				bMayPlace=false;


			// Count this as GOOD stuff
			if (iTile == PLACE_GOOD)
				iTotalRocks++;


			// Draw bad gfx on spot
			draw_sprite(temp, (BITMAP *)gfxdata[iTile].dat, iX*32, iY*32);
		}

		if (bOutOfBorder) {
			clear_to_color(temp, makecol(160,0,0));
		}


		set_trans_blender(0, 0, 0, 64);

		draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

		// reset to normal
		set_trans_blender(0, 0, 0, 128);

		destroy_bitmap(temp);

		// clicked mouse button
		if (bMousePressedLeft) {
			if (bMayPlace && bOutOfBorder == false)	{
				int iHealthPercent =  50; // the minimum is 50% (with no slabs)

				if (iTotalRocks > 0) {
					iHealthPercent += health_bar(50, iTotalRocks, iTotalBlocks);
				}

                play_sound_id(SOUND_PLACE, -1);

				//cStructureFactory::getInstance()->createStructure(iMouseCell, iStructureID, 0, iHealthPercent);

                player[HUMAN].getStructurePlacer()->placeStructure(iMouseCell, iStructureID, iHealthPercent);

				bPlaceIt=false;

				itemToPlace->decreaseTimesToBuild();
				itemToPlace->setPlaceIt(false);
				itemToPlace->setIsBuilding(false);
				itemToPlace->setProgress(0);
				if (itemToPlace->getTimesToBuild() < 1) {
					player[HUMAN].getItemBuilder()->removeItemFromList(itemToPlace);
				}
			}
		}


	//iDrawX *=32;
	//iDrawY *=32;

	//rect(bmp_screen, iDrawX, iDrawY, iDrawX+(iWidth*32), iDrawY+(iHeight*32), makecol(255,255,255));


}

void cGame::losing()
{
    blit(bmp_winlose, bmp_screen, 0, 0, 0, 0, screen_x, screen_y);

	draw_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_NORMAL].dat, mouse_x, mouse_y);

    if (bMousePressedLeft)
    {
        // OMG, MENTAT IS NOT HAPPY
        state = GAME_LOSEBRIEF;

        if (bSkirmish) {
            game.mission_init();
        }

        // PREPARE NEW MENTAT BABBLE
        iMentatSpeak=-1;

        // FADE OUT
        FADE_OUT();
    }
}

// winning animation
void cGame::winning()
{
    blit(bmp_winlose, bmp_screen, 0, 0, 0, 0, screen_x, screen_y);

	draw_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_NORMAL].dat, mouse_x, mouse_y);

    if (bMousePressedLeft)
    {
        // SELECT YOUR NEXT CONQUEST
        state = GAME_WINBRIEF;

        if (bSkirmish) {
            game.mission_init();
        }

        // PREPARE NEW MENTAT BABBLE
        iMentatSpeak=-1;


        // FADE OUT
        FADE_OUT();
    }
}

// Draw the mouse in combat mode, and do its interactions
void cGame::combat_mouse()
{
    bool bOrderingUnits=false;

	if (bPlaceIt == false && bPlacedIt==false) {
		int mc = map.mouse_cell();

        if (hover_unit > -1) {
            if (unit[hover_unit].iPlayer == 0) {
                mouse_tile = MOUSE_PICK;
            }
        }


        // Mouse is hovering above a unit
        if (hover_unit > -1) {
            // wanting to repair, check if its possible
            if (key[KEY_R] && player[0].iStructures[REPAIR] > 0) {
            	if (unit[hover_unit].iPlayer == HUMAN) {
            		if (unit[hover_unit].iHitPoints < units[unit[hover_unit].iType].hp &&
            				units[unit[hover_unit].iType].infantry == false &&
            				units[unit[hover_unit].iType].airborn == false)	{

						if (bMousePressedLeft) {
							// find closest repair bay to move to

							cStructureUtils structureUtils;
							int	iNewID = structureUtils.findClosestStructureTypeToCell(unit[hover_unit].iCell, REPAIR, &player[HUMAN]);

							if (iNewID > -1) {
								int iCarry = CARRYALL_TRANSFER(hover_unit, structure[iNewID]->getCell() + 2);

								if (iCarry > -1) {
									// yehaw we will be picked up!
									unit[hover_unit].TIMER_movewait = 100;
									unit[hover_unit].TIMER_thinkwait = 100;
								} else {
									logbook("Order move #5");
									UNIT_ORDER_MOVE(hover_unit, structure[iNewID]->getCell());
								}

								unit[hover_unit].TIMER_blink = 5;
								unit[hover_unit].iStructureID = iNewID;
								unit[hover_unit].iGoalCell = structure[iNewID]->getCell();
							}

						}

						mouse_tile = MOUSE_REPAIR;
					}
            	}
            }
	} // IF (HOVER UNIT)

    // when mouse hovers above a valid cell
	if (mc > -1) {

		if (bMousePressedRight) {
			UNIT_deselect_all();
		}

		// single clicking and moving
		if (bMousePressedLeft)
		{
			bool bParticle=false;

            if (mouse_tile == MOUSE_RALLY)
            {
                int id = game.selected_structure;
                if (id > -1)
                    if (structure[id]->getOwner() == 0)
                    {
                        structure[id]->setRallyPoint(mc);
                        bParticle=true;
                    }
            }

            if (hover_unit > -1 && (mouse_tile == MOUSE_NORMAL || mouse_tile == MOUSE_PICK))
			{
				if (unit[hover_unit].iPlayer == 0) {
                    if (!key[KEY_LSHIFT]) {
                        UNIT_deselect_all();
                    }

					unit[hover_unit].bSelected=true;

					if (units[unit[hover_unit].iType].infantry == false) {
						play_sound_id(SOUND_REPORTING, -1);
					} else {
						play_sound_id(SOUND_YESSIR, -1);
					}

				}
			} else {
                bool bPlayInf=false;
                bool bPlayRep=false;

                if (mouse_tile == MOUSE_MOVE) {
                    // any selected unit will move
					for (int i=0; i < MAX_UNITS; i++) {
						if (unit[i].isValid() && unit[i].iPlayer == HUMAN && unit[i].bSelected) {
							UNIT_ORDER_MOVE(i, mc);

							if (units[unit[i].iType].infantry)
								bPlayInf=true;
							else
								bPlayRep=true;

							bParticle=true;
						}
					}
                } else if (mouse_tile == MOUSE_ATTACK) {
                    // check who or what to attack
					for (int i=0; i < MAX_UNITS; i++) {
						if (unit[i].isValid() && unit[i].iPlayer == HUMAN && unit[i].bSelected)	{
							int iAttackCell=-1;

							if (game.hover_structure < 0 && game.hover_unit < 0)
								iAttackCell = mc;

							UNIT_ORDER_ATTACK(i, mc, game.hover_unit, game.hover_structure, iAttackCell);

							if (game.hover_unit > -1)
								unit[game.hover_unit].TIMER_blink = 5;

							if (units[unit[i].iType].infantry)
								bPlayInf=true;
							else
								bPlayRep=true;

							bParticle=true;
						}
					}
                }

                // AUDITIVE FEEDBACK
                if (bPlayInf || bPlayRep) {
                    if (bPlayInf)
                        play_sound_id(SOUND_MOVINGOUT+rnd(2), -1);

                    if (bPlayRep)
                        play_sound_id(SOUND_ACKNOWLEDGED+rnd(3), -1);

                    bOrderingUnits=true;
                }

			}

			if (bParticle) {
                if (mouse_tile == MOUSE_ATTACK) {
                    PARTICLE_CREATE(mouse_x + (map.scroll_x*32), mouse_y + (map.scroll_y*32), ATTACK_INDICATOR, -1, -1);
                } else {
                    PARTICLE_CREATE(mouse_x + (map.scroll_x*32), mouse_y + (map.scroll_y*32), MOVE_INDICATOR, -1, -1);
                }
			}
		}
	}

	if (MOUSE_BTN_LEFT()) {
		// When the mouse is pressed, we will check if the first coordinates are filled in
		// if so, we will update the second coordinates. If the player holds his mouse we
		// keep updating the second coordinates and create a 'border' (to select units with)
		// this way.

		// keep the mouse pressed ;)
		if (mouse_co_x1 > -1 && mouse_co_y1 > -1 ) {
			if (abs(mouse_x-mouse_co_x1) > 4 && abs(mouse_y-mouse_co_y1) > 4) {
			  mouse_co_x2 = mouse_x;
			  mouse_co_y2 = mouse_y;

			  rect(bmp_screen, mouse_co_x1, mouse_co_y1, mouse_co_x2, mouse_co_y2, makecol(game.fade_select, game.fade_select, game.fade_select));
			}

			// Note that we have to fix up the coordinates when checking 'within border'
			// for units (when X2 < X1 for example!)
		} else if (mc > -1) {
			mouse_co_x1 = mouse_x;
			mouse_co_y1 = mouse_y;
		}
	} else {
		 if (mouse_co_x1 > -1 && mouse_co_y1 > -1 &&
                mouse_co_x2 != mouse_co_x1 && mouse_co_y2 != mouse_co_y1 &&
                mouse_co_x2 > -1 && mouse_co_y2 > -1)
            {
                mouse_status = MOUSE_STATE_NORMAL;

                int min_x, min_y;
                int max_x, max_y;

                // sort out borders:
                if (mouse_co_x1 < mouse_co_x2)
                {
                    min_x = mouse_co_x1;
                    max_x = mouse_co_x2;
                }
                else
                {
                    max_x = mouse_co_x1;
                    min_x = mouse_co_x2;
                }

                // Y coordinates
                if (mouse_co_y1 < mouse_co_y2)
                {
                    min_y = mouse_co_y1;
                    max_y = mouse_co_y2;
                }
                else
                {
                    max_y = mouse_co_y1;
                    min_y = mouse_co_y2;
                }

              //  char msg[256];
              //  sprintf(msg, "MINX=%d, MAXX=%d, MINY=%d, MAXY=%d", min_x, min_y, max_x, max_y);
              //  logbook(msg);

                // Now do it!
           // deselect all units
         UNIT_deselect_all();

         bool bPlayRep=false;
         bool bPlayInf=false;

                for (int i=0 ; i < MAX_UNITS; i++)
                {
                    if (unit[i].isValid()) {
                        if (unit[i].iPlayer == 0)
                        {

							// do not select airborn units
							if (units[unit[i].iType].airborn) {
								// always deselect unit:
								unit[i].bSelected = false;
								continue;
							}

                            // now check X and Y coordinates (center of unit now)
                            if (((unit[i].draw_x() + units[unit[i].iType].bmp_width / 2) >= min_x &&
                            	 (unit[i].draw_x() + units[unit[i].iType].bmp_width / 2) <= max_x) &&
                                 (unit[i].draw_y() + units[unit[i].iType].bmp_height / 2 >= min_y &&
                                 (unit[i].draw_y() + units[unit[i].iType].bmp_height / 2) <= max_y) )
                            {
                                // It is in the borders, select it
                                unit[i].bSelected = true;

								if (units[unit[i].iType].infantry) {
                                    bPlayInf=true;
								} else {
                                    bPlayRep=true;
								}

                            }


                        }
                    }
                }

                if (bPlayInf || bPlayRep)
                {

                   if (bPlayRep)
                       play_sound_id(SOUND_REPORTING, -1);

                   if (bPlayInf)
                       play_sound_id(SOUND_YESSIR, -1);

                   bOrderingUnits=true;

                }

            }


		mouse_co_x1=-1;
		mouse_co_y1=-1;
		mouse_co_x2=-1;
		mouse_co_y2=-1;
	}


	} // NOT PLACING STUFF

	if (bOrderingUnits)
		game.selected_structure = -1;

    	// MAKE PRIMARY
	if (game.hover_structure > -1)
	{
		if (key[KEY_P])
		{
			int iStr=game.hover_structure;

			if (structure[iStr]->getOwner() == 0)
			{
				if (structure[iStr]->getType() == LIGHTFACTORY ||
					structure[iStr]->getType() == HEAVYFACTORY ||
					structure[iStr]->getType() == HIGHTECH ||
					structure[iStr]->getType() == STARPORT ||
					structure[iStr]->getType() == WOR ||
					structure[iStr]->getType() == BARRACKS ||
					structure[iStr]->getType() == REPAIR)
					player[0].iPrimaryBuilding[structure[iStr]->getType()] = iStr;
			}
		}

        // REPAIR
        if (key[KEY_R] && bOrderingUnits==false)
        {
            if (structure[game.hover_structure]->getOwner() == 0 &&
                structure[game.hover_structure]->getHitPoints() < structures[structure[game.hover_structure]->getType()].hp)
            {
                if (bMousePressedLeft )
                {

                    if (structure[game.hover_structure]->bRepair==false)
                        structure[game.hover_structure]->bRepair=true;
                    else
                        structure[game.hover_structure]->bRepair=false;
                }

                mouse_tile = MOUSE_REPAIR;
            }// MOUSE PRESSED
        }

		if (bMousePressedLeft && bOrderingUnits == false)
			game.selected_structure = game.hover_structure;

	}

	// DRAWING
	if (mouse_tile == MOUSE_DOWN)
		draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y-16);
	else if (mouse_tile == MOUSE_RIGHT)
		draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y);
	else if (mouse_tile == MOUSE_MOVE || mouse_tile == MOUSE_RALLY)
		draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
    else if (mouse_tile == MOUSE_ATTACK)
		draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
    else if (mouse_tile == MOUSE_REPAIR)
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
    else if (mouse_tile == MOUSE_PICK)
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
	else
		draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);


}
