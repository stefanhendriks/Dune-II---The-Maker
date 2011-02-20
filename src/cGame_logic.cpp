/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 -----------------------------------------
 Initialization of variables
 Game logic
 Think functions
 -----------------------------------------

 */

#include "include/d2tmh.h"

#include "movie/cMoviePlayer.h"
#include "movie/cMovieDrawer.h"

#include "drawers/cAllegroDrawer.h"

cGame::cGame() {
	screenResolution = new cScreenResolution(800, 600);
	screenResolutionFromIni = NULL;
	moviePlayer = NULL;
	windowed = true;
}

cGame::~cGame() {
	if (moviePlayer) delete moviePlayer;
	if (soundPlayer) delete soundPlayer;
	if (screenResolution) delete screenResolution;
	if (screenResolutionFromIni) delete screenResolutionFromIni;
}

void cGame::init() {
	iMaxVolume = 220;

	screenshot = 0;
	bPlaying = true;

	bSkirmish = false;
	iSkirmishStartPoints = 2;

	// Alpha (for fading in/out)
	iAlphaScreen = 0; // 255 = opaque , anything else
	iFadeAction = 2; // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

	iRegionState = 1;// default = 0
	iRegionScene = 0; // scene
	iRegionSceneAlpha = 0; // scene
	memset(iRegionConquer, -1, sizeof(iRegionConquer));
	memset(iRegionHouse, -1, sizeof(iRegionHouse));
	memset(cRegionText, 0, sizeof(cRegionText));
	//int iConquerRegion[MAX_REGIONS];     // INDEX = REGION NR , > -1 means conquered..

	bPlaySound = true;
	bMp3 = false;

	iSkirmishMap = -1;

	iMusicVolume = 128; // volume is 0...

	paths_created = 0;
	hover_unit = -1;

	state = INMENU;

	iWinQuota = -1; // > 0 means, get this to win the mission, else, destroy all!

	selected_structure = -1;

	// mentat
	memset(mentat_sentence, 0, sizeof(mentat_sentence));

	bPlaceIt = false; // we do not place
	bPlacedIt = false;

	map_width = 64;
	map_height = 64;

	mouse_tile = MOUSE_NORMAL;

	memset(version, 0, sizeof(version));
	sprintf(version, "0.4.6");

	fade_select = 255;

	bFadeSelectDir = true; // fade select direction

	iRegion = 1; // what region ? (calumative, from player perspective, NOT the actual region number)
	iMission = 0; // calculated by mission loading (region -> mission calculation)
	iHouse = -1; // what house is selected for playing?

	shake_x = 0;
	shake_y = 0;
	TIMER_shake = 0;

	iMusicType = MUSIC_MENU;

	map.init();

	for (int i = 0; i < MAX_PLAYERS; i++) {
		player[i].init();
		aiplayer[i].init(i);
	}

	// Units & Structures are already initialized in map.init()
	if (game.bMp3) {
		almp3_stop_autopoll_mp3(mp3_music); // stop auto poll
	}

	// Load properties
	INI_Install_Game(game_filename);

	iMentatSpeak = -1; // = sentence to draw and speak with (-1 = not ready)

	TIMER_mentat_Speaking = -1; // speaking = time
	TIMER_mentat_Mouth = 0;
	TIMER_mentat_Eyes = 0;
	TIMER_mentat_Other = 0;

	iMentatMouth = 3; // frames	... (mouth)
	iMentatEyes = 3; // ... for mentat ... (eyes)
	iMentatOther = 0; // ... animations . (book/ring)

	mp3_music = NULL;
}

// TODO: Bad smell (duplicate code)
// initialize for missions
void cGame::mission_init() {

	iMusicVolume = 128; // volume is 0...

	paths_created = 0;
	hover_unit = -1;

	iWinQuota = -1; // > 0 means, get this to win the mission, else, destroy all!

	selected_structure = -1;

	// mentat
	memset(mentat_sentence, 0, sizeof(mentat_sentence));

	bPlaceIt = false; // we do not place
	bPlacedIt = false;

	mouse_tile = MOUSE_NORMAL;

	fade_select = 255;

	bFadeSelectDir = true; // fade select direction

	shake_x = 0;
	shake_y = 0;
	TIMER_shake = 0;

	map.init();

	// clear out players but not entirely
	for (int i = 0; i < MAX_PLAYERS; i++) {
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
	assert(gameDrawer);
	assert(gameDrawer->getCreditsDrawer());
	gameDrawer->getCreditsDrawer()->setCredits();
}

bool cGame::playerHasAnyStructures(int iPlayerId) {
    for (int i = 0; i < MAX_STRUCTURES; i++) {
		if (structure[i]) {
			if (structure[i]->getOwner() == iPlayerId) {
				return true;
			}
		}
	}
    return false;
}

bool cGame::playerHasAnyGroundUnits(int iPlayerId) {
	for (int i = 0; i < MAX_UNITS; i++) {
		int type = unit[i].iType;
		if (unit[i].isValid() &&
			unit[i].iPlayer == iPlayerId &&
			!units[type].airborn) {
			return true;
		}
	}
    return false;
}

bool cGame::isWinQuotaSet() {
    return iWinQuota > 0;
}

bool cGame::playerHasMetQuota(int iPlayerId)
{
    return player[iPlayerId].credits >= iWinQuota;
}

void cGame::think_winlose() {
	bool missionAccomplished = false;
	bool humanPlayerAlive = playerHasAnyStructures(HUMAN) || playerHasAnyGroundUnits(HUMAN);

    if (isWinQuotaSet()) {
		missionAccomplished = playerHasMetQuota(HUMAN);
	} else {
		bool isAnyAIPlayerAlive = false;
		for (int i = (HUMAN + 1); i < AI_WORM; i++ ) {
			if (playerHasAnyStructures(i) || playerHasAnyGroundUnits(i)) {
				isAnyAIPlayerAlive = true;
				break;
			}
		}

		missionAccomplished = !isAnyAIPlayerAlive;
	}

    if (missionAccomplished) {
		setState(WINNING);

		shake_x = 0;
		shake_y = 0;
		TIMER_shake = 0;

		play_voice(SOUND_VOICE_07_ATR);

		playMusicByType(MUSIC_WIN);

		// copy over
		blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

		draw_sprite(bmp_winlose, (BITMAP *) gfxinter[BMP_WINNING].dat, 77, 182);
	} else if (!humanPlayerAlive) {
		state = LOSING;

		shake_x = 0;
		shake_y = 0;
		TIMER_shake = 0;

		play_voice(SOUND_VOICE_08_ATR);

		playMusicByType(MUSIC_LOSE);

		// copy over
		blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

		draw_sprite(bmp_winlose, (BITMAP *) gfxinter[BMP_LOSING].dat, 77, 182);

	}
}

//TODO: move to mentat classes
void cGame::think_mentat() {

	if (TIMER_mentat_Speaking > 0) {
		TIMER_mentat_Speaking--;
	}

	if (TIMER_mentat_Speaking == 0) {
		// calculate speaking stuff

		iMentatSpeak += 2; // makes 0, 2, etc.

		if (iMentatSpeak > 8) {
			iMentatSpeak = -2;
			TIMER_mentat_Speaking = -1;
			return;
		}

		// lentgh calculation of time
		int iLength = strlen(mentat_sentence[iMentatSpeak]);
		iLength += strlen(mentat_sentence[iMentatSpeak + 1]);

		if (iLength < 2) {
			iMentatSpeak = -2;
			TIMER_mentat_Speaking = -1;
			return;
		}

		TIMER_mentat_Speaking = iLength * 12;
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
					TIMER_mentat_Mouth = 3; // wait
				}

				// correct any frame
				if (iMentatMouth > 4) {
					iMentatMouth -= 5;
				}
			} else {
				iMentatMouth += (1 + rnd(4));

				if (iMentatMouth > 4) {
					iMentatMouth -= 5;
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
					iMentatMouth -= 5;
				}

				// Done!
			}
		} else {
			iMentatMouth = 0; // when there is no sentence, do not animate mouth
		}

		TIMER_mentat_Mouth = -1; // this way we make sure we do not update it too much
	} // speaking


	if (TIMER_mentat_Eyes > 0) {
		TIMER_mentat_Eyes--;
	} else {
		int i = rnd(100);

		int iWas = iMentatEyes;

		if (i < 30)
			iMentatEyes = 3;
		else if (i >= 30 && i < 60)
			iMentatEyes = 0;
		else
			iMentatEyes = 4;

		// its the same
		if (iMentatEyes == iWas)
			iMentatEyes = rnd(4);

		if (iMentatEyes != 4)
			TIMER_mentat_Eyes = 90 + rnd(160);
		else
			TIMER_mentat_Eyes = 30;
	}

	// think wohoo
	if (TIMER_mentat_Other > 0) {
		TIMER_mentat_Other--;
	} else {
		iMentatOther = rnd(5);
	}

}

// TODO: Move to music related class (MusicPlayer?)
void cGame::think_music() {
	// all this does is repeating music in the same theme.

	if (iMusicType < 0)
		return;

	// When mp3 mode
	if (bMp3) {

		if (mp3_music != NULL) {
			int s = almp3_poll_mp3(mp3_music);

			if (s == ALMP3_POLL_PLAYJUSTFINISHED || s == ALMP3_POLL_NOTPLAYING) {
				if (iMusicType == MUSIC_ATTACK)
					iMusicType = MUSIC_PEACE; // set back to peace

				playMusicByType(iMusicType);
			}
		}
	}
	// MIDI mode
	else {
		if (MIDI_music_playing() == false) {
			if (DEBUGGING)
				logbook("Going to play the same kind of music (MIDI)");

			if (iMusicType == MUSIC_ATTACK)
				iMusicType = MUSIC_PEACE; // set back to peace

			playMusicByType(iMusicType); //
		}
	}

}

