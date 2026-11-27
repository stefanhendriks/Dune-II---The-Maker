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

#include "include/definitions.h"
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
    bool operator==(const s_AiRandomAmount &) const = default;
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

    // a damaged ground vehicle is sent to a Repair Facility when its hit points drop below a fraction of its max:
    // ... this one when no friendly unit is fighting near it (ambushed)
    double repairRecallAmbushedBelowHpFraction = 0.35;
    // ... this one when a friendly unit is fighting near it (sieging), so it soaks up damage a bit longer
    double repairRecallSiegingBelowHpFraction = 0.20;
    // a friendly unit is "near" when it is within this many cells (horizontally and vertically) of the unit
    int repairRecallSiegeRadius = 3;

    // a unit in a Repair Facility is ejected to fight the attacker (when it can hit it) when
    // ... the Repair Facility or the Construction Yard is attacked
    bool repairEjectWhenThreatened = true;
    // ... the player has no credits left (repair stalled) and the Construction Yard, Repair Facility or Refinery is attacked
    bool repairEjectWhenStalled = false;
    // an ejected unit that is idle this many ticks after its fight (60 seconds) goes back to repair
    int repairEjectReturnAfterIdleTicks = 600;

    // max number of units sent to respond to a threat
    s_AiRandomAmount respondersWhenHarvesterAttacked = {2, 4};
    s_AiRandomAmount respondersWhenStructureAttacked = {2, 4};
    s_AiRandomAmount respondersWhenEnemyNearBase = {2, 4};

    // initial delay of a superweapon mission (deathhand, saboteur, fremen) is RNG::rnd(this)
    int superWeaponMissionDelayRandom = 10;

    double repairRecallBelowHpFraction(bool sieging) const {
        return sieging ? repairRecallSiegingBelowHpFraction : repairRecallAmbushedBelowHpFraction;
    }

    // repairStalled: the player has no credits left, so repairing has stopped
    bool ejectRepairingUnitsWhenAttacked(int attackedStructureType, bool repairStalled) const {
        if (repairEjectWhenThreatened && (attackedStructureType == REPAIR || attackedStructureType == CONSTYARD)) {
            return true;
        }
        return repairEjectWhenStalled && repairStalled &&
               (attackedStructureType == CONSTYARD || attackedStructureType == REPAIR || attackedStructureType == REFINERY);
    }
    bool operator==(const s_AiSharedParameters &) const = default;
};

/**
 * Values of the skirmish brain (cPlayerBrainSkirmish). Chances are percentages (compared with RNG::rnd(100)).
 */
struct s_AiSkirmishParameters {
    // --- personality, rolled once when the brain is created ---
    // initial rest is RNG::rnd(this) ticks
    int initialRestRandom = 25;
    // consider an additional refinery after this many ticks (30 seconds), times the multiplier below
    int considerAdditionalRefineryAfter = 300;
    s_AiRandomAmount considerAdditionalRefineryMultiplier = {1, 3};
    // may build additional units after this many ticks (3 minutes), plus the offset below
    int mayBuildAdditionalUnitsAfter = 1800;
    s_AiRandomAmount mayBuildAdditionalUnitsOffset = {-400, 800};
    int eagernessToBuildRandomUnits = 15;
    s_AiRandomAmount idealRefineries = {2, 2};
    // ideal harvesters = ideal refineries * harvestersPerRefinery + RNG::rnd(idealHarvestersRandomExtra)
    int harvestersPerRefinery = 2;
    int idealHarvestersRandomExtra = 2;
    s_AiRandomAmount idealCarryalls = {2, 2};
    s_AiRandomAmount idealOrnithopters = {0, 5};
    int initialEconomyScore = 50;

    // --- economy ---
    // the economy score is credits / idealCredits as a percentage (capped at 100)
    int idealCredits = 1500;
    // the economy score moves towards its target by (distance / this) per evaluation
    int economyScoreStepDivisor = 5;
    // economy state thresholds on the economy score
    int economyBadBelow = 15;
    int economyImproveBelow = 30;
    int economyGoodAbove = 70;
    // start a unit upgrade only with these credits (and a normal or good economy)
    int unitUpgradeMinCredits = 500;

