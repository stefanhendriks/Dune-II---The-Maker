/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

  -----------------------------------------
  Initialization of variables
  Game logic
  Think functions
  -----------------------------------------

  */

#include "d2tmh.h"

cGame::cGame() {
	creditsDrawer = NULL;
}


void cGame::init() {
	iMaxVolume = 220;

	screenshot=0;
	bPlaying=true;

    iGameTimer=0;

    bSkirmish=false;
	iSkirmishStartPoints=2;

    // Alpha (for fading in/out)
    iAlphaScreen=0;           // 255 = opaque , anything else
    iFadeAction=2;            // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

    iRegionState=1;// default = 0
    iRegionScene=0;           // scene
    iRegionSceneAlpha=0;           // scene
    memset(iRegionConquer, -1, sizeof(iRegionConquer));
    memset(iRegionHouse, -1, sizeof(iRegionHouse));
    memset(cRegionText, 0, sizeof(cRegionText));
    //int iConquerRegion[MAX_REGIONS];     // INDEX = REGION NR , > -1 means conquered..

	windowed = true;

	screen_x = 640;
    screen_y = 480;

    bPlaySound = true;

    iSkirmishMap=-1;

	bMousePressedLeft = bMousePressedRight=false;

	iCountSoundMoney=0;
    iSoundsPlayed=0;

	TIMER_scroll=0;
	iScrollSpeed=10;

	mouse_left=mouse_right=false;

    iMusicVolume=128; // volume is 0...

	paths_created=0;
	hover_structure=-1;
	hover_unit=-1;
    iMouseZ=mouse_z;

    state = GAME_MENU;
    //state=GAME_REGION;

    TIMER_money=0;

    iWinQuota=-1;              // > 0 means, get this to win the mission, else, destroy all!


	selected_structure=-1;

	memset(cMessage, 0 , sizeof(cMessage));
	iMessageAlpha=-1;

	// mentat
	memset(mentat_sentence, 0, sizeof(mentat_sentence));

	bPlaceIt=false;			// we do not place
	bPlacedIt=false;

	map_width  = 64;
	map_height = 64;

	mouse_tile = MOUSE_NORMAL;

	memset(version, 0, sizeof(version));
	sprintf(version, "0.4.3");

	fade_select=255;

    bFadeSelectDir=true;    // fade select direction

	iActiveList=LIST_CONSTYARD;		// what list is active to view? (refering to ID of list)

    iRegion=1;          // what region ? (calumative, from player perspective, NOT the actual region number)
	iMission=0;         // calculated by mission loading (region -> mission calculation)
	iHouse=-1;			// what house is selected for playing?

	TIMER_message=0;

    shake_x=0;
    shake_y=0;
    TIMER_shake=0;

    iMusicType=MUSIC_MENU;

    TIMER_movie=0;
    iMovieFrame=-1;


	// TODO:
	// Initialize units/structures/reinforcements, etc.
	TIMER_starport=-2;
	TIMER_ordered=-1;			// timer for ordering stuff ( -1 = nothing, 0 = delivering, > 0 t-minus)
	TIMER_mayorder=1;			// may not order...

	// Initialize upgrades
	memset(iStructureUpgrade, 0, sizeof(iStructureUpgrade));
	memset(iUpgradeTIMER, 0, sizeof(iUpgradeTIMER)); // reset timer
	memset(iUpgradeProgress, -1, sizeof(iUpgradeProgress)); // upgrade progress
	memset(iUpgradeProgressLimit, -1, sizeof(iUpgradeProgressLimit)); // upgrade progress


	memset(iconFrigate, 0, sizeof(iconFrigate)); // what is ordered? (how many of them?)

	map.init();

	for (int i=0; i < MAX_PLAYERS; i++) {
		player[i].init();
        aiplayer[i].init(i);
    }

	// Units & Structures are already initialized in map.init()

    //if (mp3_music != NULL)
	  //almp3_stop_autopoll_mp3(mp3_music); // stop auto poll
	// Load properties
	INI_Install_Game(game_filename);

	if (mp3_music != NULL) {
        almp3_destroy_mp3(mp3_music);
	}

	iMentatSpeak=-1;			// = sentence to draw and speak with (-1 = not ready)

	TIMER_mentat_Speaking=-1;	// speaking = time
	TIMER_mentat_Mouth=0;
	TIMER_mentat_Eyes=0;
	TIMER_mentat_Other=0;

	iMentatMouth=3;			// frames	... (mouth)
	iMentatEyes=3;				// ... for mentat ... (eyes)
	iMentatOther=0;			// ... animations . (book/ring)

    mp3_music=NULL;
}

// TODO: Bad smell (duplicate code)
// initialize for missions
void cGame::mission_init()
{

	iCountSoundMoney=0;
    iSoundsPlayed=0;

    iMusicVolume=128; // volume is 0...

	paths_created=0;
	hover_structure=-1;
	hover_unit=-1;
    iMouseZ=mouse_z;

    TIMER_money=0;

    iWinQuota=-1;              // > 0 means, get this to win the mission, else, destroy all!

	selected_structure=-1;

	memset(cMessage, 0 , sizeof(cMessage));
	iMessageAlpha=-1;

	// mentat
	memset(mentat_sentence, 0, sizeof(mentat_sentence));

	bPlaceIt=false;			// we do not place
	bPlacedIt=false;

	mouse_tile = MOUSE_NORMAL;

	fade_select=255;

    bFadeSelectDir=true;    // fade select direction

	iActiveList=LIST_CONSTYARD;		// what list is active to view? (refering to ID of list)

	TIMER_message=0;

    shake_x=0;
    shake_y=0;
    TIMER_shake=0;

    TIMER_movie=0;
    iMovieFrame=-1;


	// TODO:
	// Initialize units/structures/reinforcements, etc.


	TIMER_starport=-2;
	TIMER_ordered=-1;			// timer for ordering stuff ( -1 = nothing, 0 = delivering, > 0 t-minus)
	TIMER_mayorder=1;			// may not order...

	// Initialize upgrades
	memset(iStructureUpgrade, 0, sizeof(iStructureUpgrade));
	memset(iUpgradeTIMER, 0, sizeof(iUpgradeTIMER)); // reset timer
	memset(iUpgradeProgress, -1, sizeof(iUpgradeProgress)); // upgrade progress
	memset(iUpgradeProgressLimit, -1, sizeof(iUpgradeProgressLimit)); // upgrade progress

	memset(iconFrigate, 0, sizeof(iconFrigate)); // what is ordered? (how many of them?)

    map.init();

    // clear out players but not entirely
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        int h = player[i].getHouse();

        player[i].init();
        player[i].setHouse(h);

        aiplayer[i].init(i);

        if (bSkirmish) {
            player[i].credits = 2500;
        }
    }

    // instantiate the creditDrawer with the appropriate player. Only
    // possible once game has been initialized and player has been created.
    if (creditsDrawer == NULL) {
    	assert(&player[0] != NULL);
    	creditsDrawer = new CreditsDrawer(&player[0]);
    }

    getCreditsDrawer()->setCredits();
}


void cGame::think_winlose()
{
    bool bSucces=false;
    bool bFailed=true;

    // determine if player is still alive
    for (int i=0; i < MAX_STRUCTURES; i++)
        if (structure[i])
            if (structure[i]->getOwner() == 0)
            {
                bFailed=false; // no, we are not failing just yet
                break;
            }

    // determine if any unit is found
    if (bFailed)
    {
        // check if any unit is ours, if not, we have a problem (airborn does not count)
        for (int i=0; i < MAX_UNITS; i++)
            if (unit[i].isValid())
                if (unit[i].iPlayer == 0)
                {
                    bFailed=false;
                    break;
                }
    }


    // win by money quota
    if (iWinQuota > 0)
    {
        if (player[0].credits >= iWinQuota)
        {
            // won!
            bSucces=true;
        }
    }
    else
    {
        // determine if any player (except sandworm) is dead
        bool bAllDead=true;
        for (int i=0; i < MAX_STRUCTURES; i++)
            if (structure[i])
                if (structure[i]->getOwner() > 0 && structure[i]->getOwner() != AI_WORM)
                {
                    bAllDead=false;
                    break;
                }

        if (bAllDead)
        {
                // check units now
            for (int i=0; i < MAX_UNITS; i++)
                if (unit[i].isValid())
                    if (unit[i].iPlayer > 0 && unit[i].iPlayer != AI_WORM)
                        if (units[unit[i].iType].airborn == false)
                        {
                               bAllDead=false;
                               break;
                        }

        }

        if (bAllDead)
            bSucces=true;

    }


    // On succes...
    if (bSucces)
    {
        state = GAME_WINNING;

        shake_x=0;
        shake_y=0;
        TIMER_shake=0;

		play_voice(SOUND_VOICE_07_ATR);

        play_music(MUSIC_WIN);

        // copy over
        blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, screen_x, screen_y);

        draw_sprite(bmp_winlose, (BITMAP *)gfxinter[BMP_WINNING].dat, 77, 182);

    }

    if (bFailed)
    {
        state = GAME_LOSING;

        shake_x=0;
        shake_y=0;
        TIMER_shake=0;

		play_voice(SOUND_VOICE_08_ATR);

        play_music(MUSIC_LOSE);

        // copy over
        blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, screen_x, screen_y);

        draw_sprite(bmp_winlose, (BITMAP *)gfxinter[BMP_LOSING].dat, 77, 182);

    }
}

// MOVIE: Play frames
void cGame::think_movie()
{
    if (gfxmovie != NULL) {
        TIMER_movie++;

        if (TIMER_movie > 20) {
            iMovieFrame++;

            if (gfxmovie[iMovieFrame].type == DAT_END ||
				gfxmovie[iMovieFrame].type != DAT_BITMAP) {
                iMovieFrame=0;
			}
            TIMER_movie=0;
		}
    }
}

//TODO: move to mentat classes
void cGame::think_mentat()
{

	if (TIMER_mentat_Speaking > 0) {
		TIMER_mentat_Speaking--;
	}

	if (TIMER_mentat_Speaking == 0)	{
		// calculate speaking stuff

		iMentatSpeak += 2; // makes 0, 2, etc.

		if (iMentatSpeak > 8) {
			iMentatSpeak = -2;
			TIMER_mentat_Speaking=-1;
			return;
		}

		// lentgh calculation of time
		int iLength = strlen(mentat_sentence[iMentatSpeak]);
		iLength += strlen(mentat_sentence[iMentatSpeak+1]);

		if (iLength < 2) {
			iMentatSpeak = -2;
			TIMER_mentat_Speaking=-1;
			return;
		}

		TIMER_mentat_Speaking = iLength*12;
	}

	if (TIMER_mentat_Mouth > 0) {
		TIMER_mentat_Mouth--;
	} else if (TIMER_mentat_Mouth == 0) {

		if (TIMER_mentat_Speaking > 0) {
			int iOld = iMentatMouth;

			if (iMentatMouth == 0) {
				// when mouth is shut, we wait a bit.
				if (rnd(100) < 45) {
					iMentatMouth += (1 + rnd(4));
				} else {
					TIMER_mentat_Mouth=3; // wait
				}

				// correct any frame
				if (iMentatMouth > 4) {
					iMentatMouth-=5;
				}
			} else {
				iMentatMouth += (1 + rnd(4));

				if (iMentatMouth > 4) {
					iMentatMouth-=5;
				}
			}

			// Test if we did not set the timer, when not, we changed stuff, and we
			// have to make sure we do not reshow the same animation.. which looks
			// odd!
			if (TIMER_mentat_Mouth == 0) {
				if (iMentatMouth == iOld) {
					iMentatMouth++;
				}

				// correct if nescesary:
				if (iMentatMouth > 4) {
					iMentatMouth-=5;
				}

				// Done!
			}
		} else {
			iMentatMouth=0; // when there is no sentence, do not animate mouth
		}

		TIMER_mentat_Mouth=-1; // this way we make sure we do not update it too much
	} // speaking


	if (TIMER_mentat_Eyes > 0)
	{
		TIMER_mentat_Eyes--;
	}
	else
	{
		int i = rnd(100);

        int iWas = iMentatEyes;

        if (i < 30)
			iMentatEyes = 3;
		else if (i >= 30 && i < 60)
			iMentatEyes = 0;
		else
			iMentatEyes=4;

        // its the same
        if (iMentatEyes == iWas)
            iMentatEyes = rnd(4);

        if (iMentatEyes != 4)
            TIMER_mentat_Eyes = 90 + rnd(160);
        else
            TIMER_mentat_Eyes = 30;
	}

	// think wohoo
	if (TIMER_mentat_Other > 0)
	{
		TIMER_mentat_Other--;
	}
	else
	{
		iMentatOther = rnd(5);
	}

}

