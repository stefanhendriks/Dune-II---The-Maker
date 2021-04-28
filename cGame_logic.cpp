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

#include <vector>
#include <algorithm>
#include <random>
#include "include/d2tmh.h"


cGame::cGame() {
	screen_x = 800;
	screen_y = 600;
    windowed = false;
    bPlaySound = true;
    bPlayMusic = true;
    bMp3 = false;
	// default INI screen width and height is not loaded
	// if not loaded, we will try automatic setup
	ini_screen_width=-1;
	ini_screen_height=-1;

    memset(version, 0, sizeof(version));
    sprintf(version, "0.5.5");

    pMentat = nullptr;
}


void cGame::init() {
	iMaxVolume = 220;

	screenshot=0;
	bPlaying=true;

    bSkirmish=false;
	iSkirmishStartPoints=2;

    // Alpha (for fading in/out)
    iAlphaScreen=0;           // 255 = opaque , anything else
    iFadeAction=2;            // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

    iSkirmishMap=-1;

    iMusicVolume=128; // volume is 0...

	paths_created=0;
	hover_unit=-1;

    setState(GAME_MENU);

    iWinQuota=-1;              // > 0 means, get this to win the mission, else, destroy all!

	selected_structure=-1;

	// mentat
    delete pMentat;
    pMentat = nullptr;

	bPlaceIt=false;			// we do not place
	bPlacedIt=false;

	bDeployIt=false;
	bDeployedIt=false;

    mouse_tile = MOUSE_NORMAL;

	fade_select=255;

    bFadeSelectDir=true;    // fade select direction

    iRegion=1;          // what region ? (calumative, from player perspective, NOT the actual region number)
	iMission=0;         // calculated by mission loading (region -> mission calculation)

    shake_x=0;
    shake_y=0;
    TIMER_shake=0;

    iMusicType=MUSIC_MENU;

	map.init(64, 64);

	for (int i=0; i < MAX_PLAYERS; i++) {
		player[i].init(i);
        aiplayer[i].init(i);
    }

	for (int i=0; i < MAX_UNITS; i++) {
	    unit[i].init(i);
	}

	for (int i=0; i < MAX_PARTICLES; i++) {
	    particle[i].init();
	}

	// Units & Structures are already initialized in map.init()
	if (game.bMp3 && mp3_music) {
	    almp3_stop_autopoll_mp3(mp3_music); // stop auto updateState
	}

	// Load properties
	INI_Install_Game(game_filename);

    mp3_music=NULL;
}

// TODO: Bad smell (duplicate code)
// initialize for missions
void cGame::mission_init() {
    mapCamera->resetZoom();

    iMusicVolume=128; // volume is 0...

	paths_created=0;
	hover_unit=-1;

    iWinQuota=-1;              // > 0 means, get this to win the mission, else, destroy all!

	selected_structure=-1;

	bPlaceIt=false;			// we do not place
	bPlacedIt=false;

    bDeployIt=false;
    bDeployedIt=false;

	mouse_tile = MOUSE_NORMAL;

	fade_select=255;

    bFadeSelectDir=true;    // fade select direction

    shake_x=0;
    shake_y=0;
    TIMER_shake=0;

    map.init(64, 64);

    // clear out players but not entirely
    for (int i=0; i < MAX_PLAYERS; i++) {
        int h = player[i].getHouse();

        player[i].init(i);
        player[i].setHouse(h);

        aiplayer[i].init(i);

        if (bSkirmish) {
            player[i].credits = 2500;
        }
    }

    // instantiate the creditDrawer with the appropriate player. Only
    // possible once game has been initialized and player has been created.
    assert(drawManager);
    assert(drawManager->getCreditsDrawer());
    drawManager->getCreditsDrawer()->setCredits();
}


void cGame::think_winlose() {
    bool bSucces = false;
    bool bFailed = true;

    // determine if player is still alive
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        if (structure[i]) {
            if (structure[i]->getOwner() == 0) {
                bFailed = false; // no, we are not failing just yet
                break;
            }
        }
    }

    // determine if any unit is found
    if (bFailed) {
        // check if any unit is ours, if not, we have a problem (airborn does not count)
        for (int i = 0; i < MAX_UNITS; i++)
            if (unit[i].isValid())
                if (unit[i].iPlayer == 0) {
                    bFailed = false;
                    break;
                }
    }


    // win by money quota
    if (iWinQuota > 0) {
        if (player[HUMAN].credits >= iWinQuota) {
            bSucces = true;
        }
    } else {
        // determine if any player (except sandworm) is dead
        bool bAllDead = true;
        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *pStructure = structure[i];
            if (pStructure == nullptr) continue;
            if (pStructure->getOwner() == HUMAN || pStructure->getOwner() == AI_WORM || pStructure->getOwner() == AI_CPU5) continue;
            bAllDead = false;
            break;
        }

        // no structures found, validate units
        if (bAllDead) {
            // check units now
            for (int i = 0; i < MAX_UNITS; i++) {
                cUnit &cUnit = unit[i];
                if (!cUnit.isValid()) continue;
                if (cUnit.iPlayer == HUMAN || cUnit.iPlayer == AI_WORM || cUnit.iPlayer == AI_CPU5) continue;
                if (cUnit.isAirbornUnit()) continue; // do not count airborn units
                bAllDead = false;
                break;
            }
        }

        // all dead == mission success!
        if (bAllDead) {
            bSucces = true;
        }
    }


    // On succes...
    if (bSucces) {
        setState(GAME_WINNING);

        shake_x = 0;
        shake_y = 0;
        TIMER_shake = 0;
        mouse_tile = MOUSE_NORMAL;

        play_voice(SOUND_VOICE_07_ATR);

        playMusicByType(MUSIC_WIN);

        // copy over
        blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, screen_x, screen_y);

        allegroDrawer->drawCenteredSprite(bmp_winlose, (BITMAP *) gfxinter[BMP_WINNING].dat);
    }

    if (bFailed) {
        setState(GAME_LOSING);

        shake_x = 0;
        shake_y = 0;
        TIMER_shake = 0;
        mouse_tile = MOUSE_NORMAL;

        play_voice(SOUND_VOICE_08_ATR);

        playMusicByType(MUSIC_LOSE);

        // copy over
        blit(bmp_screen, bmp_winlose, 0, 0, 0, 0, screen_x, screen_y);

        allegroDrawer->drawCenteredSprite(bmp_winlose, (BITMAP *) gfxinter[BMP_LOSING].dat);
    }
}

void cGame::think_mentat() {
    if (pMentat) {
        pMentat->think();
    }
}

// TODO: Move to music related class (MusicPlayer?)
void cGame::think_music() {
    if (!game.bPlayMusic) // no music enabled, so no need to think
        return;

    // all this does is repeating music in the same theme.
    if (iMusicType < 0)
        return;

    if (!isMusicPlaying()) {

        if (iMusicType == MUSIC_ATTACK) {
            iMusicType = MUSIC_PEACE; // set back to peace
        }

        playMusicByType(iMusicType);
    }
}

bool cGame::isMusicPlaying() {
    if (bMp3 && mp3_music) {
        int s = almp3_poll_mp3(mp3_music);
        return !(s == ALMP3_POLL_PLAYJUSTFINISHED || s == ALMP3_POLL_NOTPLAYING);
    }

    // MIDI mode:
    return MIDI_music_playing();
}

