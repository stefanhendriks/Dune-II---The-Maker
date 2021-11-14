#include "d2tmh.h"

cOptionsState::cOptionsState(cGame &theGame, BITMAP *background, int prevState) : cGameState(theGame),
                                                                                  textDrawer(cTextDrawer(bene_font)),
                                                                                  prevState(prevState),
                                                                                  background(background) {
    int centerOfScreen = game.screen_x / 2;

    int margin = game.screen_y * 0.2;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = game.screen_x - (margin * 2);
    int mainMenuHeight = game.screen_y - (margin * 2);

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

    const eGuiButtonRenderKind buttonKinds = eGuiButtonRenderKind::OPAQUE_WITH_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = eGuiTextAlignHorizontal::CENTER;

    const cRectangle &campaign = cRectangle(buttonsX, playY, buttonWidth, buttonHeight);

    // EXIT
    int back = 444;
    const cRectangle &exit = cRectangle(buttonsX, back, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Exit = new cGuiButton(textDrawer, exit, "Back", buttonKinds);
    gui_btn_Exit->setTextAlignHorizontal(buttonTextAlignment);
    cGuiActionToGameState *action = new cGuiActionToGameState(prevState, false);
    gui_btn_Exit->setOnLeftMouseButtonClickedAction(action);
    gui_window->addGuiObject(gui_btn_Exit);
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

    // TODO: on keyboard event!?
    if (key[KEY_ESC]) {
        game.setNextStateToTransitionTo(prevState);
    }
}

void cOptionsState::onNotifyMouseEvent(const s_MouseEvent &event) {
    gui_window->onNotifyMouseEvent(event);
}

eGameStateType cOptionsState::getType() {
    return GAMESTATE_OPTIONS;
}