// thinking for starport prices
// TODO: Move to own class
void cGame::think_starport()
{
//	if (TIMER_mayorder > -1)
//		TIMER_mayorder--;
//
//		if (TIMER_ordered == 0)
//	{
//		// send out a frigate to deliver the package to the starport:
//
//		TIMER_mayorder = 1; // this is being updated by the structure deploying
//
//		// find starport:
//		int iStr = player[0].iPrimaryBuilding[STARPORT];
//
//		// no primary building yet, assign one
//		if (iStr < 0)
//			iStr = FIND_PRIMARY_BUILDING(STARPORT, 0);
//
//		// structure got destroyed or taken over
//		if (iStr > -1)
//			if (structure[iStr] == NULL ||
//				structure[iStr]->getOwner() != 0) // somehow not ours
//				iStr = FIND_PRIMARY_BUILDING(STARPORT, 0); // find new one
//
//		if (iStr > -1)
//		{
//			player[0].iPrimaryBuilding[STARPORT] = iStr; // assign
//			structure[iStr]->setAnimating(true);
//			SPAWN_FRIGATE(0, structure[iStr]->getCell());
//            TIMER_mayorder=10;
//			play_voice(SOUND_VOICE_06_ATR);
//		}
//
//
//		TIMER_ordered = -1; //
//	}
//
//
//	if ( TIMER_ordered > 0)
//	{
//		char msg[255];
//		sprintf(msg, "T-%d before Frigate arrival.", TIMER_ordered);
//		set_message(msg);
//
//		int iSnd=-1;
//
//		if (TIMER_ordered <= 5) {
//			if (player[0].getHouse() == ATREIDES) {
//				iSnd = (SOUND_ATR_S1 + TIMER_ordered)-1;
//			}
//
//			if (player[0].getHouse() == HARKONNEN) {
//				iSnd = (SOUND_HAR_S1 + TIMER_ordered)-1;
//			}
//
//			if (player[0].getHouse() == ORDOS) {
//				iSnd = (SOUND_ORD_S1 + TIMER_ordered)-1;
//			}
//
//			if (iSnd > -1) {
//				play_sound_id(iSnd, -1);
//			}
//		}
//
//		TIMER_ordered--;
//	}
//
//
//	// create list of starport
//	if (TIMER_starport < 0)
//	{
//		// step 1:
//		// clear starport list
//		int i = LIST_STARPORT;
//
//		// step 2:
//		// create new starport list
//
//	for (i=0; i < MAX_UNITTYPES; i++)
//	{
//		if (i == TRIKE ||
//			i == QUAD ||
//			i == TANK ||
//			i == HARVESTER ||
//			i == LAUNCHER ||
//			i == SIEGETANK ||
//			i == CARRYALL)
//		{
//			int iPrice = units[i].cost;
//
//			iPrice -= (iPrice/4);
//
//			iPrice += (rnd(iPrice));
//
//			// add item, with 'random price'
//			list_new_item(LIST_STARPORT, units[i].icon, iPrice, -1, i);
//		}
//	}
//
//	// seconds to change...
//	TIMER_starport = 15 + rnd(30);
//	}
//	else
//	TIMER_starport--; // decrease second
}

// set up a message
// TODO: Move to GUI related code
void cGame::set_message(char msg[266])
{
	TIMER_message=0;
	memset(cMessage, 0, sizeof(cMessage));
	sprintf(cMessage, "%s", msg);
}

// make sure messages fade in/out
void cGame::think_message()
{
    int iLimit=200;
    if (game.isState(GAME_REGION))
        iLimit=600;

	if (cMessage[0] != '\0')
	{
		TIMER_message++;
		if (TIMER_message > iLimit)
		{
			memset(cMessage, 0, sizeof(cMessage));
		}

		iMessageAlpha+=3;
		if (iMessageAlpha > 254)
			iMessageAlpha = 255;
	}
	else
	{
		iMessageAlpha-=6;
		if (iMessageAlpha < 0)
			iMessageAlpha=-1;

		TIMER_message=0;
	}

}

// TODO: Move to music related class (MusicPlayer?)
void cGame::think_music()
{
    // all this does is repeating music in the same theme.

    if (iMusicType < 0)
        return;

	// When mp3 mode
    if (bMp3)
    {

        if (mp3_music != NULL)
        {
            int s = almp3_poll_mp3(mp3_music);

            if (s == ALMP3_POLL_PLAYJUSTFINISHED || s == ALMP3_POLL_NOTPLAYING)
            {
                if (iMusicType == MUSIC_ATTACK)
                    iMusicType = MUSIC_PEACE; // set back to peace

                play_music(iMusicType);
            }
        }
    }
	// MIDI mode
    else
    {
        if (MIDI_music_playing() == false)
        {
			if (DEBUGGING)
				logbook("Going to play the same kind of music (MIDI)");

            if (iMusicType == MUSIC_ATTACK)
                iMusicType = MUSIC_PEACE; // set back to peace

            play_music(iMusicType); //
        }
    }


}

void cGame::poll()
{
    if (iMouseZ > 10 || iMouseZ < -10)
    {
        iMouseZ=0;
        position_mouse_z(0);
    }
    clear(bmp_screen);
	bMousePressedLeft=mouse_pressed_left();
	bMousePressedRight=mouse_pressed_right();
	mouse_tile = MOUSE_NORMAL;

	// change this when selecting stuff
	if (map.mouse_cell() > -1)
    {
        int mc = map.mouse_cell();

	for (int i=0; i < MAX_UNITS; i++)
		if (unit[i].isValid())
			if (unit[i].iPlayer == 0)
				if (unit[i].bSelected) {
					mouse_tile = MOUSE_MOVE;
                    break;
                }


        if (mouse_tile == MOUSE_MOVE)
        {
			if (map.iVisible[mc][0])
			{

            if (map.cell[mc].id[MAPID_UNITS] > -1)
            {
                int id = map.cell[mc].id[MAPID_UNITS];

                if (unit[id].iPlayer > 0)
                    mouse_tile = MOUSE_ATTACK;
            }

            if (map.cell[mc].id[MAPID_STRUCTURES] > -1)
            {
                int id = map.cell[mc].id[MAPID_STRUCTURES];

                if (structure[id]->getOwner() > 0)
                    mouse_tile = MOUSE_ATTACK;
            }

            if (key[KEY_LCONTROL])
                mouse_tile = MOUSE_ATTACK;

            if (key[KEY_ALT])
                mouse_tile = MOUSE_MOVE;

			} // visible
        }
    }

    if (mouse_tile == MOUSE_NORMAL)
    {
        // when selecting a structure
        if (game.selected_structure > -1)
        {
            int id = game.selected_structure;
            if (structure[id]->getOwner() == 0)
                if (key[KEY_LCONTROL])
                    mouse_tile = MOUSE_RALLY;

        }
    }

	bPlacedIt=false;

	//selected_structure=-1;
	hover_structure=-1;
	hover_unit=-1;
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
        if (hover_unit > -1)
        {
            // wanting to repair, check if its possible
            if (key[KEY_R] && player[0].iStructures[REPAIR] > 0) {
            	if (unit[hover_unit].iPlayer == HUMAN)
            		if (unit[hover_unit].iHitPoints < units[unit[hover_unit].iType].hp &&
            				units[unit[hover_unit].iType].infantry == false &&
            				units[unit[hover_unit].iType].airborn == false)	{

							if (bMousePressedLeft)
							{
								// find closest repair bay to move to

									int iNewID = STRUCTURE_FREE_TYPE(0, unit[hover_unit].iCell, REPAIR);

									if (iNewID > -1)
									{
										int iCarry = CARRYALL_TRANSFER(hover_unit, structure[iNewID]->getCell()+2);


										if (iCarry > -1)
										{
											// yehaw we will be picked up!
											unit[hover_unit].TIMER_movewait = 100;
											unit[hover_unit].TIMER_thinkwait = 100;
										}
										else
										{
											logbook("Order move #5");
											UNIT_ORDER_MOVE(hover_unit, structure[iNewID]->getCell());
										}

										unit[hover_unit].TIMER_blink  = 5;
										unit[hover_unit].iStructureID = iNewID;
										unit[hover_unit].iGoalCell = structure[iNewID]->getCell();

									}

							}

							mouse_tile = MOUSE_REPAIR;
						}
            }
        }

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

// Draw sidebar buttons, to switch lists
void cGame::draw_sidebarbuttons()
{
	if (player[HUMAN].getSideBar()) {
		cSideBarDrawer drawer;
		drawer.drawSideBar(player[HUMAN].getSideBar());
	}

	return;
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


// Interaction and drawing of / with the interface
void cGame::gerald()
{
	set_palette(player[0].pal);

	if (bPlaceIt) {
		draw_placeit();
	}

	// black out
	rectfill(bmp_screen, (game.screen_x-160), 0, game.screen_x, game.screen_y-128, makecol(0,0,0));

    // upper bar
    rectfill(bmp_screen, 0, 0, game.screen_x, 42, makecol(0,0,0));

    if (iHouse == ATREIDES)
        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_ATR].dat, 0, 0); // draw interface skeleton
    else if (iHouse == HARKONNEN)
        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_HAR].dat, 0, 0); // draw interface skeleton
    else if (iHouse == ORDOS)
        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_ORD].dat, 0, 0); // draw interface skeleton
//	else
  //      draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD].dat, 0, 0); // draw interface skeleton (THIS IS BAD IF THIS IS SHOWN)

	// Draw the bars
	int iHeight=79 - health_bar(79, player[0].use_power,player[0].has_power);

    int step = (255/79);
    int r = 255-(iHeight*step);
    int g = iHeight*step;

    if (g > 255)
        g = 255;
    if (r < 0) r= 0;

	rectfill(bmp_screen, 488,442, 490, 442-iHeight, makecol(r,g,0));

	iHeight=health_bar(79, player[0].credits, player[0].max_credits);
	rectfill(bmp_screen, 497,442, 499, 442-iHeight, makecol(0,0,255));

	// draw money, etc, etc
	if (getCreditsDrawer() != NULL) {
		getCreditsDrawer()->draw();
	}

	draw_sidebarbuttons();

	// sidebar think about reaction on fps basis here:
	assert(player[HUMAN].getSideBar());
	if (player[HUMAN].getSideBar()) {
		player[HUMAN].getSideBar()->thinkInteraction();
	}

	draw_order();

}

void cGame::mapdraw() {

	map.draw_think();

	map.draw();

	// Only draw units/structures, etc, when we do NOT press D
	if (!key[KEY_D] || !key[KEY_TAB])
	{
	    map.draw_structures(0);
	}

    // draw layer 1 (beneath units, on top of terrain
    map.draw_particles(1);

	map.draw_units();

    map.draw_bullets();

    map.draw_structures(2); // draw layer 2
    map.draw_structures_health();
	map.draw_units_2nd();

	map.draw_particles(0);

	map.draw_shroud();

	map.draw_minimap();
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

void cGame::combat()
{

    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------


	// DO MAP DRAWING
	mapdraw();

	bPlacedIt = bPlaceIt;

	// DO INTERFACE DRAWING
	gerald();

	draw_message();

    // think win/lose
    think_winlose();

	// DO COMBAT MOUSE
	combat_mouse();

}

void cGame::setup_list() {

}

void cGame::draw_mentat(int iType)
{
	select_palette( general_palette  );

    // movie
    draw_movie(iType);

	// draw proper background
	if (iType == ATREIDES)
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATA].dat, 0, 0);
	else if (iType == HARKONNEN)
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATH].dat, 0, 0);
	else if (iType == ORDOS)
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATO].dat, 0, 0);
	else // bene
    {
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MENTATM].dat, 0, 0);

        // when not speaking, draw 'do you wish to join house x'
        if (TIMER_mentat_Speaking < 0)
        {
            draw_sprite(bmp_screen, (BITMAP *)gfxmentat[MEN_WISH].dat, 16, 16);

			// todo house description
        }
    }

    if (bMousePressedLeft)
        if (TIMER_mentat_Speaking > 0)
        {
            TIMER_mentat_Speaking = 1;
        }

    // SPEAKING ANIMATIONS IS DONE IN MENTAT()



}

