#include "cSelectMissionState.h"

#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "gui/actions/cGuiActionToGameState.h"
#include "gui/cGuiButton.h"
#include "gui/cGuiWindow.h"
#include "gui/actions/cGuiActionSelectMission.h"

#include <allegro/gfx.h>

cSelectMissionState::cSelectMissionState(cGame &theGame, SDL_Surface *background, int prevState)
    : cGameState(theGame)
    ,  background(background)
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
    gui_window = new cGuiWindow(window);

    const eGuiButtonRenderKind buttonKinds = eGuiButtonRenderKind::OPAQUE_WITH_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = eGuiTextAlignHorizontal::CENTER;

    // Title
    gui_window->setTitle("Dune II - The Maker - version " + game.m_version);


    // QUIT game
//    int quit = mainMenuHeight - (buttonHeight + margin);// 464
//    int width = (buttonWidth / 2);
    int width = buttonWidth;

    int y = 40;
    for (int i = 2; i <= 9; i++) {
        const cRectangle &rect = gui_window->getRelativeRect(margin, y, width, buttonHeight);
        cGuiButton *btnMission = new cGuiButton(textDrawer, rect, fmt::format("Mission {}", i), buttonKinds);
        btnMission->setTextAlignHorizontal(buttonTextAlignment);
        btnMission->setOnLeftMouseButtonClickedAction(new cGuiActionSelectMission(i));
        gui_window->addGuiObject(btnMission);

        y += buttonHeight + margin;
    }

    // BACK to where we came from
    int back = mainMenuHeight - (buttonHeight + margin);
    width = buttonWidth;
    const cRectangle &backRect = gui_window->getRelativeRect(margin, back, (width - margin), buttonHeight);
    cGuiButton *gui_btn_Back = new cGuiButton(textDrawer, backRect, "Back", buttonKinds);
    gui_btn_Back->setTextAlignHorizontal(buttonTextAlignment);
    cGuiActionToGameState *action = new cGuiActionToGameState(prevState, false);
    gui_btn_Back->setOnLeftMouseButtonClickedAction(action);
    gui_window->addGuiObject(gui_btn_Back);
}

cSelectMissionState::~cSelectMissionState()
{
    delete gui_window;
    destroy_bitmap(background);
}

void cSelectMissionState::thinkFast()
{

}

void cSelectMissionState::draw() const
{
    renderDrawer->drawSprite(bmp_screen, background, 0, 0);

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