void cGame::updateState() {
    mouse->updateState(); // calls observers that are interested in mouse changes etc

	for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer *pPlayer = &player[i];
        cGameControlsContext *context = pPlayer->getGameControlsContext();

        pPlayer->use_power = structureUtils.getTotalPowerUsageForPlayer(pPlayer);
        pPlayer->has_power = structureUtils.getTotalPowerOutForPlayer(pPlayer);
        // update spice capacity
        pPlayer->max_credits = structureUtils.getTotalSpiceCapacityForPlayer(pPlayer);

        if (i != HUMAN) continue; // non HUMAN players are done
        mouse_tile = MOUSE_NORMAL;

        // change the mouse tile depending on what we're hovering over
        int mc = context->getMouseCell();
        if (mc > -1) {

            // check if any unit is 'selected'
            for (int j=0; j < MAX_UNITS; j++) {
                cUnit &cUnit = unit[j];
                if (!cUnit.isValid()) continue;
                if (cUnit.iPlayer != HUMAN) continue;
                if (cUnit.bSelected) {
                    mouse_tile = MOUSE_MOVE;
                    break;
                }
            }


            if (mouse_tile == MOUSE_MOVE) {
                // change to attack cursor if hovering over enemy unit
                if (map.isVisible(mc, HUMAN)) {

                    int idOfUnitOnCell = map.getCellIdUnitLayer(mc);

                    if (idOfUnitOnCell > -1) {
                        int id = idOfUnitOnCell;

                        if (!unit[id].getPlayer()->isSameTeamAs(&player[HUMAN])) {
                            mouse_tile = MOUSE_ATTACK;
                        }
                    }

                    int idOfStructureOnCell = map.getCellIdStructuresLayer(mc);

                    if (idOfStructureOnCell > -1) {
                        int id = idOfStructureOnCell;

                        if (!structure[id]->getPlayer()->isSameTeamAs(&player[HUMAN])) {
                            mouse_tile = MOUSE_ATTACK;
                        }
                    }

                    if (key[KEY_LCONTROL]) { // force attack
                        mouse_tile = MOUSE_ATTACK;
                    }

                    if (key[KEY_ALT]) { // force move
                        mouse_tile = MOUSE_MOVE;
                    }

                } // visible
            }
        }

        if (mouse_tile == MOUSE_NORMAL) {
            // when selecting a structure
            if (game.selected_structure > -1) {
                int id = game.selected_structure;
                cAbstractStructure *pStructure = structure[id];
                if (pStructure && pStructure->getOwner() == HUMAN) {
                    if (key[KEY_LCONTROL]) {
                        mouse_tile = MOUSE_RALLY;
                    }
                }
            }
        }

        bPlacedIt=false;
        bDeployedIt = false;

        hover_unit=-1;
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
	bDeployedIt = bDeployIt;

    drawManager->drawCombatState();

    // think win/lose
    think_winlose();
}

// stateMentat logic + drawing mouth/eyes
void cGame::stateMentat(cAbstractMentat *pMentat) {
    if (iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (iAlphaScreen == 0)
        iFadeAction = 2;
    // -----------------

    draw_sprite(bmp_screen, bmp_backgroundMentat, 0, 0);

	// draw speaking animation, and text, etc
    if (pMentat == nullptr) {
        cTextDrawer textDrawer(game_font);
        textDrawer.drawText(100, 100, "THIS IS WRONG 2 !!");
        return;
    }

    pMentat->draw();
    pMentat->interact();

    draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);
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

    if (iAlphaScreen == 0) {
        iFadeAction = 2;
    }
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

    cTextDrawer textDrawer = cTextDrawer(bene_font);

	// draw main menu title (picture is 640x480)
//	cAllegroDrawer allegroDrawer;
//	allegroDrawer.drawSpriteCenteredRelativelyVertical(bmp_screen, (BITMAP *)gfxinter[BMP_D2TM].dat, 0.3);
//    GUI_DRAW_FRAME(257, 319, 130,143);
//	// draw menu
	int logoWidth = ((BITMAP*)gfxinter[BMP_D2TM].dat)->w;
	int logoHeight = ((BITMAP*)gfxinter[BMP_D2TM].dat)->h;

	int logoX = (game.screen_x / 2) - (logoWidth / 2);
	int logoY = (logoHeight/10);

	draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_D2TM].dat,  logoX, logoY);

	int mainMenuFrameX = 257;
	int mainMenuFrameY = 319;
	int mainMenuWidth = 130;
	int mainMenuHeight = 143;

	// adjust x and y according to resolution, we can add because the above values
	// assume 640x480 resolution, and logoX/logoY are already taking care of > resolutions
	mainMenuFrameX += logoX;
	mainMenuFrameY += logoY;

    GUI_DRAW_FRAME(mainMenuFrameX, mainMenuFrameY, mainMenuWidth,mainMenuHeight);

	// Buttons:
	int buttonsX = mainMenuFrameX + 4;

	// PLAY
	int playY = 323 + logoY;
	if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, playY, "Campaign", makecol(255, 0, 0)))
	{
		if (mouse->isLeftButtonClicked()) {
			setState(GAME_SELECT_HOUSE); // select house
			bFadeOut = true;
		}
	}

	// SKIRMISH
	int skirmishY = 344 + logoY;
	if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, skirmishY, "Skirmish", makecol(255, 0, 0)))
	{
		if (mouse->isLeftButtonClicked())
		{
			setState(GAME_SETUPSKIRMISH);
			bFadeOut = true;
			INI_PRESCAN_SKIRMISH();

            init_skirmish();
        }
	}

    // MULTIPLAYER
	int multiplayerY = 364 + logoY;
	if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, multiplayerY, "Multiplayer", makecol(128, 128, 128)))
	{
		if (mouse->isLeftButtonClicked())
		{
			// NOT YET IMPLEMENTED
			bFadeOut = true;
		}
	}

    // LOAD
	int loadY = 384 + logoY;
	if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, loadY, "Load", makecol(128, 128, 128)))
	{
		if (mouse->isLeftButtonClicked())
		{
			// NOT YET IMPLEMENTED
			bFadeOut = true;
		}
	}

    // OPTIONS
	int optionsY = 404 + logoY;
	if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, optionsY, "Options", makecol(128, 128, 128)))
	{
		if (mouse->isLeftButtonClicked())
		{
			// NOT YET IMPLEMENTED
			bFadeOut = true;
		}
	}

	// HALL OF FAME
	int hofY = 424 + logoY;
	if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, hofY, "Hall of Fame", makecol(128, 128, 128)))
	{
		if (mouse->isLeftButtonClicked())
		{
			// NOT YET IMPLEMENTED
			bFadeOut = true;
		}
	}

	// EXIT
	int exitY = 444 + logoY;
	if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, exitY, "Exit", makecol(255, 0, 0)))
	{
		if (mouse->isLeftButtonClicked())
		{
			bFadeOut = true;
			game.bPlaying = false;
		}
	}

	int creditsX = (screen_x / 2) - (alfont_text_length(bene_font, "CREDITS") / 2);
	GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(creditsX, 1, "CREDITS", makecol(64, 64, 64));


    // draw version
	textDrawer.drawTextBottomRight(version);

	// mp3 addon?
	if (bMp3) {
		textDrawer.drawTextBottomLeft("Music: MP3 ADD-ON");
    } else {
		textDrawer.drawTextBottomLeft("Music: MIDI");
    }

   	// MOUSE
    draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);

	if (key[KEY_ESC]) {
		bPlaying=false;
	}

    if (bFadeOut) {
        game.FADE_OUT();
    }

}