void cGame::MENTAT_draw_eyes(int iMentat)
{
	int iDrawX=128;
	int iDrawY=240;

	// now draw eyes
	if (iMentat  < 0)
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BEN_EYES01+ iMentatEyes].dat, iDrawX, iDrawY);



    if (iMentat  == HARKONNEN)
    {
        iDrawX = 64;
        iDrawY = 256;
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[HAR_EYES01+ iMentatEyes].dat, iDrawX, iDrawY);
   }

    if (iMentat  == ATREIDES)
    {
        iDrawX = 80;
        iDrawY = 241;
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ATR_EYES01+ iMentatEyes].dat, iDrawX, iDrawY);
   }


    if (iMentat  == ORDOS)
    {
        iDrawX = 32;
        iDrawY = 240;
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ORD_EYES01+ iMentatEyes].dat, iDrawX, iDrawY);
   }

}


void cGame::MENTAT_draw_mouth(int iMentat)
{

	int iDrawX=112;
	int iDrawY=272;

	// now draw speaking and such
	if (iMentat  < 0)
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BEN_MOUTH01+ iMentatMouth].dat, iDrawX, iDrawY);


    if (iMentat  == HARKONNEN)
    {
        iDrawX = 64;
        iDrawY = 288;
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[HAR_MOUTH01+ iMentatMouth].dat, iDrawX, iDrawY);
    }

    // 31, 270

    if (iMentat  == ATREIDES)
    {
        iDrawX = 80;
        iDrawY = 273;
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ATR_MOUTH01+ iMentatMouth].dat, iDrawX, iDrawY);
    }

    if (iMentat  == ORDOS)
    {
        iDrawX = 31;
        iDrawY = 270;
		draw_sprite(bmp_screen, (BITMAP *)gfxmentat[ORD_MOUTH01+ iMentatMouth].dat, iDrawX, iDrawY);
    }
}


// draw mentat
void cGame::mentat(int iType)
{
    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------

    bool bFadeOut=false;

	// draw speaking animation, and text, etc

	if (iType > -1)
		draw_mentat(iType); // draw houses

	MENTAT_draw_mouth(iType);
	MENTAT_draw_eyes(iType);

	// draw text
	if (iMentatSpeak >= 0)
	{
	alfont_textprintf(bmp_screen, bene_font, 17,17, makecol(0,0,0), "%s", mentat_sentence[iMentatSpeak]);
	alfont_textprintf(bmp_screen, bene_font, 16,16, makecol(255,255,255), "%s", mentat_sentence[iMentatSpeak]);
	alfont_textprintf(bmp_screen, bene_font, 17,33, makecol(0,0,0), "%s", mentat_sentence[iMentatSpeak+1]);
	alfont_textprintf(bmp_screen, bene_font, 16,32, makecol(255,255,255), "%s", mentat_sentence[iMentatSpeak+1]);
	}

	// mentat mouth
	if (TIMER_mentat_Mouth <= 0)
	{
		TIMER_mentat_Mouth = 13+rnd(5);
	}


    if (TIMER_mentat_Speaking < 0)
    {
        // NO
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_REPEAT].dat, 293, 423);

        if ((mouse_x > 293 && mouse_x < 446) && (mouse_y > 423 && mouse_y < 468))
            if (bMousePressedLeft)
            {
                // head back to choose house
                iMentatSpeak=-1; // prepare speaking
                //state = GAME_HOUSE;
            }

        // YES/PROCEED
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_PROCEED].dat, 466, 423);
        if ((mouse_x > 446 && mouse_x < 619) && (mouse_y >423 && mouse_y < 468))
            if (bMousePressedLeft)
            {
                if (isState(GAME_BRIEFING))
                {
                // proceed, play mission
                state = GAME_PLAYING;

                // CENTER MOUSE
                position_mouse(320, 240);

                bFadeOut=true;

                play_music(MUSIC_PEACE);
                }
                else if (state == GAME_WINBRIEF)
                {
                //
					if (bSkirmish)
					{
						state = GAME_SETUPSKIRMISH;
						play_music(MUSIC_MENU);
					}
					else
					{

                    state = GAME_REGION;
                    REGION_SETUP(game.iMission, game.iHouse);


                    // PLAY THE MUSIC
                    play_music(MUSIC_CONQUEST);
					}

					// PREPARE NEW MENTAT BABBLE
                    iMentatSpeak=-1;

                    bFadeOut=true;
                }
                else if (state == GAME_LOSEBRIEF)
                {
                //
					if (bSkirmish)
					{
						state = GAME_SETUPSKIRMISH;
						play_music(MUSIC_MENU);
					}
					else
					{
						if (game.iMission > 1)
						{
							state = GAME_REGION;

							game.iMission--; // we did not win
							REGION_SETUP(game.iMission, game.iHouse);

							// PLAY THE MUSIC
							play_music(MUSIC_CONQUEST);
						}
						else
						{
							state = GAME_BRIEFING;
							play_music(MUSIC_BRIEFING);
						}

					}
                    // PREPARE NEW MENTAT BABBLE
                    iMentatSpeak=-1;

                    bFadeOut=true;
                }

            }


    }



	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);

    if (bFadeOut)
        FADE_OUT();

}