void cGame::poll() {
	cMouse::getInstance()->updateState();
	cGameControlsContext * context = player[HUMAN].getGameControlsContext();
	context->updateState();

	clear(bmp_screen);
	mouse_tile = MOUSE_NORMAL;

	// change this when selecting stuff
	int mc = context->getMouseCell();
	if (mc > -1) {

		// check if any unit is 'selected'
		for (int i = 0; i < MAX_UNITS; i++) {
			if (unit[i].isValid()) {
				if (unit[i].iPlayer == 0) {
					if (unit[i].bSelected) {
						mouse_tile = MOUSE_MOVE;
						break;
					}
				}
			}
		}

		if (mouse_tile == MOUSE_MOVE) {
			// change to attack cursor if hovering over enemy unit
			if (mapUtils->isCellVisibleForPlayerId(HUMAN, mc)) {

				if (map.cell[mc].id[MAPID_UNITS] > -1) {
					int id = map.cell[mc].id[MAPID_UNITS];

					if (unit[id].iPlayer > 0)
						mouse_tile = MOUSE_ATTACK;
				}

				if (map.cell[mc].id[MAPID_STRUCTURES] > -1) {
					int id = map.cell[mc].id[MAPID_STRUCTURES];

					if (structure[id]->getOwner() > 0)
						mouse_tile = MOUSE_ATTACK;
				}

				if (key[KEY_LCONTROL]) {
					mouse_tile = MOUSE_ATTACK;
				}

				if (key[KEY_ALT]) {
					mouse_tile = MOUSE_MOVE;
				}

			} // visible
		}
	}

	if (mouse_tile == MOUSE_NORMAL) {
		// when selecting a structure
		if (game.selected_structure > -1) {
			int id = game.selected_structure;
			if (structure[id]->getOwner() == 0)
				if (key[KEY_LCONTROL])
					mouse_tile = MOUSE_RALLY;

		}
	}

	bPlacedIt = false;

	//selected_structure=-1;
	hover_unit = -1;

	// update power
	// update total spice capacity
	for (int p = 0; p < MAX_PLAYERS; p++) {
		cPlayer * thePlayer = &player[p];
		thePlayer->use_power = structureUtils.getTotalPowerUsageForPlayer(
				thePlayer);
		thePlayer->has_power = structureUtils.getTotalPowerOutForPlayer(
				thePlayer);
		// update spice capacity
		thePlayer->max_credits = structureUtils.getTotalSpiceCapacityForPlayer(
				thePlayer);
	}
}

void cGame::combat() {

	if (iFadeAction == 1) // fading out
	{
		draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
		return;
	}

	if (iAlphaScreen == 0)
		iFadeAction = 2;
	// -----------------
	bPlacedIt = bPlaceIt;

	assert(gameDrawer);
	gameDrawer->draw();
	assert(interactionManager);
	interactionManager->interact();

	// think win/lose
	think_winlose();
}

void cGame::draw_mentat(int iType) {
	select_palette(general_palette);

	// movie
	cMovieDrawer * movieDrawer = new cMovieDrawer(moviePlayer, bmp_screen);
	movieDrawer->drawIfPlaying(256, 120);
	delete movieDrawer;

	// draw proper background
	if (iType == ATREIDES)
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[MENTATA].dat, 0, 0);
	else if (iType == HARKONNEN)
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[MENTATH].dat, 0, 0);
	else if (iType == ORDOS)
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[MENTATO].dat, 0, 0);
	else // bene
	{
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[MENTATM].dat, 0, 0);

		// when not speaking, draw 'do you wish to join house x'
		if (TIMER_mentat_Speaking < 0) {
			draw_sprite(bmp_screen, (BITMAP *) gfxmentat[MEN_WISH].dat, 16, 16);

			// todo house description
		}
	}

	if (cMouse::getInstance()->isLeftButtonClicked())
		if (TIMER_mentat_Speaking > 0) {
			TIMER_mentat_Speaking = 1;
		}

	// SPEAKING ANIMATIONS IS DONE IN MENTAT()


}

void cGame::MENTAT_draw_eyes(int iMentat) {
	int iDrawX = 128;
	int iDrawY = 240;

	// now draw eyes
	if (iMentat < 0)
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[BEN_EYES01 + iMentatEyes].dat, iDrawX,
				iDrawY);

	if (iMentat == HARKONNEN) {
		iDrawX = 64;
		iDrawY = 256;
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[HAR_EYES01 + iMentatEyes].dat, iDrawX,
				iDrawY);
	}

	if (iMentat == ATREIDES) {
		iDrawX = 80;
		iDrawY = 241;
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[ATR_EYES01 + iMentatEyes].dat, iDrawX,
				iDrawY);
	}

	if (iMentat == ORDOS) {
		iDrawX = 32;
		iDrawY = 240;
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[ORD_EYES01 + iMentatEyes].dat, iDrawX,
				iDrawY);
	}

}

void cGame::MENTAT_draw_mouth(int iMentat) {

	int iDrawX = 112;
	int iDrawY = 272;

	// now draw speaking and such
	if (iMentat < 0)
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[BEN_MOUTH01 + iMentatMouth].dat, iDrawX,
				iDrawY);

	if (iMentat == HARKONNEN) {
		iDrawX = 64;
		iDrawY = 288;
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[HAR_MOUTH01 + iMentatMouth].dat, iDrawX,
				iDrawY);
	}

	// 31, 270

	if (iMentat == ATREIDES) {
		iDrawX = 80;
		iDrawY = 273;
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[ATR_MOUTH01 + iMentatMouth].dat, iDrawX,
				iDrawY);
	}

	if (iMentat == ORDOS) {
		iDrawX = 31;
		iDrawY = 270;
		draw_sprite(bmp_screen,
				(BITMAP *) gfxmentat[ORD_MOUTH01 + iMentatMouth].dat, iDrawX,
				iDrawY);
	}
}

// draw mentat
void cGame::mentat(int iType) {
	if (iFadeAction == 1) // fading out
	{
		draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
		return;
	}

	if (iAlphaScreen == 0)
		iFadeAction = 2;
	// -----------------

	bool bFadeOut = false;

	// draw speaking animation, and text, etc
	if (iType > -1)
		draw_mentat(iType); // draw houses

	MENTAT_draw_mouth(iType);
	MENTAT_draw_eyes(iType);

	// draw text
	if (iMentatSpeak >= 0) {
		alfont_textprintf(bmp_screen, bene_font, 17, 17, makecol(0, 0, 0),
				"%s", mentat_sentence[iMentatSpeak]);
		alfont_textprintf(bmp_screen, bene_font, 16, 16,
				makecol(255, 255, 255), "%s", mentat_sentence[iMentatSpeak]);
		alfont_textprintf(bmp_screen, bene_font, 17, 33, makecol(0, 0, 0),
				"%s", mentat_sentence[iMentatSpeak + 1]);
		alfont_textprintf(bmp_screen, bene_font, 16, 32,
				makecol(255, 255, 255), "%s", mentat_sentence[iMentatSpeak + 1]);
	}

	// mentat mouth
	if (TIMER_mentat_Mouth <= 0) {
		TIMER_mentat_Mouth = 13 + rnd(5);
	}

	if (TIMER_mentat_Speaking < 0) {
		// NO
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[BTN_REPEAT].dat, 293, 423);

		if ((mouse_x > 293 && mouse_x < 446)
				&& (mouse_y > 423 && mouse_y < 468))
			if (cMouse::getInstance()->isLeftButtonClicked()) {
				// head back to choose house
				iMentatSpeak = -1; // prepare speaking
				//state = HOUSE;
			}

		// YES/PROCEED
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[BTN_PROCEED].dat, 466, 423);
		if ((mouse_x > 446 && mouse_x < 619)
				&& (mouse_y > 423 && mouse_y < 468))
			if (cMouse::getInstance()->isLeftButtonClicked()) {
				if (isState(BRIEFING)) {
					// proceed, play mission
					state = PLAYING;

					// CENTER MOUSE
					position_mouse(320, 240);

					bFadeOut = true;

					playMusicByType(MUSIC_PEACE);
				} else if (isState(WINBRIEF)) {
					if (bSkirmish) {
						state = SETUPSKIRMISH;
						playMusicByType(MUSIC_MENU);
					} else {

						state = REGION;
						REGION_SETUP(game.iMission, game.iHouse);

						// PLAY THE MUSIC
						playMusicByType(MUSIC_CONQUEST);
					}

					// PREPARE NEW MENTAT BABBLE
					iMentatSpeak = -1;

					bFadeOut = true;
				} else if (isState(LOSEBRIEF)) {
					//
					if (bSkirmish) {
						state = SETUPSKIRMISH;
						playMusicByType(MUSIC_MENU);
					} else {
						if (game.iMission > 1) {
							state = REGION;

							game.iMission--; // we did not win
							REGION_SETUP(game.iMission, game.iHouse);

							// PLAY THE MUSIC
							playMusicByType(MUSIC_CONQUEST);
						} else {
							state = BRIEFING;
							playMusicByType(MUSIC_BRIEFING);
						}

					}
					// PREPARE NEW MENTAT BABBLE
					iMentatSpeak = -1;

					bFadeOut = true;
				}

			}

	}

	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, mouse_x,
			mouse_y);

	if (bFadeOut) {
		FADE_OUT();
	}

}