    // --- units built without a mission (after mayBuildAdditionalUnitsAfter) ---
    // with a bad economy, keep at least this many harvesters and carryalls
    int badEconomyMinHarvesters = 2;
    int badEconomyMinCarryalls = 1;
    // chance per unit kind = eagernessToBuildRandomUnits + this bonus
    int additionalUnitsChanceBonusGoodEconomy = 15;
    int additionalUnitsChanceBonusOtherEconomy = 5;
    // cooldown is increased by RNG::rnd(this) ticks per attempt
    int additionalUnitsCooldownRandom = 15;
    int additionalUnitsMaxSiegeTanks = 8;
    int additionalUnitsMaxLaunchers = 6;
    int additionalUnitsMaxQuads = 5;
    int additionalUnitsMaxTanks = 4;

    // --- units ordered from the Starport ---
    // the brain considers a Starport order only with at least these credits
    int starportOrderMinCredits = 1500;
    // ... and then places one with this chance
    int starportOrderChance = 35;

    // --- missions ---
    // initial delay of a (non superweapon) mission is RNG::rnd(this)
    int missionDelayRandom = 15;
    int harvestersPerEconomyMission = 1;
    int carryallsPerEconomyMission = 1;
    int scoutsPerScoutMission = 1;
    int guardForceQuads = 1;
    int guardForceTanks = 2;
    s_AiRandomAmount airAttackOrnithopters = {1, 3};

    // --- ground attack mission composition ---
    int groundAttackSmallChance = 15;
    int groundAttackNormalChance = 50;
    int groundAttackBigChance = 75;
    // chances are multiplied when the player has more than these credits
    int groundAttackRichCredits = 2500;
    double groundAttackRichChanceMultiplier = 1.5;
    int groundAttackWealthyCredits = 1500;
    double groundAttackWealthyChanceMultiplier = 1.25;
    s_AiRandomAmount groundAttackSiegeTanks = {1, 4};
    s_AiRandomAmount groundAttackTroopers = {1, 2};
    s_AiRandomAmount groundAttackInfantry = {1, 2};
    s_AiRandomAmount groundAttackLaunchers = {1, 3};
    s_AiRandomAmount groundAttackTanks = {1, 4};
    s_AiRandomAmount groundAttackScouts = {1, 2};
    s_AiRandomAmount groundAttackQuads = {1, 3};
    s_AiRandomAmount groundAttackSpecialUnits = {1, 2};

    // --- structure to build next ---
    // with an improving or bad economy, build refineries up to this count first
    int lowEconomyRefineries = 2;
    int additionalRefineryChance = 50;
    int siloWhenStorageAlmostFullChance = 15;
    // chance to pick the radar before the heavy factory
    int radarBeforeHeavyFactoryChance = 50;
    int earlyInfantryStructureChance = 15;
    int earlyPalaceChance = 5;
    int earlyTurretChance = 35;
    int earlyTurretsMax = 2;
    int earlyHighTechChance = 40;
    // chance = base - (count already built * penalty)
    int rocketTurretBaseChance = 60;
    int rocketTurretChancePenaltyPerTurret = 10;
    int turretBaseChance = 40;
    int turretChancePenaltyPerTurret = 5;
    // chance to build a palace, tried at three points late in the build order
    int latePalaceChance = 15;
    int rocketTurretsMax = 8;
    int additionalHeavyFactoryChance = 30;
    // max heavy factories, light factories, WORs and barracks with a normal economy
    int productionStructuresMax = 2;

    // --- end game (no construction yard left) ---
    // chance to attack a unit instead of a structure
    int endGameAttackUnitChance = 50;
    // chance to stop at the current candidate while looking for a target
    int endGamePickUnitChance = 5;
    int endGamePickStructureChance = 10;

