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
#include "cGame.h"


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
		players[i].init(i, nullptr);
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

    int maxThinkingAIs = MAX_PLAYERS;
    if (bOneAi) {
        maxThinkingAIs = 1;
    }
    // clear out players but not entirely
    for (int i=0; i < MAX_PLAYERS; i++) {
        cPlayer &pPlayer = players[i];
        int h = pPlayer.getHouse();

        if (i == HUMAN) {
            pPlayer.init(i, nullptr);
        } else if (i < AI_CPU5) {
            // TODO: playing attribute? (from ai player class?)
            if (game.bDisableAI) {
                pPlayer.init(i, nullptr);
            } else {
                if (game.bSkirmish) {
                    if (maxThinkingAIs > 0) {
                        pPlayer.init(i, new brains::cPlayerBrainSkirmish(&pPlayer));
                    } else {
                        pPlayer.init(i, nullptr);
                    }
                } else {
                    if (maxThinkingAIs > 0) {
                        pPlayer.init(i, new brains::cPlayerBrainCampaign(&pPlayer));
                    } else {
                        pPlayer.init(i, nullptr);
                    }
                    pPlayer.setAutoSlabStructures(true); // campaign based AI's autoslab structures...
                }
            }
            maxThinkingAIs--;
        } else if (i == AI_CPU5) {
            pPlayer.init(i, new brains::cPlayerBrainFremenSuperWeapon(&pPlayer));
        } else if (i == AI_CPU6) {
            pPlayer.init(i, new brains::cPlayerBrainSandworm(&pPlayer));
        }
        pPlayer.setHouse(h);

        if (bSkirmish) {
            pPlayer.setCredits(2500);
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
        if (players[HUMAN].hasEnoughCreditsFor(iWinQuota)) {
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
                if (cUnit.isDead()) continue; // in case we have some 'half-dead' units that got passed the isValid check...
                                              // a better way for this would be to have such units in a separate collection.
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
        cPlayer *pPlayer = &players[i];
        cGameControlsContext *context = pPlayer->getGameControlsContext();

        pPlayer->update();

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

                        if (!unit[id].getPlayer()->isSameTeamAs(&players[HUMAN])) {
                            mouse_tile = MOUSE_ATTACK;
                        }
                    }

                    int idOfStructureOnCell = map.getCellIdStructuresLayer(mc);

                    if (idOfStructureOnCell > -1) {
                        int id = idOfStructureOnCell;

                        if (!structure[id]->getPlayer()->isSameTeamAs(&players[HUMAN])) {
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
		if (mouse->isLeftButtonClicked()) {
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
}

void cGame::setup_skirmish() {
    gameState->draw();
    gameState->interact();
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
		_interactionManager->interactWithKeyboard(); // generic interaction
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
        if (gameState->getType() != eGameStateType::GAMESTATE_SELECT_YOUR_NEXT_CONQUEST) {
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
    delete _interactionManager;


    cStructureFactory::destroy();
    cSideBarFactory::destroy();
    cBuildingListFactory::destroy();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].destroyAllegroBitmaps();
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
	LOCK_FUNCTION(allegro_timergametime);
	LOCK_FUNCTION(allegro_timerseconds);

	// Install timers
	install_int(allegro_timerunits, 100); // 100 milliseconds
    install_int(allegro_timergametime, 5); // 5 milliseconds / hence, in 1 second the gametime has passed 1000/5 = 200 times
    install_int(allegro_timerseconds, 1000); // 1000 milliseconds (seconds)

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

    logger->logHeader("GAME");

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
	game.state = GAME_INITIALIZE;

	// Mouse stuff
	mouse_tile = 0;

	set_palette(general_palette);

	// normal sounds are loud, the music is lower (its background music, so it should not be disturbing)
	set_volume(iMaxVolume, 150);

	// A few messages for the player
	logbook("Initializing:  PLAYERS");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].init(i, nullptr);
    }
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
	drawManager = new cDrawManager(&players[HUMAN]);

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
		cPlayer * thePlayer = &players[i];

		cSideBar * sidebar = cSideBarFactory::getInstance()->createSideBar(thePlayer);
		thePlayer->setSideBar(sidebar);

		cBuildingListUpdater * buildingListUpdater = new cBuildingListUpdater(thePlayer);
		thePlayer->setBuildingListUpdater(buildingListUpdater);

        cItemBuilder * itemBuilder = new cItemBuilder(thePlayer, buildingListUpdater);
        thePlayer->setItemBuilder(itemBuilder);

		cOrderProcesser * orderProcesser = new cOrderProcesser(thePlayer);
		thePlayer->setOrderProcesser(orderProcesser);

		cGameControlsContext * gameControlsContext = new cGameControlsContext(thePlayer);
		thePlayer->setGameControlsContext(gameControlsContext);

		// set tech level
		thePlayer->setTechLevel(game.iMission);
	}

    delete _interactionManager;
    cPlayer *humanPlayer = &players[HUMAN];
    _interactionManager = new cInteractionManager(humanPlayer);
    mouse->setMouseObserver(_interactionManager);
}

bool cGame::isState(int thisState) {
	return (state == thisState);
}

void cGame::setState(int newState) {
    char msg[255];
    sprintf(msg, "Setting state from %d(=%s) to %d(=%s)", state, stateString(state), newState, stateString(newState));
    logbook(msg);

    if (gameState != nullptr) {
        if (gameState->getType() != eGameStateType::GAMESTATE_SELECT_YOUR_NEXT_CONQUEST) {
            // destroy game state object, unless we talk about the region select
            // because it needs to remember while we play the game (for now)
            delete gameState;
            gameState = nullptr;
        } else {
            gameState = nullptr;
        }
    }

    if (newState == GAME_REGION) {
        gameState = selectYourNextConquestState;
    } else if (newState == GAME_SETUPSKIRMISH) {
        gameState = new cSetupSkirmishGameState(*this);
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
    int house = players[HUMAN].getHouse();
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
    int houseIndex = players[0].getHouse();
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
    int iHouse = players[HUMAN].getHouse();
    INI_Load_scenario(iHouse, game.iRegion, pMentat);
}

void cGame::think_state() {
    if (gameState) {
        gameState->think();
    }
}

void cGame::setPlayerToInteractFor(cPlayer *pPlayer) {
    _interactionManager->setPlayerToInteractFor(pPlayer);
}

void cGame::onNotify(const s_GameEvent &event) {
    logbook(s_GameEvent::toString(event).c_str());

    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].onNotify(event);
    }

    // TODO: notify game state
    // gameState->onNotify(event);

    switch (event.eventType) {
        case eGameEventType::GAME_EVENT_DISCOVERED:
            onEventDiscovered(event);
            return;
        case eGameEventType::GAME_EVENT_SPECIAL_DEPLOYED:
            onEventSpecialDeployed(event);
            return;
        default:
        return;
    }

}

