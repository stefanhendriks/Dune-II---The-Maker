#include "cMainMenuGameState.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "gui.h"
#include "gui/actions/cGuiActionExitGame.h"
#include "gui/actions/cGuiActionFadeOutOnly.h"
#include "gui/actions/cGuiActionSelectHouse.h"
#include "gui/actions/cGuiActionSetupSkirmishGame.h"
#include "gui/actions/cGuiActionShowOptions.h"
#include "gui/actions/cGuiActionToGameState.h"
#include "drawers/SDLDrawer.hpp"

#include <SDL2/SDL.h>
#include <fmt/core.h>

cMainMenuGameState::cMainMenuGameState(cGame &theGame) : cGameState(theGame), textDrawer(cTextDrawer(bene_font))
{
    bmp_D2TM_Title = gfxinter->getSurface(BMP_D2TM);

    int logoWidth = bmp_D2TM_Title->w;
    int logoHeight = bmp_D2TM_Title->h;

    int centerOfScreen = game.m_screenW / 2;

    logoX = centerOfScreen - (logoWidth / 2);
    logoY = (logoHeight/10);

    mainMenuWidth = 130;
    mainMenuHeight = 143;

    // adjust x and y according to resolution, we can add because the above values
    // assume 640x480 resolution, and logoX/logoY are already taking care of > resolutions
//    mainMenuFrameX = 257 + logoX;
    mainMenuFrameX = centerOfScreen -(mainMenuWidth/2);
    mainMenuFrameY = 319;

    logoY = mainMenuFrameY - (logoHeight)*1.2f;

    const eGuiButtonRenderKind buttonKinds = eGuiButtonRenderKind::TRANSPARENT_WITHOUT_BORDER;
    const eGuiTextAlignHorizontal buttonTextAlignment = eGuiTextAlignHorizontal::CENTER;

//    /////////////////////////////////
//    //// Credits (top)
//    ////////////////////////////////
    int buttonWidth = 12; //Mira TEXT alfont_text_length(bene_font, "CREDITS") / 2;
    int buttonHeight = textDrawer.getFontHeight() + 4; // a bit more space

    int creditsX = (game.m_screenW / 2) - buttonWidth;
    GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(creditsX, 1, "CREDITS", SDL_Color{64, 64, 64,255});
    const cRectangle &creditsRect = cRectangle(creditsX, 0, buttonWidth, buttonHeight);
    gui_btn_credits = new cGuiButton(textDrawer, creditsRect, "CREDITS", buttonKinds);
    gui_btn_credits->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_credits->setOnLeftMouseButtonClickedAction(new cGuiActionToGameState(GAME_CREDITS, true));


    /////////////////////////////////
    //// Main Menu
    //// Beneath title
    ////////////////////////////////

    // Buttons:
    int buttonsX = mainMenuFrameX + 2;

    SDL_Color colorInactiveHover = SDL_Color{128, 128, 128,255};
    SDL_Color colorInactive = SDL_Color{192, 192, 192,255};

    // PLAY
    int playY = 323;

    buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = new cGuiWindow(window);

    const cRectangle &campaign = cRectangle(buttonsX, playY, buttonWidth, buttonHeight);

    cGuiButton *gui_btn_SelectHouse = new cGuiButton(textDrawer, campaign, "Campaign", buttonKinds);
    gui_btn_SelectHouse->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_SelectHouse->setOnLeftMouseButtonClickedAction(new cGuiActionSelectHouse());
    gui_window->addGuiObject(gui_btn_SelectHouse);

    int skirmishY = 344;
    const cRectangle &skirmish = cRectangle(buttonsX, skirmishY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Skirmish = new cGuiButton(textDrawer, skirmish, "Skirmish", buttonKinds);
    gui_btn_Skirmish->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Skirmish->setOnLeftMouseButtonClickedAction(new cGuiActionSetupSkirmishGame());
    gui_window->addGuiObject(gui_btn_Skirmish);

    int multiplayerY = 364;
    const cRectangle &multiplayer = cRectangle(buttonsX, multiplayerY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Multiplayer = new cGuiButton(textDrawer, multiplayer, "Multiplayer", buttonKinds);
    gui_btn_Multiplayer->setTextColor(colorInactive);
    gui_btn_Multiplayer->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Multiplayer->setTextColorHover(colorInactiveHover);
    gui_btn_Multiplayer->setOnLeftMouseButtonClickedAction(new cGuiActionFadeOutOnly());
    gui_window->addGuiObject(gui_btn_Multiplayer);

    // LOAD
    int loadY = 384;
    const cRectangle &load = cRectangle(buttonsX, loadY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Load = new cGuiButton(textDrawer, load, "Load", buttonKinds);
    gui_btn_Load->setTextColor(colorInactive);
    gui_btn_Load->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Load->setTextColorHover(colorInactiveHover);
    gui_btn_Load->setOnLeftMouseButtonClickedAction(new cGuiActionFadeOutOnly());
    gui_window->addGuiObject(gui_btn_Load);

    // OPTIONS
    int optionsY = 404;
    const cRectangle &options = cRectangle(buttonsX, optionsY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Options = new cGuiButton(textDrawer, options, "Options", buttonKinds);
    gui_btn_Options->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Options->setOnLeftMouseButtonClickedAction(new cGuiActionShowOptions());
    gui_window->addGuiObject(gui_btn_Options);

    // HALL OF FAME
    int hofY = 424;
    const cRectangle &hof = cRectangle(buttonsX, hofY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Hof = new cGuiButton(textDrawer, hof, "Hall of Fame", buttonKinds);
    gui_btn_Hof->setTextColor(colorInactive);
    gui_btn_Hof->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Hof->setTextColorHover(colorInactiveHover);
    gui_btn_Hof->setOnLeftMouseButtonClickedAction(new cGuiActionFadeOutOnly());
    gui_window->addGuiObject(gui_btn_Hof);

    // EXIT
    int exitY = 444;
    const cRectangle &exit = cRectangle(buttonsX, exitY, buttonWidth, buttonHeight);
    cGuiButton *gui_btn_Exit = new cGuiButton(textDrawer, exit, "Exit", buttonKinds);
    gui_btn_Exit->setTextAlignHorizontal(buttonTextAlignment);
    gui_btn_Exit->setOnLeftMouseButtonClickedAction(new cGuiActionExitGame());
    gui_window->addGuiObject(gui_btn_Exit);
}

cMainMenuGameState::~cMainMenuGameState()
{
    delete gui_window;
    delete gui_btn_credits;
}

void cMainMenuGameState::thinkFast()
{

}

void cMainMenuGameState::draw() const
{
    if (game.isDebugMode()) {
        for (int x = 0; x < game.m_screenW; x += 60) {
            for (int y = 0; y < game.m_screenH; y += 20) {
                //Mira TEXT alfont_textprintf(bmp_screen, bene_font, x, y, SDL_Color{48, 48, 48), "DEBUG");
            }
        }
    }

    renderDrawer->drawSprite(bmp_screen, bmp_D2TM_Title, logoX, logoY);

    GUI_DRAW_FRAME(mainMenuFrameX, mainMenuFrameY, mainMenuWidth,mainMenuHeight);

    gui_window->draw();

    gui_btn_credits->draw();

    // draw version
    textDrawer.drawTextBottomRight(game.m_version.c_str());

    if (game.isDebugMode()) {
        auto m_mouse = game.getMouse();
        textDrawer.drawText(0, 0, fmt::format("{}, {}", m_mouse->getX(), m_mouse->getY()).c_str());
    }

    // MOUSE
    game.getMouse()->draw();

}

void cMainMenuGameState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    gui_window->onNotifyMouseEvent(event);
    gui_btn_credits->onNotifyMouseEvent(event);
}

eGameStateType cMainMenuGameState::getType()
{
    return GAMESTATE_MAIN_MENU;
}

void cMainMenuGameState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.eventType == eKeyEventType::PRESSED) {
        if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
            game.m_playing=false;
        }
    }
}
