#include "cOptionsState.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "gui/GuiButton.hpp"
#include "gui/GuiWindow.h"



cOptionsState::cOptionsState(cGame &theGame, int prevState)
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
    m_guiWindow = std::make_unique<GuiWindow>(window);
    m_guiWindow->setTheme(GuiTheme::Light());

    // Title
    m_guiWindow->setTitle("Dune II - The Maker - version " + game.m_version);

    // EXIT
    int rows = 2;
    int toMainMenu = mainMenuHeight - ((buttonHeight*rows)+(margin*rows));// 424
    const cRectangle &toMainMenuRect = m_guiWindow->getRelativeRect(margin, toMainMenu, buttonWidth, buttonHeight);
    std::unique_ptr<GuiButton> gui_btn_toMenu = GuiButtonBuilder()
            .withRect(toMainMenuRect)        
            .withLabel("Back to main menu")
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                game.setNextStateToTransitionTo(GAME_MENU);
                game.initiateFadingOut();})
            .build();   
    m_guiWindow->addGuiObject(std::move(gui_btn_toMenu));

    // QUIT game
    int quit = mainMenuHeight - (buttonHeight + margin);// 464
    int width = (buttonWidth / 2);
    const cRectangle &quitRect = m_guiWindow->getRelativeRect(margin, quit, width, buttonHeight);
    std::unique_ptr<GuiButton> gui_btn_Quit = GuiButtonBuilder()
            .withRect(quitRect)        
            .withLabel("Quit game")
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                game.m_playing = false;
                game.initiateFadingOut();})
            .build();   
    m_guiWindow->addGuiObject(std::move(gui_btn_Quit));

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);// 444
    const cRectangle &backRect = m_guiWindow->getRelativeRect(margin + width + margin, back, (width - margin), buttonHeight);
    
    std::unique_ptr<GuiButton> gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)        
            .withLabel("Back")
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this,prevState](){
                game.setNextStateToTransitionTo(prevState);})
            .build();   
    m_guiWindow->addGuiObject(std::move(gui_btn_Back));

    // Mission select from options menu, only when playing the game
    if (m_prevState == GAME_PLAYING) {
        rows = 3;
        int toMissionSelect = mainMenuHeight - ((buttonHeight * rows) + (margin * rows));// 424
        const cRectangle &toMissionSelectRect = m_guiWindow->getRelativeRect(margin, toMissionSelect, buttonWidth,
                                                buttonHeight);
    std::unique_ptr<GuiButton> gui_btn_toMissionSelect = GuiButtonBuilder()
            .withRect(toMissionSelectRect)        
            .withLabel("Mission select")
            .withTextDrawer(&m_textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this]() {
                game.setNextStateToTransitionTo(GAME_MISSIONSELECT);})
            .build();   
        m_guiWindow->addGuiObject(std::move(gui_btn_toMissionSelect));
    }
}

cOptionsState::~cOptionsState()
{
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
    // delete m_guiWindow;
    // m_guiWindow = nullptr;
    m_guiWindow.reset();
    constructWindow(m_prevState);
}