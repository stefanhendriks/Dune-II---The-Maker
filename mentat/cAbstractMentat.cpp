/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"
#include "cAbstractMentat.h"


// "default" Constructor
cAbstractMentat::cAbstractMentat() {
	iMentatSentence = -1;

	TIMER_Speaking = -1;
	TIMER_Mouth = 0;
	TIMER_Eyes = 0;
	TIMER_Other = 0;
    TIMER_movie = 0;

	iMentatMouth = 3;
	iMentatEyes = 3;
    iMovieFrame=-1;

    iBackgroundFrame = -1;

    gfxmovie = nullptr;

	memset(sentence, 0, sizeof(sentence));
}

cAbstractMentat::~cAbstractMentat() {
	iMentatSentence = -1;

	TIMER_Speaking = -1;
	TIMER_Mouth = 0;
	TIMER_Eyes = 0;
	TIMER_Other = 0;
    TIMER_movie = 0;

	iMentatMouth = 3;
	iMentatEyes = 3;
    iMovieFrame=-1;

    iBackgroundFrame = -1;

    // we can do this because Mentats are created/deleted before allegro gets destroyed
    if (gfxmovie) {
        unload_datafile(gfxmovie);
    }

    gfxmovie = nullptr;

	memset(sentence, 0, sizeof(sentence));
}

