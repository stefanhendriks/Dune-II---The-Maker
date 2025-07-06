#include "cMainMenuGameState.h"

#include "d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
//#include "gui/actions/cGuiActionExitGame.h"
// #include "gui/actions/cGuiActionFadeOutOnly.h"
// #include "gui/actions/cGuiActionSelectHouse.h"
// #include "gui/actions/cGuiActionSetupSkirmishGame.h"
// #include "gui/actions/cGuiActionShowOptions.h"
// #include "gui/actions/cGuiActionToGameState.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"

#include <SDL2/SDL.h>
#include <format>

cMainMenuGameState::cMainMenuGameState(cGame &theGame) : cGameState(theGame), textDrawer(cTextDrawer(bene_font))
{
    bmp_D2TM_Title = gfxinter->getTexture(BMP_D2TM);

    int logoWidth = bmp_D2TM_Title->w;
    int logoHeight = bmp_D2TM_Title->h;

    int centerOfScreen = game.m_screenW / 2;

    logoX = centerOfScreen - (logoWidth / 2);
    logoY = (logoHeight/10);

    mainMenuWidth = 130;
    mainMenuHeight = 143;

    sdl2power = cRectangle{centerOfScreen+logoWidth/4, logoY+logoHeight+75,0,0};

    // adjust x and y according to resolution, we can add because the above values
    // assume 640x480 resolution, and logoX/logoY are already taking care of > resolutions
//    mainMenuFrameX = 257 + logoX;
    mainMenuFrameX = centerOfScreen -(mainMenuWidth/2);
    mainMenuFrameY = 319;

    logoY = mainMenuFrameY - (logoHeight)*1.2f;

    //const GuiRenderKind buttonKinds = GuiRenderKind::TRANSPARENT_WITHOUT_BORDER;
    //const GuiTextAlignHorizontal buttonTextAlignment = GuiTextAlignHorizontal::CENTER;

//    /////////////////////////////////
//    //// Credits (top)
//    ////////////////////////////////
    int buttonWidth = textDrawer.getTextLength("CREDITS") / 2;
    int buttonHeight = textDrawer.getFontHeight() + 4; // a bit more space

    int creditsX = (game.m_screenW / 2) - buttonWidth;
    const cRectangle &creditsRect = cRectangle(creditsX, 0, buttonWidth, buttonHeight);

    gui_btn_credits = GuiButtonBuilder()
            .withRect(creditsRect)        
            .withLabel("CREDITS")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                game.setNextStateToTransitionTo(GAME_CREDITS);
                game.initiateFadingOut();})
            .build();

    // gui_btn_credits = new GuiButton(creditsRect, "CREDITS");
    // gui_btn_credits->setTextDrawer(&textDrawer);
    // gui_btn_credits->setTheme(GuiTheme::Light());
    // gui_btn_credits->setTextAlignHorizontal(buttonTextAlignment);
    // gui_btn_credits->setRenderKind(buttonKinds);
    // gui_btn_credits->setOnLeftMouseButtonClickedAction([this]() {
    //     game.setNextStateToTransitionTo(GAME_CREDITS);
    //     game.initiateFadingOut();});

    /////////////////////////////////
    //// Main Menu
    //// Beneath title
    ////////////////////////////////

    // Buttons:
    int buttonsX = mainMenuFrameX + 2;

    // Color colorInactiveHover = Color{128, 128, 128,255};
    // Color colorInactive = Color{192, 192, 192,255};

    // PLAY
    int playY = 323;

    buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = new GuiWindow(window);
    gui_window->setTheme(GuiTheme::Light());

    const cRectangle &campaign = cRectangle(buttonsX, playY, buttonWidth, buttonHeight);
    GuiButton *gui_btn_SelectHouse = GuiButtonBuilder()
            .withRect(campaign)        
            .withLabel("Campaign")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
                game.initiateFadingOut();})
            .build();
    // GuiButton *gui_btn_SelectHouse = new GuiButton(textDrawer, campaign, "Campaign", buttonKinds);
    // gui_btn_SelectHouse->setTheme(GuiTheme::Light());
    // gui_btn_SelectHouse->setTextAlignHorizontal(buttonTextAlignment);
    // gui_btn_SelectHouse->setOnLeftMouseButtonClickedAction([this]() {
    //     game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
    //     game.initiateFadingOut();});
    gui_window->addGuiObject(gui_btn_SelectHouse);

    int skirmishY = 344;
    const cRectangle &skirmish = cRectangle(buttonsX, skirmishY, buttonWidth, buttonHeight);
    GuiButton *gui_btn_Skirmish = GuiButtonBuilder()
            .withRect(skirmish)        
            .withLabel("Skirmish")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                game.loadSkirmishMaps();
                game.initSkirmish();
                game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
                game.initiateFadingOut();})
            .build();
    // GuiButton *gui_btn_Skirmish = new GuiButton(textDrawer, skirmish, "Skirmish", buttonKinds);
    // gui_btn_Skirmish->setTheme(GuiTheme::Light());
    // gui_btn_Skirmish->setTextAlignHorizontal(buttonTextAlignment);
    // gui_btn_Skirmish->setOnLeftMouseButtonClickedAction([this](){
    //     game.loadSkirmishMaps();
    //     game.initSkirmish();
    //     game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
    //     game.initiateFadingOut();});
    gui_window->addGuiObject(gui_btn_Skirmish);

    int multiplayerY = 364;
    const cRectangle &multiplayer = cRectangle(buttonsX, multiplayerY, buttonWidth, buttonHeight);
    GuiButton *gui_btn_Multiplayer = GuiButtonBuilder()
            .withRect(multiplayer)        
            .withLabel("Multiplayer")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Inactive())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){game.initiateFadingOut();})
            .build();
    // GuiButton *gui_btn_Multiplayer = new GuiButton(textDrawer, multiplayer, "Multiplayer", buttonKinds);
    // gui_btn_Multiplayer->setTheme(GuiTheme::Inactive());
    // // gui_btn_Multiplayer->setTextColor(colorInactive);
    // gui_btn_Multiplayer->setTextAlignHorizontal(buttonTextAlignment);
    // // gui_btn_Multiplayer->setTextColorHover(colorInactiveHover);
    // gui_btn_Multiplayer->setOnLeftMouseButtonClickedAction([this](){game.initiateFadingOut();});
    gui_window->addGuiObject(gui_btn_Multiplayer);

    // LOAD
    int loadY = 384;
    const cRectangle &load = cRectangle(buttonsX, loadY, buttonWidth, buttonHeight);
    GuiButton *gui_btn_Load = GuiButtonBuilder()
            .withRect(load)        
            .withLabel("Load")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Inactive())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){game.initiateFadingOut();})
            .build();
    // GuiButton *gui_btn_Load = new GuiButton(textDrawer, load, "Load", buttonKinds);
    // gui_btn_Load->setTheme(GuiTheme::Inactive());
    // // gui_btn_Load->setTextColor(colorInactive);
    // gui_btn_Load->setTextAlignHorizontal(buttonTextAlignment);
    // // gui_btn_Load->setTextColorHover(colorInactiveHover);
    // gui_btn_Load->setOnLeftMouseButtonClickedAction([this](){game.initiateFadingOut();});
    gui_window->addGuiObject(gui_btn_Load);

    // OPTIONS
    int optionsY = 404;
    const cRectangle &options = cRectangle(buttonsX, optionsY, buttonWidth, buttonHeight);
    GuiButton *gui_btn_Options = GuiButtonBuilder()
            .withRect(options)        
            .withLabel("Options")
            .withTextDrawer(&textDrawer)
            .withTheme(GuiTheme::Light())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){game.setNextStateToTransitionTo(GAME_OPTIONS);})
            .build();
    // GuiButton *gui_btn_Options = new GuiButton(textDrawer, options, "Options", buttonKinds);
    // gui_btn_Options->setTheme(GuiTheme::Light());
    // gui_btn_Options->setTextAlignHorizontal(buttonTextAlignment);
    // gui_btn_Options->setOnLeftMouseButtonClickedAction([this](){game.setNextStateToTransitionTo(GAME_OPTIONS);});
    gui_window->addGuiObject(gui_btn_Options);

    // HALL OF FAME
    int hofY = 424;
    const cRectangle &hof = cRectangle(buttonsX, hofY, buttonWidth, buttonHeight);
    GuiButton *gui_btn_Hof = GuiButtonBuilder()
            .withRect(hof)        
            .withLabel("Hall of Fame")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Inactive())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){game.initiateFadingOut();})
            .build();
    // GuiButton *gui_btn_Hof = new GuiButton(textDrawer, hof, "Hall of Fame", buttonKinds);
    // gui_btn_Hof->setTheme(GuiTheme::Inactive());
    // // gui_btn_Hof->setTextColor(colorInactive);
    // gui_btn_Hof->setTextAlignHorizontal(buttonTextAlignment);
    // // gui_btn_Hof->setTextColorHover(colorInactiveHover);
    // gui_btn_Hof->setOnLeftMouseButtonClickedAction([this](){game.initiateFadingOut();});
    gui_window->addGuiObject(gui_btn_Hof);

    // EXIT
    int exitY = 444;
    const cRectangle &exit = cRectangle(buttonsX, exitY, buttonWidth, buttonHeight);
    GuiButton *gui_btn_Exit = GuiButtonBuilder()
            .withRect(exit)        
            .withLabel("Exit")
            .withTextDrawer(&textDrawer)    
            .withTheme(GuiTheme::Light())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                game.m_playing = false;
                game.initiateFadingOut();})
            .build();    
    // GuiButton *gui_btn_Exit = new GuiButton(textDrawer, exit, "Exit", buttonKinds);
    // gui_btn_Exit->setTheme(GuiTheme::Light());
    // gui_btn_Exit->setTextAlignHorizontal(buttonTextAlignment);
    // gui_btn_Exit->setOnLeftMouseButtonClickedAction([this]() {
    //     game.m_playing = false;
    //     game.initiateFadingOut();});
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
                textDrawer.drawText(x, y, Color{48, 48, 48,255}, "DEBUG");
            }
        }
    }

    renderDrawer->renderSprite(bmp_D2TM_Title, logoX, logoY);

    gui_window->draw();
    gui_btn_credits->draw();

    // draw version
    textDrawer.drawTextBottomRight(D2TM_VERSION,20);
    textDrawer.drawText(sdl2power.getX(),sdl2power.getY(),Color{255,255,0,200},"SDL2 powered");

    if (game.isDebugMode()) {
        auto m_mouse = game.getMouse();
        textDrawer.drawText(0, 0, std::format("{}, {}", m_mouse->getX(), m_mouse->getY()).c_str());
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
