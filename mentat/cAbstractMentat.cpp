/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"

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

    house = GENERALHOUSE;

    gfxmovie = nullptr;
    leftButton = nullptr;
    rightButton = nullptr;
    // the mentat does not *own* the bitmaps
    leftButtonBmp = nullptr;
    rightButtonBmp = nullptr;
    leftButtonCommand = nullptr;
    rightButtonCommand = nullptr;

    state = INIT;

    font = alfont_load_font("data/arrak.ttf");

    // offsetX = 0 for screen resolution 640x480, ie, meaning > 640 we take the difference / 2
    offsetX = (game.screen_x - 640) / 2;
    offsetY = (game.screen_y - 480) / 2; // same goes for offsetY (but then for 480 height).

	memset(sentence, 0, sizeof(sentence));
	logbook("cAbstractMentat::cAbstractMentat()");
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

    state = INIT;

    iBackgroundFrame = -1;

    // we can do this because Mentats are created/deleted before allegro gets destroyed
    if (gfxmovie) {
        unload_datafile(gfxmovie);
    }

    gfxmovie = nullptr;
    delete leftButton;
    delete rightButton;
    leftButtonBmp = nullptr;
    rightButtonBmp = nullptr;

    delete leftButtonCommand;
    delete rightButtonCommand;

    alfont_destroy_font(font);

	memset(sentence, 0, sizeof(sentence));

    logbook("cAbstractMentat::~cAbstractMentat()");
}

void cAbstractMentat::think() {
    if (state == INIT) {
        // no thinking for init state
        return;
    }

    if (state == SPEAKING) {
        if (TIMER_Speaking > 0) {
            TIMER_Speaking--;
        } else if (TIMER_Speaking == 0) {
            // calculate speaking stuff

            iMentatSentence += 2; // makes 0, 2, etc.

            // done talking
            if (iMentatSentence > 8) {
                state = AWAITING_RESPONSE;
                return;
            }

            // lentgh calculation of time
            int iLength = strlen(sentence[iMentatSentence]);
            iLength += strlen(sentence[iMentatSentence + 1]);

            // nothing to say, await response
            if (iLength < 2) {
                state = AWAITING_RESPONSE;
                return;
            }

            TIMER_Speaking = iLength * 12;
        }

        thinkMouth();
    } else {
        iMentatMouth = 0; // keep lips sealed
    }

    // eyes blink always
    thinkEyes();

    // regardless, think about movie animation
    thinkMovie();
}