void cGame::init_skirmish() const {
    game.mission_init();

    for (int p = HUMAN; p < AI_WORM; p++) {
        player[p].credits = 2500;
        player[p].setTeam(p);
    }

    // Fremen allies with Human by default
    player[FREMEN].setTeam(HUMAN);
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

    int darkishBackgroundColor = makecol(32, 32, 32);
    int darkishBorderColor = makecol(227, 229, 211);
    int yellow = makecol(255, 207, 41);

    bool bFadeOut=false;

    draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_GAME_DUNE].dat, game.screen_x * 0.2, (game.screen_y * 0.5));

	for (int dy=0; dy < game.screen_y; dy+=2) {
		line(bmp_screen, 0, dy, screen_x, dy, makecol(0,0,0));
	}

	int topBarWidth = screen_x + 4;
	int topBarHeight = 21;
	int previewMapHeight = 129;
	int previewMapWidth = 129;

    int sidebarWidth = 158;

    // title box
    GUI_DRAW_FRAME(-1, -1, topBarWidth, topBarHeight);

    int creditsX = (screen_x / 2) - (alfont_text_length(bene_font, "Skirmish") / 2);
    GUI_DRAW_BENE_TEXT(creditsX, 1, "Skirmish");

    int topRightBoxWidth = 276;

    // Players title bar
    int playerTitleBarWidth = screen_x - topRightBoxWidth;
    int playerTitleBarHeight = topBarHeight;
    int playerTitleBarX = 0;
    int playerTitleBarY = topBarHeight;
    GUI_DRAW_FRAME_WITH_COLORS(playerTitleBarX, playerTitleBarY, playerTitleBarWidth, playerTitleBarHeight, makecol(255, 255, 255), darkishBackgroundColor );

    // this is the box at the right from the Player list
    int topRightBoxHeight = playerTitleBarHeight + previewMapHeight;
    int topRightBoxX = screen_x - topRightBoxWidth;
    int topRightBoxY = topBarHeight;
    GUI_DRAW_FRAME(topRightBoxX, topRightBoxY, topRightBoxWidth, topRightBoxHeight);

    // player list
    int playerListWidth = playerTitleBarWidth;
    int playerListBarHeight = topRightBoxHeight;
    int playerListBarX = 0;
    int playerListBarY = playerTitleBarY + topBarHeight;
    GUI_DRAW_FRAME_WITH_COLORS(playerListBarX, playerListBarY, playerListWidth, playerListBarHeight, makecol(255, 255, 255), darkishBackgroundColor);

    // map list
    int mapListHeight = screen_y - (topBarHeight + topRightBoxHeight + topBarHeight + topBarHeight);
    int mapListWidth = topRightBoxWidth;
    int mapListTopX = screen_x - mapListWidth;
    int mapListTopY = topRightBoxY + topRightBoxHeight; // ??

    int mapListFrameX = screen_x - mapListWidth;
    int mapListFrameY = (playerListBarY + playerListBarHeight) - playerTitleBarHeight;
    int mapListFrameWidth = screen_x - mapListFrameX;
    int mapListFrameHeight = topBarHeight;

    // rectangle for map list
    GUI_DRAW_FRAME_WITH_COLORS(mapListTopX, mapListTopY, mapListWidth, mapListHeight, darkishBorderColor, darkishBackgroundColor);

    int previewMapY = topBarHeight + 6;
    int previewMapX = screen_x - (previewMapWidth + 6);

    // TITLE: Map list
    GUI_DRAW_FRAME_WITH_COLORS(mapListFrameX, mapListFrameY, mapListFrameWidth, mapListFrameHeight, darkishBorderColor, darkishBackgroundColor);

    cTextDrawer textDrawer = cTextDrawer(bene_font);
    textDrawer.drawTextCentered("Maps", mapListFrameX, mapListFrameWidth, mapListFrameY + 4, yellow);

    int iStartingPoints=0;

    ///////
	// DRAW PREVIEW MAP
	//////

	// iSkirmishMap holds an index of which map to load, where index 0 means random map generated, although
	// this is only meaningful for rendering, the loading (more below) of that map does not care if it is
	// randomly generated or not.
    s_PreviewMap &selectedMap = PreviewMap[iSkirmishMap];
    if (iSkirmishMap > -1) {
	    // Render skirmish map as-is (pre-loaded map)
		if (iSkirmishMap > 0) {
            if (selectedMap.name[0] != '\0') {
                if (selectedMap.terrain) {
                    draw_sprite(bmp_screen, selectedMap.terrain, previewMapX, previewMapY);
                }

                // count starting points
                for (int s : selectedMap.iStartCell) {
                    if (s > -1) {
                        iStartingPoints++;
                    }
                }
            }
        } else {
            // render the 'random generated skirmish map'
            iStartingPoints = iSkirmishStartPoints;

            // when mouse is hovering, draw it, else do not
            if ((mouse_x >= previewMapX && mouse_x < (previewMapX + previewMapWidth) && (mouse_y >= previewMapY && mouse_y < (previewMapY + previewMapHeight))))
            {
                if (selectedMap.name[0] != '\0') {
                    if (selectedMap.terrain) {
                        draw_sprite(bmp_screen, selectedMap.terrain, previewMapX, previewMapY);
                    }
                }
            }
            else
            {
                if (selectedMap.name[0] != '\0') {
                    if (selectedMap.terrain) {
                        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat, previewMapX, previewMapY);
                    }
                }
            }
        }
	}

	int widthOfSomething = 274; //??
	int startPointsX = screen_x - widthOfSomething;
	int startPointsY = previewMapY;
	int startPointHitBoxWidth = 130;
	int startPointHitBoxHeight = 16;

	textDrawer.drawTextWithOneInteger(startPointsX, startPointsY, "Startpoints: %d", iStartingPoints);

	bool bDoRandomMap=false;

	if ((mouse_x >= startPointsX && mouse_x <= (startPointsX + startPointHitBoxWidth)) && (mouse_y >= startPointsY && mouse_y <= (startPointsY + startPointHitBoxHeight)))
	{
        textDrawer.drawTextWithOneInteger(startPointsX, startPointsY, makecol(255, 0, 0), "Startpoints: %d", iStartingPoints);

		if (mouse->isLeftButtonClicked())
		{
			iSkirmishStartPoints++;

			if (iSkirmishStartPoints > 4) {
				iSkirmishStartPoints = 2;
			}

			bDoRandomMap=true;
		}

		if (mouse->isRightButtonClicked())
		{
			iSkirmishStartPoints--;

			if (iSkirmishStartPoints < 2) {
				iSkirmishStartPoints = 4;
			}

			bDoRandomMap=true;
		}
	}

	int const iHeightPixels=topBarHeight;

	int iDrawY=-1;
	int iDrawX=screen_x - widthOfSomething;
	int iEndX=screen_y;
	int iColor=makecol(255,255,255);

	// yes, this means higher resolutions can show more maps.. for now
	int maxMapsInList=std::min((mapListHeight / iHeightPixels), MAX_SKIRMISHMAPS);


    // for every map that we read , draw here
    for (int i=0; i < maxMapsInList; i++) {
		if (PreviewMap[i].name[0] != '\0')
		{
			bool bHover=false;

			iDrawY=mapListFrameY+(i*iHeightPixels)+i+iHeightPixels; // skip 1 bar because the 1st = 'random map'

            bHover = GUI_DRAW_FRAME(iDrawX, iDrawY, mapListFrameWidth, iHeightPixels);

            iColor=makecol(255,255,255);

			if (bHover)	{
			    // Mouse reaction
                iColor=makecol(255,0,0);

				if (mouse->isLeftButtonClicked()) {
                    GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapListFrameWidth, iHeightPixels);
					iSkirmishMap=i;

					if (i == 0) {
						bDoRandomMap=true;
					}
				}
			}

			if (i == iSkirmishMap) {
				iColor=yellow;
				if (bHover) {
				    iColor = makecol(225, 177, 21); // a bit darker yellow to give some visual clue
				}
                GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapListFrameWidth, iHeightPixels);
			}

			textDrawer.drawText(mapListFrameX + 4, iDrawY+4, iColor, PreviewMap[i].name);
		}
    }

	alfont_textprintf(bmp_screen, bene_font, 4, 26, makecol(0,0,0), "Player      House      Credits       Units    Team");
	alfont_textprintf(bmp_screen, bene_font, 4, 25, makecol(255,255,255), "Player      House      Credits       Units    Team");


	bool bHover=false;

	// draw players who will be playing ;)
	for (int p=0; p < (AI_WORM-1); p++)	{
		int iDrawY=playerListBarY + 4 +(p*22);
		if (p < iStartingPoints) {
			// player playing or not
            cAIPlayer &aiPlayer = aiplayer[p];
            if (p == HUMAN)	{
				alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "Human");
				alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(255,255,255), "Human");
			} else {

				alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "  CPU");

				// move hovers over... :/
				if ((mouse_x >= 4 && mouse_x <= 73) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16))) {
					if (aiPlayer.bPlaying) {
                        alfont_textprintf(bmp_screen, bene_font, 4, iDrawY, makecol(fade_select, 0, 0), "  CPU");
                    } else {
					    // not available
                        alfont_textprintf(bmp_screen, bene_font, 4, iDrawY,
                                          makecol((fade_select / 2), (fade_select / 2), (fade_select / 2)), "  CPU");
                    }

					if (mouse->isLeftButtonClicked())	{
						if (aiPlayer.bPlaying) {
                            aiPlayer.bPlaying = false;
                        } else {
                            aiPlayer.bPlaying = true;
                        }
					}
				}
				else
				{
					if (aiPlayer.bPlaying)
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

			if ((mouse_x >= 74 && mouse_x <= 150) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
				bHover=true;

			if (p == 0)
			{
				alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(255,255,255), "%s", cHouse);
			}
			else
			{
				if (aiPlayer.bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(255,255,255), "%s", cHouse);
				else
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(128,128,128), "%s", cHouse);

			}

			if (bHover)
			{
				if (aiPlayer.bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(fade_select,0,0), "%s", cHouse);
				else
					alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol((fade_select/2),(fade_select/2),(fade_select/2)), "%s", cHouse);


				if (mouse->isLeftButtonClicked())
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

				if (mouse->isRightButtonClicked())
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
				if (aiPlayer.bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(255,255,255), "%d", (int)player[p].credits);
				else
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(128,128,128), "%d", (int)player[p].credits);

			}

			if (bHover)
			{
				if (aiPlayer.bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(fade_select,0,0), "%d", (int)player[p].credits);
				else
					alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol((fade_select/2),(fade_select/2),(fade_select/2)), "%d", player[p].credits);

				if (mouse->isLeftButtonClicked())
				{
					player[p].credits += 500;
					if (player[p].credits > 10000) {
						player[p].credits = 1000;
					}
				}

				if (mouse->isRightButtonClicked())
				{
					player[p].credits -= 500;
					if (player[p].credits < 1000) {
						player[p].credits = 10000;
					}
				}
			}

			// Units
			bHover = false;

			alfont_textprintf(bmp_screen, bene_font, 269,iDrawY+1, makecol(0,0,0), "%d", aiPlayer.iUnits);

			//rect(bmp_screen, 269, iDrawY, 290, iDrawY+16, makecol(255,255,255));

			if ((mouse_x >= 269 && mouse_x <= 290) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
				bHover=true;

			if (p == 0)
			{
				alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(255,255,255), "%d", aiPlayer.iUnits);
			}
			else
			{
				if (aiPlayer.bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(255,255,255), "%d", aiPlayer.iUnits);
				else
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(128,128,128), "%d", aiPlayer.iUnits);

			}

			if (bHover)
			{
				if (aiPlayer.bPlaying)
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(fade_select,0,0), "%d", aiPlayer.iUnits);
				else
					alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol((fade_select/2),(fade_select/2),(fade_select/2)), "%d", aiPlayer.iUnits);

				if (mouse->isLeftButtonClicked())
				{
					aiPlayer.iUnits++;
					if (aiPlayer.iUnits > 10) {
                        aiPlayer.iUnits = 1;
					}
				}

				if (mouse->isRightButtonClicked())
				{
					aiPlayer.iUnits--;
					if (aiPlayer.iUnits < 1) {
                        aiPlayer.iUnits = 10;
					}
				}
			}

			// Team
			bHover=false;
		}
	}

    GUI_DRAW_FRAME(-1, screen_y - topBarHeight, screen_x + 2, topBarHeight + 2);


	// back
	int backButtonWidth = textDrawer.textLength(" BACK");
    int backButtonHeight = topBarHeight;
	int backButtonY = screen_y - topBarHeight;
	int backButtonX = 0;
    textDrawer.drawTextBottomLeft(" BACK");

    // start
    int startButtonWidth = textDrawer.textLength("START");
    int startButtonHeight = topBarHeight;
    int startButtonY = screen_y - topBarHeight;
    int startButtonX = screen_x - startButtonWidth;

    textDrawer.drawTextBottomRight("START");

	if (bDoRandomMap) {
		randomMapGenerator.generateRandomMap();
	}

    // back
    if (MOUSE_WITHIN_RECT(backButtonX, backButtonY, backButtonWidth, backButtonHeight)) {
        textDrawer.drawTextBottomLeft(makecol(255,0,0), " BACK");

        if (mouse->isLeftButtonClicked()) {
            bFadeOut=true;
            setState(GAME_MENU);
        }
    }

    if (MOUSE_WITHIN_RECT(startButtonX, startButtonY, startButtonWidth, startButtonHeight)) {
        textDrawer.drawTextBottomRight(makecol(255, 0, 0), "START");

        // this needs to be before setup_players :/
        iMission=9; // high tech level (TODO: make this customizable)

        game.setup_players();

        // START
        if ((mouse->isLeftButtonClicked() && iSkirmishMap > -1)) {
            // Starting skirmish mode
            bSkirmish=true;

            /* set up starting positions */
            std::vector<int> iStartPositions;

            int startCellsOnSkirmishMap=0;
            for (int s=0; s < 5; s++) {
                int startPosition = selectedMap.iStartCell[s];
                if (startPosition < 0) continue;
                iStartPositions.push_back(startPosition);
            }

            startCellsOnSkirmishMap = iStartPositions.size();

            // REGENERATE MAP DATA FROM INFO
            map.init(selectedMap.width, selectedMap.height);

            for (int c=0; c < map.getMaxCells(); c++) {
                mapEditor.createCell(c, selectedMap.mapdata[c], 0);
            }

            mapEditor.smoothMap();

            if (DEBUGGING) {
                logbook("Starting positions before shuffling:");
                for (int i = 0; i < startCellsOnSkirmishMap; i++) {
                    char msg[255];
                    sprintf(msg, "iStartPositions[%d] = [%d]", i, iStartPositions[i]);
                    logbook(msg);
                }
            }

            logbook("Shuffling starting positions");
            std::random_shuffle(iStartPositions.begin(), iStartPositions.end());

            if (DEBUGGING) {
                logbook("Starting positions after shuffling:");
                for (int i = 0; i < startCellsOnSkirmishMap; i++) {
                    char msg[255];
                    sprintf(msg, "iStartPositions[%d] = [%d]", i, iStartPositions[i]);
                    logbook(msg);
                }
            }

            // set up players and their units
            for (int p=0; p < MAX_PLAYERS; p++)	{

                cPlayer &cPlayer = player[p];
                cAIPlayer &aiPlayer = aiplayer[p];
                int iHouse = cPlayer.getHouse(); // get house selected, which can be 0 for RANDOM

                // not playing.. do nothing (only for playable factions)
                bool playableFaction = p < AI_CPU5;

                if (playableFaction) {
                    if (!aiPlayer.bPlaying) continue;

                    // house = 0 means pick random house
                    if (iHouse == 0) {
                        bool bOk=false;

                        while (bOk == false) {
                            if (p > HUMAN) {
                                iHouse = rnd(4)+1;
                                // cpu player
                            } else {// human may not be sardaukar
                                iHouse = rnd(3) + 1; // hark = 1, atr = 2, ord = 3, sar = 4
                            }

                            bool houseInUse=false;
                            for (int pl=0; pl < AI_WORM; pl++) {
                                if (player[pl].getHouse() > 0 &&
                                    player[pl].getHouse() == iHouse) {
                                    houseInUse=true;
                                }
                            }

                            if (!houseInUse) {
                                bOk=true;
                            }
                        }
                    }
                } else {
                    aiPlayer.bPlaying = true;
                    if (p == AI_CPU5) {
                        iHouse = FREMEN;
                    } else {
                        iHouse = GENERALHOUSE;
                    }
                }

                // TEAM Logic
                if (p == HUMAN) {
                    cPlayer.setTeam(0);
                } else if (p == AI_CPU5) {
                    cPlayer.setTeam(0);
                } else if (p == AI_CPU6) {
                    cPlayer.setTeam(2); // worm team is against everyone
                } else {
                    // all other AI's are their own team (campaign == AI's are friends, here they are enemies)
                    cPlayer.setTeam(p);
                }

                cPlayer.setHouse(iHouse);

                // from here, ignore non playable factions
                if (!playableFaction) continue;

                cPlayer.focus_cell = iStartPositions[p];

                // Set map position
                if (p == HUMAN) {
                    mapCamera->centerAndJumpViewPortToCell(cPlayer.focus_cell);
                }

                // create constyard
                cAbstractStructure *s = cStructureFactory::getInstance()->createStructure(cPlayer.focus_cell, CONSTYARD, p);

                // when failure, create mcv instead
                if (s == NULL) {
                    UNIT_CREATE(cPlayer.focus_cell, MCV, p, true);
                }

                // amount of units
                int u=0;

                // create units
                while (u < aiPlayer.iUnits) {
                    int iX= map.getCellX(cPlayer.focus_cell);
                    int iY= map.getCellY(cPlayer.focus_cell);
                    int iType=rnd(12);

                    iX-=4;
                    iY-=4;
                    iX+=rnd(9);
                    iY+=rnd(9);

                    // convert house specific stuff
                    if (cPlayer.getHouse() == ATREIDES) {
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
                    if (cPlayer.getHouse() == ORDOS) {
                        if (iType == DEVASTATOR || iType == SONICTANK) {
                            iType = DEVIATOR;
                        }

                        if (iType == TRIKE) {
                            iType = RAIDER;
                        }
                    }

                    // harkonnen
                    if (cPlayer.getHouse() == HARKONNEN) {
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

                    int cell = map.getCellWithMapBorders(iX, iY);
                    int r = UNIT_CREATE(cell, iType, p, true);
                    if (r > -1)
                    {
                        u++;
                    }
                }

                char msg[255];
                sprintf(msg, "Wants %d amount of units; amount created %d", aiPlayer.iUnits, u);
                cLogger::getInstance()->log(LOG_TRACE, COMP_SKIRMISHSETUP, "Creating units", msg, OUTC_NONE, p, iHouse);
            }

            bFadeOut=true;
            playMusicByType(MUSIC_PEACE);

            // TODO: spawn a few worms
            setState(GAME_PLAYING);
            drawManager->getMessageDrawer()->initCombatPosition();

        } // mouse clicks on START (and skirmish map is selected)
    } // mouse hovers over "START"

   	// MOUSE
    draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);

    if (bFadeOut) {
        game.FADE_OUT();
    }
}

