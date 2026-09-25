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

    m_stats = m_interface->getMissionStats();
}

void cScoringState::continueToWinBrief() const
{
    m_interface->goingToWinLoseBrief(GAME_WINBRIEF);
    m_interface->initiateFadingOut();
}

cScoringState::~cScoringState()
{
}

void cScoringState::thinkFast()
{
}

void cScoringState::draw() const
{
    m_sdlDrawer->renderClearToColor();

    int lineHeight = m_textDrawer->getFontHeight() + 4;
    int y = 40;

    m_textDrawer->drawTextCentered("MISSION REPORT", y, Color::Yellow);
    y += lineHeight * 2;

    uint64_t minutes = m_stats.elapsedSeconds / 60;
    uint64_t seconds = m_stats.elapsedSeconds % 60;
    m_textDrawer->drawTextCentered(std::format("Mission time: {:02}:{:02}", minutes, seconds), y, Color::White);
    y += lineHeight * 2;

    // AI_CPU5 (Fremen superweapon trigger) and AI_WORM are not real opponents, never shown here.
    for (int playerId = HUMAN; playerId < AI_WORM - 1; playerId++) {
        const PlayerMissionStats &playerStats = m_stats.players[playerId];
        // Read from the mission-start snapshot, not the live player: cGamePlaying resets a
        // defeated player's house to GENERALHOUSE as elimination cleanup, which would otherwise
        // hide anyone the player actually beat during the mission.
        bool isPlaying = playerId == HUMAN || playerStats.house != GENERALHOUSE;
        if (!isPlaying) {
            continue;
        }

        std::string label = playerId == HUMAN ? "YOU" : "ENEMY";
        std::string playerName = std::format("{} - {}", label, cPlayer::getHouseNameForId(playerStats.house));
        Color nameColor = m_interface->getColorFadeSelected(playerStats.minimapColor.r, playerStats.minimapColor.g, playerStats.minimapColor.b);

        m_textDrawer->drawTextCentered(playerName, y, nameColor);
        y += lineHeight;
        m_textDrawer->drawTextCentered(
            std::format("Units built: {}   lost: {}   destroyed: {}",
                playerStats.unitsBuilt, playerStats.unitsLost, playerStats.unitsDestroyed),
            y, Color::White);
        y += lineHeight;
        m_textDrawer->drawTextCentered(
            std::format("Structures built: {}   lost: {}   destroyed: {}",
                playerStats.structuresBuilt, playerStats.structuresLost, playerStats.structuresDestroyed),
            y, Color::White);
        y += lineHeight;
        m_textDrawer->drawTextCentered(
            std::format("Damage dealt: {}", playerStats.damageDealt),
            y, Color::White);
        y += lineHeight * 2;
    }

    m_textDrawer->drawTextCentered("CLICK TO CONTINUE", m_settings->getScreenH() - lineHeight - 8, Color::White);
    m_interface->drawCursor();
}

void cScoringState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    // Any left click continues, matching the WINNING screen's click-anywhere behavior the
    // player just came from.
    if (event.eventType == MOUSE_LEFT_BUTTON_CLICKED) {
        continueToWinBrief();
    }
}

void cScoringState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}

eGameStateType cScoringState::getType()
{
    return GAMESTATE_SCORING;
}