// draw menu
void cGame::menu()
{
    // FADING STUFF
    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------

    bool bFadeOut=false;

	if (DEBUGGING)
	{

		for (int x=0; x < game.screen_x; x+= 60)
		{
			for (int y=0; y < game.screen_y; y+= 20)
			{
				rect(bmp_screen, x, y, x+50, y+10, makecol(64,64,64));
				putpixel(bmp_screen, x, y, makecol(255,255,255));
				alfont_textprintf(bmp_screen, bene_font, x, y, makecol(32,32,32), "Debug");
			}

		}
	}

	// draw menu
	draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_D2TM].dat,  0, 0);
    GUI_DRAW_FRAME(257, 319, 130,143);

	// Buttons:

	// PLAY
	if ((mouse_x >= 246 && mouse_x <=373) &&
		(mouse_y >= 323 && mouse_y <=340))
	{
        alfont_textprintf(bmp_screen, bene_font, 261, 324, makecol(0,0,0), "Campaign");
        alfont_textprintf(bmp_screen, bene_font, 261, 323, makecol(255,0,0), "Campaign");

		if (bMousePressedLeft)
        {
			state = GAME_HOUSE; // select house
            bFadeOut=true;
        }

	}
    else
    {
        alfont_textprintf(bmp_screen, bene_font, 261, 324, makecol(0,0,0), "Campaign");
        alfont_textprintf(bmp_screen, bene_font, 261, 323, makecol(255,255,255), "Campaign");

    }


	// SKIRMISH
	if ((mouse_x >= 246 && mouse_x <=373) &&
		(mouse_y >= 344 && mouse_y <=362))
	{
        alfont_textprintf(bmp_screen, bene_font, 261, 344, makecol(0,0,0), "Skirmish");
        alfont_textprintf(bmp_screen, bene_font, 261, 343, makecol(255,0,0), "Skirmish");

        if (bMousePressedLeft)
        {
            game.state = GAME_SETUPSKIRMISH;
            bFadeOut=true;
            INI_PRESCAN_SKIRMISH();

			game.mission_init();

			for (int p=0; p < AI_WORM; p++)
			{
				player[p].credits = 2500;
				player[p].iTeam = p;
			}
        }


	//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	}
    else
    {
        alfont_textprintf(bmp_screen, bene_font, 261, 344, makecol(0,0,0), "Skirmish");
        alfont_textprintf(bmp_screen, bene_font, 261, 343, makecol(255,255,255), "Skirmish");

    }

    // LOAD
    if ((mouse_x >= 246 && mouse_x <=373) &&
		(mouse_y >= 364 && mouse_y <=382))
	{
        alfont_textprintf(bmp_screen, bene_font, 261, 364, makecol(0,0,0), "Multiplayer");
        alfont_textprintf(bmp_screen, bene_font, 261, 363, makecol(255,0,0), "Multiplayer");

        if (bMousePressedLeft)
        {
            //game.state = GAME_SETUPSKIRMISH;
            bFadeOut=true;

			// check if the game can be server or client
			//bCanBeServerOrClient();
        }

	//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	}
    else
    {
        alfont_textprintf(bmp_screen, bene_font, 261, 364, makecol(0,0,0), "Multiplayer");
        alfont_textprintf(bmp_screen, bene_font, 261, 363, makecol(255,255,255), "Multiplayer");

    }

    // OPTIONS
	if ((mouse_x >= 246 && mouse_x <=373) &&
		(mouse_y >= 384 && mouse_y <=402))
	{
        alfont_textprintf(bmp_screen, bene_font, 261, 384, makecol(0,0,0), "Load");
        alfont_textprintf(bmp_screen, bene_font, 261, 383, makecol(255,0,0), "Load");

        if (bMousePressedLeft)
        {
//            game.state = GAME_SETUPSKIRMISH;
            bFadeOut=true;
        }


	//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	}
    else
    {
        alfont_textprintf(bmp_screen, bene_font, 261, 384, makecol(0,0,0), "Load");
        alfont_textprintf(bmp_screen, bene_font, 261, 383, makecol(255,255,255), "Load");
    }

    // HALL OF FAME
	if ((mouse_x >= 246 && mouse_x <=373) &&
		(mouse_y >= 404 && mouse_y <=422))
	{
        alfont_textprintf(bmp_screen, bene_font, 261, 404, makecol(0,0,0), "Options");
        alfont_textprintf(bmp_screen, bene_font, 261, 403, makecol(255,0,0), "Options");

        if (bMousePressedLeft)
        {
//            game.state = GAME_SETUPSKIRMISH;
            bFadeOut=true;
        }

	//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	}
    else
    {
        alfont_textprintf(bmp_screen, bene_font, 261, 404, makecol(0,0,0), "Options");
        alfont_textprintf(bmp_screen, bene_font, 261, 403, makecol(255,255,255), "Options");
    }

	// EXIT
	if ((mouse_x >= 246 && mouse_x <=373) &&
		(mouse_y >= 424 && mouse_y <=442))
	{

        alfont_textprintf(bmp_screen, bene_font, 261, 424, makecol(0,0,0), "Hall of Fame");
        alfont_textprintf(bmp_screen, bene_font, 261, 423, makecol(255,0,0), "Hall of Fame");
	}
    else
    {
        alfont_textprintf(bmp_screen, bene_font, 261, 424, makecol(0,0,0), "Hall of Fame");
        alfont_textprintf(bmp_screen, bene_font, 261, 423, makecol(255,255,255), "Hall of Fame");
    }


    // EXIT
	if ((mouse_x >= 246 && mouse_x <=373) &&
		(mouse_y >= 444 && mouse_y <=462))
	{
        alfont_textprintf(bmp_screen, bene_font, 261, 444, makecol(0,0,0), "Exit");
        alfont_textprintf(bmp_screen, bene_font, 261, 443, makecol(255,0,0), "Exit");

		// quit
		if (bMousePressedLeft)
			game.bPlaying = false;
	}
    else
    {
        alfont_textprintf(bmp_screen, bene_font, 261, 444, makecol(0,0,0), "Exit");
        alfont_textprintf(bmp_screen, bene_font, 261, 443, makecol(255,255,255), "Exit");
    }


    alfont_textprintf(bmp_screen, bene_font, 291, 1, makecol(64,64,64), "CREDITS");

    if (mouse_y < 24)
        alfont_textprintf(bmp_screen, bene_font, 290, 0, makecol(255,0,0), "CREDITS");
    else
        alfont_textprintf(bmp_screen, bene_font, 290, 0, makecol(255,255,255), "CREDITS");

	// version
    //alfont_textprintf(bmp_screen, bene_font, 621,467, makecol(64,64,64), "%s", version);
	//alfont_textprintf(bmp_screen, bene_font, 620,466, makecol(255,255,255), "%s", version);

    // version (demo)
    alfont_textprintf(bmp_screen, bene_font, 581,467, makecol(64,64,64), "%s", version);
	alfont_textprintf(bmp_screen, bene_font, 580,466, makecol(255,255,255), "%s", version);

	// mp3 addon?
	if (bMp3)
    {
        alfont_textprintf(bmp_screen, bene_font, 1,467, makecol(64,64,64), "MP3");
		alfont_textprintf(bmp_screen, bene_font, 0,466, makecol(255,255,255), "MP3");
    }
	else
    {
        alfont_textprintf(bmp_screen, bene_font, 1,467, makecol(64,64,64), "MIDI");
		alfont_textprintf(bmp_screen, bene_font, 0,466, makecol(255,255,255), "MIDI");
    }

   	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);


	if (key[KEY_ESC])
		bPlaying=false;

    if (bFadeOut)
        game.FADE_OUT();

}

void cGame::setup_skirmish()
{
    // FADING STUFF
    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------

    bool bFadeOut=false;

    draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_GAME_DUNE].dat, 0, 350);

	for (int dy=0; dy < game.screen_y; dy+=2) {
		line(bmp_screen, 0, dy, 640, dy, makecol(0,0,0));
	}

    // title box
    GUI_DRAW_FRAME(-1, -1, 642, 21);

    // title name
    alfont_textprintf(bmp_screen, bene_font, 280,3, makecol(0,0,0), "Skirmish");
    alfont_textprintf(bmp_screen, bene_font, 280,2, makecol(255,0,0), "Skirmish");

    // box at the right
    GUI_DRAW_FRAME(364, 21, 276, 443);

	// draw box for map data
    GUI_DRAW_FRAME(510, 26, 129, 129);
    //rectfill(bmp_screen, 640-130, 26, 640-1, 26+129, makecol(186,190,149));
    //rect(bmp_screen, 640-130, 26, 640-1, 26+129, makecol(227,229,211));

    // rectangle for map list
    rectfill(bmp_screen, 366, (26+128)+4, 638, 461, makecol(32,32,32));
    rect(bmp_screen, 366, (26+128)+4, 638, 461, makecol(227,229,211));

	int iStartingPoints=0;

	// draw preview map (if any)
	if (iSkirmishMap > -1)
	{
		if (iSkirmishMap > 0)
		{
			if (PreviewMap[iSkirmishMap].name[0] != '\0')
				if (PreviewMap[iSkirmishMap].terrain)
					draw_sprite(bmp_screen, PreviewMap[iSkirmishMap].terrain, 640-129, 27);

		for (int s=0; s < 5; s++)
			if (PreviewMap[iSkirmishMap].iStartCell[s] > -1)
				iStartingPoints++;
		}
		else
		{
			iStartingPoints = iSkirmishStartPoints;

			// when mouse is hovering, draw it, else do not
			if ((mouse_x >= (640-129) && mouse_x < 640) && (mouse_y >= 27 && mouse_y < 160))
			{
				if (PreviewMap[iSkirmishMap].name[0] != '\0')
					if (PreviewMap[iSkirmishMap].terrain)
						draw_sprite(bmp_screen, PreviewMap[iSkirmishMap].terrain, 640-129, 27);
			}
			else
			{
				if (PreviewMap[iSkirmishMap].name[0] != '\0')
					if (PreviewMap[iSkirmishMap].terrain)
						draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat, 640-129, 27);
			}
		}

	}

	alfont_textprintf(bmp_screen, bene_font, 366, 27, makecol(0,0,0),"Startpoints: %d", iStartingPoints);
	alfont_textprintf(bmp_screen, bene_font, 366, 26, makecol(255,255,255),"Startpoints: %d", iStartingPoints);

	bool bDoRandomMap=false;

	if ((mouse_x >= 366 && mouse_x <= (640-130)) && (mouse_y >= 27 && mouse_y <= 43))
	{
		alfont_textprintf(bmp_screen, bene_font, 366, 27, makecol(0,0,0),"Startpoints: %d", iStartingPoints);
		alfont_textprintf(bmp_screen, bene_font, 366, 26, makecol(255,0,0),"Startpoints: %d", iStartingPoints);

		if (bMousePressedLeft)
		{
			iSkirmishStartPoints++;

			if (iSkirmishStartPoints > 4) {
				iSkirmishStartPoints = 2;
			}

			bDoRandomMap=true;
		}

		if (bMousePressedRight)
		{
			iSkirmishStartPoints--;

			if (iSkirmishStartPoints < 2) {
				iSkirmishStartPoints = 4;
			}

			bDoRandomMap=true;
		}
	}

	// TITLE: Map list
    GUI_DRAW_FRAME_PRESSED(367, 159, 254, 17);

	//rectfill(bmp_screen, 367, 159, 637, 159+17, makecol(186,190,149));
	//rect(bmp_screen, 367,159, 637, 159+17,makecol(255,255,255));

	alfont_textprintf(bmp_screen, bene_font, 447, 160, makecol(0,0,0),"Map List" );

	int const iHeightPixels=17;

	int iDrawY=-1;
	int iDrawX=367;
	int iEndX=637-16;
	int iColor=makecol(255,255,255);

	// scroll barr
	if ((mouse_x >= (iEndX+1) && mouse_x <= 637) && (mouse_y >= 159+1 && mouse_y <= 477))
	{
		// hovers
		rectfill(bmp_screen, iEndX+1, 159, 637, 460, makecol(128,128,128));
		rect(bmp_screen, iEndX+1,159, 637, 460,makecol(64,64,64));
	}
	else
	{
		rectfill(bmp_screen, iEndX+1, 159, 637, 460, makecol(186,190,149));
		rect(bmp_screen, iEndX+1,159, 637, 460,makecol(64,64,64));
	}

    // for every map that we read , draw here
    for (int i=0; i < MAX_SKIRMISHMAPS; i++)
    {
		if (PreviewMap[i].name[0] != '\0')
		{
			bool bHover=false;

			iDrawY=159+(i*iHeightPixels)+i+iHeightPixels; // skip 1 bar because the 1st = 'random map'

            bHover = GUI_DRAW_FRAME(iDrawX, iDrawY, 254, iHeightPixels);


			//if ((mouse_x >= iDrawX && mouse_x <= iEndX) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+iHeightPixels)))
			//	bHover=true;

			if (bHover==false)
			{
				//rectfill(bmp_screen, 367, iDrawY, iEndX,iDrawY+iHeightPixels, makecol(186,190,149));
				//rect(bmp_screen, 367, iDrawY, iEndX,iDrawY+iHeightPixels, makecol(255,255,255));
			}
			else
			{
				//rectfill(bmp_screen, 367, iDrawY-1, iEndX,iDrawY+iHeightPixels, makecol(128,128,128));
				//rect(bmp_screen, 367, iDrawY-1, iEndX,iDrawY+iHeightPixels,makecol(186,190,149));
                GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, 254, iHeightPixels);
			}


			// dark sides
		//	line(bmp_screen, 367, iDrawY+iHeightPixels, iEndX,iDrawY+iHeightPixels, makecol(128,128,128));
		//	line(bmp_screen, iEndX, iDrawY, iEndX,iDrawY+iHeightPixels, makecol(128,128,128));

			iColor=makecol(255,255,255);

			if (bHover)
			{
			// Mouse reaction
				iColor = makecol(255,207,41);

				if (bMousePressedLeft)
				{
					iSkirmishMap=i;

					if (i == 0)
						bDoRandomMap=true;
				}


			}

			if (i==iSkirmishMap)
				iColor=makecol(255,0, 0);

			alfont_textprintf(bmp_screen, bene_font, 368,iDrawY+3, makecol(0,0,0), PreviewMap[i].name);
			alfont_textprintf(bmp_screen, bene_font, 368,iDrawY+2, iColor, PreviewMap[i].name);
		}
    }

	rectfill(bmp_screen, 0, 21, 363, 37, makecol(128,128,128));
	line(bmp_screen, 0, 38, 363, 38, makecol(255,255,255));

	// player list
	rectfill(bmp_screen, 0, 39, 363, 122, makecol(32,32,32));
	line(bmp_screen, 0, 123, 363, 123, makecol(255,255,255));

	// bottom bar
	rectfill(bmp_screen, 0, 464, 640, 480, makecol(32,32,32));

	alfont_textprintf(bmp_screen, bene_font, 4, 26, makecol(0,0,0), "Player      House      Credits       Units    Team");
	alfont_textprintf(bmp_screen, bene_font, 4, 25, makecol(255,255,255), "Player      House      Credits       Units    Team");



	bool bHover=false;

	// draw players who will be playing ;)
	for (int p=0; p < (AI_WORM-1); p++)
	{
		int iDrawY=40+(p*22);
		if (p < iStartingPoints)
		{
			// player playing or not
			if (p == 0)
			{
				alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "Human");
				alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(255,255,255), "Human");
			}
			else
			{
				alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "  CPU");
				if ((mouse_x >= 4 && mouse_x <= 73) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
				{
					if (aiplayer[p].bPlaying)
						alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(fade_select,0,0), "  CPU");
					else
						alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol((fade_select/2),(fade_select/2),(fade_select/2)), "  CPU");

					if (bMousePressedLeft && p > 1)
					{
						if (aiplayer[p].bPlaying)
							aiplayer[p].bPlaying=false;
						else
							aiplayer[p].bPlaying=true;


					}
				}
				else
				{
					if (aiplayer[p].bPlaying)
						alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(255,255,255), "  CPU");
					else
						alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(128,128,128), "  CPU");
				}

			}

			// HOUSE
			bHover=false;
			char cHouse[30];
			memset(cHouse, 0, sizeof(cHouse));

			if (player[p].getHouse() == ATREIDES) {
				sprintf(cHouse, "Atreides");
			} else if (player[p].getHouse() == HARKONNEN) {
				sprintf(cHouse, "Harkonnen");
			} else if (player[p].getHouse() == ORDOS) {
				sprintf(cHouse, "Ordos");
			} else if (player[p].getHouse() == SARDAUKAR) {
				sprintf(cHouse, "Sardaukar");
			} else {
				sprintf(cHouse, "Random");
			}

			alfont_textprintf(bmp_screen, bene_font, 74,iDrawY+1, makecol(0,0,0), "%s", cHouse);

