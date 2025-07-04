#include "cOptionsState.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
// #include "gui/actions/cGuiActionExitGame.h"
// #include "gui/actions/cGuiActionToGameState.h"
#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"



cOptionsState::cOptionsState(cGame &theGame, SDL_Surface *background, int prevState)
    : cGameState(theGame)
    , m_textDrawer(cTextDrawer(bene_font))
    , m_prevState(prevState)
    , m_guiWindow(nullptr)
{
    refresh();
}

void cOptionsState::constructWindow(int prevState)
{
    int margin = game.m_screenH * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = game.m_screenW - (margin * 2);
    int mainMenuHeight = game.m_screenH - (margin * 2);

    margin = 4;
    int buttonHeight = (m_textDrawer.getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    m_guiWindow = new GuiWindow(window);
    m_guiWindow->setTheme(GuiTheme::Light());

    const eGuiButtonRenderKind buttonKinds = OPAQUE_WITH_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = CENTER;

    // Title
    m_guiWindow->setTitle("Dune II - The Maker - version " + game.m_version);

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = m_guiWindow->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    GuiButton *gui_btn_toMenu = new GuiButton(m_textDrawer, toMainMenuRect, "Back to main menu", buttonKinds);
    gui_btn_toMenu->setTheme(GuiTheme::Light());
    gui_btn_toMenu->setTextAlignHorizontal(buttonTextAlignment);
    // cGuiActionToGameState *action2 = new cGuiActionToGameState(GAME_MENU, true);
    gui_btn_toMenu->setOnLeftMouseButtonClickedAction([this]() 
        {game.setNextStateToTransitionTo(GAME_MENU);
        game.initiateFadingOut();});
    m_guiWindow->addGuiObject(gui_btn_toMenu);

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    GuiButton *gui_btn_Quit = new GuiButton(m_textDrawer, quitRect, "Quit game", buttonKinds);
    gui_btn_Quit->setTheme(GuiTheme::Light());
    gui_btn_Quit->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Quit->setOnLeftMouseButtonClickedAction([this]() {
        game.m_playing = false;
        game.initiateFadingOut();});
    m_guiWindow->addGuiObject(gui_btn_Quit);

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    GuiButton *gui_btn_Back = new GuiButton(m_textDrawer, backRect, "Back", buttonKinds);
    gui_btn_Back->setTheme(GuiTheme::Light());
    gui_btn_Back->setTextAlignHorizontal(buttonTextAlignment);
    // cGuiActionToGameState *action = new cGuiActionToGameState(prevState, false);
    gui_btn_Back->setOnLeftMouseButtonClickedAction([this]() 
        {game.setNextStateToTransitionTo(GAME_MENU);});
    m_guiWindow->addGuiObject(gui_btn_Back);

    // Mission select from options menu, only when playing the game
    if (m_prevState == GAME_PLAYING) {
        rows = 3;
        int toMissionSelect = mainMenuHeight - ((buttonHeight * rows) + (margin * rows));// 424
        const cRectangle &toMissionSelectRect = m_guiWindow->getRelativeRect(margin, toMissionSelect, buttonWidth,
                                                buttonHeight);
        GuiButton *gui_btn_toMissionSelect = new GuiButton(m_textDrawer, toMissionSelectRect, "Mission select",
                buttonKinds);
        gui_btn_toMissionSelect->setTheme(GuiTheme::Light());
        gui_btn_toMissionSelect->setTextAlignHorizontal(buttonTextAlignment);
        // cGuiActionToGameState *action3 = new cGuiActionToGameState(GAME_MISSIONSELECT, false);
        gui_btn_toMissionSelect->setOnLeftMouseButtonClickedAction([this]() 
            {game.setNextStateToTransitionTo(GAME_MISSIONSELECT);});
        m_guiWindow->addGuiObject(gui_btn_toMissionSelect);
    }
}

cOptionsState::~cOptionsState()
{
    delete m_guiWindow;
}

void cOptionsState::thinkFast()
{

}

void cOptionsState::draw() const
{
    //@Mira draw here screenTexture
    m_guiWindow->draw();

    // MOUSE
    game.getMouse()->draw();
}

void cOptionsState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_guiWindow->onNotifyMouseEvent(event);
}

eGameStateType cOptionsState::getType()
{
    return GAMESTATE_OPTIONS;
}

void cOptionsState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}

/**
 * Reconstructs window so that it knows to which previous state it should go 'back to'
 * @param prevState
 */
void cOptionsState::setPrevState(int prevState)
{
    m_prevState = prevState;
    refresh();
}

void cOptionsState::refresh()
{
    delete m_guiWindow;
    m_guiWindow = nullptr;
    constructWindow(m_prevState);
}