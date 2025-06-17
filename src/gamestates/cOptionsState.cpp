#include "cOptionsState.h"

#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "gui/actions/cGuiActionExitGame.h"
#include "gui/actions/cGuiActionToGameState.h"
#include "gui/cGuiButton.h"
#include "gui/cGuiWindow.h"

#include <allegro/gfx.h>

cOptionsState::cOptionsState(cGame &theGame, BITMAP *background, int prevState)
  : cGameState(theGame)
  , m_background(background)
  , m_textDrawer(cTextDrawer(bene_font))
  , m_prevState(prevState)
  , m_guiWindow(nullptr) {
    refresh();
}

void cOptionsState::constructWindow(int prevState) {
    int margin = game.m_screenY * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = game.m_screenX - (margin * 2);
    int mainMenuHeight = game.m_screenY - (margin * 2);

    margin = 4;
    int buttonHeight = (m_textDrawer.getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    m_guiWindow = new cGuiWindow(window);

    const eGuiButtonRenderKind buttonKinds = OPAQUE_WITH_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = CENTER;

    // Title
    m_guiWindow->setTitle("Dune II - The Maker - version " + game.m_version);

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = m_guiWindow->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_toMenu = new cGuiButton(m_textDrawer, toMainMenuRect, "Back to main menu", buttonKinds);
    gui_btn_toMenu->setTextAlignHorizontal(buttonTextAlignment);
    cGuiActionToGameState *action2 = new cGuiActionToGameState(GAME_MENU, true);
    gui_btn_toMenu->setOnLeftMouseButtonClickedAction(action2);
    m_guiWindow->addGuiObject(gui_btn_toMenu);

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    cGuiButton *gui_btn_Quit = new cGuiButton(m_textDrawer, quitRect, "Quit game", buttonKinds);
    gui_btn_Quit->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Quit->setOnLeftMouseButtonClickedAction(new cGuiActionExitGame());
    m_guiWindow->addGuiObject(gui_btn_Quit);

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    cGuiButton *gui_btn_Back = new cGuiButton(m_textDrawer, backRect, "Back", buttonKinds);
    gui_btn_Back->setTextAlignHorizontal(buttonTextAlignment);
    cGuiActionToGameState *action = new cGuiActionToGameState(prevState, false);
    gui_btn_Back->setOnLeftMouseButtonClickedAction(action);
    m_guiWindow->addGuiObject(gui_btn_Back);

    // Mission select from options menu, only when playing the game
    if (m_prevState == GAME_PLAYING) {
        rows = 3;
        int toMissionSelect = mainMenuHeight - ((buttonHeight * rows) + (margin * rows));// 424
        const cRectangle &toMissionSelectRect = m_guiWindow->getRelativeRect(margin, toMissionSelect, buttonWidth,
                                                                             buttonHeight);
        cGuiButton *gui_btn_toMissionSelect = new cGuiButton(m_textDrawer, toMissionSelectRect, "Mission select",
                                                             buttonKinds);
        gui_btn_toMissionSelect->setTextAlignHorizontal(buttonTextAlignment);
        cGuiActionToGameState *action3 = new cGuiActionToGameState(GAME_MISSIONSELECT, false);
        gui_btn_toMissionSelect->setOnLeftMouseButtonClickedAction(action3);
        m_guiWindow->addGuiObject(gui_btn_toMissionSelect);
    }
}

cOptionsState::~cOptionsState() {
    delete m_guiWindow;
    destroy_bitmap(m_background);
}

void cOptionsState::thinkFast() {

}

void cOptionsState::draw() const {
    renderDrawer->drawSprite(bmp_screen, m_background, 0, 0);

    m_guiWindow->draw();

    // MOUSE
    game.getMouse()->draw();
}

void cOptionsState::onNotifyMouseEvent(const s_MouseEvent &event) {
    m_guiWindow->onNotifyMouseEvent(event);
}

eGameStateType cOptionsState::getType() {
    return GAMESTATE_OPTIONS;
}

void cOptionsState::onNotifyKeyboardEvent(const cKeyboardEvent &) {
}

/**
 * Reconstructs window so that it knows to which previous state it should go 'back to'
 * @param prevState
 */
void cOptionsState::setPrevState(int prevState) {
    m_prevState = prevState;
    refresh();
}

void cOptionsState::refresh() {
    delete m_guiWindow;
    m_guiWindow = nullptr;
    constructWindow(m_prevState);
}