//			rect(bmp_screen, 74, (40+(p*22)), 150, (40+(p*22))+16, makecol(255,255,255));

			if ((mouse_x >= 74 && mouse_x <= 150) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
				bHover=true;

			if (p == 0)
			{
				alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(255,255,255), "%s", cHouse);
			}
			else
			{
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(255,255,255), "%s", cHouse);
				else
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(128,128,128), "%s", cHouse);

			}

			if (bHover)
			{
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(fade_select,0,0), "%s", cHouse);
				else
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol((fade_select/2),(fade_select/2),(fade_select/2)), "%s", cHouse);


				if (bMousePressedLeft)
				{
					player[p].setHouse((player[p].getHouse()+1));
					if (p > 0)
					{
						if (player[p].getHouse() > 4) {
							player[p].setHouse(0);
						}
					}
					else
					{
						if (player[p].getHouse() > 3) {
							player[p].setHouse(0);
						}
					}
				}

				if (bMousePressedRight)
				{
					player[p].setHouse((player[p].getHouse()-1));
					if (p > 0)
					{
						if (player[p].getHouse() < 0) {
							player[p].setHouse(4);
						}
					}
					else
					{
						if (player[p].getHouse() < 0) {
							player[p].setHouse(3);
						}
					}
				}
			}

			// Credits
			bHover=false;

			alfont_textprintf(bmp_screen, bene_font, 174,iDrawY+1, makecol(0,0,0), "%d", (int)player[p].credits);

			//rect(bmp_screen, 174, iDrawY, 230, iDrawY+16, makecol(255,255,255));

			if ((mouse_x >= 174 && mouse_x <= 230) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
				bHover=true;

			if (p == 0)
			{
				alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(255,255,255), "%d", (int)player[p].credits);
			}
			else
			{
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(255,255,255), "%d", (int)player[p].credits);
				else
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(128,128,128), "%d", (int)player[p].credits);

			}

			if (bHover)
			{
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(fade_select,0,0), "%d", (int)player[p].credits);
				else
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol((fade_select/2),(fade_select/2),(fade_select/2)), "%d", player[p].credits);

				if (bMousePressedLeft)
				{
					player[p].credits += 500;
					if (player[p].credits > 10000)
						player[p].credits = 1000;
				}

				if (bMousePressedRight)
				{
					player[p].credits -= 500;
					if (player[p].credits < 1000)
						player[p].credits = 10000;
				}
			}

			// Units
			bHover = false;

			alfont_textprintf(bmp_screen, bene_font, 269,iDrawY+1, makecol(0,0,0), "%d",aiplayer[p].iUnits);

			//rect(bmp_screen, 269, iDrawY, 290, iDrawY+16, makecol(255,255,255));

			if ((mouse_x >= 269 && mouse_x <= 290) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
				bHover=true;

			if (p == 0)
			{
				alfont_textprintf(bmp_screen, bene_font, 269,iDrawY, makecol(255,255,255), "%d", aiplayer[p].iUnits);
			}
			else
			{
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 269,iDrawY, makecol(255,255,255), "%d", aiplayer[p].iUnits);
				else
					alfont_textprintf(bmp_screen, bene_font, 269,iDrawY, makecol(128,128,128), "%d", aiplayer[p].iUnits);

			}

			if (bHover)
			{
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 269,iDrawY, makecol(fade_select,0,0), "%d", aiplayer[p].iUnits);
				else
					alfont_textprintf(bmp_screen, bene_font, 269,iDrawY, makecol((fade_select/2),(fade_select/2),(fade_select/2)), "%d", aiplayer[p].iUnits);

				if (bMousePressedLeft)
				{
					aiplayer[p].iUnits++;
					if (aiplayer[p].iUnits > 10)
						aiplayer[p].iUnits = 1;
				}

				if (bMousePressedRight)
				{
					aiplayer[p].iUnits--;
					if (aiplayer[p].iUnits < 1)
						aiplayer[p].iUnits = 10;
				}
			}

			// Team
			bHover=false;
		}
	}


    GUI_DRAW_FRAME(-1, 465, 642, 21);

    // back
    alfont_textprintf(bmp_screen, bene_font, 0, 467, makecol(0,0,0), " BACK");
    alfont_textprintf(bmp_screen, bene_font, 0, 466, makecol(255,255,255), " BACK");

    // start
    alfont_textprintf(bmp_screen, bene_font, 580, 467, makecol(0,0,0), "START");
    alfont_textprintf(bmp_screen, bene_font, 580, 466, makecol(255,255,255), "START");


	if (bDoRandomMap) {
		map.randommap();
	}

    // back
    if ((mouse_x >= 0 && mouse_x <= 50) && (mouse_y >= 465 && mouse_y <= 480))
    {
        alfont_textprintf(bmp_screen, bene_font, 0, 466, makecol(255,0,0), " BACK");

        if (bMousePressedLeft)
        {
            bFadeOut=true;
            state = GAME_MENU;
        }
    }

    if ((mouse_x >= 580 && mouse_x <= 640) && (mouse_y >= 465 && mouse_y <= 480))
        alfont_textprintf(bmp_screen, bene_font, 580, 466, makecol(255,0,0), "START");


    cCellCalculator *cellCalculator = new cCellCalculator();
    // START
	if ((mouse_x >= 580 && mouse_x <= 640) && (mouse_y >= 465 && mouse_y <= 480) && bMousePressedLeft && iSkirmishMap > -1)
	{
        // Starting skirmish mode
		bSkirmish=true;

        /* set up starting positions */
		int iStartPositions[5];
		int iMax=0;
		for (int s=0; s < 5; s++)
		{
			iStartPositions[s] = PreviewMap[iSkirmishMap].iStartCell[s];

			if (PreviewMap[iSkirmishMap].iStartCell[s] > -1)
				iMax=s;
		}

        // REGENERATE MAP DATA FROM INFO
        if (iSkirmishMap > -1)
        {
            for (int c=0; c < MAX_CELLS; c++)
            {
                map.create_spot(c, PreviewMap[iSkirmishMap].mapdata[c], 0);
            }

            map.smooth();
        }

		int iShuffles=3;

		while (iShuffles > 0)
		{
			int one = rnd(iMax);
			int two = rnd(iMax);

			if (one == two)
			{
				if (rnd(100) < 25)
					iShuffles--;

				continue;
			}

			int back = iStartPositions[one];
			iStartPositions[one] = iStartPositions[two];
			iStartPositions[two] = back; // backup

			iShuffles--;
		}

		// set up players and their units
		for (int p=0; p < AI_WORM; p++)	{

			int iHouse = player[p].getHouse();

			// house = 0 , random.
			if (iHouse==0 && p < 4) { // (all players above 4 are non-playing AI 'sides'
				bool bOk=false;

				while (bOk == false) {
					if (p > 0)
						iHouse = rnd(4)+1;
					else
						iHouse = rnd(3)+1;

					bool bFound=false;
					for (int pl=0; pl < AI_WORM; pl++) {
						if (player[pl].getHouse() > 0 && player[pl].getHouse() == iHouse) {
							bFound=true;
						}
					}

					if (!bFound) {
						bOk=true;
					}


				}
			}

			if (p == 5) {
				iHouse = FREMEN;
			}

		    player[p].setHouse(iHouse);

			// not playing.. do nothing
			if (aiplayer[p].bPlaying == false) {
				continue;
			}

			// set credits
			player[p].focus_cell = iStartPositions[p];

			// Set map position
			if (p == 0) {
				map.set_pos(-1, -1, player[p].focus_cell);
			}

			// create constyard
			cAbstractStructure *s = cStructureFactory::getInstance()->createStructure(player[p].focus_cell, CONSTYARD, p);

			// when failure, create mcv instead
			if (s == NULL) {
				UNIT_CREATE(player[p].focus_cell, MCV, p, true);
			}

			// amount of units
			int u=0;

			// create units
			while (u < aiplayer[p].iUnits)
			{
				int iX=iCellGiveX(player[p].focus_cell);
				int iY=iCellGiveY(player[p].focus_cell);
				int iType=rnd(12);

				iX-=4;
				iY-=4;
				iX+=rnd(9);
				iY+=rnd(9);

				// convert house specific stuff
				if (player[p].getHouse() == ATREIDES) {
					if (iType == DEVASTATOR || iType == DEVIATOR) {
						iType = SONICTANK;
					}

					if (iType == TROOPERS) {
						iType = INFANTRY;
					}

					if (iType == TROOPER) {
						iType = SOLDIER;
					}

					if (iType == RAIDER) {
						iType = TRIKE;
					}
				}

				// ordos
				if (player[p].getHouse() == ORDOS)
				{
					if (iType == DEVASTATOR || iType == SONICTANK) {
						iType = DEVIATOR;
					}

					if (iType == TRIKE) {
						iType = RAIDER;
					}
				}

				// harkonnen
				if (player[p].getHouse() == HARKONNEN)
				{
					if (iType == DEVIATOR || iType == SONICTANK) {
						iType = DEVASTATOR;
					}

					if (iType == TRIKE || iType == RAIDER) {
						iType = QUAD;
					}

					if (iType == SOLDIER) {
						iType = TROOPER;
					}

					if (iType == INFANTRY) {
						iType = TROOPERS;
					}
				}

				int cell = cellCalculator->getCellWithMapBorders(iX, iY);
				int r = UNIT_CREATE(cell, iType, p, true);
				if (r > -1)
				{
					u++;
				}
			}
			char msg[255];
			sprintf(msg,"Wants %d amount of units; amount created %d", aiplayer[p].iUnits, u);
			cLogger::getInstance()->log(LOG_TRACE, COMP_SKIRMISHSETUP, "Creating units", msg, OUTC_NONE, p, iHouse);
		}

		// TODO: spawn a few worms
		iHouse=player[HUMAN].getHouse();
		iMission=9; // high tech level (TODO: make this customizable)
		state = GAME_PLAYING;

		game.setup_players();
		assert(player[HUMAN].getItemBuilder() != NULL);

		bFadeOut=true;
		setup_list();
        play_music(MUSIC_PEACE);
	}

	// delete cell calculator
	delete cellCalculator;


   	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);

    if (bFadeOut) {
        game.FADE_OUT();
    }
}

// select house
void cGame::house()
{
    // FADING STUFF
    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------

    bool bFadeOut=false;

	// draw menu
	draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_HERALD].dat,  0, 0);
    draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_GAME_DUNE].dat, 0, 350);

	// HOUSES

	// ATREIDES
	if ((mouse_y >= 168 && mouse_y <=267) &&
		(mouse_x >= 116 && mouse_x <=207))
	{
		if (bMousePressedLeft)
		{
			iHouse=ATREIDES;

			// let bene gesserit tell about atreides
			// when NO, iHouse gets reset to -1

			play_sound_id(SOUND_ATREIDES,-1);

            LOAD_SCENE("platr"); // load planet of atreides

			state = GAME_TELLHOUSE;
            iMentatSpeak=-1;
            bFadeOut=true;
        }
	}

	// ORDOS
	if ((mouse_y >= 168 && mouse_y <=267) &&
		(mouse_x >= 271 && mouse_x <=360))
	{
		if (bMousePressedLeft)
		{
			iHouse=ORDOS;

			// let bene gesserit tell about harkonnen
			// when NO, iHouse gets reset to -1

			play_sound_id(SOUND_ORDOS,-1);

            LOAD_SCENE("plord"); // load planet of ordos

			state = GAME_TELLHOUSE;
            iMentatSpeak=-1;
            bFadeOut=true;
		}
	}

	// ORDOS
	if ((mouse_y >= 168 && mouse_y <=267) &&
		(mouse_x >= 418 && mouse_x <=506))
	{
		if (bMousePressedLeft)
		{
			iHouse=HARKONNEN;

			// let bene gesserit tell about harkonnen
			// when NO, iHouse gets reset to -1

			play_sound_id(SOUND_HARKONNEN,-1);

            LOAD_SCENE("plhar"); // load planet of harkonnen

			state = GAME_TELLHOUSE;
            iMentatSpeak=-1;
            bFadeOut=true;
		}
	}


	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);




    if (bFadeOut)
        game.FADE_OUT();

}



void cGame::preparementat(bool bTellHouse)
{
    // clear first
    memset(mentat_sentence, 0, sizeof(mentat_sentence));

	if (bTellHouse)
	{
		if (iHouse == ATREIDES)
		{
            INI_LOAD_BRIEFING(ATREIDES, 0, INI_DESCRIPTION);
            //LOAD_BRIEFING("atreides.txt");
		}
		else if (iHouse == HARKONNEN)
		{
            INI_LOAD_BRIEFING(HARKONNEN, 0, INI_DESCRIPTION);
            //LOAD_BRIEFING("harkonnen.txt");
		}
		else if (iHouse == ORDOS)
		{
            INI_LOAD_BRIEFING(ORDOS, 0, INI_DESCRIPTION);
            //LOAD_BRIEFING("ordos.txt");
		}
	}
	else
	{
        if (state == GAME_BRIEFING)
        {
        	game.setup_players();
			INI_Load_scenario(iHouse, iRegion);
			INI_LOAD_BRIEFING(iHouse, iRegion, INI_BRIEFING);
        }
        else if (state == GAME_WINBRIEF)
        {
            if (rnd(100) < 50)
                LOAD_SCENE("win01"); // ltank
            else
                LOAD_SCENE("win02"); // ltank

            INI_LOAD_BRIEFING(iHouse, iRegion, INI_WIN);
        }
        else if (state == GAME_LOSEBRIEF)
        {
            if (rnd(100) < 50)
                LOAD_SCENE("lose01"); // ltank
            else
                LOAD_SCENE("lose02"); // ltank

            INI_LOAD_BRIEFING(iHouse, iRegion, INI_LOSE);
        }
    }

	logbook("MENTAT: sentences prepared 1");
	iMentatSpeak=-2;			// = sentence to draw and speak with (-1 = not ready, -2 means starting)
	TIMER_mentat_Speaking=0; //	0 means, set it up
}

void cGame::tellhouse()
{
    // FADING

    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------

    bool bFadeOut=false;

	draw_mentat(-1); // draw benegesserit

	// -1 means prepare
	if (iMentatSpeak == -1)
		preparementat(true); // prepare for house telling
	else if (iMentatSpeak > -1)
	{
		mentat(-1); // speak dammit!
	}
	else if (iMentatSpeak == -2)
	{
		// do you wish to , bla bla?
	}

    // draw buttons

    if (TIMER_mentat_Speaking < 0)
    {
        // NO
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_NO].dat, 293, 423);

        if ((mouse_x > 293 && mouse_x < 446) && (mouse_y > 423 && mouse_y < 468))
            if (bMousePressedLeft)
            {
                // head back to choose house
                iHouse=-1;
                state = GAME_HOUSE;
                bFadeOut=true;
            }

        // YES
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_YES].dat, 466, 423);
        if ((mouse_x > 446 && mouse_x < 619) && (mouse_y >423 && mouse_y < 468))
            if (bMousePressedLeft)
            {
                // yes!
                state = GAME_BRIEFING; // briefing
                iMission = 1;
                iRegion  = 1;
                iMentatSpeak=-1; // prepare speaking

                player[0].setHouse(iHouse);

                // play correct mentat music
                play_music(MUSIC_BRIEFING);
                bFadeOut=true;

            }


    }

    // draw mouse
	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);


    if (bFadeOut)
        FADE_OUT();

}