// draw menu
void cGame::menu() {
	// FADING STUFF
	if (iFadeAction == 1) // fading out
	{
		draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
		return;
	}

	if (iAlphaScreen == 0) {
		iFadeAction = 2;
	}
	// -----------------

	bool bFadeOut = false;

	if (DEBUGGING) {
		for (int x = 0; x < game.getScreenResolution()->getWidth(); x += 60) {
			for (int y = 0; y < game.getScreenResolution()->getHeight(); y += 20) {
				rect(bmp_screen, x, y, x + 50, y + 10, makecol(64, 64, 64));
				putpixel(bmp_screen, x, y, makecol(255, 255, 255));
				alfont_textprintf(bmp_screen, bene_font, x, y, makecol(32, 32, 32), "Debug");
			}

		}
	}

	cTextDrawer * textDrawer = new cTextDrawer(bene_font);

	// draw main menu title (picture is 640x480)
	cAllegroDrawer * allegroDrawer = new cAllegroDrawer(getScreenResolution());
	allegroDrawer->drawSpriteCenteredRelativelyVertical(bmp_screen, (BITMAP *) gfxinter[BMP_D2TM].dat, 0.3);
	delete allegroDrawer;
	GUI_DRAW_FRAME(257, 319, 130, 143);

	// Buttons:

	// PLAY
	if ((mouse_x >= 246 && mouse_x <= 373)
			&& (mouse_y >= 323 && mouse_y <= 340)) {
		alfont_textprintf(bmp_screen, bene_font, 261, 324, makecol(0, 0, 0),
				"Campaign");
		alfont_textprintf(bmp_screen, bene_font, 261, 323, makecol(255, 0, 0),
				"Campaign");

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			state = HOUSE; // select house
			bFadeOut = true;
		}

	} else {
		alfont_textprintf(bmp_screen, bene_font, 261, 324, makecol(0, 0, 0),
				"Campaign");
		alfont_textprintf(bmp_screen, bene_font, 261, 323, makecol(255, 255,
				255), "Campaign");

	}

	// SKIRMISH
	if ((mouse_x >= 246 && mouse_x <= 373)
			&& (mouse_y >= 344 && mouse_y <= 362)) {
		alfont_textprintf(bmp_screen, bene_font, 261, 344, makecol(0, 0, 0),
				"Skirmish");
		alfont_textprintf(bmp_screen, bene_font, 261, 343, makecol(255, 0, 0),
				"Skirmish");

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			game.state = SETUPSKIRMISH;
			bFadeOut = true;
			INI_PRESCAN_SKIRMISH();

			game.mission_init();

			for (int p = 0; p < AI_WORM; p++) {
				player[p].credits = 2500;
				player[p].iTeam = p;
			}
		}

		//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	} else {
		alfont_textprintf(bmp_screen, bene_font, 261, 344, makecol(0, 0, 0),
				"Skirmish");
		alfont_textprintf(bmp_screen, bene_font, 261, 343, makecol(255, 255,
				255), "Skirmish");

	}

	// LOAD
	if ((mouse_x >= 246 && mouse_x <= 373)
			&& (mouse_y >= 364 && mouse_y <= 382)) {
		alfont_textprintf(bmp_screen, bene_font, 261, 364, makecol(0, 0, 0),
				"Multiplayer");
		alfont_textprintf(bmp_screen, bene_font, 261, 363, makecol(255, 0, 0),
				"Multiplayer");

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			//game.state = SETUPSKIRMISH;
			bFadeOut = true;

			// check if the game can be server or client
			//bCanBeServerOrClient();
		}

		//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	} else {
		alfont_textprintf(bmp_screen, bene_font, 261, 364, makecol(0, 0, 0),
				"Multiplayer");
		alfont_textprintf(bmp_screen, bene_font, 261, 363, makecol(255, 255,
				255), "Multiplayer");

	}

	// OPTIONS
	if ((mouse_x >= 246 && mouse_x <= 373)
			&& (mouse_y >= 384 && mouse_y <= 402)) {
		alfont_textprintf(bmp_screen, bene_font, 261, 384, makecol(0, 0, 0),
				"Load");
		alfont_textprintf(bmp_screen, bene_font, 261, 383, makecol(255, 0, 0),
				"Load");

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			//            game.state = SETUPSKIRMISH;
			bFadeOut = true;
		}

		//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	} else {
		alfont_textprintf(bmp_screen, bene_font, 261, 384, makecol(0, 0, 0),
				"Load");
		alfont_textprintf(bmp_screen, bene_font, 261, 383, makecol(255, 255,
				255), "Load");
	}

	// HALL OF FAME
	if ((mouse_x >= 246 && mouse_x <= 373)
			&& (mouse_y >= 404 && mouse_y <= 422)) {
		alfont_textprintf(bmp_screen, bene_font, 261, 404, makecol(0, 0, 0),
				"Options");
		alfont_textprintf(bmp_screen, bene_font, 261, 403, makecol(255, 0, 0),
				"Options");

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			//            game.state = SETUPSKIRMISH;
			bFadeOut = true;
		}

		//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[D2TM_LOAD].dat, 303, 369);
	} else {
		alfont_textprintf(bmp_screen, bene_font, 261, 404, makecol(0, 0, 0),
				"Options");
		alfont_textprintf(bmp_screen, bene_font, 261, 403, makecol(255, 255,
				255), "Options");
	}

	// EXIT
	if ((mouse_x >= 246 && mouse_x <= 373)
			&& (mouse_y >= 424 && mouse_y <= 442)) {

		alfont_textprintf(bmp_screen, bene_font, 261, 424, makecol(0, 0, 0),
				"Hall of Fame");
		alfont_textprintf(bmp_screen, bene_font, 261, 423, makecol(255, 0, 0),
				"Hall of Fame");
	} else {
		alfont_textprintf(bmp_screen, bene_font, 261, 424, makecol(0, 0, 0),
				"Hall of Fame");
		alfont_textprintf(bmp_screen, bene_font, 261, 423, makecol(255, 255,
				255), "Hall of Fame");
	}

	// EXIT
	if ((mouse_x >= 246 && mouse_x <= 373)
			&& (mouse_y >= 444 && mouse_y <= 462)) {
		alfont_textprintf(bmp_screen, bene_font, 261, 444, makecol(0, 0, 0),
				"Exit");
		alfont_textprintf(bmp_screen, bene_font, 261, 443, makecol(255, 0, 0),
				"Exit");

		// quit
		if (cMouse::getInstance()->isLeftButtonClicked())
			game.bPlaying = false;
	} else {
		alfont_textprintf(bmp_screen, bene_font, 261, 444, makecol(0, 0, 0),
				"Exit");
		alfont_textprintf(bmp_screen, bene_font, 261, 443, makecol(255, 255,
				255), "Exit");
	}

	alfont_textprintf(bmp_screen, bene_font, 291, 1, makecol(64, 64, 64),
			"CREDITS");

	if (mouse_y < 24)
		alfont_textprintf(bmp_screen, bene_font, 290, 0, makecol(255, 0, 0),
				"CREDITS");
	else
		alfont_textprintf(bmp_screen, bene_font, 290, 0,
				makecol(255, 255, 255), "CREDITS");

	// draw version
	textDrawer->drawTextBottomRight(version);

	// mp3 addon?
	if (bMp3) {
		textDrawer->drawTextBottomLeft("Music: MP3 ADD-ON");
	} else {
		textDrawer->drawTextBottomLeft("Music: MIDI");
	}

	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, mouse_x,
			mouse_y);

	delete textDrawer;

	if (key[KEY_ESC]) {
		bPlaying = false;
	}

	if (bFadeOut) {
		game.FADE_OUT();
	}

}