    // a unit in the way of a new structure moves to a random cell within this distance
    int moveAsideDistance = 3;

    // roll: a value in [0, 100), ie RNG::rnd(100)
    bool orderFromStarport(int credits, int roll) const {
        return credits >= starportOrderMinCredits && roll < starportOrderChance;
    }
    bool operator==(const s_AiSkirmishParameters &) const = default;
};

/**
 * Values of the campaign brain (cPlayerBrainCampaign), outside the per-level mission scripts
 * (produceLevelNMissions()).
 */
struct s_AiCampaignParameters {
    // ticks to rest before the first think cycle (4 minutes)
    int initialRestTime = (10 * 60) * 4;
    // initial delay of the scout mission is RNG::rnd(this)
    int scoutMissionDelayRandom = 5;
    int scoutsPerScoutMission = 1;
    bool operator==(const s_AiCampaignParameters &) const = default;
};

/**
 * Values of the brain missions (cPlayerBrainMission and its kinds). Chances are percentages.
 * Mission timers count mission ticks (missions think every 5 ms).
 */
struct s_AiMissionParameters {
    // --- waiting for a build order (cPlayerBrainMission) ---
    // ticks to wait when the build time is unknown; a wait at or below this is replaced by the fallback below
    int buildOrderDefaultWaitTicks = 15;
    // wait this factor times the item's build time before giving up
    float buildOrderWaitBuildTimeFactor = 1.25f;
    int buildOrderFallbackWaitMs = 1000 * 60 * 5;
    // extra wait on top, so the brain has time to process the build order
    int buildOrderExtraWaitMs = 1000 * 60;
    // a mission without units waits this long again after gathering resources
    int missionWithoutUnitsWaitMs = 1000 * 60 * 10;

    // --- attack ---
    // chance to look for a unit target first (else a structure)
    int attackTargetUnitFirstChance = 50;
    // chance to stop at the current candidate while looking for a target
    int attackPickStructureChance = 10;
    int attackPickUnitChance = 5;

    // --- deathhand ---
    int deathHandPickStructureChance = 25;
    int deathHandPickUnitChance = 5;

    // --- saboteur ---
    int saboteurPickStructureChance = 25;

    // --- explore ---
    // explore targets keep this distance (in cells) from the map border
    int exploreBorderDistance = 2;
    // a new target is picked when an idle unit is within this distance of the target
    int exploreArrivedDistanceIdle = 4;
    // ... or a moving unit is within this distance
    int exploreArrivedDistanceMoving = 2;
    bool operator==(const s_AiMissionParameters &) const = default;
};

/**
 * Values of the sandworm brain (cPlayerBrainSandworm).
 */
struct s_AiSandwormParameters {
    // the sandworms are moved every this many brain ticks
    int thinkInterval = 5;
    bool operator==(const s_AiSandwormParameters &) const = default;
};

/**
 * Values of the brain that controls the Fremen called in with the superweapon (cPlayerBrainFremenSuperWeapon).
 * A roll (RNG::rnd(100)) above a threshold triggers the action.
 */
struct s_AiFremenSuperWeaponParameters {
    // an enemy player is skipped when the roll is above this
    int skipPlayerAbove = 70;
    // the current unit / structure target is kept when the roll is above this
    int keepUnitTargetAbove = 30;
    int keepStructureTargetAbove = 30;
    bool operator==(const s_AiFremenSuperWeaponParameters &) const = default;
};

/**
 * All hard-coded AI tuning values in one place. Each brain owns one (see cPlayerBrain::getAiParameters());
 * missions read the values of the brain they belong to.
 */
struct s_AiParameters {
    s_AiSharedParameters shared;
    s_AiSkirmishParameters skirmish;
    s_AiCampaignParameters campaign;
    s_AiMissionParameters missions;
    s_AiSandwormParameters sandworm;
    s_AiFremenSuperWeaponParameters fremenSuperWeapon;
    bool operator==(const s_AiParameters &) const = default;
};

}
