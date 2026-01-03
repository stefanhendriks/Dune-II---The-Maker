#include "gamestates/cNewMapEditorState.h"

#include "d2tmc.h"
#include "config.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "gui/GuiLabel.hpp"
#include "gui/GuiCheckBox.hpp"
#include "gui/GuiSlider.hpp"
#include "context/GameContext.hpp"
#include "utils/cSoundPlayer.h"



cNewMapEditorState::cNewMapEditorState(cGame &theGame, GameContext *ctx)
    : cGameState(theGame, ctx),
    m_textDrawer(ctx->getTextContext()->getBeneTextDrawer()),
    m_guiWindow(nullptr)
{
    constructWindow();
}

void cNewMapEditorState::constructWindow()
{
    int margin = m_game.m_screenH * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = m_game.m_screenW - (margin * 2);
    int mainMenuHeight = m_game.m_screenH - (margin * 2);

    margin = 4;
    int buttonHeight = (m_textDrawer->getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    m_guiWindow = new GuiWindow(window, m_textDrawer);
    m_guiWindow->setTheme(GuiTheme::Light());

    // Title
    m_guiWindow->setTitle("New map editor configuration");

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = m_guiWindow->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    GuiButton *gui_btn_toMenu = GuiButtonBuilder()
            .withRect(toMainMenuRect)        
            .withLabel("Back to main menu")
            .withTextDrawer(m_textDrawer)
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                m_game.setNextStateToTransitionTo(GAME_MENU);
                m_game.initiateFadingOut();})
            .build();
    m_guiWindow->addGuiObject(gui_btn_toMenu);

    // // QUIT game
    // int quit = mainMenuHeight - (buttonHeight + margin);// 464
    // int width = (buttonWidth / 2);
    // const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    // GuiButton *gui_btn_Quit = GuiButtonBuilder()
    //         .withRect(quitRect)        
    //         .withLabel("Quit game")
    //         .withTextDrawer(m_textDrawer)
    //         .withTheme(GuiTheme::Light())
    //         .onClick([this]() {
    //             m_game.m_playing = false;
    //             m_game.initiateFadingOut();})
    //         .build();
    // m_guiWindow->addGuiObject(gui_btn_Quit);


}

cNewMapEditorState::~cNewMapEditorState()
{
    delete m_guiWindow;
}

void cNewMapEditorState::thinkFast()
{

}

void cNewMapEditorState::draw() const
{
    m_guiWindow->draw();

    // MOUSE
    m_game.getMouse()->draw();
}

void cNewMapEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_guiWindow->onNotifyMouseEvent(event);
}

eGameStateType cNewMapEditorState::getType()
{
    return GAMESTATE_NEW_MAP_EDITOR;
}

void cNewMapEditorState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}