void cAbstractMentat::think() {
    // mentat mouth
    if (TIMER_Mouth <= 0) {
        TIMER_Mouth = 13 + rnd(5);
    }

    if (TIMER_Speaking > 0) {
        TIMER_Speaking--;
    }

    if (TIMER_Speaking == 0) {
        // calculate speaking stuff

        iMentatSentence += 2; // makes 0, 2, etc.

        if (iMentatSentence > 8) {
            iMentatSentence = -2;
            TIMER_Speaking = -1;
            return;
        }

        // lentgh calculation of time
        int iLength = strlen(sentence[iMentatSentence]);
        iLength += strlen(sentence[iMentatSentence + 1]);

        if (iLength < 2) {
            iMentatSentence = -2;
            TIMER_Speaking = -1;
            return;
        }

        TIMER_Speaking = iLength * 12;
    }

    if (TIMER_Mouth > 0) {
        TIMER_Mouth--;
    } else if (TIMER_Mouth == 0) {

        if (TIMER_Speaking > 0) {
            int iOld = iMentatMouth;

            if (iMentatMouth == 0) {
                // when mouth is shut, we wait a bit.
                if (rnd(100) < 45) {
                    iMentatMouth += (1 + rnd(4));
                } else {
                    TIMER_Mouth = 3; // wait
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
            if (TIMER_Mouth == 0) {
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

        TIMER_Mouth = -1; // this way we make sure we do not update it too much
    } // speaking


    if (TIMER_Eyes > 0) {
        TIMER_Eyes--;
    } else {
        int i = rnd(100);

        int iWas = iMentatEyes;

        if (i < 30) {
            iMentatEyes = 3;
        } else if (i >= 30 && i < 60) {
            iMentatEyes = 0;
        } else {
            iMentatEyes = 4;
        }

        // its the same
        if (iMentatEyes == iWas) {
            iMentatEyes = rnd(4);
        }

        if (iMentatEyes != 4) {
            TIMER_Eyes = 90 + rnd(160);
        } else {
            TIMER_Eyes = 30;
        }
    }

    if (gfxmovie != NULL) {
        TIMER_movie++;

        if (TIMER_movie > 20) {
            iMovieFrame++;

            if (gfxmovie[iMovieFrame].type == DAT_END ||
                gfxmovie[iMovieFrame].type != DAT_BITMAP) {
                iMovieFrame = 0;
            }
            TIMER_movie = 0;
        }
    }
}

void cAbstractMentat::prepare(bool bTellHouse, int state, int house, int region) {
//    if (state == GAME_BRIEFING) {
//        INI_Load_scenario(house, region);
//        INI_LOAD_BRIEFING(house, region, INI_BRIEFING);
//    } else if (state == GAME_WINBRIEF) {
//        if (rnd(100) < 50) {
//            std::string scene = "win01";
//            LOAD_SCENE(scene); // ltank
//        } else {
//            std::string scene = "win02";
//            LOAD_SCENE(scene); // ltank
//        }
//
//        INI_LOAD_BRIEFING(house, region, INI_WIN);
//    } else if (state == GAME_LOSEBRIEF)	{
//        if (rnd(100) < 50) {
//            std::string scene = "lose01";
//            LOAD_SCENE(scene); // ltank
//        } else {
//            std::string scene = "lose02";
//            LOAD_SCENE(scene); // ltank
//        }
//        INI_LOAD_BRIEFING(house, region, INI_LOSE);
//    }
}

void cAbstractMentat::draw() {
    select_palette(general_palette);

    // movie
    draw_movie();

    draw_sprite(bmp_screen, getBackgroundBitmap(), 0, 0);

    draw_eyes();
    draw_mouth();
    draw_other();

    // draw text
    if (iMentatSentence >= 0) {
        alfont_textprintf(bmp_screen, bene_font, 17, 17, makecol(0, 0, 0), "%s", sentence[iMentatSentence]);
        alfont_textprintf(bmp_screen, bene_font, 16, 16, makecol(255, 255, 255), "%s", sentence[iMentatSentence]);
        alfont_textprintf(bmp_screen, bene_font, 17, 33, makecol(0, 0, 0), "%s", sentence[iMentatSentence + 1]);
        alfont_textprintf(bmp_screen, bene_font, 16, 32, makecol(255, 255, 255), "%s", sentence[iMentatSentence + 1]);
    }

    bool bFadeOut=false;

    if (TIMER_Speaking < 0) {
        // NO
        draw_sprite(bmp_screen, (BITMAP *) gfxmentat[BTN_REPEAT].dat, 293, 423);

        if ((mouse_x > 293 && mouse_x < 446) && (mouse_y > 423 && mouse_y < 468))
            if (cMouse::isLeftButtonClicked()) {
                // head back to choose house
                iMentatSentence = -1; // prepare speaking
                //state = GAME_SELECT_HOUSE;
            }

        // YES/PROCEED
        draw_sprite(bmp_screen, (BITMAP *) gfxmentat[BTN_PROCEED].dat, 466, 423);
        if ((mouse_x > 446 && mouse_x < 619) && (mouse_y > 423 && mouse_y < 468))
            if (cMouse::isLeftButtonClicked()) {
                if (game.isState(GAME_BRIEFING)) {
                    // proceed, play mission
                    game.setState(GAME_PLAYING);
                    drawManager->getMessageDrawer()->initCombatPosition();

                    // CENTER MOUSE
                    cMouse::positionMouseCursor(game.screen_x / 2, game.screen_y / 2);

                    bFadeOut = true;

                    playMusicByType(MUSIC_PEACE);
                } else if (game.isState(GAME_WINBRIEF)) {
                    //
                    if (game.bSkirmish) {
                        game.setState(GAME_SETUPSKIRMISH);
                        game.init_skirmish();
                        playMusicByType(MUSIC_MENU);
                    } else {

                        game.setState(GAME_REGION);
                        REGION_SETUP(game.iMission, game.iHouse);

                        drawManager->getMessageDrawer()->initRegionPosition();

                        // PLAY THE MUSIC
                        playMusicByType(MUSIC_CONQUEST);
                    }

                    // PREPARE NEW MENTAT BABBLE
                    iMentatSentence = -1;

                    bFadeOut = true;
                } else if (game.isState(GAME_LOSEBRIEF)) {
                    //
                    if (game.bSkirmish) {
                        game.setState(GAME_SETUPSKIRMISH);
                        game.init_skirmish();
                        playMusicByType(MUSIC_MENU);
                    } else {
                        if (game.iMission > 1) {
                            game.setState(GAME_REGION);

                            game.iMission--; // we did not win
                            REGION_SETUP(game.iMission, game.iHouse);
                            drawManager->getMessageDrawer()->initRegionPosition();

                            // PLAY THE MUSIC
                            playMusicByType(MUSIC_CONQUEST);
                        } else {
                            game.setState(GAME_BRIEFING);
                            playMusicByType(MUSIC_BRIEFING);
                        }

                    }

                    // PREPARE NEW MENTAT BABBLE
                    iMentatSentence = -1;

                    bFadeOut = true;
                }

            }
    }

    if (bFadeOut) {
        game.FADE_OUT();
    }
}

BITMAP *cAbstractMentat::getBackgroundBitmap() const {
    if (iBackgroundFrame < 0) return nullptr;
    return (BITMAP *) gfxmentat[iBackgroundFrame].dat;
}

void cAbstractMentat::draw_movie() {
    if (gfxmovie == nullptr) return;
    if (iMovieFrame < 0) return;
    
    // drawing only, circulating is done in think function
    int movieTopleftX = 256;
    int movieTopleftY = 120;

    draw_sprite(bmp_screen, (BITMAP *)gfxmovie[iMovieFrame].dat, movieTopleftX, movieTopleftY);
}

void cAbstractMentat::interact() {
    if (cMouse::isLeftButtonClicked()) {
        if (TIMER_Speaking > 0) {
            TIMER_Speaking = 1;
        }
    }
}

void cAbstractMentat::playMovie(DATAFILE *movie) {
    gfxmovie = movie;
    TIMER_movie = 0;
    iMovieFrame = 0;
}

void cAbstractMentat::initSentences() {
    memset(sentence, 0, sizeof(sentence));
}

void cAbstractMentat::setSentence(int i, char *text) {
    sprintf(sentence[i], "%s", text);
}

void cAbstractMentat::loadScene(std::string scene) {
    gfxmovie = nullptr;

    char filename[255];
    sprintf(filename, "data/scenes/%s.dat", scene.c_str());

    gfxmovie = load_datafile(filename);

    if (gfxmovie != nullptr) {
        iMovieFrame=0;
        char msg[255];
        sprintf(msg, "Successful loaded scene [%s]", filename);
        logbook(msg);
    } else {
        gfxmovie=nullptr;
        iMovieFrame=-1;
        char msg[255];
        sprintf(msg, "Failed to load scene [%s]", filename);
        logbook(msg);
    }
}
