#include "d2tmh.h"

cMainMenuGameState::cMainMenuGameState(cGame &theGame) : cGameState(theGame) {
    textDrawer = cTextDrawer(bene_font);

    bmp_D2TM_Title = (BITMAP *) gfxinter[BMP_D2TM].dat;

    int logoWidth = bmp_D2TM_Title->w;
    int logoHeight = bmp_D2TM_Title->h;

    logoX = (game.screen_x / 2) - (logoWidth / 2);
    logoY = (logoHeight/10);

    mainMenuWidth = 130;
    mainMenuHeight = 143;

    // adjust x and y according to resolution, we can add because the above values
    // assume 640x480 resolution, and logoX/logoY are already taking care of > resolutions
    mainMenuFrameX = 257 + logoX;
    mainMenuFrameY = 319 + logoY;

}

cMainMenuGameState::~cMainMenuGameState() {

}

void cMainMenuGameState::thinkFast() {

}

void cMainMenuGameState::draw() {
    bool bFadeOut=false;

    if (DEBUGGING) {
        for (int x = 0; x < game.screen_x; x += 60) {
            for (int y = 0; y < game.screen_y; y += 20) {
                rect(bmp_screen, x, y, x + 50, y + 10, makecol(64, 64, 64));
                putpixel(bmp_screen, x, y, makecol(255, 255, 255));
                alfont_textprintf(bmp_screen, bene_font, x, y, makecol(32, 32, 32), "Debug");
            }
        }
    }

    draw_sprite(bmp_screen, bmp_D2TM_Title, logoX, logoY);

    GUI_DRAW_FRAME(mainMenuFrameX, mainMenuFrameY, mainMenuWidth,mainMenuHeight);

    // Buttons:
    int buttonsX = mainMenuFrameX + 4;

    // PLAY
    int playY = 323 + logoY;
    if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, playY, "Campaign", makecol(255, 0, 0)))
    {
        if (game.getMouse()->isLeftButtonClicked()) {
            game.setState(GAME_SELECT_HOUSE); // select house
            bFadeOut = true;
        }
    }

    // SKIRMISH
    int skirmishY = 344 + logoY;
    if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, skirmishY, "Skirmish", makecol(255, 0, 0)))
    {
        if (game.getMouse()->isLeftButtonClicked()) {
            game.setState(GAME_SETUPSKIRMISH);
            bFadeOut = true;
            INI_PRESCAN_SKIRMISH();

            game.init_skirmish();
        }
    }

    // MULTIPLAYER
    int multiplayerY = 364 + logoY;
    if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, multiplayerY, "Multiplayer", makecol(128, 128, 128)))
    {
        if (game.getMouse()->isLeftButtonClicked())
        {
            // NOT YET IMPLEMENTED
            bFadeOut = true;
        }
    }

    // LOAD
    int loadY = 384 + logoY;
    if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, loadY, "Load", makecol(128, 128, 128)))
    {
        if (game.getMouse()->isLeftButtonClicked())
        {
            // NOT YET IMPLEMENTED
            bFadeOut = true;
        }
    }

    // OPTIONS
    int optionsY = 404 + logoY;
    if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, optionsY, "Options", makecol(128, 128, 128)))
    {
        if (game.getMouse()->isLeftButtonClicked())
        {
            // NOT YET IMPLEMENTED
            bFadeOut = true;
        }
    }

    // HALL OF FAME
    int hofY = 424 + logoY;
    if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, hofY, "Hall of Fame", makecol(128, 128, 128)))
    {
        if (game.getMouse()->isLeftButtonClicked())
        {
            // NOT YET IMPLEMENTED
            bFadeOut = true;
        }
    }

    // EXIT
    int exitY = 444 + logoY;
    if (GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(buttonsX, exitY, "Exit", makecol(255, 0, 0)))
    {
        if (game.getMouse()->isLeftButtonClicked())
        {
            bFadeOut = true;
            game.bPlaying = false;
        }
    }

    int creditsX = (game.screen_x / 2) - (alfont_text_length(bene_font, "CREDITS") / 2);
    GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(creditsX, 1, "CREDITS", makecol(64, 64, 64));


    // draw version
    textDrawer.drawTextBottomRight(game.version);

    // mp3 addon?
    if (game.bMp3) {
        textDrawer.drawTextBottomLeft("Music: MP3 ADD-ON");
    } else {
        textDrawer.drawTextBottomLeft("Music: MIDI");
    }

    if (DEBUGGING) {
        char mouseTxt[255];
        sprintf(mouseTxt, "%d, %d", mouse_x, mouse_y);
        textDrawer.drawText(0, 0, mouseTxt);
    }

    // MOUSE
    game.getMouse()->draw();

    if (key[KEY_ESC]) {
        game.bPlaying=false;
    }

    if (bFadeOut) {
        game.START_FADING_OUT();
    }
}

void cMainMenuGameState::onNotifyMouseEvent(const s_MouseEvent &event) {

}

eGameStateType cMainMenuGameState::getType() {
    return GAMESTATE_MAIN_MENU;
}
