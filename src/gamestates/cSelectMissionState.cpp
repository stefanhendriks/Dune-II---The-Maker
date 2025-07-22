#include "cSelectMissionState.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "gui/GuiButton.hpp"
#include "gui/GuiWindow.hpp"


cSelectMissionState::cSelectMissionState(cGame &theGame, int prevState)
    : cGameState(theGame)
    ,  prevState(prevState)
    ,  textDrawer(cTextDrawer(bene_font))
{
    int margin = game.m_screenH * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = game.m_screenW - (margin * 2);
    int mainMenuHeight = game.m_screenH - (margin * 2);

    margin = 4;
    int buttonHeight = (textDrawer.getFontHeight() + margin);
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = std::make_unique<GuiWindow>(window);
    gui_window->setTheme(GuiTheme::Light());

    // Title
    gui_window->setTitle("Dune II - The Maker - version " + game.m_version);


    // QUIT game
//    int quit = mainMenuHeight - (buttonHeight + margin);// 464
//    int width = (buttonWidth / 2);
    int width = buttonWidth;

    int y = 40;
    for (int i = 2; i <= 9; i++) {
        const cRectangle &rect = gui_window->getRelativeRect(margin, y, width, buttonHeight);
        std::unique_ptr<GuiButton> btnMission = GuiButtonBuilder()
            .withRect(rect)        
            .withLabel(fmt::format("Mission {}", i))
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this,i]() {
                game.jumpToSelectYourNextConquestMission(i);
                game.setNextStateToTransitionTo(GAME_REGION);
                game.initiateFadingOut();})
            .build();   
        gui_window->addGuiObject(std::move(btnMission));

        y += buttonHeight + margin;
    }

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);
    width = buttonWidth;
    const cRectangle &backRect = gui_window->getRelativeRect(margin, back, (width - margin), buttonHeight);
    std::unique_ptr<GuiButton> gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)        
            .withLabel("Back")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this,prevState]() {
                game.setNextStateToTransitionTo(prevState);})
            .build();  
    gui_window->addGuiObject(std::move(gui_btn_Back));
}

cSelectMissionState::~cSelectMissionState()
{
}

void cSelectMissionState::thinkFast()
{

}

void cSelectMissionState::draw() const
{
    // Mira: draw here screenTexture
    gui_window->draw();

    // MOUSE
    game.getMouse()->draw();
}

void cSelectMissionState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    gui_window->onNotifyMouseEvent(event);
}

eGameStateType cSelectMissionState::getType()
{
    return GAMESTATE_MISSION_SELECT;
}

void cSelectMissionState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}
