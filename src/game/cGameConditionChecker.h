#pragma once

#include <cstdint>

class cGame;
class cPlayer;

class cGameConditionChecker {
public:
    cGameConditionChecker(cGame* game);
    bool isMissionWon() const;
    bool isMissionFailed() const;
    void missionInit();
    void setWinFlags(int value);
    void setLoseFlags(int value);

private:
    [[nodiscard]] bool hasGameOverConditionHarvestForSpiceQuota() const;

    [[nodiscard]] bool hasGameOverConditionPlayerHasNoBuildings() const;

    [[nodiscard]] bool hasWinConditionHumanMustLoseAllBuildings() const;

    [[nodiscard]] bool hasWinConditionAIShouldLoseEverything() const;

    [[nodiscard]] bool allEnemyAIPlayersAreDestroyed() const;

    [[nodiscard]] bool hasGameOverConditionAIHasNoBuildings() const;
    
    cGame* m_game;
    // win/lose flags
    int8_t m_winFlags, m_loseFlags;
};