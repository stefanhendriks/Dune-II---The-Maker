/**
 * @file cMainMenuState.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cMainMenuState.h"
#include "controls/cMouse.h"

#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "controls/eKeyAction.h"
#include "config.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "utils/Graphics.hpp"
#include "context/AudioContext.hpp"
#include "context/GameContext.hpp"
#include "utils/cSoundPlayer.h"
#include "game/GameStateTransition.hpp"
#include "game/MissionFlow.hpp"
#include "game/VideoFlow.hpp"

#include <SDL3/SDL.h>
#include <format>
#include <functional>
#include "include/cAssert.h"

cMainMenuState::cMainMenuState(sGameServices* services) :
    cGameState(services),
    m_settings(services->settings),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_missionFlow(services->missionFlow),
    m_videoFlow(services->videoFlow),
    m_stateTransitions(services->stateTransitions),
    m_mouse(services->mouse)
{
    d2tm_assert(m_settings != nullptr);
    d2tm_assert(m_textDrawer != nullptr);
    d2tm_assert(m_missionFlow != nullptr);
    d2tm_assert(m_videoFlow != nullptr);
    d2tm_assert(m_stateTransitions != nullptr);
    d2tm_assert(m_mouse != nullptr);
    auto *gfxinter = m_ctx->getGraphicsContext()->gfxinter.get();
    bmp_D2TM_Title = gfxinter->getTexture(BMP_D2TM);

    int logoWidth = bmp_D2TM_Title->w;
    int logoHeight = bmp_D2TM_Title->h;

    int centerOfScreen = m_settings->getScreenW() / 2;

    logoX = centerOfScreen - (logoWidth / 2);
    logoY = (logoHeight/10);

    mainMenuWidth = 130;
    mainMenuHeight = 161 + 22; // 22 = one more button (Introduction), same as heightBetweenButton below

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
            .withRenderer(m_sdlDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                m_stateTransitions->setNextStateWithFade(GAME_CREDITS);
                })
            .build();

    /////////////////////////////////
    //// Main Menu
    //// Beneath title
    ////////////////////////////////

    const cRectangle &window = cRectangle(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = std::make_unique<GuiWindow>(m_sdlDrawer, window, m_textDrawer);
    gui_window->setTheme(cGuiThemeBuilder().light().build());

    // Buttons are placed relative to the window, one below the other
    const int buttonX = 2;
    const int firstButtonY = 4;
    const int heightBetweenButton = 22;
    buttonWidth = mainMenuWidth - 8;

    int buttonY = firstButtonY;
    auto addButton = [&](const std::string &label, const GuiTheme &theme, std::function<void()> onClick) {
        auto button = GuiButtonBuilder()
                .withRect(gui_window->getRelativeRect(buttonX, buttonY, buttonWidth, buttonHeight))
                .withLabel(label)
                .withTextDrawer(m_textDrawer)
                .withRenderer(m_sdlDrawer)
                .withTheme(theme)
                .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
                .onClick(std::move(onClick))
                .build();
        gui_window->addGuiObject(std::move(button));
        buttonY += heightBetweenButton;
    };
    const GuiTheme light = cGuiThemeBuilder().light().build();
    const GuiTheme inactive = cGuiThemeBuilder().inactive().build();

    addButton("Campaign", light, [this]() {
        m_stateTransitions->setNextStateWithFade(GAME_SELECT_HOUSE);
    });

    buttonY += 1; // Skirmish sits one pixel lower than the rest
    addButton("Skirmish", light, [this]() {
        m_missionFlow->init();
        m_stateTransitions->setNextStateWithFade(GAME_SETUPSKIRMISH);
    });

    addButton("Load", inactive, [this]() { m_stateTransitions->fadeOut(); });
    addButton("Map Editor", light, [this]() { m_stateTransitions->setNextStateWithFade(GAME_NEW_MAP_EDITOR); });
    addButton("Options", light, [this]() { m_stateTransitions->setNextState(GAME_OPTIONS); });
    addButton("Hall of Fame", inactive, [this]() { m_stateTransitions->fadeOut(); });
    addButton("Introduction", light, [this]() { m_videoFlow->play("intro.pak"); });
    addButton("Exit", light, [this]() {
        m_settings->setPlaying(false);
        m_stateTransitions->fadeOut();
    });

    // prepare to drawing in cache texture
    if (m_settings->isDebugMode()) {
        backGroundDebug = m_sdlDrawer->createRenderTargetTexture(m_settings->getScreenW(), m_settings->getScreenH());
        m_sdlDrawer->beginDrawingToTexture(backGroundDebug);
        for (int x = 0; x < m_settings->getScreenW(); x += 60) {
            for (int y = 0; y < m_settings->getScreenH(); y += 20) {
                m_textDrawer->drawText(x, y, Color{48, 48, 48,255}, "DEBUG");
            }
        }
        m_sdlDrawer->endDrawingToTexture();
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
        m_sdlDrawer->renderSprite(backGroundDebug,0,0);
    }

    m_sdlDrawer->renderSprite(bmp_D2TM_Title, logoX, logoY);

    gui_window->draw();
    gui_btn_credits->draw();

    // draw version
    int fontH = m_textDrawer->getFontHeight();
    int versionX = m_settings->getScreenW() - m_textDrawer->getTextLength(D2TM_VERSION) - 20;
    int versionY = m_settings->getScreenH() - fontH * 2 - 42;
    m_textDrawer->drawText(versionX, versionY, Color{255,255,255,255}, D2TM_VERSION);
    m_textDrawer->drawTextBottomRight(D2TM_BUILD_DATETIME, 20);

    if (m_settings->isDebugMode()) {
        m_textDrawer->drawText(0, 0, std::format("{}, {}", m_mouse->getX(), m_mouse->getY()).c_str());
    }

    // MOUSE
    m_mouse->draw();

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
            auto m_soundPlayer = m_ctx->getAudioContext()->getSoundPlayer();
            m_settings->setPlayMusic(!m_settings->isPlayMusic());
            if (!m_settings->isPlayMusic()) {
                m_soundPlayer->stopMusic();
            }
        }
    }
}