// select your next conquest
void cGame::region()
{
        // FADING

	int mouse_tile = MOUSE_NORMAL;

    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------

    bool bFadeOut=false;



    // STEPS:
    // 1. Show current conquered regions
    // 2. Show next progress + story (in message bar)
    // 3. Click next region
    // 4. Set up region and go to GAME_BRIEFING, which will do the rest...-> fade out

    select_palette(player[0].pal);


    // region = one we have won, only changing after we have choosen this one
    if (iRegionState <= 0)
        iRegionState = 1;

    if (iRegionSceneAlpha > 255)
        iRegionSceneAlpha = 255;

    // tell the story
    if (iRegionState == 1)
    {
        // depending on the mission, we tell the story
        if (iRegionScene == 0)
        {
            REGION_SETUP(iMission, iHouse);
            iRegionScene++;
            set_message("3 Houses have come to Dune.");
            iRegionSceneAlpha=-5;
        }
        else if (iRegionScene == 1)
        {
            // draw the
            set_trans_blender(0,0,0,iRegionSceneAlpha);
            draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GAME_DUNE].dat, 0, 12);
            if (cMessage[0] == '\0' && iRegionSceneAlpha >= 255)
            {
                set_message("To take control of the land.");
                iRegionScene++;
                iRegionSceneAlpha=-5;
            }
        }
        else if (iRegionScene == 2)
        {
            draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GAME_DUNE].dat, 0, 12);
            set_trans_blender(0,0,0,iRegionSceneAlpha);
            draw_trans_sprite(bmp_screen, (BITMAP *)gfxworld[WORLD_DUNE].dat, 16, 73);

            if (cMessage[0] == '\0' && iRegionSceneAlpha >= 255)
            {
                set_message("That has become divided.");
                iRegionScene++;
                iRegionSceneAlpha=-5;
            }
        }
        else if (iRegionScene == 3)
        {
            draw_sprite(bmp_screen, (BITMAP *)gfxworld[WORLD_DUNE].dat, 16, 73);
            set_trans_blender(0,0,0,iRegionSceneAlpha);
            draw_trans_sprite(bmp_screen, (BITMAP *)gfxworld[WORLD_DUNE_REGIONS].dat, 16, 73);

            if (iRegionSceneAlpha >= 255)
            {
            iRegionScene = 4;
            iRegionState++;
            }


        }
        else if (iRegionScene > 3)
            iRegionState++;

        if (iRegionSceneAlpha < 255)
		{
			iRegionSceneAlpha+=5;
		}

        // when  mission is 1, do the '3 houses has come to dune, blah blah story)

        //iRegionState++;
    }

    // Draw
    draw_sprite(bmp_screen, (BITMAP *)gfxworld[BMP_NEXTCONQ].dat, 0, 0);

    int iLogo=-1;

    // Draw your logo
    if (iHouse == ATREIDES)
        iLogo = BMP_NCATR;

    if (iHouse == ORDOS)
        iLogo=BMP_NCORD;

    if (iHouse == HARKONNEN)
        iLogo=BMP_NCHAR;

    //iHouse = ATREIDES;

    // Draw 4 times the logo (in each corner)
    if (iLogo > -1)
    {
    draw_sprite(bmp_screen, (BITMAP *)gfxworld[iLogo].dat, 0,0);
    draw_sprite(bmp_screen, (BITMAP *)gfxworld[iLogo].dat, (640)-64,0);
    draw_sprite(bmp_screen, (BITMAP *)gfxworld[iLogo].dat, 0,(480)-64);
    draw_sprite(bmp_screen, (BITMAP *)gfxworld[iLogo].dat, (640)-64,(480)-64);
    }


    if (iRegionState == 2)
    {
        // draw dune first
        draw_sprite(bmp_screen, (BITMAP *)gfxworld[WORLD_DUNE].dat, 16, 73);
        draw_sprite(bmp_screen, (BITMAP *)gfxworld[WORLD_DUNE_REGIONS].dat, 16, 73);

        // draw here stuff
        for (int i=0; i < 27; i++)
            REGION_DRAW(i);

        // Animate here (so add regions that are conquered)

        bool bDone=true;
        for (int i=0; i < MAX_REGIONS; i++)
        {
            // anything in the list
            if (iRegionConquer[i] > -1)
            {
                int iRegNr = iRegionConquer[i];

                if (iRegionHouse[i] > -1)
                {
                    // when the region is NOT this house, turn it into this house
                    if (world[iRegNr].iHouse != iRegionHouse[i])
                    {

                        if ((cRegionText[i][0] != '\0' && cMessage[0] == '\0') ||
                            (cRegionText[i][0] == '\0'))
                        {

                        // set this up
                        world[iRegNr].iHouse = iRegionHouse[i];
                        world[iRegNr].iAlpha = 1;

                        if (cRegionText[i][0] != '\0')
                            set_message(cRegionText[i]);

                        bDone=false;
                        break;

                        }
                        else
                        {
                            bDone=false;
                            break;

                        }
                    }
                    else
                    {
                        // house = ok
                        if (world[iRegNr].iAlpha >= 255)
                        {
                            // remove from list
                            iRegionConquer[i] = -1;
                            iRegionHouse[i] = -1; //
                            bDone=false;

                            break;
                        }
                        else if (world[iRegNr].iAlpha < 255)
                        {
                            bDone=false;
                            break; // not done yet, so wait before we do another region!
                        }
                    }
                }
            }
        }

        if (bDone && cMessage[0] == '\0')
        {
            iRegionState++;
			set_message("Select your next region.");
         //   allegro_message("done2");
        }
    }
    else if (iRegionState == 3)
    {

                // draw dune first
        draw_sprite(bmp_screen, (BITMAP *)gfxworld[WORLD_DUNE].dat, 16, 73);
        draw_sprite(bmp_screen, (BITMAP *)gfxworld[WORLD_DUNE_REGIONS].dat, 16, 73);

        // draw here stuff
        for (int i=0; i < 27; i++)
            REGION_DRAW(i);

    int r = REGION_OVER();

	bool bClickable=false;

    if (r > -1)
        if (world[r].bSelectable)
		{
                world[r].iAlpha = 255;
				mouse_tile = MOUSE_ATTACK;
				bClickable=true;
		}

	if (bMousePressedLeft && bClickable)
	{
		// selected....
		int iReg=0;
		for (int ir=0; ir < MAX_REGIONS; ir++)
			if (world[ir].bSelectable)
				if (ir != r)
					iReg++;
				else
					break;

		// calculate region stuff, and add it up
		int iNewReg=0;
		if (iMission == 0)	iNewReg=1;
		if (iMission == 1)	iNewReg=2;
		if (iMission == 2)	iNewReg=5;
		if (iMission == 3)	iNewReg=8;
		if (iMission == 4)	iNewReg=11;
		if (iMission == 5)	iNewReg=14;
		if (iMission == 6)	iNewReg=17;
		if (iMission == 7)	iNewReg=20;
		if (iMission == 8)	iNewReg=22;

		iNewReg += iReg;

		//char msg[255];
		//sprintf(msg, "Mission = %d", game.iMission);
		//allegro_message(msg);

        game.mission_init();
		game.iRegionState=0;
        game.state = GAME_BRIEFING;
		game.iRegion = iNewReg;
		game.iMission++;						// FINALLY ADD MISSION NUMBER...
        //    iRegion++;
        iMentatSpeak=-1;

		INI_Load_scenario(iHouse, game.iRegion);

		//sprintf(msg, "Mission = %d", game.iMission);
		//allegro_message(msg);

		play_music(MUSIC_BRIEFING);

		//allegro_message(msg);

		bFadeOut=true;

		// Calculate mission from region:
		// region 1 = mission 1
		// region 2, 3, 4 = mission 2
		// region 5, 6, 7 = mission 3
		// region 8, 9, 10 = mission 4
		// region 11,12,13 = mission 5
		// region 14,15,16 = mission 6
		// region 17,18,19 = mission 7
		// region 20,21    = mission 8
		// region 22 = mission 9

	}

    }

    // draw message
    if (iMessageAlpha > -1)
	{
		set_trans_blender(0,0,0,iMessageAlpha);
		BITMAP *temp = create_bitmap(480,30);
		clear_bitmap(temp);
		rectfill(temp, 0,0,480,40, makecol(255,0,255));
		//draw_sprite(temp, (BITMAP *)gfxinter[BMP_MESSAGEBAR].dat, 0,0);

		// draw message
		alfont_textprintf(temp, game_font, 13,18, makecol(0,0,0), cMessage);

		// draw temp
		draw_trans_sprite(bmp_screen, temp, 73, 358);

		destroy_bitmap(temp);
	}


    // mouse
	if (mouse_tile == MOUSE_ATTACK)
		draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
	else
		draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);

    if (bFadeOut)
        FADE_OUT();

	vsync();

}

void destroyAllUnits(bool bHumanPlayer) {
	if (bHumanPlayer) {
		for (int i=0; i < MAX_UNITS; i++) {
			if (unit[i].isValid()) {
				if (unit[i].iPlayer == 0) {
					unit[i].die(true, false); {
					}
				}
			}
		}

	} else {
		for (int i=0; i < MAX_UNITS; i++) {
			if (unit[i].isValid()) {
				if (unit[i].iPlayer > 0) {
					unit[i].die(true, false);
				}
			}
		}
	}
}

void destroyAllStructures(bool bHumanPlayer) {
	if (bHumanPlayer) {
		for (int i=0; i < MAX_STRUCTURES; i++) {
			if (structure[i]) {
				if (structure[i]->getOwner() == 0) {
					structure[i]->die();
				}
			}
		}
	} else {
		for (int i=0; i < MAX_STRUCTURES; i++) {
			if (structure[i]) {
				if (structure[i]->getOwner() > 0) {
					structure[i]->die();
				}
			}
		}
	}
}


void DEBUG_KEYS()
{
	// WIN MISSION
	if (key[KEY_F2])
	{
		if (game.iWinQuota > -1)
			player[0].credits = game.iWinQuota + 1;
		else
		{
			destroyAllStructures(false);
			destroyAllUnits(false);
		}
	}

	// LOSE MISSION
	if (key[KEY_F3])
	{
		destroyAllStructures(true);
		destroyAllUnits(true);
	}

	if (key[KEY_F1] && game.iHouse > 0)
	{
		game.mission_init();
		game.iMission = 9;
		game.iRegion  = 22;
		game.iWinQuota = -1;
		game.setState(GAME_BRIEFING);
		play_music(MUSIC_BRIEFING);
		game.iMentatSpeak=-1;
	}

	if (key[KEY_F6] && game.iHouse > 0)
	{
		game.mission_init();
		game.iMission = 3;
		game.iRegion  = 6;
		game.iWinQuota = -1;
		game.setState(GAME_BRIEFING);
		play_music(MUSIC_BRIEFING);
		game.iMentatSpeak=-1;
	}

	if (key[KEY_F7] && game.iHouse > 0)
	{
		game.mission_init();
		game.iMission = 4;
		game.iRegion  = 10;
		game.iWinQuota = -1;
		game.setState(GAME_BRIEFING);
		play_music(MUSIC_BRIEFING);
		game.iMentatSpeak=-1;
	}

	if (key[KEY_F8] && game.iHouse > 0)
	{
		game.mission_init();
		game.iMission = 5;
		game.iRegion  = 13;
		game.iWinQuota = -1;
		game.setState(GAME_BRIEFING);
		play_music(MUSIC_BRIEFING);
		game.iMentatSpeak=-1;
	}

	if (key[KEY_F4])
	{
		player[0].credits = 299999;
	}

	if (key[KEY_F5]) {
		map.clear_all();
	}
}

int getGroupNumberFromKeyboard() {
	if (key[KEY_1]) {
		return 1;
	}
	if (key[KEY_2]) {
		return 2;
	}
	if (key[KEY_3]) {
		return 3;
	}
	if (key[KEY_4]) {
		return 4;
	}
	if (key[KEY_5]) {
		return 5;
	}

	return 0;
}