// select house
void cGame::stateSelectHouse() {
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

	// Render the planet Dune a bit downward
    BITMAP *duneBitmap = (BITMAP *) gfxinter[BMP_GAME_DUNE].dat;
    draw_sprite(bmp_screen, duneBitmap, ((game.screen_x - duneBitmap->w)), ((game.screen_y - (duneBitmap->h * 0.90))));

	// HOUSES
    BITMAP *sprite = (BITMAP *) gfxinter[BMP_SELECT_YOUR_HOUSE].dat;
    int selectYourHouseXCentered = (game.screen_x / 2) - sprite->w / 2;
    draw_sprite(bmp_screen, sprite, selectYourHouseXCentered, 0);

    int selectYourHouseY = game.screen_y * 0.25f;

    int columnWidth = game.screen_x / 7; // empty, atr, empty, har, empty, ord, empty (7 columns)
    int offset = (columnWidth / 2) - (((BITMAP *)gfxinter[BMP_SELECT_HOUSE_ATREIDES].dat)->w / 2);
    cRectangle houseAtreides = cRectangle((columnWidth * 1) + offset, selectYourHouseY, 90, 98);
    cRectangle houseOrdos = cRectangle((columnWidth * 3) + offset, selectYourHouseY, 90, 98);
    cRectangle houseHarkonnen = cRectangle((columnWidth * 5) + offset, selectYourHouseY, 90, 98);
    allegroDrawer->blitSprite((BITMAP *)gfxinter[BMP_SELECT_HOUSE_ATREIDES].dat, bmp_screen, &houseAtreides);
    allegroDrawer->blitSprite((BITMAP *)gfxinter[BMP_SELECT_HOUSE_ORDOS].dat, bmp_screen, &houseOrdos);
    allegroDrawer->blitSprite((BITMAP *)gfxinter[BMP_SELECT_HOUSE_HARKONNEN].dat, bmp_screen, &houseHarkonnen);

    cTextDrawer textDrawer = cTextDrawer(bene_font);

    // back
    cRectangle *backButtonRect = textDrawer.getAsRectangle(0, screen_y - textDrawer.getFontHeight(), " BACK");
    textDrawer.drawText(backButtonRect->getX(), backButtonRect->getY(), makecol(255,255,255), " BACK");

    if (backButtonRect->isMouseOver(mouse->getX(), mouse->getY())) {
        textDrawer.drawText(backButtonRect->getX(), backButtonRect->getY(), makecol(255, 0, 0), " BACK");
    }

    if (mouse->isLeftButtonClicked()) {
        delete pMentat;
        pMentat = nullptr;
        if (mouse->isOverRectangle(&houseAtreides)) {
            prepareMentatToTellAboutHouse(ATREIDES);

            play_sound_id(SOUND_ATREIDES);

            setState(GAME_TELLHOUSE);
            bFadeOut=true;
        } else if (mouse->isOverRectangle(&houseOrdos)) {
            prepareMentatToTellAboutHouse(ORDOS);

            play_sound_id(SOUND_ORDOS);

            setState(GAME_TELLHOUSE);
            bFadeOut=true;
        } else if (mouse->isOverRectangle(&houseHarkonnen)) {
            prepareMentatToTellAboutHouse(HARKONNEN);

            play_sound_id(SOUND_HARKONNEN);

            setState(GAME_TELLHOUSE);
            bFadeOut=true;
        } else if (backButtonRect->isMouseOver(mouse->getX(), mouse->getY())) {
            bFadeOut=true;
            setState(GAME_MENU);
        }
    }

    delete backButtonRect;

	// MOUSE
    draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);

    if (bFadeOut) {
        game.FADE_OUT();
    }
}