void cGame::setup_skirmish() {
	// FADING STUFF
	if (iFadeAction == 1) // fading out
	{
		draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
		return;
	}

	if (iAlphaScreen == 0)
		iFadeAction = 2;
	// -----------------

	bool bFadeOut = false;

	draw_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_GAME_DUNE].dat, 0,
			(game.getScreenResolution()->getHeight() * 0.72));

	for (int dy = 0; dy < game.getScreenResolution()->getHeight(); dy += 2) {
		line(bmp_screen, 0, dy, 640, dy, makecol(0, 0, 0));
	}

	// title box
	GUI_DRAW_FRAME(-1, -1, 642, 21);

	// title name
	alfont_textprintf(bmp_screen, bene_font, 280, 3, makecol(0, 0, 0),
			"Skirmish");
	alfont_textprintf(bmp_screen, bene_font, 280, 2, makecol(255, 0, 0),
			"Skirmish");

	// box at the right
	GUI_DRAW_FRAME(364, 21, 276, 443);

	// draw box for map data
	GUI_DRAW_FRAME(510, 26, 129, 129);
	//rectfill(bmp_screen, 640-130, 26, 640-1, 26+129, makecol(186,190,149));
	//rect(bmp_screen, 640-130, 26, 640-1, 26+129, makecol(227,229,211));

	// rectangle for map list
	rectfill(bmp_screen, 366, (26 + 128) + 4, 638, 461, makecol(32, 32, 32));
	rect(bmp_screen, 366, (26 + 128) + 4, 638, 461, makecol(227, 229, 211));

	int iStartingPoints = 0;

	// draw preview map (if any)
	if (iSkirmishMap > -1) {
		if (iSkirmishMap > 0) {
			if (PreviewMap[iSkirmishMap].name[0] != '\0') {
				if (PreviewMap[iSkirmishMap].terrain) {
					draw_sprite(bmp_screen, PreviewMap[iSkirmishMap].terrain,
							game.getScreenResolution()->getWidth() - 129, 27);
				}

				for (int s = 0; s < 5; s++) {
					if (PreviewMap[iSkirmishMap].iStartCell[s] > -1) {
						iStartingPoints++;
					}
				}
			} else {
				iStartingPoints = iSkirmishStartPoints;

				// when mouse is hovering, draw it, else do not
				if ((mouse_x >= (game.screenResolution->getWidth() - 129) && mouse_x
						< game.screenResolution->getWidth()) && (mouse_y >= 27 && mouse_y < 160)) {
					if (PreviewMap[iSkirmishMap].name[0] != '\0') {
						if (PreviewMap[iSkirmishMap].terrain) {
							draw_sprite(bmp_screen,
									PreviewMap[iSkirmishMap].terrain,
									game.screenResolution->getWidth() - 129, 27);
						}
					}
				} else {
					if (PreviewMap[iSkirmishMap].name[0] != '\0') {
						if (PreviewMap[iSkirmishMap].terrain) {
							draw_sprite(bmp_screen,
									(BITMAP *) gfxinter[BMP_UNKNOWNMAP].dat,
									game.getScreenResolution()->getWidth() - 129, 27);
						}
					}
				}
			}
		}
	}

	alfont_textprintf(bmp_screen, bene_font, 366, 27, makecol(0, 0, 0),
			"Startpoints: %d", iStartingPoints);
	alfont_textprintf(bmp_screen, bene_font, 366, 26, makecol(255, 255, 255),
			"Startpoints: %d", iStartingPoints);

	bool bDoRandomMap = false;

	if ((mouse_x >= 366 && mouse_x <= (640 - 130)) && (mouse_y >= 27 && mouse_y
			<= 43)) {
		alfont_textprintf(bmp_screen, bene_font, 366, 27, makecol(0, 0, 0),
				"Startpoints: %d", iStartingPoints);
		alfont_textprintf(bmp_screen, bene_font, 366, 26, makecol(255, 0, 0),
				"Startpoints: %d", iStartingPoints);

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			iSkirmishStartPoints++;

			if (iSkirmishStartPoints > 4) {
				iSkirmishStartPoints = 2;
			}

			bDoRandomMap = true;
		}

		if (cMouse::getInstance()->isRightButtonClicked()) {
			iSkirmishStartPoints--;

			if (iSkirmishStartPoints < 2) {
				iSkirmishStartPoints = 4;
			}

			bDoRandomMap = true;
		}
	}

	// TITLE: Map list
	GUI_DRAW_FRAME_PRESSED(367, 159, 254, 17);

	//rectfill(bmp_screen, 367, 159, 637, 159+17, makecol(186,190,149));
	//rect(bmp_screen, 367,159, 637, 159+17,makecol(255,255,255));

	alfont_textprintf(bmp_screen, bene_font, 447, 160, makecol(0, 0, 0),
			"Map List");

	int const iHeightPixels = 17;

	int iDrawY = -1;
	int iDrawX = 367;
	int iEndX = 637 - 16;
	int iColor = makecol(255, 255, 255);

	// scroll barr
	if ((mouse_x >= (iEndX + 1) && mouse_x <= 637) && (mouse_y >= 159 + 1
			&& mouse_y <= 477)) {
		// hovers
		rectfill(bmp_screen, iEndX + 1, 159, 637, 460, makecol(128, 128, 128));
		rect(bmp_screen, iEndX + 1, 159, 637, 460, makecol(64, 64, 64));
	} else {
		rectfill(bmp_screen, iEndX + 1, 159, 637, 460, makecol(186, 190, 149));
		rect(bmp_screen, iEndX + 1, 159, 637, 460, makecol(64, 64, 64));
	}

	// for every map that we read , draw here
	for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
		if (PreviewMap[i].name[0] != '\0') {
			bool bHover = false;

			iDrawY = 159 + (i * iHeightPixels) + i + iHeightPixels; // skip 1 bar because the 1st = 'random map'

			bHover = GUI_DRAW_FRAME(iDrawX, iDrawY, 254, iHeightPixels);

			//if ((mouse_x >= iDrawX && mouse_x <= iEndX) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+iHeightPixels)))
			//	bHover=true;

			if (bHover == false) {
				//rectfill(bmp_screen, 367, iDrawY, iEndX,iDrawY+iHeightPixels, makecol(186,190,149));
				//rect(bmp_screen, 367, iDrawY, iEndX,iDrawY+iHeightPixels, makecol(255,255,255));
			} else {
				//rectfill(bmp_screen, 367, iDrawY-1, iEndX,iDrawY+iHeightPixels, makecol(128,128,128));
				//rect(bmp_screen, 367, iDrawY-1, iEndX,iDrawY+iHeightPixels,makecol(186,190,149));
				GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, 254, iHeightPixels);
			}

			// dark sides
			//	line(bmp_screen, 367, iDrawY+iHeightPixels, iEndX,iDrawY+iHeightPixels, makecol(128,128,128));
			//	line(bmp_screen, iEndX, iDrawY, iEndX,iDrawY+iHeightPixels, makecol(128,128,128));

			iColor = makecol(255, 255, 255);

			if (bHover) {
				// Mouse reaction
				iColor = makecol(255, 207, 41);

				if (cMouse::getInstance()->isLeftButtonClicked()) {
					iSkirmishMap = i;

					if (i == 0) {
						bDoRandomMap = true;
					}
				}

			}

			if (i == iSkirmishMap) {
				iColor = makecol(255, 0, 0);
			}

			alfont_textprintf(bmp_screen, bene_font, 368, iDrawY + 3, makecol(
					0, 0, 0), PreviewMap[i].name);
			alfont_textprintf(bmp_screen, bene_font, 368, iDrawY + 2, iColor,
					PreviewMap[i].name);
		}
	}

	rectfill(bmp_screen, 0, 21, 363, 37, makecol(128, 128, 128));
	line(bmp_screen, 0, 38, 363, 38, makecol(255, 255, 255));

	// player list
	rectfill(bmp_screen, 0, 39, 363, 122, makecol(32, 32, 32));
	line(bmp_screen, 0, 123, 363, 123, makecol(255, 255, 255));

	// bottom bar
	rectfill(bmp_screen, 0, 464, 640, 480, makecol(32, 32, 32));

	alfont_textprintf(bmp_screen, bene_font, 4, 26, makecol(0, 0, 0),
			"Player      House      Credits       Units    Team");
	alfont_textprintf(bmp_screen, bene_font, 4, 25, makecol(255, 255, 255),
			"Player      House      Credits       Units    Team");

	bool bHover = false;

	// draw players who will be playing ;)
	for (int p = 0; p < (AI_WORM - 1); p++) {
		int iDrawY = 40 + (p * 22);
		if (p < iStartingPoints) {
			// player playing or not
			if (p == 0) {
				alfont_textprintf(bmp_screen, bene_font, 4, iDrawY + 1,
						makecol(0, 0, 0), "Human");
				alfont_textprintf(bmp_screen, bene_font, 4, iDrawY, makecol(
						255, 255, 255), "Human");
			} else {
				alfont_textprintf(bmp_screen, bene_font, 4, iDrawY + 1,
						makecol(0, 0, 0), "  CPU");
				if ((mouse_x >= 4 && mouse_x <= 73) && (mouse_y >= iDrawY
						&& mouse_y <= (iDrawY + 16))) {
					if (aiplayer[p].bPlaying)
						alfont_textprintf(bmp_screen, bene_font, 4, iDrawY,
								makecol(fade_select, 0, 0), "  CPU");
					else
						alfont_textprintf(bmp_screen, bene_font, 4, iDrawY,
								makecol((fade_select / 2), (fade_select / 2),
										(fade_select / 2)), "  CPU");

					if (cMouse::getInstance()->isLeftButtonClicked() && p > 1) {
						if (aiplayer[p].bPlaying)
							aiplayer[p].bPlaying = false;
						else
							aiplayer[p].bPlaying = true;

					}
				} else {
					if (aiplayer[p].bPlaying)
						alfont_textprintf(bmp_screen, bene_font, 4, iDrawY,
								makecol(255, 255, 255), "  CPU");
					else
						alfont_textprintf(bmp_screen, bene_font, 4, iDrawY,
								makecol(128, 128, 128), "  CPU");
				}

			}

			// HOUSE
			bHover = false;
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

			alfont_textprintf(bmp_screen, bene_font, 74, iDrawY + 1, makecol(0,
					0, 0), "%s", cHouse);

			//			rect(bmp_screen, 74, (40+(p*22)), 150, (40+(p*22))+16, makecol(255,255,255));

			if ((mouse_x >= 74 && mouse_x <= 150) && (mouse_y >= iDrawY
					&& mouse_y <= (iDrawY + 16)))
				bHover = true;

			if (p == 0) {
				alfont_textprintf(bmp_screen, bene_font, 74, iDrawY, makecol(
						255, 255, 255), "%s", cHouse);
			} else {
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 74, iDrawY,
							makecol(255, 255, 255), "%s", cHouse);
				else
					alfont_textprintf(bmp_screen, bene_font, 74, iDrawY,
							makecol(128, 128, 128), "%s", cHouse);

			}

			if (bHover) {
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 74, iDrawY,
							makecol(fade_select, 0, 0), "%s", cHouse);
				else
					alfont_textprintf(bmp_screen, bene_font, 74, iDrawY,
							makecol((fade_select / 2), (fade_select / 2),
									(fade_select / 2)), "%s", cHouse);

				if (cMouse::getInstance()->isLeftButtonClicked()) {
					player[p].setHouse((player[p].getHouse() + 1));
					if (p > 0) {
						if (player[p].getHouse() > 4) {
							player[p].setHouse(0);
						}
					} else {
						if (player[p].getHouse() > 3) {
							player[p].setHouse(0);
						}
					}
				}

				if (cMouse::getInstance()->isRightButtonClicked()) {
					player[p].setHouse((player[p].getHouse() - 1));
					if (p > 0) {
						if (player[p].getHouse() < 0) {
							player[p].setHouse(4);
						}
					} else {
						if (player[p].getHouse() < 0) {
							player[p].setHouse(3);
						}
					}
				}
			}

			// Credits
			bHover = false;

			alfont_textprintf(bmp_screen, bene_font, 174, iDrawY + 1, makecol(
					0, 0, 0), "%d", (int) player[p].credits);

			//rect(bmp_screen, 174, iDrawY, 230, iDrawY+16, makecol(255,255,255));

			if ((mouse_x >= 174 && mouse_x <= 230) && (mouse_y >= iDrawY
					&& mouse_y <= (iDrawY + 16)))
				bHover = true;

			if (p == 0) {
				alfont_textprintf(bmp_screen, bene_font, 174, iDrawY, makecol(
						255, 255, 255), "%d", (int) player[p].credits);
			} else {
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 174, iDrawY,
							makecol(255, 255, 255), "%d",
							(int) player[p].credits);
				else
					alfont_textprintf(bmp_screen, bene_font, 174, iDrawY,
							makecol(128, 128, 128), "%d",
							(int) player[p].credits);

			}

			if (bHover) {
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 174, iDrawY,
							makecol(fade_select, 0, 0), "%d",
							(int) player[p].credits);
				else
					alfont_textprintf(bmp_screen, bene_font, 174, iDrawY,
							makecol((fade_select / 2), (fade_select / 2),
									(fade_select / 2)), "%d", player[p].credits);

				if (cMouse::getInstance()->isLeftButtonClicked()) {
					player[p].credits += 500;
					if (player[p].credits > 10000) {
						player[p].credits = 1000;
					}
				}

				if (cMouse::getInstance()->isRightButtonClicked()) {
					player[p].credits -= 500;
					if (player[p].credits < 1000) {
						player[p].credits = 10000;
					}
				}
			}

			// Units
			bHover = false;

			alfont_textprintf(bmp_screen, bene_font, 269, iDrawY + 1, makecol(
					0, 0, 0), "%d", aiplayer[p].iUnits);

			//rect(bmp_screen, 269, iDrawY, 290, iDrawY+16, makecol(255,255,255));

			if ((mouse_x >= 269 && mouse_x <= 290) && (mouse_y >= iDrawY
					&& mouse_y <= (iDrawY + 16)))
				bHover = true;

			if (p == 0) {
				alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(
						255, 255, 255), "%d", aiplayer[p].iUnits);
			} else {
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY,
							makecol(255, 255, 255), "%d", aiplayer[p].iUnits);
				else
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY,
							makecol(128, 128, 128), "%d", aiplayer[p].iUnits);

			}

			if (bHover) {
				if (aiplayer[p].bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY,
							makecol(fade_select, 0, 0), "%d",
							aiplayer[p].iUnits);
				else
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY,
							makecol((fade_select / 2), (fade_select / 2),
									(fade_select / 2)), "%d",
							aiplayer[p].iUnits);

				if (cMouse::getInstance()->isLeftButtonClicked()) {
					aiplayer[p].iUnits++;
					if (aiplayer[p].iUnits > 10) {
						aiplayer[p].iUnits = 1;
					}
				}

				if (cMouse::getInstance()->isRightButtonClicked()) {
					aiplayer[p].iUnits--;
					if (aiplayer[p].iUnits < 1) {
						aiplayer[p].iUnits = 10;
					}
				}
			}

			// Team
			bHover = false;
		}
	}

	GUI_DRAW_FRAME(-1, 465, 642, 21);

	// back
	alfont_textprintf(bmp_screen, bene_font, 0, 467, makecol(0, 0, 0), " BACK");
	alfont_textprintf(bmp_screen, bene_font, 0, 466, makecol(255, 255, 255),
			" BACK");

	// start
	alfont_textprintf(bmp_screen, bene_font, 580, 467, makecol(0, 0, 0),
			"START");
	alfont_textprintf(bmp_screen, bene_font, 580, 466, makecol(255, 255, 255),
			"START");

	if (bDoRandomMap) {
		randomMapGenerator.generateRandomMap();
	}

	// back
	if ((mouse_x >= 0 && mouse_x <= 50) && (mouse_y >= 465 && mouse_y <= 480)) {
		alfont_textprintf(bmp_screen, bene_font, 0, 466, makecol(255, 0, 0),
				" BACK");

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			bFadeOut = true;
			state = INMENU;
		}
	}

	if ((mouse_x >= 580 && mouse_x <= 640)
			&& (mouse_y >= 465 && mouse_y <= 480))
		alfont_textprintf(bmp_screen, bene_font, 580, 466, makecol(255, 0, 0),
				"START");

	cCellCalculator *cellCalculator = new cCellCalculator(&map);
	// START
	if ((mouse_x >= 580 && mouse_x <= 640)
			&& (mouse_y >= 465 && mouse_y <= 480)
			&& cMouse::getInstance()->isLeftButtonClicked() && iSkirmishMap
			> -1) {
		// Starting skirmish mode
		bSkirmish = true;

		/* set up starting positions */
		int iStartPositions[5];
		int iMax = 0;
		for (int s = 0; s < 5; s++) {
			iStartPositions[s] = PreviewMap[iSkirmishMap].iStartCell[s];

			if (PreviewMap[iSkirmishMap].iStartCell[s] > -1) {
				iMax = s;
			}
		}

		// REGENERATE MAP DATA FROM INFO
		if (iSkirmishMap > -1) {
			for (int c = 0; c < MAX_CELLS; c++) {
				mapEditor.createCell(c, PreviewMap[iSkirmishMap].mapdata[c], 0);
			}
			mapEditor.smoothMap();
		}

		int iShuffles = 3;

		while (iShuffles > 0) {
			int one = rnd(iMax);
			int two = rnd(iMax);

			if (one == two) {
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
		for (int p = 0; p < AI_WORM; p++) {

			int iHouse = player[p].getHouse();

			// house = 0 , random.
			if (iHouse == 0 && p < 4) { // (all players above 4 are non-playing AI 'sides'
				bool bOk = false;

				while (bOk == false) {
					if (p > 0)
						iHouse = rnd(4) + 1;
					else
						iHouse = rnd(3) + 1;

					bool bFound = false;
					for (int pl = 0; pl < AI_WORM; pl++) {
						if (player[pl].getHouse() > 0 && player[pl].getHouse()
								== iHouse) {
							bFound = true;
						}
					}

					if (!bFound) {
						bOk = true;
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
				mapCamera->centerAndJumpViewPortToCell(player[p].focus_cell);
			}

			// create constyard
			cAbstractStructure *s =
					cStructureFactory::getInstance()->createStructure(
							player[p].focus_cell, CONSTYARD, p);

			// when failure, create mcv instead
			if (s == NULL) {
				UNIT_CREATE(player[p].focus_cell, MCV, p, true);
			}

			// amount of units
			int u = 0;

			// create units
			while (u < aiplayer[p].iUnits) {
				int iX = iCellGiveX(player[p].focus_cell);
				int iY = iCellGiveY(player[p].focus_cell);
				int iType = rnd(12);

				iX -= 4;
				iY -= 4;
				iX += rnd(9);
				iY += rnd(9);

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
				if (player[p].getHouse() == ORDOS) {
					if (iType == DEVASTATOR || iType == SONICTANK) {
						iType = DEVIATOR;
					}

					if (iType == TRIKE) {
						iType = RAIDER;
					}
				}

				// harkonnen
				if (player[p].getHouse() == HARKONNEN) {
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
				if (r > -1) {
					u++;
				}
			}
			char msg[255];
			sprintf(msg, "Wants %d amount of units; amount created %d",
					aiplayer[p].iUnits, u);
			cLogger::getInstance()->log(LOG_TRACE, COMP_SKIRMISHSETUP,
					"Creating units", msg, OUTC_NONE, p, iHouse);
		}

		// TODO: spawn a few worms
		iHouse = player[HUMAN].getHouse();
		iMission = 9; // high tech level (TODO: make this customizable)
		state = PLAYING;

		game.setup_players();
		assert(player[HUMAN].getItemBuilder() != NULL);

		bFadeOut = true;
		playMusicByType(MUSIC_PEACE);
	}

	// delete cell calculator
	delete cellCalculator;

	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, mouse_x,
			mouse_y);

	if (bFadeOut) {
		game.FADE_OUT();
	}
}

// select house
void cGame::house() {
	// FADING STUFF
	if (iFadeAction == 1) // fading out
	{
		draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
		return;
	}

	if (iAlphaScreen == 0)
		iFadeAction = 2;
	// -----------------

	bool bFadeOut = false;

	// draw menu
	draw_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_HERALD].dat, 0, 0);
	draw_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_GAME_DUNE].dat, 0, 350);

	// HOUSES

	// ATREIDES
	if ((mouse_y >= 168 && mouse_y <= 267)
			&& (mouse_x >= 116 && mouse_x <= 207)) {
		if (cMouse::getInstance()->isLeftButtonClicked()) {
			iHouse = ATREIDES;

			// let bene gesserit tell about atreides
			// when NO, iHouse gets reset to -1

			play_sound_id(SOUND_ATREIDES, -1);

			LOAD_SCENE("platr"); // load planet of atreides

			setState(TELLHOUSE);
			iMentatSpeak = -1;
			bFadeOut = true;
		}
	}

	// ORDOS
	if ((mouse_y >= 168 && mouse_y <= 267)
			&& (mouse_x >= 271 && mouse_x <= 360)) {
		if (cMouse::getInstance()->isLeftButtonClicked()) {
			iHouse = ORDOS;

			// let bene gesserit tell about harkonnen
			// when NO, iHouse gets reset to -1

			play_sound_id(SOUND_ORDOS, -1);

			LOAD_SCENE("plord"); // load planet of ordos

			setState(TELLHOUSE);
			iMentatSpeak = -1;
			bFadeOut = true;
		}
	}

	// ORDOS
	if ((mouse_y >= 168 && mouse_y <= 267)
			&& (mouse_x >= 418 && mouse_x <= 506)) {
		if (cMouse::getInstance()->isLeftButtonClicked()) {
			iHouse = HARKONNEN;

			// let bene gesserit tell about harkonnen
			// when NO, iHouse gets reset to -1

			play_sound_id(SOUND_HARKONNEN, -1);

			LOAD_SCENE("plhar"); // load planet of harkonnen

			setState(TELLHOUSE);
			iMentatSpeak = -1;
			bFadeOut = true;
		}
	}

	// MOUSE
	draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, mouse_x,
			mouse_y);

	if (bFadeOut)
		game.FADE_OUT();

}

