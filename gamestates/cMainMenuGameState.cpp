#include "d2tmh.h"
#include "cMainMenuGameState.h"


cMainMenuGameState::cMainMenuGameState(cGame &theGame) : cGameState(theGame), textDrawer(cTextDrawer(bene_font)) {
    bmp_D2TM_Title = (BITMAP *) gfxinter[BMP_D2TM].dat;

    int logoWidth = bmp_D2TM_Title->w;
    int logoHeight = bmp_D2TM_Title->h;

    int centerOfScreen = game.screen_x / 2;

    logoX = centerOfScreen - (logoWidth / 2);
    logoY = (logoHeight/10);

    mainMenuWidth = 130;
    mainMenuHeight = 143;

    // adjust x and y according to resolution, we can add because the above values
    // assume 640x480 resolution, and logoX/logoY are already taking care of > resolutions
//    mainMenuFrameX = 257 + logoX;
    mainMenuFrameX = centerOfScreen -(mainMenuWidth/2);
    mainMenuFrameY = 319;

    logoY = mainMenuFrameY - (logoHeight)*1.2f;

    // Buttons:
    int buttonsX = mainMenuFrameX + 2;

    int colorInactiveHover = makecol(128, 128, 128);
    int colorInactive = makecol(192, 192, 192);

    // PLAY
    int playY = 323;

    int buttonHeight = textDrawer.getFontHeight();
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = new cGuiWindow(window);

    const eGuiButtonRenderKind buttonKinds = eGuiButtonRenderKind::TRANSPARENT_WITHOUT_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = eGuiTextAlignHorizontal::CENTER;

    const cRectangle &campaign = cRectangle(buttonsX, playY, buttonWidth, buttonHeight);

    cGuiButton *gui_btn_SelectHouse = new cGuiButton(textDrawer, campaign, "Campaign", buttonKinds);
    gui_btn_SelectHouse->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_SelectHouse->setOnLeftMouseButtonClickedAction(new cGuiActionSelectHouse());
    gui_window->addGuiObject(gui_btn_SelectHouse);

    int skirmishY = 344;
    const cRectangle &skirmish = cRectangle(buttonsX, skirmishY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Skirmish = new cGuiButton(textDrawer, skirmish, "Skirmish", buttonKinds);
    gui_btn_Skirmish->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Skirmish->setOnLeftMouseButtonClickedAction(new cGuiActionSetupSkirmishGame());
    gui_window->addGuiObject(gui_btn_Skirmish);

    int multiplayerY = 364;
    const cRectangle &multiplayer = cRectangle(buttonsX, multiplayerY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Multiplayer = new cGuiButton(textDrawer, multiplayer, "Multiplayer", buttonKinds);
    gui_btn_Multiplayer->setTextColor(colorInactive);
    gui_btn_Multiplayer->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Multiplayer->setTextColorHover(colorInactiveHover);
    gui_btn_Multiplayer->setOnLeftMouseButtonClickedAction(new cGuiActionFadeOutOnly());
    gui_window->addGuiObject(gui_btn_Multiplayer);

    // LOAD
    int loadY = 384;
    const cRectangle &load = cRectangle(buttonsX, loadY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Load = new cGuiButton(textDrawer, load, "Load", buttonKinds);
    gui_btn_Load->setTextColor(colorInactive);
    gui_btn_Load->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Load->setTextColorHover(colorInactiveHover);
    gui_btn_Load->setOnLeftMouseButtonClickedAction(new cGuiActionFadeOutOnly());
    gui_window->addGuiObject(gui_btn_Load);

    // OPTIONS
    int optionsY = 404;
    const cRectangle &options = cRectangle(buttonsX, optionsY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Options = new cGuiButton(textDrawer, options, "Options", buttonKinds);
    gui_btn_Options->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Options->setOnLeftMouseButtonClickedAction(new cGuiActionShowOptions());
    gui_window->addGuiObject(gui_btn_Options);

    // HALL OF FAME
    int hofY = 424;
    const cRectangle &hof = cRectangle(buttonsX, hofY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Hof = new cGuiButton(textDrawer, hof, "Hall of Fame", buttonKinds);
    gui_btn_Hof->setTextColor(colorInactive);
    gui_btn_Hof->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Hof->setTextColorHover(colorInactiveHover);
    gui_btn_Hof->setOnLeftMouseButtonClickedAction(new cGuiActionFadeOutOnly());
    gui_window->addGuiObject(gui_btn_Hof);

    // EXIT
    int exitY = 444;
    const cRectangle &exit = cRectangle(buttonsX, exitY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Exit = new cGuiButton(textDrawer, exit, "Exit", buttonKinds);
    gui_btn_Exit->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Exit->setOnLeftMouseButtonClickedAction(new cGuiActionExitGame());
    gui_window->addGuiObject(gui_btn_Exit);
}

cMainMenuGameState::~cMainMenuGameState() {
    delete gui_window;
}

void cMainMenuGameState::thinkFast() {

}

void cMainMenuGameState::draw() const {
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

    gui_window->draw();

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
    gui_window->onNotifyMouseEvent(event);
}

eGameStateType cMainMenuGameState::getType() {
    return GAMESTATE_MAIN_MENU;
}

void cMainMenuGameState::onNotifyKeyboardEvent(const s_KeyboardEvent &) {
}