// select your next conquest
void cGame::stateSelectYourNextConquest() {
    gameState->draw();
    gameState->interact();
}

void cGame::destroyAllUnits(bool bHumanPlayer) {
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

void cGame::destroyAllStructures(bool bHumanPlayer) {
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
	// blitSprite on screen

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
        case GAME_SELECT_HOUSE:
            stateSelectHouse();
            break;
        case GAME_TELLHOUSE:
            stateMentat(pMentat);
            break;
		case GAME_PLAYING:
			combat();
			break;
		case GAME_BRIEFING:
            stateMentat(pMentat);
			break;
		case GAME_SETUPSKIRMISH:
			setup_skirmish();
			break;
		case GAME_MENU:
			menu();
			break;
		case GAME_REGION:
            stateSelectYourNextConquest();
			break;
		case GAME_WINNING:
			winning();
			break;
		case GAME_LOSING:
			losing();
			break;
		case GAME_WINBRIEF:
            stateMentat(pMentat);
			break;
		case GAME_LOSEBRIEF:
            stateMentat(pMentat);
			break;
        // TODO: GAME_STATISTICS, ETC
	}
}

/**
	Main game loop
*/
void cGame::run() {
	set_trans_blender(0, 0, 0, 128);

	while (bPlaying) {
		TimeManager.processTime();
		clear(bmp_screen);
        updateState();
		handleTimeSlicing(); // handle time diff (needs to change!)
		runGameState(); // run game state, includes interaction + drawing
		interactionManager->interactWithKeyboard(); // generic interaction
		shakeScreenAndBlitBuffer(); // finally draw the bmp_screen to real screen (double buffering)
		frame_count++;
	}
}