void cGame::preparementat(bool bTellHouse) {
	// clear first
	memset(mentat_sentence, 0, sizeof(mentat_sentence));

	if (bTellHouse) {
		if (iHouse == ATREIDES) {
			INI_LOAD_BRIEFING(ATREIDES, 0, INI_DESCRIPTION);
			//LOAD_BRIEFING("atreides.txt");
		} else if (iHouse == HARKONNEN) {
			INI_LOAD_BRIEFING(HARKONNEN, 0, INI_DESCRIPTION);
			//LOAD_BRIEFING("harkonnen.txt");
		} else if (iHouse == ORDOS) {
			INI_LOAD_BRIEFING(ORDOS, 0, INI_DESCRIPTION);
			//LOAD_BRIEFING("ordos.txt");
		}
	} else {
		if (isState(BRIEFING)) {
			game.setup_players();
			INI_Load_scenario(iHouse, iRegion);
			INI_LOAD_BRIEFING(iHouse, iRegion, INI_BRIEFING);
		} else if (isState(WINBRIEF)) {
			if (rnd(100) < 50) {
				LOAD_SCENE("win01"); // ltank
			} else {
				LOAD_SCENE("win02"); // ltank
			}
			INI_LOAD_BRIEFING(iHouse, iRegion, INI_WIN);
		} else if (isState(LOSEBRIEF)) {
			if (rnd(100) < 50) {
				LOAD_SCENE("lose01"); // ltank
			} else {
				LOAD_SCENE("lose02"); // ltank
			}
			INI_LOAD_BRIEFING(iHouse, iRegion, INI_LOSE);
		}
	}

	logbook("MENTAT: sentences prepared 1");
	iMentatSpeak = -2; // = sentence to draw and speak with (-1 = not ready, -2 means starting)
	TIMER_mentat_Speaking = 0; //	0 means, set it up
}

