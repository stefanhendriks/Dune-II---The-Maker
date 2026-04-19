#include "cSelectMissionState.h"
#include "context/GameContext.hpp"
//#include "game/cGame.h"
#include "config.h"
#include "drawers/cTextDrawer.h"
#include "gui/GuiTheme.hpp"
#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "game/cGameInterface.h"
#include "game/cGameSettings.h"

#include <cassert>

cSelectMissionState::cSelectMissionState(sGameServices* services, int prevState) :
    cGameState(services),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_settings(services->settings),
    m_interface(m_ctx->getGameInterface())
{
    assert(services != nullptr);
    int margin = m_settings->getScreenH() * 0.3;
    int mainMenuFrameX = margin;
    int mainMenuFrameY = margin;
    int mainMenuWidth = m_settings->getScreenW() - (margin * 2);
    int mainMenuHeight = m_settings->getScreenH() - (margin * 2);

    margin = 4;
    int buttonHeight = m_textDrawer->getFontHeight() + margin;
    int buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = std::make_unique<GuiWindow>(m_renderDrawer, window, m_textDrawer);
    gui_window->setTheme(cGuiThemeBuilder().light().build());

    // Title
    gui_window->setTitle("Dune II - The Maker - version " + D2TM_VERSION);


    // QUIT game
    int width = buttonWidth;

    int y = 40;
    for (int i = 2; i <= 9; i++) {
        const cRectangle &rect = gui_window->getRelativeRect(margin, y, width, buttonHeight);
        auto btnMission = GuiButtonBuilder()
            .withRect(rect)        
            .withLabel(std::format("Mission {}", i))
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)    
            .withTheme(cGuiThemeBuilder().light().build())
            .onClick([this,i]() {
                m_interface->jumpToSelectYourNextConquestMission(i);
                m_interface->setTransitionToWithFadingOut(GAME_REGION);
            })
            .build();   
        gui_window->addGuiObject(std::move(btnMission));

        y += buttonHeight + margin;
    }

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);
    width = buttonWidth;
    const cRectangle &backRect = gui_window->getRelativeRect(margin, back, (width - margin), buttonHeight);
    auto gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)        
            .withLabel("Back")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)    
            .withTheme(cGuiThemeBuilder().light().build())
            .onClick([this,prevState]() {
                m_interface->setNextStateToTransitionTo(prevState);
            })
            .build();  
    gui_window->addGuiObject(std::move(gui_btn_Back));
}

cSelectMissionState::~cSelectMissionState()
{
    // Smart pointers handle cleanup
}

void cSelectMissionState::thinkFast()
{

}

void cSelectMissionState::draw() const
{
    // Mira: draw here screenTexture
    gui_window->draw();

    // MOUSE
    m_interface->drawCursor();
}

void cSelectMissionState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    gui_window->onNotifyMouseEvent(event);
}

eGameStateType cSelectMissionState::getType()
{
    return GAMESTATE_SELECT_MISSION;
}

void cSelectMissionState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}