/**
	Shutdown the game
*/
void cGame::shutdown() {
	cLogger *logger = cLogger::getInstance();
	logger->logHeader("SHUTDOWN");

    if (gameState != nullptr) {
        if (gameState->getType() != eGameStateType::SELECT_YOUR_NEXT_CONQUEST) {
            // destroy game state object, unless we talk about the region select
            delete gameState;
        } else {
            gameState = nullptr;
        }
    }

    selectYourNextConquestState->destroy(); // destroy the world bitmaps
    // delete explicitly here
	delete selectYourNextConquestState;

	if (soundPlayer) {
        soundPlayer->destroyAllSounds();
    }
    delete soundPlayer;

    delete pMentat;
    delete mapViewport;

    drawManager->destroy();
    delete drawManager;

    delete mapCamera;
    delete interactionManager;


    cStructureFactory::destroy();
    cSideBarFactory::destroy();
    cBuildingListFactory::destroy();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        player[i].destroyAllegroBitmaps();
    }

    delete allegroDrawer;
    delete mouse;

    if (gfxdata) {
        unload_datafile(gfxdata);
    }
    if (gfxinter) {
        unload_datafile(gfxinter);
    }
    if (gfxworld) {
        unload_datafile(gfxworld);
    }
    if (gfxmentat) {
        unload_datafile(gfxmentat);
    }

    // Destroy font of Allegro FONT library
	alfont_destroy_font(game_font);
	alfont_destroy_font(bene_font);

	// Exit the font library (must be first)
	alfont_exit();

	logbook("Allegro FONT library shut down.");

	// MP3 Library
	if (mp3_music) {
		almp3_stop_autopoll_mp3(mp3_music); // stop auto updateState
		almp3_destroy_mp3(mp3_music);
	}

	logbook("Allegro MP3 library shut down.");

	// Now we are all neatly closed, we exit Allegro and return to OS.
	allegro_exit();

	logbook("Allegro shut down.");
	logbook("Thanks for playing.");

    cLogger::destroy();
}

bool cGame::isResolutionInGameINIFoundAndSet() {
    return game.ini_screen_height != -1 && game.ini_screen_width != -1;
}

void cGame::setScreenResolutionFromGameIniSettings() {
    if (game.ini_screen_width < 800) {
        game.ini_screen_width = 800;
        logbook("INI screen width < 800; unsupported; will set to 800.");
    }
    if (game.ini_screen_height < 600) {
        game.ini_screen_height = 600;
        logbook("INI screen height < 600; unsupported; will set to 600.");
    }
    game.screen_x = game.ini_screen_width;
    game.screen_y = game.ini_screen_height;
    char msg[255];
    sprintf(msg, "Resolution %dx%d loaded from ini file.", game.ini_screen_width, game.ini_screen_height);
    cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Resolution from ini file", msg);
}

