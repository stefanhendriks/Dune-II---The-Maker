#include "cSelectMissionState.h"

#include "d2tmc.h"
#include "config.h"
#include "drawers/SDLDrawer.hpp"
// #include "gui/actions/cGuiActionToGameState.h"
#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
// #include "gui/actions/cGuiActionSelectMission.h"


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
    gui_window = new GuiWindow(window);
    gui_window->setTheme(GuiTheme::Light());

    //const GuiRenderKind buttonKinds = GuiRenderKind::OPAQUE_WITH_BORDER;
    //const GuiTextAlignHorizontal buttonTextAlignment = GuiTextAlignHorizontal::CENTER;

    // Title
    gui_window->setTitle("Dune II - The Maker - version " + D2TM_VERSION);


    // QUIT game
//    int quit = mainMenuHeight - (buttonHeight + margin);// 464
//    int width = (buttonWidth / 2);
    int width = buttonWidth;

    int y = 40;
    for (int i = 2; i <= 9; i++) {
        const cRectangle &rect = gui_window->getRelativeRect(margin, y, width, buttonHeight);
        GuiButton *btnMission = GuiButtonBuilder()
            .withRect(rect)        
            .withLabel(std::format("Mission {}", i))
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this,i]() {
                game.jumpToSelectYourNextConquestMission(i);
                game.setNextStateToTransitionTo(GAME_REGION);
                game.initiateFadingOut();})
            .build();   
        // GuiButton *btnMission = new GuiButton(textDrawer, rect, fmt::format("Mission {}", i), buttonKinds);
        // btnMission->setTheme(GuiTheme::Light());
        // btnMission->setTextAlignHorizontal(buttonTextAlignment);
        // btnMission->setOnLeftMouseButtonClickedAction([this,i]() {
        //     game.jumpToSelectYourNextConquestMission(i);
        //     game.setNextStateToTransitionTo(GAME_REGION);
        //     game.initiateFadingOut();});
        gui_window->addGuiObject(btnMission);

        y += buttonHeight + margin;
    }

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);
    width = buttonWidth;
    const cRectangle &backRect = gui_window->getRelativeRect(margin, back, (width - margin), buttonHeight);
    GuiButton *gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)        
            .withLabel("Back")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .onClick([this,prevState]() {
                game.setNextStateToTransitionTo(prevState);})
            .build();  
    // GuiButton *gui_btn_Back = new GuiButton(textDrawer, backRect, "Back", buttonKinds);
    // gui_btn_Back->setTheme(GuiTheme::Light());
    // gui_btn_Back->setTextAlignHorizontal(buttonTextAlignment);
    // // cGuiActionToGameState *action = new cGuiActionToGameState(prevState, false);
    // gui_btn_Back->setOnLeftMouseButtonClickedAction([this,prevState]() {
    //     game.setNextStateToTransitionTo(prevState);});
    gui_window->addGuiObject(gui_btn_Back);
}

cSelectMissionState::~cSelectMissionState()
{
    delete gui_window;
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
