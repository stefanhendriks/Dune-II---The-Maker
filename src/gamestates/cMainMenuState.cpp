#include "cMainMenuState.h"
#include "controls/cMouse.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "controls/eKeyAction.h"
//#include "game/cGame.h"
#include "config.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
//#include "context/GraphicsContext.hpp"
#include "utils/cSoundPlayer.h"
#include "game/cGameInterface.h"

#include <SDL2/SDL_scancode.h>
#include <format>
#include <cassert>

cMainMenuState::cMainMenuState(cGame &theGame, sGameServices* services) :
    cGameState(theGame, services),
    m_settings(services->settings),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_interface(m_ctx->getGameInterface())
{
    assert(m_settings != nullptr);
    assert(m_textDrawer != nullptr);
    assert(m_interface != nullptr);
    auto *gfxinter = m_ctx->getGraphicsContext()->gfxinter.get();
    bmp_D2TM_Title = gfxinter->getTexture(BMP_D2TM);

    int logoWidth = bmp_D2TM_Title->w;
    int logoHeight = bmp_D2TM_Title->h;

    int centerOfScreen = m_settings->getScreenW() / 2;

    logoX = centerOfScreen - (logoWidth / 2);
    logoY = (logoHeight/10);

    mainMenuWidth = 130;
    mainMenuHeight = 183;

    sdl2power = cRectangle{centerOfScreen+logoWidth/4, logoY+logoHeight+75,0,0};

    // adjust x and y according to resolution, we can add because the above values
    // assume 640x480 resolution, and logoX/logoY are already taking care of > resolutions
    mainMenuFrameX = centerOfScreen -(mainMenuWidth/2);
    mainMenuFrameY = 319;

    logoY = mainMenuFrameY - (logoHeight)*1.2f;

    /////////////////////////////////
    //// Credits (top)
    ////////////////////////////////
    int buttonWidth = m_textDrawer->getTextLength("CREDITS") / 2;
    int buttonHeight = m_textDrawer->getFontHeight() + 4; // a bit more space

    int creditsX = (m_settings->getScreenW() / 2) - buttonWidth;
    const cRectangle &creditsRect = cRectangle(creditsX, 0, buttonWidth, buttonHeight);

    gui_btn_credits = GuiButtonBuilder()
            .withRect(creditsRect)        
            .withLabel("CREDITS")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                m_interface->setTransitionToWithFadingOut(GAME_CREDITS);
                })
            .build();

    /////////////////////////////////
    //// Main Menu
    //// Beneath title
    ////////////////////////////////

    // Buttons:
    int buttonsX = mainMenuFrameX + 2;
    // height
    const int heightBetweenButton = 22;
    // PLAY
    int playY = 323;

    buttonWidth = mainMenuWidth - 8;

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = std::make_unique<GuiWindow>(m_renderDrawer, window, m_textDrawer);
    gui_window->setTheme(cGuiThemeBuilder().light().build());

    const cRectangle &campaign = cRectangle(buttonsX, playY, buttonWidth, buttonHeight);
    auto gui_btn_SelectHouse = GuiButtonBuilder()
            .withRect(campaign)        
            .withLabel("Campaign")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                m_interface->setTransitionToWithFadingOut(GAME_SELECT_HOUSE);
                })
            .build();
    gui_window->addGuiObject(std::move(gui_btn_SelectHouse));

    int skirmishY = playY + heightBetweenButton+1;
    const cRectangle &skirmish = cRectangle(buttonsX, skirmishY, buttonWidth, buttonHeight);
    auto gui_btn_Skirmish = GuiButtonBuilder()
            .withRect(skirmish)        
            .withLabel("Skirmish")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                m_interface->loadSkirmishMaps();
                m_interface->initSkirmish();
                m_interface->setTransitionToWithFadingOut(GAME_SETUPSKIRMISH);
                })
            .build();
    gui_window->addGuiObject(std::move(gui_btn_Skirmish));

    int multiplayerY = skirmishY+heightBetweenButton;
    const cRectangle &multiplayer = cRectangle(buttonsX, multiplayerY, buttonWidth, buttonHeight);
    auto gui_btn_Multiplayer = GuiButtonBuilder()
            .withRect(multiplayer)        
            .withLabel("Multiplayer")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().inactive().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){m_interface->initiateFadingOut();})
            .build();
    gui_window->addGuiObject(std::move(gui_btn_Multiplayer));

    // LOAD
    int loadY = multiplayerY+heightBetweenButton;
    const cRectangle &load = cRectangle(buttonsX, loadY, buttonWidth, buttonHeight);
    auto gui_btn_Load = GuiButtonBuilder()
            .withRect(load)        
            .withLabel("Load")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().inactive().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){m_interface->initiateFadingOut();})
            .build();
    gui_window->addGuiObject(std::move(gui_btn_Load));

    // Editor
    int editorY = loadY+heightBetweenButton;
    const cRectangle &editors = cRectangle(buttonsX, editorY, buttonWidth, buttonHeight);
    auto gui_btn_Editor = GuiButtonBuilder()
            .withRect(editors)        
            .withLabel("New Map Editor")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){m_interface->setTransitionToWithFadingOut(GAME_NEW_MAP_EDITOR);})
            .build();
    gui_window->addGuiObject(std::move(gui_btn_Editor));

    // OPTIONS
    int optionsY = editorY+heightBetweenButton;
    const cRectangle &options = cRectangle(buttonsX, optionsY, buttonWidth, buttonHeight);
    auto gui_btn_Options = GuiButtonBuilder()
            .withRect(options)        
            .withLabel("Options")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){m_interface->setNextStateToTransitionTo(GAME_OPTIONS);})
            .build();
    gui_window->addGuiObject(std::move(gui_btn_Options));

    // HALL OF FAME
    int hofY = optionsY+heightBetweenButton;
    const cRectangle &hof = cRectangle(buttonsX, hofY, buttonWidth, buttonHeight);
    auto gui_btn_Hof = GuiButtonBuilder()
            .withRect(hof)        
            .withLabel("Hall of Fame")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().inactive().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this](){m_interface->initiateFadingOut();})
            .build();
    gui_window->addGuiObject(std::move(gui_btn_Hof));

    // EXIT
    int exitY = hofY+heightBetweenButton;
    const cRectangle &exit = cRectangle(buttonsX, exitY, buttonWidth, buttonHeight);
    auto gui_btn_Exit = GuiButtonBuilder()
            .withRect(exit)        
            .withLabel("Exit")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                m_settings->setPlaying(false);
                m_interface->initiateFadingOut();})
            .build();
    gui_window->addGuiObject(std::move(gui_btn_Exit));

    // prepare to drawing in cache texture
    if (m_settings->isDebugMode()) {
        backGroundDebug = m_renderDrawer->createRenderTargetTexture(m_settings->getScreenW(), m_settings->getScreenH());
        m_renderDrawer->beginDrawingToTexture(backGroundDebug);
        for (int x = 0; x < m_settings->getScreenW(); x += 60) {
            for (int y = 0; y < m_settings->getScreenH(); y += 20) {
                m_textDrawer->drawText(x, y, Color{48, 48, 48,255}, "DEBUG");
            }
        }
        m_renderDrawer->endDrawingToTexture();
    }
}

