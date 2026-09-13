#include "gamestates/cScoringState.h"

#include "context/GameContext.hpp"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "game/cGameInterface.h"
#include "game/cGameSettings.h"
#include "gameobjects/players/cPlayer.h"
#include "include/cAssert.h"
#include "include/definitions.h"

#include <format>

cScoringState::cScoringState(sGameServices* services) :
    cGameState(services),
    m_settings(services->settings),
    m_interface(m_ctx->getGameInterface()),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer())
{
    d2tm_assert(m_settings != nullptr);
    d2tm_assert(m_interface != nullptr);
    d2tm_assert(m_textDrawer != nullptr);

    m_backgroundTexture = m_interface->getScreenTexture();
    m_stats = m_interface->getMissionStats();

    int continueButtonWidth = m_textDrawer->getTextLength(" Continue ");
    int continueButtonHeight = 21;
    int continueButtonY = m_settings->getScreenH() - 21;
    int continueButtonX = m_settings->getScreenW() - continueButtonWidth;
    cRectangle continueButtonRect(continueButtonX, continueButtonY, continueButtonWidth, continueButtonHeight);
    m_continueButton = GuiButtonBuilder()
            .withRect(continueButtonRect)
            .withLabel("Continue")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_sdlDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this]() {
                m_interface->goingToWinLoseBrief(GAME_WINBRIEF);
                m_interface->initiateFadingOut();
            })
            .build();
}

cScoringState::~cScoringState()
{
    m_backgroundTexture = nullptr;
}

void cScoringState::thinkFast()
{
}

void cScoringState::draw() const
{
    if (m_backgroundTexture) {
        m_sdlDrawer->renderSprite(m_backgroundTexture, 0, 0);
    }

    int lineHeight = m_textDrawer->getFontHeight() + 4;
    int y = 40;

    m_textDrawer->drawTextCentered("MISSION REPORT", y, Color::Yellow);
    y += lineHeight * 2;

    uint64_t minutes = m_stats.elapsedSeconds / 60;
    uint64_t seconds = m_stats.elapsedSeconds % 60;
    m_textDrawer->drawTextCentered(std::format("Mission time: {:02}:{:02}", minutes, seconds), y, Color::White);
    y += lineHeight * 2;

    for (int playerId = 0; playerId < MAX_PLAYERS; playerId++) {
        const PlayerMissionStats &playerStats = m_stats.players[playerId];
        if (playerStats.unitsBuilt == 0 && playerStats.unitsLost == 0 &&
            playerStats.structuresBuilt == 0 && playerStats.structuresLost == 0) {
            continue;
        }

        cPlayer* player = m_interface->getPlayer(playerId);
        std::string playerName = player != nullptr ? player->getHouseName() : std::format("Player {}", playerId);

        m_textDrawer->drawTextCentered(playerName, y, Color::Green);
        y += lineHeight;
        m_textDrawer->drawTextCentered(
            std::format("Units built: {}   Units lost: {}", playerStats.unitsBuilt, playerStats.unitsLost),
            y, Color::White);
        y += lineHeight;
        m_textDrawer->drawTextCentered(
            std::format("Structures built: {}   Structures lost: {}", playerStats.structuresBuilt, playerStats.structuresLost),
            y, Color::White);
        y += lineHeight * 2;
    }

    m_continueButton->draw();
    m_interface->drawCursor();
}

void cScoringState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_continueButton->onNotifyMouseEvent(event);
}

void cScoringState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}

eGameStateType cScoringState::getType()
{
    return GAMESTATE_SCORING;
}