void cGame::tellhouse() {
	// FADING
	if (iFadeAction == 1) {
		draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
		return;
	}

	if (iAlphaScreen == 0) {
		iFadeAction = 2;
	}

	bool bFadeOut = false;

	draw_mentat(-1); // draw benegesserit

	// -1 means prepare
	if (iMentatSpeak == -1) {
		preparementat(true); // prepare for house telling
	} else if (iMentatSpeak > -1) {
		mentat(-1); // speak dammit!
	} else if (iMentatSpeak == -2) {
		// do you wish to , bla bla?
	}

	// draw buttons

	if (TIMER_mentat_Speaking < 0) {
		// NO
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[BTN_NO].dat, 293, 423);

		if ((mouse_x > 293 && mouse_x < 446) && (mouse_y > 423 && mouse_y < 468)) {
			if (cMouse::getInstance()->isLeftButtonClicked()) {
				// head back to choose house
				iHouse = -1;
				setState(HOUSE);
				bFadeOut = true;
			}
		}

		// YES
		draw_sprite(bmp_screen, (BITMAP *) gfxmentat[BTN_YES].dat, 466, 423);
		if ( (mouse_x > 446 && mouse_x < 619) && (mouse_y > 423 && mouse_y < 468)) {
			if (cMouse::getInstance()->isLeftButtonClicked()) {
				// yes!
				setState(BRIEFING);
				iMission = 1;
				iRegion = 1;
				iMentatSpeak = -1; // prepare speaking

				player[0].setHouse(iHouse);

				// play correct mentat music
				playMusicByType(MUSIC_BRIEFING);
				bFadeOut = true;
			}
		}
	}

	// draw mouse
	draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, mouse_x, mouse_y);

	if (bFadeOut) {
		FADE_OUT();
	}
}

// select your next conquest
void cGame::region() {
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

	bool bFadeOut = false;

	// STEPS:
	// 1. Show current conquered regions
	// 2. Show next progress + story (in message bar)
	// 3. Click next region
	// 4. Set up region and go to BRIEFING, which will do the rest...-> fade out

	select_palette(player[0].pal);

	// region = one we have won, only changing after we have choosen this one
	if (iRegionState <= 0)
		iRegionState = 1;

	if (iRegionSceneAlpha > 255)
		iRegionSceneAlpha = 255;

	// tell the story
	if (iRegionState == 1) {
		// depending on the mission, we tell the story
		if (iRegionScene == 0) {
			REGION_SETUP(iMission, iHouse);
			iRegionScene++;
			gameDrawer->getMessageDrawer()->setMessage(
					"3 Houses have come to Dune.");
			iRegionSceneAlpha = -5;
		} else if (iRegionScene == 1) {
			// draw the
			set_trans_blender(0, 0, 0, iRegionSceneAlpha);
			draw_trans_sprite(bmp_screen,
					(BITMAP *) gfxinter[BMP_GAME_DUNE].dat, 0, 12);
			char * cMessage = gameDrawer->getMessageDrawer()->getMessage();
			if (cMessage[0] == '\0' && iRegionSceneAlpha >= 255) {
				gameDrawer->getMessageDrawer()->setMessage(
						"To take control of the land.");
				iRegionScene++;
				iRegionSceneAlpha = -5;
			}
		} else if (iRegionScene == 2) {
			draw_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_GAME_DUNE].dat, 0,
					12);
			set_trans_blender(0, 0, 0, iRegionSceneAlpha);
			draw_trans_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat,
					16, 73);
			char * cMessage = gameDrawer->getMessageDrawer()->getMessage();
			if (cMessage[0] == '\0' && iRegionSceneAlpha >= 255) {
				gameDrawer->getMessageDrawer()->setMessage(
						"That has become divided.");
				iRegionScene++;
				iRegionSceneAlpha = -5;
			}
		} else if (iRegionScene == 3) {
			draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, 16, 73);
			set_trans_blender(0, 0, 0, iRegionSceneAlpha);
			draw_trans_sprite(bmp_screen,
					(BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, 16, 73);

			if (iRegionSceneAlpha >= 255) {
				iRegionScene = 4;
				iRegionState++;
			}

		} else if (iRegionScene > 3)
			iRegionState++;

		if (iRegionSceneAlpha < 255) {
			iRegionSceneAlpha += 5;
		}

		// when  mission is 1, do the '3 houses has come to dune, blah blah story)

		//iRegionState++;
	}

	// Draw
	draw_sprite(bmp_screen, (BITMAP *) gfxworld[BMP_NEXTCONQ].dat, 0, 0);

	int iLogo = -1;

	// Draw your logo
	if (iHouse == ATREIDES)
		iLogo = BMP_NCATR;

	if (iHouse == ORDOS)
		iLogo = BMP_NCORD;

	if (iHouse == HARKONNEN)
		iLogo = BMP_NCHAR;

	//iHouse = ATREIDES;

	// Draw 4 times the logo (in each corner)
	if (iLogo > -1) {
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, 0, 0);
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, (640) - 64, 0);
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, 0, (480) - 64);
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, (640) - 64,
				(480) - 64);
	}

	char * cMessage = gameDrawer->getMessageDrawer()->getMessage();
	if (iRegionState == 2) {
		// draw dune first
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, 16, 73);
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat,
				16, 73);

		// draw here stuff
		for (int i = 0; i < 27; i++)
			REGION_DRAW(i);

		// Animate here (so add regions that are conquered)
		char * cMessage = gameDrawer->getMessageDrawer()->getMessage();

		bool bDone = true;
		for (int i = 0; i < MAX_REGIONS; i++) {
			// anything in the list
			if (iRegionConquer[i] > -1) {
				int iRegNr = iRegionConquer[i];

				if (iRegionHouse[i] > -1) {
					// when the region is NOT this house, turn it into this house
					if (world[iRegNr].iHouse != iRegionHouse[i]) {

						if ((cRegionText[i][0] != '\0' && cMessage[0] == '\0')
								|| (cRegionText[i][0] == '\0')) {

							// set this up
							world[iRegNr].iHouse = iRegionHouse[i];
							world[iRegNr].iAlpha = 1;

							if (cRegionText[i][0] != '\0') {
								gameDrawer->getMessageDrawer()->setMessage(
										cRegionText[i]);
							}

							bDone = false;
							break;

						} else {
							bDone = false;
							break;

						}
					} else {
						// house = ok
						if (world[iRegNr].iAlpha >= 255) {
							// remove from list
							iRegionConquer[i] = -1;
							iRegionHouse[i] = -1; //
							bDone = false;

							break;
						} else if (world[iRegNr].iAlpha < 255) {
							bDone = false;
							break; // not done yet, so wait before we do another region!
						}
					}
				}
			}
		}

		if (bDone && cMessage[0] == '\0') {
			iRegionState++;
			gameDrawer->getMessageDrawer()->setMessage(
					"Select your next region.");
			//   allegro_message("done2");
		}
	} else if (iRegionState == 3) {

		// draw dune first
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, 16, 73);
		draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat,
				16, 73);

		// draw here stuff
		for (int i = 0; i < 27; i++)
			REGION_DRAW(i);

		int r = REGION_OVER();

		bool bClickable = false;

		if (r > -1)
			if (world[r].bSelectable) {
				world[r].iAlpha = 255;
				mouse_tile = MOUSE_ATTACK;
				bClickable = true;
			}

		if (cMouse::getInstance()->isLeftButtonClicked() && bClickable) {
			// selected....
			int iReg = 0;
			for (int ir = 0; ir < MAX_REGIONS; ir++)
				if (world[ir].bSelectable)
					if (ir != r)
						iReg++;
					else
						break;

			// calculate region stuff, and add it up
			int iNewReg = 0;
			if (iMission == 0)
				iNewReg = 1;
			if (iMission == 1)
				iNewReg = 2;
			if (iMission == 2)
				iNewReg = 5;
			if (iMission == 3)
				iNewReg = 8;
			if (iMission == 4)
				iNewReg = 11;
			if (iMission == 5)
				iNewReg = 14;
			if (iMission == 6)
				iNewReg = 17;
			if (iMission == 7)
				iNewReg = 20;
			if (iMission == 8)
				iNewReg = 22;

			iNewReg += iReg;

			//char msg[255];
			//sprintf(msg, "Mission = %d", game.iMission);
			//allegro_message(msg);

			game.mission_init();
			game.iRegionState = 0;
			game.state = BRIEFING;
			game.iRegion = iNewReg;
			game.iMission++; // FINALLY ADD MISSION NUMBER...
			//    iRegion++;
			iMentatSpeak = -1;

			INI_Load_scenario(iHouse, game.iRegion);

			//sprintf(msg, "Mission = %d", game.iMission);
			//allegro_message(msg);

			playMusicByType(MUSIC_BRIEFING);

			//allegro_message(msg);

			bFadeOut = true;

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
	// TODO: Use own message drawer here or something, with other coordinates
	//    if (iMessageAlpha > -1)
	//	{
	//		set_trans_blender(0,0,0,iMessageAlpha);
	//		BITMAP *temp = create_bitmap(480,30);
	//		clear_bitmap(temp);
	//		rectfill(temp, 0,0,480,40, makecol(255,0,255));
	//		//draw_sprite(temp, (BITMAP *)gfxinter[BMP_MESSAGEBAR].dat, 0,0);
	//
	//		// draw message
	//		alfont_textprintf(temp, game_font, 13,18, makecol(0,0,0), cMessage);
	//
	//		// draw temp
	//		draw_trans_sprite(bmp_screen, temp, 73, 358);
	//
	//		destroy_bitmap(temp);
	//	}


	// mouse
	if (mouse_tile == MOUSE_ATTACK)
		draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, mouse_x
				- 16, mouse_y - 16);
	else
		draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, mouse_x,
				mouse_y);

	if (bFadeOut)
		FADE_OUT();

	vsync();

}

