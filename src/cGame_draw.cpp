/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2011 (c) code by Stefan Hendriks

  -----------------------------------------------
  Game menu items
  -----------------------------------------------
*/

#include "include/d2tmh.h"

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

void cGame::losing()
{
    blit(bmp_winlose, bmp_screen, 0, 0, 0, 0, screen_x, screen_y);

	draw_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_NORMAL].dat, mouse_x, mouse_y);

    if (cMouse::getInstance()->isLeftButtonClicked())
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

    if (cMouse::getInstance()->isLeftButtonClicked())
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
	cGameControlsContext *context = player[HUMAN].getGameControlsContext();
    bool bOrderingUnits=false;

	if (bPlaceIt == false && bPlacedIt==false) {
		int mc = context->getMouseCell();

        if (hover_unit > -1) {
            if (unit[hover_unit].iPlayer == 0) {
                mouse_tile = MOUSE_PICK;
            }
        }


        // Mouse is hovering above a unit
        if (hover_unit > -1) {

        	// wanting to repair UNITS, check if its possible
            if (key[KEY_R] && player[0].iStructures[REPAIR] > 0) {
            	if (unit[hover_unit].iPlayer == HUMAN) {
            		if (unit[hover_unit].iHitPoints < units[unit[hover_unit].iType].hp &&
            				units[unit[hover_unit].iType].infantry == false &&
            				units[unit[hover_unit].iType].airborn == false)	{

						if (cMouse::getInstance()->isLeftButtonClicked()) {
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

		if (cMouse::getInstance()->isRightButtonClicked()) {
			UNIT_deselect_all();
		}

		// single clicking and moving
		if (cMouse::getInstance()->isLeftButtonClicked())
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

							if (!context->isMouseOverStructure() < 0 && game.hover_unit < 0) {
								iAttackCell = mc;
							}

							UNIT_ORDER_ATTACK(i, mc, game.hover_unit, context->getIdOfStructureWhereMouseHovers(), iAttackCell);

							if (game.hover_unit > -1) {
								unit[game.hover_unit].TIMER_blink = 5;
							}

							if (units[unit[i].iType].infantry) {
								bPlayInf=true;
							} else {
								bPlayRep=true;
							}

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
                    PARTICLE_CREATE(mouse_x + (mapCamera->getX()*32), mouse_y + (mapCamera->getY()*32), ATTACK_INDICATOR, -1, -1);
                } else {
                    PARTICLE_CREATE(mouse_x + (mapCamera->getX()*32), mouse_y + (mapCamera->getY()*32), MOVE_INDICATOR, -1, -1);
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

	if (bOrderingUnits) {
		game.selected_structure = -1;
	}

	if (context->isMouseOverStructure())
	{
		if (key[KEY_P])
		{
			int iStr=context->getIdOfStructureWhereMouseHovers();

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
        if (key[KEY_R] && !bOrderingUnits)
        {
        	int structureId = context->getIdOfStructureWhereMouseHovers();

            if (structure[structureId]->getOwner() == 0 &&
                structure[structureId]->getHitPoints() < structures[structure[structureId]->getType()].hp)
            {
                if (cMouse::getInstance()->isLeftButtonClicked())
                {

                    if (!structure[structureId]->isRepairing()) {
                        structure[structureId]->setRepairing(true);
                    } else {
                        structure[structureId]->setRepairing(false);
                    }
                }

                mouse_tile = MOUSE_REPAIR;
            }// MOUSE PRESSED
        }

		if (cMouse::getInstance()->isLeftButtonClicked() && bOrderingUnits == false && !key[KEY_R]) {
			game.selected_structure = context->getIdOfStructureWhereMouseHovers();

			// select list that belongs to structure when it is ours
			cAbstractStructure * theSelectedStructure = structure[game.selected_structure];
			if (theSelectedStructure->getOwner() == HUMAN) {
				int typeOfStructure = theSelectedStructure->getType();
				cListUtils listUtils;
				int listId = listUtils.findListTypeByStructureType(typeOfStructure);
				if (listId != LIST_NONE) {
					player[HUMAN].getSideBar()->setSelectedListId(listId);
				}
			}
		}

	}
}
