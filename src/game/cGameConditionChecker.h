#pragma once

#include <cstdint>

class cGameObjectContext;
class cPlayer;

class cGameConditionChecker {
public:
    cGameConditionChecker(cGameObjectContext* objects);
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
    
    cGameObjectContext* m_objects;
    // win/lose flags
    int8_t m_winFlags, m_loseFlags;
};