void cGame::destroyAllUnits(bool bHumanPlayer) {
	if (bHumanPlayer) {
		for (int i = 0; i < MAX_UNITS; i++) {
			if (unit[i].isValid()) {
				if (unit[i].iPlayer == 0) {
					unit[i].die(true, false);
					{
					}
				}
			}
		}

	} else {
		for (int i = 0; i < MAX_UNITS; i++) {
			if (unit[i].isValid()) {
				if (unit[i].iPlayer > 0) {
					unit[i].die(true, false);
				}
			}
		}
	}
}

void cGame::destroyAllStructures(bool bHumanPlayer) {
	if (bHumanPlayer) {
		for (int i = 0; i < MAX_STRUCTURES; i++) {
			if (structure[i]) {
				if (structure[i]->getOwner() == 0) {
					structure[i]->die();
				}
			}
		}
	} else {
		for (int i = 0; i < MAX_STRUCTURES; i++) {
			if (structure[i]) {
				if (structure[i]->getOwner() > 0) {
					structure[i]->die();
				}
			}
		}
	}
}

int cGame::getGroupNumberFromKeyboard() {
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

void cGame::handleTimeSlicing() {
	if (iRest > 0) {
		rest(iRest);
	}
}

void cGame::shakeScreenAndBlitBuffer() {
	if (TIMER_shake == 0) {
		TIMER_shake = -1;
	}
	// blit on screen

	if (TIMER_shake > 0) {
		// the more we get to the 'end' the less we 'throttle'.
		// Structure explosions are 6 time units per cell.
		// Max is 9 cells (9*6=54)
		// the max border is then 9. So, we do time / 6
		if (TIMER_shake > 69)
			TIMER_shake = 69;

		int offset = TIMER_shake / 5;
		if (offset > 9)
			offset = 9;

		shake_x = -abs(offset / 2) + rnd(offset);
		shake_y = -abs(offset / 2) + rnd(offset);

		blit(bmp_screen, bmp_throttle, 0, 0, 0 + shake_x, 0 + shake_y,
				game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
		blit(bmp_throttle, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
	} else {
		// when fading
		if (iAlphaScreen == 255)
			blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
		else {
			BITMAP *temp = create_bitmap(game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
			assert(temp != NULL);
			clear(temp);
			fblend_trans(bmp_screen, temp, 0, 0, iAlphaScreen);
			blit(temp, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
			destroy_bitmap(temp);
		}

	}
}

void cGame::runGameState() {
	switch (state) {
	case PLAYING:
		combat();
		break;
	case BRIEFING:
		if (iMentatSpeak == -1) {
			preparementat(false);
		}
		mentat(iHouse);
		break;
	case SETUPSKIRMISH:
		setup_skirmish();
		break;
	case INMENU:
		menu();
		break;
	case REGION:
		region();
		break;
	case HOUSE:
		house();
		break;
	case TELLHOUSE:
		tellhouse();
		break;
	case WINNING:
		winning();
		break;
	case LOSING:
		losing();
		break;
	case WINBRIEF:
		if (iMentatSpeak == -1) {
			preparementat(false);
		}
		mentat(iHouse);
		break;
	case LOSEBRIEF:
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
void cGame::run() {
	set_trans_blender(0, 0, 0, 128);

	while (bPlaying) {
		TimeManager.processTime();
		poll();
		handleTimeSlicing();
		runGameState();
		assert(interactionManager);
		interactionManager->interactWithKeyboard();
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

	if (mp3_music != NULL) {
		almp3_stop_autopoll_mp3(mp3_music); // stop auto poll
		almp3_destroy_mp3(mp3_music);
	}

	logbook("Allegro MP3 library shut down.");

	// Now we are all neatly closed, we exit Allegro and return to OS hell.
	allegro_exit();
	logbook("Allegro shut down.");
	logbook("Thanks for playing.");
}

bool cGame::isResolutionInGameINIFoundAndSet() {
	cScreenResolution * resolution = getScreenResolutionFromIni();
	return (resolution && resolution->getWidth() > -1 && resolution->getHeight() > -1);
}

void cGame::setScreenResolutionFromGameIniSettings() {
	cScreenResolution * screenResolutionFromIni = getScreenResolutionFromIni();
	setScreenResolution(screenResolutionFromIni);
	char msg[255];
	sprintf(msg, "Setting up %dx%d resolution from ini file.", screenResolutionFromIni->getWidth(), screenResolutionFromIni->getHeight());
	cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Custom resolution in windowed mode.", msg);
}

bool cGame::setupGame() {
	cLogger *logger = cLogger::getInstance();

	game.init(); // Must be first!

	// Each time we run the game, we clear out the logbook
	FILE *fp;
	fp = fopen("log.txt", "wt");

	// this will empty the log file (create a new one)
	if (fp) {
		fclose(fp);
	}

	logger->logHeader("Dune II - The Maker");
	logger->logCommentLine(""); // white space

	logger->logHeader("Version information");
	char msg[255];
	sprintf(msg, "Version %s, Compiled at %s , %s", game.version, __DATE__,
			__TIME__);
	logger->log(LOG_INFO, COMP_VERSION, "Initializing", msg);

	// init game
	if (game.windowed) {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Windowed mode");
	} else {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Fullscreen mode");
	}

	mouse_co_x1 = -1; // coordinates
	mouse_co_y1 = -1; // of
	mouse_co_x2 = -1; // the
	mouse_co_y2 = -1; // mouse border

	// TODO: load eventual game settings (resolution, etc)


	// Logbook notification
	logger->logHeader("Allegro");

	// ALLEGRO - INIT
	if (allegro_init() != 0) {
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Allegro init", allegro_id,
				OUTC_FAILED);
		return false;
	}

	logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro init", allegro_id,
			OUTC_SUCCESS);

	int r = install_timer();
	if (r > -1) {
		logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing timer functions",
				"install_timer()", OUTC_SUCCESS);
	} else {
		allegro_message("Failed to install timer");
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Initializing timer functions",
				"install_timer()", OUTC_FAILED);
		return false;
	}

	alfont_init();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing ALFONT", "alfont_init()",
			OUTC_SUCCESS);
	install_keyboard();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Keyboard",
			"install_keyboard()", OUTC_SUCCESS);
	install_mouse();
	assert(cMouse::getInstance());
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Mouse",
			"install_mouse()", OUTC_SUCCESS);

	/* set up the interrupt routines... */
	game.TIMER_shake = 0;

	LOCK_VARIABLE(allegro_timerUnits);LOCK_VARIABLE(allegro_timerGlobal);LOCK_VARIABLE(allegro_timerSecond);

	LOCK_FUNCTION(allegro_timerunits);LOCK_FUNCTION(allegro_timerglobal);LOCK_FUNCTION(allegro_timerfps);

	// Install timers
	install_int(allegro_timerunits, 100); // 100 miliseconds
	install_int(allegro_timerglobal, 5); // 5 miliseconds
	install_int(allegro_timerfps, 1000); // 1000 miliseconds (seconds)

	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up timer related variables",
			"LOCK_VARIABLE/LOCK_FUNCTION", OUTC_SUCCESS);

	frame_count = fps = 0;

	// set window title
	char title[128];
	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)",
			game.version);

	// Set window title
	set_window_title(title);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", title,
			OUTC_SUCCESS);

	set_color_depth(16);

	// TODO: read/write rest value so it does not have to 'fine-tune'
	// but is already set up. Perhaps even offer it in the options screen? So the user
	// can specify how much CPU this game may use?

	if (game.windowed) {
		cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO,
				"Windowed mode requested.",
				"Searching for optimal graphics settings");
		int iDepth = desktop_color_depth();

		if (iDepth > 15 && iDepth != 24) {
			char msg[255];
			sprintf(msg, "Desktop color dept is %d.", iDepth);
			cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO,
					"Analyzing desktop color depth.", msg);
			set_color_depth(iDepth); // run in the same bit depth as the desktop
		} else {
			cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO,
					"Analyzing desktop color depth.",
					"Could not find color depth, or unsupported color depth found. Will use 16 bit");
			set_color_depth(16);
		}

		if (isResolutionInGameINIFoundAndSet()) {
			setScreenResolutionFromGameIniSettings();
		}

		//GFX_AUTODETECT_WINDOWED
		int r = 0;
