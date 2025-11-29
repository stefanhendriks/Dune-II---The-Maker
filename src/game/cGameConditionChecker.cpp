#include "game/cGameConditionChecker.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "player/cPlayer.h"

cGameConditionChecker::cGameConditionChecker(cGame* game) : m_game(game)
{    
    m_winFlags = 0;
    m_loseFlags = 0;
}

void cGameConditionChecker::missionInit()
{
    m_winFlags = 0;
    m_loseFlags = 0;
}

void cGameConditionChecker::setWinFlags(int value)
{
    if (game.isDebugMode()) {
        logbook(std::format("Changing m_winFlags from {} to {}", m_winFlags, value));
    }
    m_winFlags = value;
}

void cGameConditionChecker::setLoseFlags(int value)
{
    if (game.isDebugMode()) {
        logbook(std::format("Changing m_loseFlags from {} to {}", m_loseFlags, value));
    }
    m_loseFlags = value;
}

bool cGameConditionChecker::isMissionWon() const
{
    cPlayer &humanPlayer = players[HUMAN];
    if (hasGameOverConditionHarvestForSpiceQuota()) {
        if (humanPlayer.hasMetQuota()) {
            return true;
        }
    }

    if (hasGameOverConditionPlayerHasNoBuildings()) {
        if (hasWinConditionHumanMustLoseAllBuildings()) {
            if (!humanPlayer.isAlive()) {
                return true;
            }
        }

        if (hasWinConditionAIShouldLoseEverything()) {
            if (allEnemyAIPlayersAreDestroyed()) {
                return true;
            }
        }
    }
    else if (hasGameOverConditionAIHasNoBuildings()) {
        if (hasWinConditionAIShouldLoseEverything()) {
            if (allEnemyAIPlayersAreDestroyed()) {
                return true;
            }
        }
    }
    return false;
}

bool cGameConditionChecker::isMissionFailed() const
{
    if (hasGameOverConditionHarvestForSpiceQuota()) {
        // check for non-human players if they have met spice quota, if so, they win (and thus human player loses)
        for (int i = 1; i < MAX_PLAYERS; i++) {
            cPlayer &player = players[i];
            if (player.isAlive() && player.hasMetQuota()) {
                return true;
            }
        }
    }

    if (hasGameOverConditionPlayerHasNoBuildings()) {
        cPlayer &humanPlayer = players[HUMAN];
        if (!humanPlayer.isAlive()) {
            /**
             * If any of the bits in “LoseFlags” is set and the corresponding condition holds true
             * the player has won (and the computer has lost)
             */
            if (hasWinConditionHumanMustLoseAllBuildings()) {
                // this means, if any other player has lost all, that player wins, this is not (yet)
                // supported. Mainly because we can't distinguish yet between 'active' and non-active players
                // since we have a fixed list of players.

                // so for now just do this:
                return false; // it is meant to win the game by drawStateLosing all...
            }
            else {
                return true; // nope, it should lose mission now
            }
        }
    }

    return false;
}



bool cGameConditionChecker::allEnemyAIPlayersAreDestroyed() const
{
    cPlayer &humanPlayer = players[HUMAN];
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (i == HUMAN || i == AI_WORM || i == AI_CPU5) continue; // do not evaluate these players
        cPlayer *player = &players[i];
        if (!player->isAlive()) continue;
        if (humanPlayer.isSameTeamAs(player)) continue; // skip allied AI players
        return false;
    }
    return true;
}


/**
 * Remember, the 'm_winFlags' are used to determine when the game is "over". Only then the lose flags are evaluated
 * and used to determine who has won. (According to the SCEN specification).
 * @return
 */
bool cGameConditionChecker::hasWinConditionHumanMustLoseAllBuildings() const
{
    return (m_loseFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0;
}

bool cGameConditionChecker::hasWinConditionAIShouldLoseEverything() const
{
    return (m_loseFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0;
}

/**
 * Game over condition: player has no buildings (WinFlags)
 * @return
 */
bool cGameConditionChecker::hasGameOverConditionPlayerHasNoBuildings() const
{
    return (m_winFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0;
}

/**
 * Game over condition: Spice quota reached by player
 * @return
 */
bool cGameConditionChecker::hasGameOverConditionHarvestForSpiceQuota() const
{
    return (m_winFlags & WINLOSEFLAGS_QUOTA) != 0;
}

bool cGameConditionChecker::hasGameOverConditionAIHasNoBuildings() const
{
    return (m_winFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0;
}