void cGame::onEventDiscovered(const s_GameEvent &event) {
    int voiceId = -1;

    // when state of music is not attacking, do attacking stuff and say "Warning enemy unit approaching
    if (game.iMusicType == MUSIC_PEACE) {
        bool triggerMusic = false;

        if (event.entityType == eBuildType::UNIT) {
            cUnit &cUnit = unit[event.entityID];

            if (!event.player->isSameTeamAs(&players[HUMAN])) {
                triggerMusic = true;
                if (cUnit.iType == SANDWORM) {
                    voiceId = SOUND_VOICE_10_ATR;
                } else {
                    voiceId = SOUND_VOICE_09_ATR;
                }
            }
        } else if (event.entityType == eBuildType::STRUCTURE) {
            if (!event.player->isSameTeamAs(&players[HUMAN])) {
                // only things that can harm us will trigger attack music?
                if (event.entitySpecificType == RTURRET || event.entitySpecificType == TURRET) {
                    triggerMusic = true;
                }
            }
        }

        if (triggerMusic) {
            playMusicByType(MUSIC_ATTACK);
        }
    }

    if (voiceId > -1) {
        play_voice(voiceId);
    }
}

void cGame::onEventSpecialDeployed(const s_GameEvent &event) {
    cBuildingListItem *itemToDeploy = event.buildingListItem;
    int iMouseCell = event.atCell;
    cPlayer *player = event.player;
    if (itemToDeploy->getBuildType() == eBuildType::SPECIAL) {
        const s_Special &special = itemToDeploy->getS_Special();

        int deployCell = -1;
        if (special.deployTargetType == eDeployTargetType::TARGET_SPECIFIC_CELL) {
            deployCell = iMouseCell;
        } else if (special.deployTargetType == eDeployTargetType::TARGET_INACCURATE_CELL) {
            int precision = special.deployTargetPrecision;
            int mouseCellX = map.getCellX(iMouseCell) - precision;
            int mouseCellY = map.getCellY(iMouseCell) - precision;

            int posX = mouseCellX + rnd((precision*2) + 1);
            int posY = mouseCellY + rnd((precision*2) + 1);
            FIX_POS(posX, posY);

            char msg[255];
            sprintf(msg, "eDeployTargetType::TARGET_INACCURATE_CELL, mouse cell X,Y = %d,%d - target pos =%d,%d - precision %d", mouseCellY, mouseCellY, posX, posY,
                    precision);
            logbook(msg);

            deployCell = map.makeCell(posX, posY);
        }


        if (special.providesType == eBuildType::BULLET) {
            // from where
            int structureId = structureUtils.findStructureBy(player->getId(), special.deployAtStructure, false);
            if (structureId > -1) {
                cAbstractStructure *pStructure = structure[structureId];
                if (pStructure && pStructure->isValid()) {
                    play_sound_id(SOUND_PLACE);
                    create_bullet(special.providesTypeId, pStructure->getCell(), deployCell, -1, structureId);
                }
            }
        }
    }

    itemToDeploy->decreaseTimesToBuild();
    itemToDeploy->setDeployIt(false);
    itemToDeploy->setIsBuilding(false);
    itemToDeploy->resetProgress();
    if (itemToDeploy->getTimesToBuild() < 1) {
        player->getItemBuilder()->removeItemFromList(itemToDeploy);
    }

    game.bDeployIt = false;
}