/**
	Setup the game

	Should not be called twice.
*/
bool cGame::setupGame() {
	cLogger *logger = cLogger::getInstance();

	game.init(); // Must be first!

	logger->clearLogFile();

	logger->logHeader("Dune II - The Maker");
	logger->logCommentLine(""); // whitespace

	logger->logHeader("Version information");
	char msg[255];
	sprintf(msg, "Version %s, Compiled at %s , %s", game.version, __DATE__, __TIME__);
	logger->log(LOG_INFO, COMP_VERSION, "Initializing", msg);

	// init game
	if (game.windowed) {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Windowed mode");
	} else {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Fullscreen mode");
	}

	// TODO: load eventual game settings (resolution, etc)

	// Logbook notification
	logger->logHeader("Allegro");

	// ALLEGRO - INIT
	if (allegro_init() != 0) {
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_FAILED);
		return false;
	}

    allegroDrawer = new cAllegroDrawer();

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
	mouse = new cMouse();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Mouse", "install_mouse()", OUTC_SUCCESS);

	/* set up the interrupt routines... */
	game.TIMER_shake=0;

	LOCK_VARIABLE(allegro_timerUnits);
	LOCK_VARIABLE(allegro_timerGlobal);
	LOCK_VARIABLE(allegro_timerSecond);

	LOCK_FUNCTION(allegro_timerunits);
	LOCK_FUNCTION(allegro_timerglobal);
	LOCK_FUNCTION(allegro_timerfps);

	// Install timers
	install_int(allegro_timerunits, 100); // 100 miliseconds
	install_int(allegro_timerglobal, 5); // 5 miliseconds
	install_int(allegro_timerfps, 1000); // 1000 miliseconds (seconds)

	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up timer related variables", "LOCK_VARIABLE/LOCK_FUNCTION", OUTC_SUCCESS);

	frame_count = fps = 0;

	// set window title
	char title[128];
	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)", game.version);

	// Set window title
	set_window_title(title);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", title, OUTC_SUCCESS);

    int colorDepth = desktop_color_depth();
    set_color_depth(colorDepth);

    char colorDepthMsg[255];
    sprintf(colorDepthMsg,"Desktop color dept is %d.", colorDepth);
    cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.", colorDepthMsg);


	// TODO: read/write rest value so it does not have to 'fine-tune'
	// but is already set up. Perhaps even offer it in the options screen? So the user
	// can specify how much CPU this game may use?

	if (game.windowed) {
		cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Windowed mode requested.", "");

		if (isResolutionInGameINIFoundAndSet()) {
			setScreenResolutionFromGameIniSettings();
		}

        r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, game.screen_x, game.screen_y, game.screen_x, game.screen_y);

		char msg[255];
		sprintf(msg, "Initializing graphics mode (windowed) with resolution %d by %d, colorDepth %d.", game.screen_x, game.screen_y, colorDepth);

		if (r > -1) {
			logger->log(LOG_INFO, COMP_ALLEGRO, msg, "Succesfully created window with graphics mode.", OUTC_SUCCESS);
		} else {
		    allegro_message("Failed to initialize graphics mode");
		    return false;
		}
	} else {
        /**
         * Fullscreen mode
        */

		bool mustAutoDetectResolution = false;
		if (isResolutionInGameINIFoundAndSet()) {
			setScreenResolutionFromGameIniSettings();
			r = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
			char msg[255];
			sprintf(msg,"Setting up %dx%d resolution from ini file.", game.ini_screen_width, game.ini_screen_height);
			cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Custom resolution from ini file.", msg);
            mustAutoDetectResolution = (r > -1);
		} else {
            cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Custom resolution from ini file.", "No resolution defined in ini file.");
		}

		// find best possible resolution
		if (!mustAutoDetectResolution) {
            cLogger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Autodetecting resolutions", "Commencing");
            // find best possible resolution
            cBestScreenResolutionFinder bestScreenResolutionFinder;
            bestScreenResolutionFinder.checkResolutions();
            bool result = bestScreenResolutionFinder.acquireBestScreenResolutionFullScreen();

            // success
            if (result) {
                logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fullscreen)", "Succesfully initialized graphics mode.", OUTC_SUCCESS);
            } else {
                logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fullscreen)", "Failed to initializ graphics mode.", OUTC_FAILED);
                allegro_message("Fatal error:\n\nCould not start game.\n\nGraphics mode (fullscreen) could not be initialized.");
                return false;
            }
		}

	}

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

	small_font = alfont_load_font("data/small.ttf");

	if (small_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded small.ttf", OUTC_SUCCESS);
		alfont_set_font_size(small_font, 10); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load small.ttf", OUTC_FAILED);
		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load small.ttf");
		return false;
	}

	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0)	{
		set_display_switch_mode(SWITCH_PAUSE);
		logbook("Display 'switch and pause' mode set");
	} else {
		logbook("Display 'switch to background' mode set");
	}

	int maxSounds = getAmountReservedVoicesAndInstallSound();
	memset(msg, 0, sizeof(msg));

	if (maxSounds > -1) {
		sprintf(msg, "Successfully installed sound. %d voices reserved", maxSounds);
		logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
	} else {
		logger->log(LOG_INFO, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
	}
	soundPlayer = new cSoundPlayer(maxSounds);

	/***
	Bitmap Creation
	***/
    mapViewport = new cRectangle(0, cSideBar::TopBarHeight, game.screen_x-cSideBar::SidebarWidth, game.screen_y-cSideBar::TopBarHeight);

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

    bmp_backgroundMentat = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_backgroundMentat == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_backgroundMentat");
		return false;
	}
	else
	{
		logbook("Memory bitmap created: bmp_backgroundMentat");
		clear(bmp_backgroundMentat);

		// create only once
        clear_to_color(bmp_backgroundMentat, makecol(8,8,16));
        bool offsetX = false;

        float horizon = game.screen_y / 2;
        float centered = game.screen_x / 2;
        for (int y = 0; y < game.screen_y; y++) {
            float diffYToCenter = 1.0f;
            if (y < horizon) {
                diffYToCenter = y / horizon;
            } else {
                diffYToCenter = 1 - ((y-horizon) / horizon);
            }

            for (int x = offsetX ? 0 : 1; x < game.screen_x; x += 2) {
                float diffXToCenter = 1.0f;
                if (x < centered) {
                    diffXToCenter = x / centered;
                } else {
                    diffXToCenter = 1-((x-centered) / centered);
                }

                float red = 2 + (12 * diffXToCenter) + (12 * diffYToCenter);
                float green = 2 + (12 * diffXToCenter) + (12 * diffYToCenter);
                float blue = 4 + (24 * diffXToCenter) + (24 * diffYToCenter);
                putpixel(bmp_backgroundMentat, x, y, makecol((int)red, (int)green, (int)blue));
            }
            // flip offset every y row
            offsetX = !offsetX;
        }
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
	set_mouse_speed(0,0);

	logbook("MOUSE: Mouse speed set");

	logbook("\n----");
	logbook("GAME ");
	logbook("----");

	/*** Data files ***/

	// load datafiles
	gfxdata = load_datafile("data/gfxdata.dat");
	if (gfxdata == nullptr) {
		logbook("ERROR: Could not hook/load datafile: gfxdata.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxdata.dat");
		memcpy(general_palette, gfxdata[PALETTE_D2TM].dat, sizeof general_palette);
	}

	gfxaudio = load_datafile("data/gfxaudio.dat");
	if (gfxaudio == nullptr)  {
		logbook("ERROR: Could not hook/load datafile: gfxaudio.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxaudio.dat");
	}

	gfxinter = load_datafile("data/gfxinter.dat");
	if (gfxinter == nullptr)  {
		logbook("ERROR: Could not hook/load datafile: gfxinter.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxinter.dat");
	}

	gfxworld = load_datafile("data/gfxworld.dat");
	if (gfxworld == nullptr) {
		logbook("ERROR: Could not hook/load datafile: gfxworld.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxworld.dat");
	}

	gfxmentat = load_datafile("data/gfxmentat.dat");
	if (gfxworld == nullptr) {
		logbook("ERROR: Could not hook/load datafile: gfxmentat.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxmentat.dat");
	}

	// randomize timer
	unsigned int t = (unsigned int) time(0);
	char seedtxt[80];
	sprintf(seedtxt, "Seed is %u", t);
	logbook(seedtxt);
	srand(t);

	game.bPlaying = true;
	game.screenshot = 0;
	game.state = -1;

	// Mouse stuff
	mouse_status = MOUSE_STATE_NORMAL;
	mouse_tile = 0;

	set_palette(general_palette);

	// normal sounds are loud, the music is lower (its background music, so it should not be disturbing)
	set_volume(iMaxVolume, 150);

	// A few messages for the player
	logbook("Initializing:  PLAYERS");
    INIT_ALL_PLAYERS();
	logbook("Setup:  HOUSES");
	INSTALL_HOUSES();
    logbook("Setup:  STRUCTURES");
	install_structures();
    logbook("Setup:  PROJECTILES");
	install_bullets();
    logbook("Setup:  UNITS");
	install_units();
    logbook("Setup:  SPECIALS");
	install_specials();
	logbook("Setup:  WORLD");
	selectYourNextConquestState->INSTALL_WORLD();

    delete mapCamera;
	mapCamera = new cMapCamera(&map);

    delete drawManager;
	drawManager = new cDrawManager(&player[HUMAN]);

	game.init();

	// do install_upgrades after game.init, because game.init loads the INI file and then has the very latest
	// unit/structures catalog loaded - which the install_upgrades depends on.
    install_upgrades();

	game.setup_players();

	playMusicByType(MUSIC_MENU);

	// all has installed well. Lets rock and roll.
	return true;

}

