#include "d2tmh.h"

cMainMenuGameState::cMainMenuGameState(cGame &theGame) : cGameState(theGame), textDrawer(cTextDrawer(bene_font)) {
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

    // Buttons:
    int buttonsX = mainMenuFrameX + 4;

    // PLAY
    int playY = 323 + logoY;

    int buttonHeight = textDrawer.getFontHeight();
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &campaign = cRectangle(buttonsX, playY, buttonWidth, buttonHeight);
    gui_btn_SelectHouse = new cGuiButton(textDrawer, campaign, "Campaign", eGuiButtonRenderKind::OPAQUE_WITH_BORDER);
    gui_btn_SelectHouse->setTextAlignHorizontal(eGuiTextAlignHorizontal::LEFT);

    gui_btn_SelectHouse->setGui_ColorButton(makecol(128, 0, 128));

    int skirmishY = 344 + logoY;
    const cRectangle &skirmish = cRectangle(buttonsX, skirmishY, buttonWidth, buttonHeight);
    gui_btn_Skirmish = new cGuiButton(textDrawer, skirmish, "Skirmish", eGuiButtonRenderKind::OPAQUE_WITH_BORDER);

    int multiplayerY = 364 + logoY;
    const cRectangle &multiplayer = cRectangle(buttonsX, multiplayerY, buttonWidth, buttonHeight);
    gui_btn_Multiplayer = new cGuiButton(textDrawer, multiplayer, "Multiplayer", eGuiButtonRenderKind::OPAQUE_WITH_BORDER);
    gui_btn_Multiplayer->setTextColor(makecol(225, 225, 225));
    gui_btn_Multiplayer->setTextColorHover(makecol(128, 128, 128));

    // LOAD
    int loadY = 384 + logoY;
    const cRectangle &load = cRectangle(buttonsX, loadY, buttonWidth, buttonHeight);
    gui_btn_Load = new cGuiButton(textDrawer, load, "Load", eGuiButtonRenderKind::OPAQUE_WITH_BORDER);
    gui_btn_Load->setTextColor(makecol(225, 225, 225));
    gui_btn_Load->setTextColorHover(makecol(128, 128, 128));

    // OPTIONS
    int optionsY = 404 + logoY;
    const cRectangle &options = cRectangle(buttonsX, optionsY, buttonWidth, buttonHeight);
    gui_btn_Options = new cGuiButton(textDrawer, options, "Options", eGuiButtonRenderKind::OPAQUE_WITH_BORDER);
    gui_btn_Options->setTextColor(makecol(225, 225, 225));
    gui_btn_Options->setTextColorHover(makecol(128, 128, 128));

    // HALL OF FAME
    int hofY = 424 + logoY;
    const cRectangle &hof = cRectangle(buttonsX, hofY, buttonWidth, buttonHeight);
    gui_btn_Hof = new cGuiButton(textDrawer, hof, "Hall of Fame", eGuiButtonRenderKind::OPAQUE_WITH_BORDER);
    gui_btn_Hof->setTextColor(makecol(225, 225, 225));
    gui_btn_Hof->setTextColorHover(makecol(128, 128, 128));

    // EXIT
    int exitY = 444 + logoY;
    const cRectangle &exit = cRectangle(buttonsX, exitY, buttonWidth, buttonHeight);
    gui_btn_Exit = new cGuiButton(textDrawer, exit, "Exit", eGuiButtonRenderKind::OPAQUE_WITH_BORDER);
}

cMainMenuGameState::~cMainMenuGameState() {
    delete gui_btn_SelectHouse;
    delete gui_btn_Skirmish;
    delete gui_btn_Multiplayer;
    delete gui_btn_Load;
    delete gui_btn_Options;
    delete gui_btn_Hof;
    delete gui_btn_Exit;
}

void cMainMenuGameState::thinkFast() {

}

void cMainMenuGameState::draw() const {
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

    // Menu buttons
    gui_btn_SelectHouse->draw();
    gui_btn_Skirmish->draw();
    gui_btn_Multiplayer->draw();
    gui_btn_Load->draw();
    gui_btn_Options->draw();
    gui_btn_Hof->draw();
    gui_btn_Exit->draw();

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
}

void cMainMenuGameState::onNotifyMouseEvent(const s_MouseEvent &event) {
    gui_btn_SelectHouse->onNotifyMouseEvent(event);
    gui_btn_Skirmish->onNotifyMouseEvent(event);
    gui_btn_Multiplayer->onNotifyMouseEvent(event);
    gui_btn_Load->onNotifyMouseEvent(event);
    gui_btn_Options->onNotifyMouseEvent(event);
    gui_btn_Hof->onNotifyMouseEvent(event);
    gui_btn_Exit->onNotifyMouseEvent(event);

    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
    }
}

eGameStateType cMainMenuGameState::getType() {
    return GAMESTATE_MAIN_MENU;
}

void cMainMenuGameState::onMouseLeftButtonClicked(const s_MouseEvent &event) {
    if (gui_btn_SelectHouse->hasFocus()) {
        game.setState(GAME_SELECT_HOUSE);
        game.START_FADING_OUT();
        return;
    }

    if (gui_btn_Skirmish->hasFocus()) {
        game.START_FADING_OUT();
        INI_PRESCAN_SKIRMISH();
        game.init_skirmish();
        game.setState(GAME_SETUPSKIRMISH);
        return;
    }

    if (gui_btn_Multiplayer->hasFocus()) {
        game.START_FADING_OUT();
        return;
    }

    if (gui_btn_Load->hasFocus()) {
        game.START_FADING_OUT();
        return;
    }

    if (gui_btn_Hof->hasFocus()) {
        game.START_FADING_OUT();
        return;
    }

    if (gui_btn_Options->hasFocus()) {
        game.START_FADING_OUT();
        return;
    }

    if (gui_btn_Exit->hasFocus()) {
        game.bPlaying = false;
        return;
    }
}
