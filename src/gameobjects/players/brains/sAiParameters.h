/**
 * @file sAiParameters.h
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

#pragma once

#include "utils/RNG.hpp"

namespace brains {

/**
 * An amount of "base + RNG::rnd(randomExtra)", ie a value in [base, base + randomExtra - 1].
 */
struct s_AiRandomAmount {
    int base;
    int randomExtra;

    int roll() const {
        return base + RNG::rnd(randomExtra);
    }
};

/**
 * Values used by both the campaign and the skirmish brain.
 *
 * Timers count brain ticks: a brain thinks every 100 ms, so 10 ticks is 1 second.
 */
struct s_AiSharedParameters {
    // ticks to rest between two think cycles
    int restTime = 10;
    // ticks to rest when the "no AI rest" setting is on
    int restTimeWhenNoAiRest = 10;
    // an enemy unit discovered within this distance (in cells) of the base center triggers a response
    int baseDefenseRadius = 20;

    // a damaged or decaying structure is repaired only when the player has these credits
    int repairStructureMinCredits = 50;
    // ... and its hit points are below this fraction of its max hit points
    double repairStructureBelowHpFraction = 0.75;

    // max number of units sent to respond to a threat
    s_AiRandomAmount respondersWhenHarvesterAttacked = {2, 4};
    s_AiRandomAmount respondersWhenStructureAttacked = {2, 4};
    s_AiRandomAmount respondersWhenEnemyNearBase = {2, 4};

    // initial delay of a superweapon mission (deathhand, saboteur, fremen) is RNG::rnd(this)
    int superWeaponMissionDelayRandom = 10;
};

/**
 * All hard-coded AI tuning values in one place. Each brain owns one (see cPlayerBrain::getAiParameters());
 * missions read the values of the brain they belong to.
 */
struct s_AiParameters {
    s_AiSharedParameters shared;
};

}
