/**
 * @file cGameConditionChecker.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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