#ifdef UNIX
		r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
#else
		r = set_gfx_mode(GFX_DIRECTX_WIN, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
#endif

		char msg[255];
		sprintf(msg, "Initializing graphics mode (windowed) with resolution %d by %d.",
				getScreenResolution()->getWidth(), getScreenResolution()->getHeight());

		if (r > -1) {
			logger->log(LOG_INFO, COMP_ALLEGRO, msg,
					"Successfully created window with graphics mode.",
					OUTC_SUCCESS);
		} else {
			logger->log(
					LOG_INFO,
					COMP_ALLEGRO,
					msg,
					"Failed to create window with graphics mode. Fallback to fullscreen.",
					OUTC_FAILED);

			set_color_depth(16);

			// GFX_DIRECTX_ACCEL / GFX_AUTODETECT
#ifdef UNIX
			r = set_gfx_mode(GFX_XWINDOWS, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
#else
			r = set_gfx_mode(GFX_DIRECTX_ACCEL, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
#endif

			if (r > -1) {
				logger->log(LOG_INFO, COMP_ALLEGRO,
						"Initializing graphics mode (fallback, fullscreen)",
						"Fallback succeeded.", OUTC_SUCCESS);
				game.windowed = false;
			} else {
				logger->log(LOG_INFO, COMP_ALLEGRO,
						"Initializing graphics mode (fallback, fullscreen)",
						"Fallback failed!", OUTC_FAILED);
				allegro_message(
						"Fatal error:\n\nCould not start game.\n\nGraphics mode (windowed mode & fallback) could not be initialized.");
				return false;
			}
		}
	} else {

		bool resolutionIsSetProperly = false;
		if (isResolutionInGameINIFoundAndSet()) {
			setScreenResolutionFromGameIniSettings();
			r = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
			char msg[255];

			sprintf(msg, "Setting up %dx%d resolution from ini file.",
					getScreenResolution()->getWidth(), getScreenResolution()->getHeight());

			cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO,
					"Custom resolution from ini file.", msg);

			resolutionIsSetProperly = (r > -1);
		}

		// find best possible resolution
		if (!resolutionIsSetProperly) {
			cBestScreenResolutionFinder bestScreenResolutionFinder;
			bestScreenResolutionFinder.checkResolutions();
			bestScreenResolutionFinder.aquireBestScreenResolutionFullScreen();
		}

		// succes
		if (r > -1) {
			logger->log(LOG_INFO, COMP_ALLEGRO,
					"Initializing graphics mode (fullscreen)",
					"Succesfully initialized graphics mode.", OUTC_SUCCESS);
		} else {
			logger->log(LOG_INFO, COMP_ALLEGRO,
					"Initializing graphics mode (fullscreen)",
					"Failed to initializ graphics mode.", OUTC_FAILED);
			allegro_message(
					"Fatal error:\n\nCould not start game.\n\nGraphics mode (fullscreen) could not be initialized.");
			return false;
		}
	}

	alfont_text_mode(-1);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Font settings", "Set mode to -1",
			OUTC_SUCCESS);

	game_font = alfont_load_font("data/arakeen.fon");

	if (game_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font",
				"loaded arakeen.fon", OUTC_SUCCESS);
		alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font",
				"failed to load arakeen.fon", OUTC_FAILED);
		allegro_message(
				"Fatal error:\n\nCould not start game.\n\nFailed to load arakeen.fon");
		return false;
	}

	bene_font = alfont_load_font("data/benegess.fon");

	if (bene_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font",
				"loaded benegess.fon", OUTC_SUCCESS);
		alfont_set_font_size(bene_font, 10); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font",
				"failed to load benegess.fon", OUTC_FAILED);
		allegro_message(
				"Fatal error:\n\nCould not start game.\n\nFailed to load benegess.fon");
		return false;
	}

	small_font = alfont_load_font("data/small.ttf");

	if (small_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded small.ttf",
				OUTC_SUCCESS);
		alfont_set_font_size(small_font, 10); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font",
				"failed to load small.ttf", OUTC_FAILED);
		allegro_message(
				"Fatal error:\n\nCould not start game.\n\nFailed to load small.ttf");
		return false;
	}

	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0) {
		set_display_switch_mode(SWITCH_PAUSE);
		logbook("Display 'switch and pause' mode set");
	} else {
		logbook("Display 'switch to background' mode set");
	}

	int maxSounds = getAmountReservedVoicesAndInstallSound();
	memset(msg, 0, sizeof(msg));

	if (maxSounds > -1) {
		sprintf(msg, "Successful installed sound. %d voices reserved",
				maxSounds);
		logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
	} else {
		logger->log(LOG_INFO, COMP_SOUND, "Initialization",
				"Failed installing sound.", OUTC_FAILED);
	}
	soundPlayer = new cSoundPlayer(maxSounds);
	moviePlayer = NULL;

	/***
	 Bitmap Creation
	 ***/

	bmp_screen = create_bitmap(getScreenResolution()->getWidth(), getScreenResolution()->getHeight());

	if (bmp_screen == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_screen");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_screen");
		clear(bmp_screen);
	}

	bmp_throttle = create_bitmap(getScreenResolution()->getWidth(), getScreenResolution()->getHeight());

	if (bmp_throttle == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_throttle");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_throttle");
	}

	bmp_winlose = create_bitmap(getScreenResolution()->getWidth(), getScreenResolution()->getHeight());

	if (bmp_winlose == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_winlose");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_winlose");
	}

	bmp_fadeout = create_bitmap(getScreenResolution()->getWidth(), getScreenResolution()->getHeight());

	if (bmp_fadeout == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_fadeout");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_fadeout");
	}

	/*** End of Bitmap Creation ***/
	set_color_conversion(COLORCONV_MOST);

	logbook("Color conversion method set");

	// setup mouse speed
	set_mouse_speed(-1, -1);

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
		memcpy(general_palette, gfxdata[PALETTE_D2TM].dat,
				sizeof general_palette);
	}

	gfxaudio = load_datafile("data/gfxaudio.dat");
	if (gfxaudio == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxaudio.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxaudio.dat");
	}

	gfxinter = load_datafile("data/gfxinter.dat");
	if (gfxinter == NULL) {
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

	// randomize timer
	unsigned int t = (unsigned int) time(0);
	char seedtxt[80];
	sprintf(seedtxt, "Seed is %d", t);
	logbook(seedtxt);
	srand(t);

	game.bPlaying = true;
	game.screenshot = 0;
	game.state = INITIAL;

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

	mapCamera = new cMapCamera();
	gameDrawer = new cGameDrawer(&player[HUMAN]);

	mapUtils = new cMapUtils(&map);

	game.init();
	game.setup_players();

	playMusicByType(MUSIC_MENU);

	// all has installed well. Lets rock and role.
	return true;

}

void cGame::setup_players() {
	if (interactionManager) {
		delete interactionManager;
	}

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

		cUpgradeBuilder * upgradeBuilder = new cUpgradeBuilder(thePlayer);
		thePlayer->setUpgradeBuilder(upgradeBuilder);

		cOrderProcesser * orderProcesser = new cOrderProcesser(thePlayer);
		thePlayer->setOrderProcesser(orderProcesser);

		cGameControlsContext * gameControlsContext = new cGameControlsContext(thePlayer);
		thePlayer->setGameControlsContext(gameControlsContext);

		thePlayer->setTechLevel(game.iMission);
	}

	interactionManager = new cInteractionManager(&player[HUMAN]);
}

bool cGame::isState(GameState theState) {
	return (state == theState);
}

void cGame::setState(GameState theState) {
	state = theState;
}