// WHen holding CTRL
void GAME_KEYS()
{
	int iGroup = getGroupNumberFromKeyboard();

	// WHEN PRESSED CTRL, MEANING, ADD....
	if (key[KEY_LCONTROL])
	{
		// UNIT GROUPING
		if (iGroup > 0)
		{

			// First: Any unit that is already this group number, but NOT selected, must be removed
			for (int i=0; i < MAX_UNITS; i++)
			{
				// TODO: This can be done smaller.
				if (unit[i].isValid()) {
					if (unit[i].iPlayer == 0) {
						if (unit[i].iGroup == iGroup) {
							if (unit[i].bSelected == false) {
								unit[i].iGroup = -1;
							}
						}
					}
				}
			}

			// now add
			for (int i=0; i < MAX_UNITS; i++)
			{
				if (unit[i].isValid()) {
					if (unit[i].iPlayer == 0) {
						if (unit[i].bSelected) {
							unit[i].iGroup = iGroup;
						}
					}
				}
			}
		}

	} // HOLDING CTRL -> create group
	else
	{
		// Center on focus cell
		if (key[KEY_H]) {
			map.set_pos(-1,-1, player[0].focus_cell);
		}

		// Center on the selected structure
		if (key[KEY_C]) {
			if (game.selected_structure > -1) {
				if (structure[game.selected_structure]) {
					map.set_pos(-1,-1, structure[game.selected_structure]->getCell());
				}
			}
		}

		if (iGroup > 0)
		{
			// First: Any unit that is already this group number, but NOT selected, must be removed

			// not pressing shift, meaning, we remove all selected stuff
			if (key[KEY_LSHIFT] == false)
			{
				for (int i=0; i < MAX_UNITS; i++)
				{
					if (unit[i].isValid()) {
						if (unit[i].iPlayer == 0) {
							if (unit[i].bSelected) {
								unit[i].bSelected = false;
							}
						}
					}
				}
			}

			bool bPlayRep=false;
			bool bPlayInf=false;
			// now add
			for (int i=0; i < MAX_UNITS; i++)
			{
				if (unit[i].isValid())
					if (unit[i].iPlayer == 0)
						if (unit[i].iGroup == iGroup)
						{
							unit[i].bSelected=true;

							if (units[unit[i].iType].infantry) {
								bPlayInf=true;
							} else {
								bPlayRep=true;
							}
						}
			}

			// HACK HACK: This should actually not be randomized. This is done
			// so you will not hear 100x "yes sir" at a time, blowing your speakers
			if (rnd(100) < 15)
			{
				if (bPlayRep) {
					play_sound_id(SOUND_REPORTING,-1);
				}

				if (bPlayInf) {
					play_sound_id(SOUND_YESSIR,-1);
				}
			} // END HACK
		}
	}

	// fast-key's to switch activelists
	// TODO: check if available.
	if (key[KEY_F1]) {
		game.iActiveList = LIST_CONSTYARD;
	}
	if (key[KEY_F2]) {
		game.iActiveList = LIST_INFANTRY;
	}
	if (key[KEY_F3]) {
		game.iActiveList = LIST_LIGHTFC;
	}
	if (key[KEY_F4]) {
		game.iActiveList = LIST_HEAVYFC;
	}
	if (key[KEY_F5]) {
		game.iActiveList = LIST_ORNI;
	}
	if (key[KEY_F6]) {
		game.iActiveList = LIST_PALACE;
	}
	if (key[KEY_F7]) {
		game.iActiveList = LIST_PALACE;
	}
}


void cGame::handleTimeSlicing() {

	if (iRest > 0) {
		rest(iRest);
	}
}

/**
	Handle keyboard keys.

	TAB + key = debug action
*/
void cGame::handleKeys() {

	if (key[KEY_TAB]) {
		if (key[KEY_F4] && key[KEY_LSHIFT])
		{
			int mc = map.mouse_cell();
			if (mc > -1)
			{
				if (map.cell[mc].id[MAPID_UNITS] > -1)
				{
					int id = map.cell[mc].id[MAPID_UNITS];
					unit[id].die(true, false);
				}

				if (map.cell[mc].id[MAPID_STRUCTURES] > -1)
				{
					int id = map.cell[mc].id[MAPID_STRUCTURES];
					structure[id]->die();
				}
			}
		}
		DEBUG_KEYS();
	} else {
		GAME_KEYS();

		if (key[KEY_ESC]) {
			bPlaying=false;
		}

		// take screenshot
		if (key[KEY_F11])
		{
			char filename[25];

			if (screenshot < 10) {
				sprintf(filename, "%dx%d_000%d.bmp", screen_x, screen_y, screenshot);
			} else if (screenshot < 100) {
				sprintf(filename, "%dx%d_00%d.bmp", screen_x, screen_y, screenshot);
			} else if (screenshot < 1000) {
				sprintf(filename, "%dx%d_0%d.bmp", screen_x, screen_y, screenshot);
			} else {
				sprintf(filename, "%dx%d_%d.bmp", screen_x, screen_y, screenshot);
			}

			save_bmp(filename, bmp_screen, general_palette);

			screenshot++;
		}


	}

	if (key[KEY_F]) {
		alfont_textprintf(bmp_screen, game_font, 0,44, makecol(255,255,255), "FPS: %d", fps);
	}

	/* Handle here keys that are only active when debugging */
	if (DEBUGGING) {

		if (key[KEY_F4])
		{
			if (map.mouse_cell() > -1)
			{
				map.clear_spot(map.mouse_cell(), 3, 0);
			}
		}
	}
}

void cGame::shakeScreenAndBlitBuffer() {
	if (TIMER_shake == 0) {
		TIMER_shake = -1;
	}
	// blit on screen

	if (TIMER_shake > 0)
	{
		// the more we get to the 'end' the less we 'throttle'.
		// Structure explosions are 6 time units per cell.
		// Max is 9 cells (9*6=54)
		// the max border is then 9. So, we do time / 6
		if (TIMER_shake > 69) TIMER_shake = 69;

		int offset = TIMER_shake / 5;
		if (offset > 9)
			offset = 9;

		shake_x = -abs(offset/2) + rnd(offset);
		shake_y = -abs(offset/2) + rnd(offset);

		blit(bmp_screen, bmp_throttle, 0, 0, 0+shake_x, 0+shake_y, screen_x, screen_y);
		blit(bmp_throttle, screen, 0, 0, 0, 0, screen_x, screen_y);
	}
	else
	{
		// when fading
		if (iAlphaScreen == 255)
			blit(bmp_screen, screen, 0, 0, 0, 0, screen_x, screen_y);
		else
		{
			BITMAP *temp = create_bitmap(game.screen_x, game.screen_y);
			assert(temp != NULL);
			clear(temp);
			fblend_trans(bmp_screen, temp, 0, 0, iAlphaScreen);
			blit(temp, screen, 0, 0, 0, 0, screen_x, screen_y);
			destroy_bitmap(temp);
		}

	}
}

void cGame::runGameState() {
	switch (state) {
		case GAME_PLAYING:
			combat();
			break;
		case GAME_BRIEFING:
			if (iMentatSpeak == -1) {
				preparementat(false);
			}
			mentat(iHouse);
			break;
		case GAME_SETUPSKIRMISH:
			setup_skirmish();
			break;
		case GAME_MENU:
			menu();
			break;
		case GAME_REGION:
			region();
			break;
		case GAME_HOUSE:
			house();
			break;
		case GAME_TELLHOUSE:
			tellhouse();
			break;
		case GAME_WINNING:
			winning();
			break;
		case GAME_LOSING:
			losing();
			break;
		case GAME_WINBRIEF:
			if (iMentatSpeak == -1) {
				preparementat(false);
			}
			mentat(iHouse);
			break;
		case GAME_LOSEBRIEF:
			if (iMentatSpeak == -1) {
				preparementat(false);
			}
			mentat(iHouse);
			break;
	}
}

/**
	Main game loop
*/
void cGame::run()
{
	set_trans_blender(0, 0, 0, 128);

	while (bPlaying)
	{
		poll();
		TimeManager.processTime();
		handleTimeSlicing();
        runGameState();
		handleKeys();
		shakeScreenAndBlitBuffer();
		frame_count++;
	}
}


/**
	Shutdown the game
*/
void cGame::shutdown() {
	cLogger *logger = cLogger::getInstance();
	logger->logHeader("SHUTDOWN");

	// Destroy font of Allegro FONT library
	alfont_destroy_font(game_font);
	alfont_destroy_font(bene_font);

	// Exit the font library (must be first)

	alfont_exit();
	logbook("Allegro FONT library shut down.");

	if (mp3_music != NULL)
	{
		almp3_stop_autopoll_mp3(mp3_music); // stop auto poll
		almp3_destroy_mp3(mp3_music);
	}

	logbook("Allegro MP3 library shut down.");

	// Now we are all neatly closed, we exit Allegro and return to OS hell.
	allegro_exit();
	logbook("Allegro shut down.");
	logbook("Thanks for playing.");
}