void cAbstractMentat::thinkMovie() {
    if (gfxmovie != nullptr) {
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

void cAbstractMentat::thinkEyes() {
    if (TIMER_Eyes > 0) {
        TIMER_Eyes--;
    } else {
        int i = rnd(100);

        int iWas = iMentatEyes;

        if (i < 30) {
            iMentatEyes = 3;
        } else if (i < 60) {
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
}

void cAbstractMentat::thinkMouth() {// MOUTH
    if (TIMER_Mouth > 0) {
        TIMER_Mouth--;
    } else {
        // still speaking
        if (TIMER_Speaking > 0) {

            int iOld = iMentatMouth;

            if (iMentatMouth == 0) { // mouth is shut
                // when mouth is shut, we wait a bit.
                if (rnd(100) < 45) {
                    iMentatMouth += (1 + rnd(4));
                } else {
                    TIMER_Mouth = 25; // wait
                }
            } else {
                iMentatMouth += (1 + rnd(4));
            }

            // correct any frame
            if (iMentatMouth > 4) {
                iMentatMouth -= 5;
            }

            // Test if we did not set the timer, when not, we changed stuff, and we
            // have to make sure we do not reshow the same animation.. which looks
            // odd!
            if (TIMER_Mouth <= 0) {
                // did not change mouth, force it
                if (iMentatMouth == iOld) {
                    iMentatMouth++;
                }

                // correct if needed:
                if (iMentatMouth > 4) {
                    iMentatMouth -= 5;
                }
            }
        } else {
            iMentatMouth = 0; // when there is no sentence, do not animate mouth
        }

        if (TIMER_Mouth <= 0) {
            TIMER_Mouth = 13 + rnd(15);
        }
    } // Animating mouth

}

void cAbstractMentat::draw() {
    rectfill(bmp_screen, offsetX, offsetY, offsetX + 640, offsetY + 480, makecol(0,0,0));
    rect(bmp_screen, offsetX-1, offsetY-1, offsetX + 641, offsetY + 481, makecol(64, 64,89));
    rect(bmp_screen, offsetX-2, offsetY-2, offsetX + 642, offsetY + 482, makecol(40,40,60));
    rect(bmp_screen, offsetX-3, offsetY-3, offsetX + 643, offsetY + 483, makecol(0,0,0));
    select_palette(general_palette);

    // movie
    draw_movie();

    draw_sprite(bmp_screen, getBackgroundBitmap(), offsetX, offsetY);

    draw_eyes();
    draw_mouth();
    draw_other();

    if (state == SPEAKING) {
        // draw text that is being spoken
        if (iMentatSentence >= 0) {
            alfont_textprintf(bmp_screen, bene_font, offsetX + 17, offsetY + 17, makecol(0, 0, 0), "%s", sentence[iMentatSentence]);
            alfont_textprintf(bmp_screen, bene_font, offsetX + 16, offsetY + 16, makecol(255, 255, 255), "%s", sentence[iMentatSentence]);

            alfont_textprintf(bmp_screen, bene_font, offsetX + 17, offsetY + 37, makecol(0, 0, 0), "%s", sentence[iMentatSentence + 1]);
            alfont_textprintf(bmp_screen, bene_font, offsetX + 16, offsetY + 36, makecol(255, 255, 255), "%s",
                              sentence[iMentatSentence + 1]);
        }
    }

    if (state == AWAITING_RESPONSE) {
        allegroDrawer->blitSprite(leftButtonBmp, bmp_screen, leftButton);
        allegroDrawer->blitSprite(rightButtonBmp, bmp_screen, rightButton);
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
    int movieTopleftX = offsetX + 256;
    int movieTopleftY = offsetY + 120;

    draw_sprite(bmp_screen, (BITMAP *)gfxmovie[iMovieFrame].dat, movieTopleftX, movieTopleftY);
}

void cAbstractMentat::interact() {
    if (state == INIT) return;
    if (state == SPEAKING) {
        if (game.getMouse()->isLeftButtonClicked()) {
            if (TIMER_Speaking > 0) {
                TIMER_Speaking = 1;
            }
        }
        return;
    }
    if (state != AWAITING_RESPONSE) return;

    if (game.getMouse()->isLeftButtonClicked()) {
        // execute left button logic
        if (leftButton && leftButton->isMouseOver(mouse_x, mouse_y)) {
            leftButtonCommand->execute(*this);
        }

        // execute right button logic
        if (rightButton && rightButton->isMouseOver(mouse_x, mouse_y)) {
            rightButtonCommand->execute(*this);
        }
    }
}

void cAbstractMentat::initSentences() {
    memset(sentence, 0, sizeof(sentence));
}

void cAbstractMentat::setSentence(int i, char *text) {
    sprintf(sentence[i], "%s", text);
    char msg[512];
    sprintf(msg, "Sentence[%d]=%s", i, text);
    logbook(msg);
}

void cAbstractMentat::loadScene(const std::string& scene) {
    gfxmovie = nullptr;

    char filename[255];
    sprintf(filename, "data/scenes/%s.dat", scene.c_str());

    gfxmovie = load_datafile(filename);

    TIMER_movie = 0;
    iMovieFrame=0;

    if (gfxmovie != nullptr) {
        char msg[255];
        sprintf(msg, "Successful loaded scene [%s]", filename);
        logbook(msg);
        return;
    }

    gfxmovie=nullptr;
    char msg[255];
    sprintf(msg, "Failed to load scene [%s]", filename);
    logbook(msg);
}

void cAbstractMentat::speak() {
    TIMER_Speaking = 0;
    TIMER_Mouth = 0;
    iMentatSentence = -2; // ugh
    state = SPEAKING;
}

void cAbstractMentat::buildLeftButton(BITMAP *bmp, int x, int y) {
    delete leftButton;
    leftButton = new cRectangle(offsetX + x, offsetY + y, bmp->w, bmp->h);
    leftButtonBmp = bmp;
}

void cAbstractMentat::buildRightButton(BITMAP *bmp, int x, int y) {
    delete rightButton;
    rightButton = new cRectangle(offsetX + x, offsetY + y, bmp->w, bmp->h);
    rightButtonBmp = bmp;
}

void cAbstractMentat::resetSpeak() {
    speak();
}
