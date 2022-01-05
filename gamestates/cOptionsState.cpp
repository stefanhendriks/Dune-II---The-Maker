#include "d2tmh.h"
#include "cOptionsState.h"


cOptionsState::cOptionsState(cGame &theGame, BITMAP *background, int prevState)
  : cGameState(theGame)
  ,  background(background)
  ,  prevState(prevState)
  ,  textDrawer(cTextDrawer(bene_font)) {
    int margin = game.screen_y * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = game.screen_x - (margin * 2);
    int mainMenuHeight = game.screen_y - (margin * 2);

    margin = 4;
    int buttonHeight = (textDrawer.getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = new cGuiWindow(window);

    const eGuiButtonRenderKind buttonKinds = eGuiButtonRenderKind::OPAQUE_WITH_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = eGuiTextAlignHorizontal::CENTER;

    // Title
    gui_window->setTitle("Dune II - The Maker - version " + game.version);

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = gui_window->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_toMenu = new cGuiButton(textDrawer, toMainMenuRect, "Back to main menu", buttonKinds);
    gui_btn_toMenu->setTextAlignHorizontal(buttonTextAlignment);
    cGuiActionToGameState *action2 = new cGuiActionToGameState(GAME_MENU, true);
    gui_btn_toMenu->setOnLeftMouseButtonClickedAction(action2);
    gui_window->addGuiObject(gui_btn_toMenu);

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = gui_window->getRelativeRect(margin, quit, width, buttonHeight);
    cGuiButton *gui_btn_Quit = new cGuiButton(textDrawer, quitRect, "Quit game", buttonKinds);
    gui_btn_Quit->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Quit->setOnLeftMouseButtonClickedAction(new cGuiActionExitGame());
    gui_window->addGuiObject(gui_btn_Quit);

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = gui_window->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    cGuiButton *gui_btn_Back = new cGuiButton(textDrawer, backRect, "Back", buttonKinds);
    gui_btn_Back->setTextAlignHorizontal(buttonTextAlignment);
    cGuiActionToGameState *action = new cGuiActionToGameState(prevState, false);
    gui_btn_Back->setOnLeftMouseButtonClickedAction(action);
    gui_window->addGuiObject(gui_btn_Back);
}

cOptionsState::~cOptionsState() {
    delete gui_window;
    destroy_bitmap(background);
}

void cOptionsState::thinkFast() {

}

void cOptionsState::draw() const {
    allegroDrawer->drawSprite(bmp_screen, background, 0, 0);

    gui_window->draw();

    // MOUSE
    game.getMouse()->draw();
}

void cOptionsState::onNotifyMouseEvent(const s_MouseEvent &event) {
    gui_window->onNotifyMouseEvent(event);
}

eGameStateType cOptionsState::getType() {
    return GAMESTATE_OPTIONS;
}

void cOptionsState::onNotifyKeyboardEvent(const s_KeyboardEvent &) {
}