/**
	Setup the game

	TODO: Will eventually call MultiMediaEngine specific functions

	return true when succesfull

	Should not be called twice.

*/
bool cGame::setupGame() {
	cLogger *logger = cLogger::getInstance();

	game.init(); // Must be first!

	// Each time we run the game, we clear out the logbook
	FILE *fp;
	fp = fopen("log.txt", "wt");

	// this will empty the log file (create a new one)
	if (fp)	{
		fclose(fp);
	}

	logger->logHeader("Dune II - The Maker");
	logger->logCommentLine(""); // white space

	logger->logHeader("Version information");
	char msg[255];
	sprintf(msg, "Version %s, Compiled at %s , %s", game.version, __DATE__, __TIME__);
	logger->log(LOG_INFO, COMP_VERSION, "Initializing", msg);

	// init game
	if (game.windowed) {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Windowed mode");
	} else {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Windowed mode");
	}


	mouse_co_x1 = -1;      // coordinates
	mouse_co_y1 = -1;      // of
	mouse_co_x2 = -1;      // the
	mouse_co_y2 = -1;      // mouse border

	// TODO: load eventual game settings (resolution, etc)


	// Logbook notification
	logger->logHeader("Allegro");

	// ALLEGRO - INIT
	if (allegro_init() != 0) {
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_FAILED);
		return false;
	}

	logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_SUCCESS);

	int r = install_timer();
	if (r > -1) {
		logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_SUCCESS);
	}
	else
	{
		allegro_message("Failed to install timer");
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_FAILED);
		return false;
	}

	alfont_init();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing ALFONT", "alfont_init()", OUTC_SUCCESS);
	install_keyboard();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Keyboard", "install_keyboard()", OUTC_SUCCESS);
	install_mouse();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Mouse", "install_mouse()", OUTC_SUCCESS);

	/* set up the interrupt routines... */
	game.TIMER_money=0;
	game.TIMER_message=0;
	game.TIMER_shake=0;

	LOCK_VARIABLE(allegro_timerUnits);
	LOCK_VARIABLE(allegro_timerGlobal);
	LOCK_VARIABLE(allegro_timerSecond);

	LOCK_FUNCTION(allegro_timerunits);
	LOCK_FUNCTION(allegro_timerglobal);
	LOCK_FUNCTION(allegro_timerfps);

	// Install timers
	install_int(allegro_timerunits, 100);
	install_int(allegro_timerglobal, 5);
	install_int(allegro_timerfps, 1000);

	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up timer related variables", "LOCK_VARIABLE/LOCK_FUNCTION", OUTC_SUCCESS);

	frame_count = fps = 0;

	// set window title
	char title[128];
	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)", game.version);

	// Set window title
	set_window_title(title);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", title, OUTC_SUCCESS);

	set_window_close_button(0);

	set_color_depth(16);

	// TODO: read/write rest value so it does not have to 'fine-tune'
	// but is already set up. Perhaps even offer it in the options screen? So the user
	// can specify how much CPU this game may use?


	//if (iDepth > 15 && iDepth != 24)
	//set_color_depth(iDepth);

	if (game.windowed) {
		cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Windows mode requested.", "Searching for optimal graphics settings");
		int 	iDepth = desktop_color_depth();

		// dont switch to 15 bit or lower, or at 24 bit
		if (iDepth > 15 && iDepth != 24) {
			char msg[255];
			sprintf(msg,"Desktop color dept is %d.", iDepth);
			cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.", msg);
			set_color_depth(iDepth);      // run in the same bit depth as the desktop
		} else {
			// default color depth is 16
			cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.", "Could not find color depth, or unsupported color depth found. Will use 16 bit");
			set_color_depth(16);
		}

		//GFX_AUTODETECT_WINDOWED
		int r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
		if (r > -1) {
			logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (windowed)", "Succesfully created window with graphics mode.", OUTC_SUCCESS);
		} else {
			logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (windowed)", "Failed to create window with graphics mode. Fallback to fullscreen.", OUTC_FAILED);
			// GFX_DIRECTX_ACCEL / GFX_AUTODETECT
#ifdef UNIX
			r = set_gfx_mode(GFX_XWINDOWS, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
#else
			r = set_gfx_mode(GFX_DIRECTX_ACCEL, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
#endif

			if (r > -1)	{
				logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fallback, fullscreen)", "Fallback succeeded.", OUTC_SUCCESS);
				game.windowed = false;
				/*
				FILE *f;
				f = fopen("settings.d3", "wb");

				fwrite(&game.play_music , sizeof(bool)    ,1 , f);
				fwrite(&game.play_sound , sizeof(bool)    ,1 , f);
				fwrite(&game.fade , sizeof(bool)    ,1 , f);
				fwrite(&game.windowed , sizeof(bool)    ,1 , f);
				fwrite(&game.screen_x , sizeof(int)    ,1 , f);
				fwrite(&game.screen_y , sizeof(int)    ,1 , f);
				fclose(f);
				logbook("Could not enter windowed-mode; settings.d3 adjusted"); */

			}
			else
			{
				logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fallback, fullscreen)", "Fallback failed!", OUTC_FAILED);
				allegro_message("Fatal error:\n\nCould not start game.\n\nGraphics mode (windowed mode & fallback) could not be initialized.");
				return false;
			}
		}
	} else {

		/**
		 * Fullscreen mode
		 */

		int r = set_gfx_mode(GFX_AUTODETECT, game.screen_x, game.screen_y, game.screen_x, game.screen_y);

		// succes
		if (r > -1) {
			logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fullscreen)", "Succesfully initialized graphics mode.", OUTC_SUCCESS);
		} else {
			logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fullscreen)", "Succesfully initialized graphics mode.", OUTC_FAILED);
			allegro_message("Fatal error:\n\nCould not start game.\n\nGraphics mode (fullscreen) could not be initialized.");
			return false;
		}
	}


	text_mode(-1);
	alfont_text_mode(-1);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Font settings", "Set mode to -1", OUTC_SUCCESS);


	game_font = alfont_load_font("data/arakeen.fon");

	if (game_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded arakeen.fon", OUTC_SUCCESS);
		alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load arakeen.fon", OUTC_FAILED);
		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load arakeen.fon");
		return false;
	}


	bene_font = alfont_load_font("data/benegess.fon");

	if (bene_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded benegess.fon", OUTC_SUCCESS);
		alfont_set_font_size(bene_font, 10); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load benegess.fon", OUTC_FAILED);
		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load benegess.fon");
		return false;
	}

	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0)	{
		set_display_switch_mode(SWITCH_PAUSE);
		logbook("Display 'switch and pause' mode set");
	} else {
		logbook("Display 'switch to background' mode set");
	}



	// sound
	bool bSucces = false;
	int voices = 32;
	while (1) {
		if (voices <= 4) {
			break;
		}

		reserve_voices(voices, 0);
		char msg[255];
		if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == 0)
		{
			sprintf(msg, "Success reserving %d voices.", voices);
			logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
			MAXVOICES=voices;
			bSucces=true;
			break;
		}
		else {
			sprintf(msg, "Failed reserving %d voices. Will try %d.", voices, (voices / 2));
			logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_FAILED);
			voices /= 2;
		}
	}

	if (!bSucces) {
		char msg[255];
		sprintf(msg, "%s", allegro_error);
		logbook(msg);
		logbook("ERROR: Cannot initialize sound card");
	}

	/***
	Bitmap Creation
	***/

	bmp_screen = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_screen == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_screen");
		return false;
	}
	else
	{
		logbook("Memory bitmap created: bmp_screen");
		clear(bmp_screen);
	}

	bmp_throttle = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_throttle == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_throttle");
		return false;
	}
	else {
		logbook("Memory bitmap created: bmp_throttle");
	}

	bmp_winlose = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_winlose == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_winlose");
		return false;
	}
	else {
		logbook("Memory bitmap created: bmp_winlose");
	}

	bmp_fadeout = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_fadeout == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_fadeout");
		return false;
	}
	else {
		logbook("Memory bitmap created: bmp_fadeout");
	}

	/*** End of Bitmap Creation ***/
	set_color_conversion(COLORCONV_MOST);

	logbook("Color conversion method set");

	// setup mouse speed
	set_mouse_speed(-1,-1);

	logbook("MOUSE: Mouse speed set");

	logbook("\n----");
	logbook("GAME ");
	logbook("----");

	/*** Data files ***/

	// load datafiles
	gfxdata = load_datafile("data/gfxdata.dat");
	if (gfxdata == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxdata.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxdata.dat");
		memcpy (general_palette, gfxdata[PALETTE_D2TM].dat, sizeof general_palette);
	}

	gfxaudio = load_datafile("data/gfxaudio.dat");
	if (gfxaudio == NULL)  {
		logbook("ERROR: Could not hook/load datafile: gfxaudio.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxaudio.dat");
	}

	gfxinter = load_datafile("data/gfxinter.dat");
	if (gfxinter == NULL)  {
		logbook("ERROR: Could not hook/load datafile: gfxinter.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxinter.dat");
	}

	gfxworld = load_datafile("data/gfxworld.dat");
	if (gfxworld == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxworld.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxworld.dat");
	}

	gfxmentat = load_datafile("data/gfxmentat.dat");
	if (gfxworld == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxmentat.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxmentat.dat");
	}

	gfxmovie = NULL; // nothing loaded at start. This is done when loading a mission briefing.

	DATAFILE *mp3music = load_datafile("data/mp3mus.dat");

	if (mp3music == NULL) {
		logbook("MP3MUS.DAT not found, using MIDI to play music");
		game.bMp3=false;
	} else {
		logbook("MP3MUS.DAT found, using mp3 files to play music");
		// Immidiatly load menu music
		game.bMp3=true;
	}

	// randomize timer
	unsigned int t = (unsigned int) time(0);
	char seedtxt[80];
	sprintf(seedtxt, "Seed is %d", t);
	logbook(seedtxt);

	srand(t);

	game.bPlaying = true;
	game.screenshot = 0;
	game.state = -1;

	// Mentat class pointer set at null
	Mentat = NULL;

	// Mouse stuff
	mouse_status = MOUSE_STATE_NORMAL;
	mouse_tile = 0;

	set_palette(general_palette);

	// normal sounds are loud, the music is lower (its background music, so it should not be disturbing)
	set_volume(iMaxVolume, 150);

	// A few messages for the player
	logbook("Installing:  PLAYERS");
	INSTALL_PLAYERS();
	logbook("Installing:  HOUSES");
	INSTALL_HOUSES();
	logbook("Installing:  STRUCTURES");
	install_structures();
	logbook("Installing:  BULLET TYPES");
	install_bullets();
	logbook("Installing:  UNITS");
	install_units();
	logbook("Installing:  WORLD");
	INSTALL_WORLD();

	game.init();
	game.setup_players();
	game.setup_list();

	play_music(MUSIC_MENU);

	// all has installed well. Lets rock and role.
	return true;

}

/**
 * Set up players
 */
void cGame::setup_players() {
	// make sure each player has an own item builder
	for (int i = HUMAN; i < MAX_PLAYERS; i++) {
		cPlayer * thePlayer = &player[i];
		thePlayer->setId(i);

		cItemBuilder * itemBuilder = new cItemBuilder(thePlayer);
		thePlayer->setItemBuilder(itemBuilder);

		cSideBar * sidebar = cSideBarFactory::getInstance()->createSideBar(&player[i], game.iMission, iHouse);
		thePlayer->setSideBar(sidebar);

		cBuildingListUpdater * buildingListUpdater = new cBuildingListUpdater(thePlayer);
		thePlayer->setBuildingListUpdater(buildingListUpdater);

		cStructurePlacer * structurePlacer = new cStructurePlacer(thePlayer);
		thePlayer->setStructurePlacer(structurePlacer);

		// set tech level
		thePlayer->setTechLevel(game.iMission);
	}
}

bool cGame::isState(int thisState) {
	return (state == thisState);
}

void cGame::setState(int thisState) {
	state = thisState;
}

/**
 * void cGame::think_upgrade() {
	// loop through all upgrades and upgrade when needed
	for (int i=0; i < MAX_STRUCTURETYPES; i++)
	{
		// ok, we upgrade
		if (iUpgradeProgress[i] > -1 && iUpgradeProgressLimit[i] > -1)
		{

			iUpgradeTIMER[i]++;

			if (iUpgradeTIMER[i] > 35)
			{
				iUpgradeProgress[i]++;
				iUpgradeTIMER[i]=0;
			}

			if (iUpgradeProgress[i] >= iUpgradeProgressLimit[i])
			{
				iStructureUpgrade[i]++;
				int iLevel = iStructureUpgrade[i];

				// Ok now upgrade the structure
				if (i == CONSTYARD)
					if (iLevel == 1)
					{
						// 4 slabs
						//list_new_item(LIST_CONSTYARD, ICON_STR_4SLAB, structures[SLAB4].cost, SLAB4, -1);
						list_insert_item(LIST_CONSTYARD, ICON_STR_4SLAB, structures[SLAB4].cost, SLAB4, -1, 2);
					}
					else if (iLevel == 2)
					{
						list_new_item(LIST_CONSTYARD, ICON_STR_RTURRET, structures[RTURRET].cost, RTURRET, -1);
					}

				if (i == LIGHTFACTORY)
				{
					if (iLevel == 1)
						list_new_item(LIST_LIGHTFC, ICON_UNIT_QUAD, units[QUAD].cost, -1, QUAD);


				}


                if (i == BARRACKS)
				{
                    if (iLevel == 1)
                    {
                        if (player[0].house == ATREIDES || player[0].house == ORDOS)
                        {
                            list_new_item(LIST_INFANTRY, ICON_UNIT_INFANTRY, units[INFANTRY].cost, -1, INFANTRY);

                            if (player[0].house == ORDOS)
                                list_new_item(LIST_INFANTRY, ICON_UNIT_TROOPERS, units[TROOPERS].cost, -1, TROOPERS);
                        }

                    }

                }

                if (i == WOR)
				{
                    if (iLevel == 1)
                    {
                        if (player[0].house == HARKONNEN || player[0].house == SARDAUKAR || player[0].house == ORDOS)
                        {
                            list_new_item(LIST_INFANTRY, ICON_UNIT_TROOPERS, units[TROOPERS].cost, -1, TROOPERS);

                            if (player[0].house == ORDOS)
                               list_new_item(LIST_INFANTRY, ICON_UNIT_INFANTRY, units[INFANTRY].cost, -1, INFANTRY);
                        }
                    }

                }

                if (i == HIGHTECH)
				{
                    if (iLevel == 1)
   						list_new_item(LIST_ORNI, ICON_UNIT_ORNITHOPTER, units[ORNITHOPTER].cost, -1, ORNITHOPTER);

                }

				if (i == HEAVYFACTORY)
				{
					if (iLevel == 1)
						list_new_item(LIST_HEAVYFC, ICON_UNIT_MCV, units[MCV].cost, -1, MCV);
					if (iLevel == 2)
						list_new_item(LIST_HEAVYFC, ICON_UNIT_LAUNCHER, units[LAUNCHER].cost, -1, LAUNCHER);
					if (iLevel == 3)
						list_new_item(LIST_HEAVYFC, ICON_UNIT_SIEGETANK, units[SIEGETANK].cost, -1, SIEGETANK);
                    if (iLevel == 4)
                    {
                        if (player[0].house == ATREIDES)
                            list_new_item(LIST_HEAVYFC, ICON_UNIT_SONICTANK, units[SONICTANK].cost, -1, SONICTANK);
                        if (player[0].house == ORDOS)
                            list_new_item(LIST_HEAVYFC, ICON_UNIT_DEVIATOR, units[DEVIATOR].cost, -1, DEVIATOR);
                        if (player[0].house == HARKONNEN)
                            list_new_item(LIST_HEAVYFC, ICON_UNIT_DEVASTATOR, units[DEVASTATOR].cost, -1, DEVASTATOR);

                    }


				}


				// reset
				iUpgradeProgress[i]=-1;
				iUpgradeProgressLimit[i]=-1;
			}
		}
	}

}
 *
 */
