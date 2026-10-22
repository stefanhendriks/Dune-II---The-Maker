/**
 * @file cSelectMissionState.cpp
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
#include "game/CampaignProgress.hpp"
#include "controls/cMouse.h"
#include "context/cGameObjectContext.h"
#include "gameobjects/players/cPlayer.h"
#include "include/Texture.hpp"
#include "data/gfxinter.h"
#include "context/GraphicsContext.hpp"
#include "utils/Graphics.hpp"

#include "include/cAssert.h"

#include <algorithm>
#include <format>

cSelectMissionState::cSelectMissionState(sGameServices* services, int prevState) :
    cGameState(services),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_settings(services->settings),
    m_interface(m_ctx->getGameInterface()),
    m_mouse(services->mouse)
{
    d2tm_assert(services != nullptr);
    cPlayer* humanPlayer = services->objects->getPlayer(HUMAN);
    const int house = humanPlayer->getHouse();
    Graphics* gfxinter = m_ctx->getGraphicsContext()->gfxinter.get();
    if (house == ATREIDES) {
        m_houseFlag = gfxinter->getTexture(BMP_SELECT_HOUSE_ATREIDES);
    } else if (house == HARKONNEN) {
        m_houseFlag = gfxinter->getTexture(BMP_SELECT_HOUSE_HARKONNEN);
    } else if (house == ORDOS) {
        m_houseFlag = gfxinter->getTexture(BMP_SELECT_HOUSE_ORDOS);
    } else if (house == SARDAUKAR) {
        m_houseFlag = gfxinter->getTexture(BMP_SELECT_HOUSE_SARDAUKAR);
    } else {
        m_houseFlag = humanPlayer->getFlagBitmap();
    }
    const int highestUnlockedMission = services->campaignProgress && CampaignProgress::supportsHouse(house)
        ? services->campaignProgress->highestUnlockedMission(house)
        : CampaignProgress::LAST_MISSION;
    const bool campaignCompleted = services->campaignProgress &&
        services->campaignProgress->nextMission(house) == CampaignProgress::COMPLETED;
    const int mainMenuFrameX = (m_settings->getScreenW() - 640) / 2;
    const int mainMenuFrameY = (m_settings->getScreenH() - 480) / 2;
    const int mainMenuWidth = 640;
    const int mainMenuHeight = 480;

    const cRectangle window(mainMenuFrameX, mainMenuFrameY, mainMenuWidth, mainMenuHeight);
    gui_window = std::make_unique<GuiWindow>(m_sdlDrawer, window, m_textDrawer);
    gui_window->setTheme(cGuiThemeBuilder().light().build());

    gui_window->setTitle(humanPlayer->getHouseName() + " Campaign");

    constexpr int flagMaxWidth = 270;
    constexpr int flagMaxHeight = 220;
    const float flagScale = std::min(
        static_cast<float>(flagMaxWidth) / static_cast<float>(m_houseFlag->w),
        static_cast<float>(flagMaxHeight) / static_cast<float>(m_houseFlag->h));
    m_flagWidth = static_cast<int>(static_cast<float>(m_houseFlag->w) * flagScale);
    m_flagHeight = static_cast<int>(static_cast<float>(m_houseFlag->h) * flagScale);
    m_flagX = mainMenuFrameX + (320 - m_flagWidth) / 2;
    m_flagY = mainMenuFrameY + 65 + (230 - m_flagHeight) / 2;

    const int buttonHeight = std::max(30, m_textDrawer->getFontHeight() + 12);
    const int buttonWidth = 270;
    const int leftButtonX = mainMenuFrameX + 25;
    const int rightButtonX = mainMenuFrameX + 345;
    m_headerX = rightButtonX;
    m_headerWidth = buttonWidth;
    m_headerY = mainMenuFrameY + 42;
    const int missionButtonY = mainMenuFrameY + 62;
    const int missionButtonHeight = 34;
    const int missionButtonSpacing = 38;

    auto continueButton = GuiButtonBuilder()
        .withRect(cRectangle(leftButtonX, mainMenuFrameY + 340, buttonWidth, buttonHeight))
        .withLabel("CONTINUE")
        .withTextDrawer(m_textDrawer)
        .withRenderer(m_sdlDrawer)
        .withTheme(campaignCompleted
                       ? cGuiThemeBuilder().inactive().build()
                       : cGuiThemeBuilder().light().build())
        .onClick([this, campaignCompleted, highestUnlockedMission]() {
            if (campaignCompleted) {
                return;
            }
            m_interface->jumpToSelectYourNextConquestMission(highestUnlockedMission);
            m_interface->setTransitionToWithFadingOut(
                highestUnlockedMission == CampaignProgress::FIRST_MISSION ? GAME_BRIEFING : GAME_REGION);
        })
        .build();
    gui_window->addGuiObject(std::move(continueButton));

    for (int i = 1; i <= highestUnlockedMission; i++) {
        auto btnMission = GuiButtonBuilder()
            .withRect(cRectangle(
                rightButtonX,
                missionButtonY + (i - 1) * missionButtonSpacing,
                buttonWidth,
                missionButtonHeight))
            .withLabel(std::format("MISSION {}", i))
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_sdlDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .onClick([this, i]() {
                m_interface->jumpToSelectYourNextConquestMission(i);
                m_interface->setTransitionToWithFadingOut(i == CampaignProgress::FIRST_MISSION ? GAME_BRIEFING : GAME_REGION);
            })
            .build();
        gui_window->addGuiObject(std::move(btnMission));
    }

    const cRectangle backRect = gui_window->getRelativeRect(570, 450, 55, 22);
    auto gui_btn_Back = GuiButtonBuilder()
            .withRect(backRect)
            .withLabel("Back")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_sdlDrawer)
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
    gui_window->draw();
    m_sdlDrawer->renderTexture(
        m_houseFlag->tex.get(), m_flagX, m_flagY, m_flagWidth, m_flagHeight);
    m_textDrawer->drawTextCentered(
        "REPLAY MISSION",
        m_headerX,
        m_headerWidth,
        m_headerY,
        Color::Yellow);

    // MOUSE
    m_mouse->draw();
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
