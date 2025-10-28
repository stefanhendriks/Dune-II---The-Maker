#include "cMainMenuState.h"

#include "d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include <SDL2/SDL.h>
#include <format>

cMainMenuState::cMainMenuState(cGame &theGame, GameContext* ctx) : cGameState(theGame, ctx), textDrawer(cTextDrawer(bene_font))
{
    auto *gfxinter = ctx->getGraphicsContext()->gfxinter.get();
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
    mainMenuFrameX = centerOfScreen -(mainMenuWidth/2);
    mainMenuFrameY = 319;

    logoY = mainMenuFrameY - (logoHeight)*1.2f;

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

    /////////////////////////////////
    //// Main Menu
    //// Beneath title
    ////////////////////////////////

    // Buttons:
    int buttonsX = mainMenuFrameX + 2;

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
    gui_window->addGuiObject(gui_btn_Exit);
}

cMainMenuState::~cMainMenuState()
{
    delete gui_window;
    delete gui_btn_credits;
}

void cMainMenuState::thinkFast()
{
}

void cMainMenuState::draw() const
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

void cMainMenuState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    gui_window->onNotifyMouseEvent(event);
    gui_btn_credits->onNotifyMouseEvent(event);
}

eGameStateType cMainMenuState::getType()
{
    return GAMESTATE_MAIN_MENU;
}

void cMainMenuState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.eventType == eKeyEventType::PRESSED) {
        if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
            game.m_playing=false;
        }
    }
}