cMainMenuState::~cMainMenuState()
{
    // Smart pointers handle cleanup
    if (m_settings->isDebugMode()) {
        delete backGroundDebug;
    }
}

void cMainMenuState::thinkFast()
{
}

void cMainMenuState::draw() const
{
    if (m_settings->isDebugMode()) {
        m_renderDrawer->renderSprite(backGroundDebug,0,0);
    }

    m_renderDrawer->renderSprite(bmp_D2TM_Title, logoX, logoY);

    gui_window->draw();
    gui_btn_credits->draw();

    // draw version
    m_textDrawer->drawTextBottomRight(D2TM_VERSION,20);
    m_textDrawer->drawText(sdl2power.getX(),sdl2power.getY(),Color{255,255,0,200},"SDL2 powered");

    if (m_settings->isDebugMode()) {
        auto m_mouse = m_interface->getMouse();
        m_textDrawer->drawText(0, 0, std::format("{}, {}", m_mouse->getX(), m_mouse->getY()).c_str());
    }

    // MOUSE
    m_interface->drawCursor();

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
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.isAction(eKeyAction::MENU_BACK)) {
            m_settings->setPlaying(false);
        }

        if (event.isAction(eKeyAction::TOGGLE_MUSIC) || event.hasKey(SDL_SCANCODE_MUTE)) {
            auto m_soundPlayer = m_ctx->getSoundPlayer();
            m_settings->setPlayMusic(!m_settings->isPlayMusic());
            if (!m_settings->isPlayMusic()) {
                m_soundPlayer->stopMusic();
            }
        }
    }
}