/**
 * Set up players
 */
void cGame::setup_players() {
    mouse->setMouseObserver(nullptr);

	// make sure each player has an own item builder
	for (int i = HUMAN; i < MAX_PLAYERS; i++) {
		cPlayer * thePlayer = &player[i];

		cSideBar * sidebar = cSideBarFactory::getInstance()->createSideBar(thePlayer);
		thePlayer->setSideBar(sidebar);

		cBuildingListUpdater * buildingListUpdater = new cBuildingListUpdater(thePlayer);
		thePlayer->setBuildingListUpdater(buildingListUpdater);

        cItemBuilder * itemBuilder = new cItemBuilder(thePlayer, buildingListUpdater);
        thePlayer->setItemBuilder(itemBuilder);

		cStructurePlacer * structurePlacer = new cStructurePlacer(thePlayer);
		thePlayer->setStructurePlacer(structurePlacer);

		cOrderProcesser * orderProcesser = new cOrderProcesser(thePlayer);
		thePlayer->setOrderProcesser(orderProcesser);

		cGameControlsContext * gameControlsContext = new cGameControlsContext(thePlayer);
		thePlayer->setGameControlsContext(gameControlsContext);

		// set tech level
		thePlayer->setTechLevel(game.iMission);
	}

    delete interactionManager;
    cPlayer *humanPlayer = &player[HUMAN];
    interactionManager = new cInteractionManager(humanPlayer);
    mouse->setMouseObserver(interactionManager);
}

bool cGame::isState(int thisState) {
	return (state == thisState);
}

void cGame::setState(int newState) {
    char msg[255];
    sprintf(msg, "Setting state from %d to %d", state, newState);
    logbook(msg);

    if (gameState != nullptr) {
        if (gameState->getType() != eGameStateType::SELECT_YOUR_NEXT_CONQUEST) {
            // destroy game state object, unless we talk about the region select
            // because it needs to remember while we play the game (for now)
            delete gameState;
        } else {
            gameState = nullptr;
        }
    }

    if (newState == GAME_REGION) {
        gameState = selectYourNextConquestState;
    }

	state = newState;
}

int cGame::getFadeSelect() {
    return fade_select;
}

void cGame::think_fading() {
    // Fading / pulsating of selected stuff
    if (bFadeSelectDir) {
        fade_select++;

        // when 255, then fade back
        if (fade_select > 254) bFadeSelectDir=false;

        return;
    }

    fade_select--;
    // not too dark,
    if (fade_select < 32) bFadeSelectDir = true;
}

cGame::~cGame() {
    // cannot do this, because when game is being quit, and the cGame object being deleted, Allegro has been shut down
    // already, so the deletion of drawManager has to happen *before* that, hence look in shutdown() method
//    if (drawManager) {
//        delete drawManager;
//    }
}

void cGame::prepareMentatForPlayer() {
    int house = player[HUMAN].getHouse();
    if (state == GAME_BRIEFING) {
        game.mission_init();
        game.setup_players();
        INI_Load_scenario(house, iRegion, pMentat);
        INI_LOAD_BRIEFING(house, iRegion, INI_BRIEFING, pMentat);
    } else if (state == GAME_WINBRIEF) {
        if (rnd(100) < 50) {
            pMentat->loadScene("win01");
        } else {
            pMentat->loadScene("win02");
        }
        INI_LOAD_BRIEFING(house, iRegion, INI_WIN, pMentat);
    } else if (state == GAME_LOSEBRIEF) {
        if (rnd(100) < 50) {
            pMentat->loadScene("lose01");
        } else {
            pMentat->loadScene("lose02");
        }

        INI_LOAD_BRIEFING(house, iRegion, INI_LOSE, pMentat);
    }
}

void cGame::createAndPrepareMentatForHumanPlayer() {
    delete pMentat;
    int houseIndex = player[0].getHouse();
    if (houseIndex == ATREIDES) {
        pMentat = new cAtreidesMentat();
    } else if (houseIndex == HARKONNEN) {
        pMentat = new cHarkonnenMentat();
    } else if (houseIndex == ORDOS) {
        pMentat = new cOrdosMentat();
    } else {
        // fallback
        pMentat = new cBeneMentat();
    }
    prepareMentatForPlayer();
    pMentat->speak();
}

void cGame::prepareMentatToTellAboutHouse(int house) {
    delete pMentat;
    pMentat = new cBeneMentat();
    pMentat->setHouse(house);
    // create new stateMentat
    if (house == ATREIDES) {
        INI_LOAD_BRIEFING(ATREIDES, 0, INI_DESCRIPTION, pMentat);
        pMentat->loadScene("platr"); // load planet of atreides
    } else if (house == HARKONNEN) {
        INI_LOAD_BRIEFING(HARKONNEN, 0, INI_DESCRIPTION, pMentat);
        pMentat->loadScene("plhar"); // load planet of harkonnen
    } else if (house == ORDOS) {
        INI_LOAD_BRIEFING(ORDOS, 0, INI_DESCRIPTION, pMentat);
        pMentat->loadScene("plord"); // load planet of ordos
    }
    // todo: Sardaukar, etc? (Super Dune 2 features)

    pMentat->speak();
}

void cGame::loadScenario() {
    int iHouse = player[HUMAN].getHouse();
    INI_Load_scenario(iHouse, game.iRegion, pMentat);
}

void cGame::think_state() {
    if (gameState) {
        gameState->think();
    }
}
