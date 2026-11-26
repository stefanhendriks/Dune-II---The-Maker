/**
 * @file AiParametersLoader.cpp
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

#include "context/AiParametersLoader.hpp"

#include "gameobjects/players/brains/sAiParameters.h"
#include "utils/Log.h"
#include "utils/cIniFile.h"

#include <filesystem>

namespace {

cSection sectionOrEmpty(const cIniFile& file, bool loaded, const std::string& name)
{
    return (loaded && file.hasSection(name)) ? file.getSection(name) : cSection();
}

// a random amount is two keys: <key>Base and <key>Random
void readRandomAmount(const cSection& section, const std::string& key, brains::s_AiRandomAmount& amount)
{
    amount.base = section.getInt(key + "Base", amount.base);
    amount.randomExtra = section.getInt(key + "Random", amount.randomExtra);
}

} // namespace

bool AiParametersLoader::load(const std::string& filename, brains::s_AiParameters& parameters) const
{
    cIniFile file;
    bool loaded = false;
    if (std::filesystem::exists(std::filesystem::path(std::u8string(filename.begin(), filename.end())))) {
        loaded = file.load(filename);
    }
    if (!loaded) {
        Logger::info(COMP_GAMERULES, "AiParametersLoader::load", "Could not open file [{}], using default AI values", filename);
    }
    // Without a file, a section or a key, the value already in the struct is used
    {
        const cSection section = sectionOrEmpty(file, loaded, "SHARED");
        auto &p = parameters.shared;
        p.restTime = section.getInt("RestTime", p.restTime);
        p.restTimeWhenNoAiRest = section.getInt("RestTimeWhenNoAiRest", p.restTimeWhenNoAiRest);
        p.baseDefenseRadius = section.getInt("BaseDefenseRadius", p.baseDefenseRadius);
        p.repairStructureMinCredits = section.getInt("RepairStructureMinCredits", p.repairStructureMinCredits);
        p.repairStructureBelowHpFraction = section.getDouble("RepairStructureBelowHpFraction", p.repairStructureBelowHpFraction);
        p.repairRecallAmbushedBelowHpFraction = section.getDouble("RepairRecallAmbushedBelowHpFraction", p.repairRecallAmbushedBelowHpFraction);
        p.repairRecallSiegingBelowHpFraction = section.getDouble("RepairRecallSiegingBelowHpFraction", p.repairRecallSiegingBelowHpFraction);
        p.repairRecallSiegeRadius = section.getInt("RepairRecallSiegeRadius", p.repairRecallSiegeRadius);
        p.repairEjectWhenThreatened = section.getBoolean("RepairEjectWhenThreatened", p.repairEjectWhenThreatened);
        p.repairEjectWhenStalled = section.getBoolean("RepairEjectWhenStalled", p.repairEjectWhenStalled);
        p.repairEjectReturnAfterIdleTicks = section.getInt("RepairEjectReturnAfterIdleTicks", p.repairEjectReturnAfterIdleTicks);
        readRandomAmount(section, "RespondersWhenHarvesterAttacked", p.respondersWhenHarvesterAttacked);
        readRandomAmount(section, "RespondersWhenStructureAttacked", p.respondersWhenStructureAttacked);
        readRandomAmount(section, "RespondersWhenEnemyNearBase", p.respondersWhenEnemyNearBase);
        p.superWeaponMissionDelayRandom = section.getInt("SuperWeaponMissionDelayRandom", p.superWeaponMissionDelayRandom);
    }
    {
        const cSection section = sectionOrEmpty(file, loaded, "SKIRMISH");
        auto &p = parameters.skirmish;
        p.initialRestRandom = section.getInt("InitialRestRandom", p.initialRestRandom);
        p.considerAdditionalRefineryAfter = section.getInt("ConsiderAdditionalRefineryAfter", p.considerAdditionalRefineryAfter);
        readRandomAmount(section, "ConsiderAdditionalRefineryMultiplier", p.considerAdditionalRefineryMultiplier);
        p.mayBuildAdditionalUnitsAfter = section.getInt("MayBuildAdditionalUnitsAfter", p.mayBuildAdditionalUnitsAfter);
        readRandomAmount(section, "MayBuildAdditionalUnitsOffset", p.mayBuildAdditionalUnitsOffset);
        p.eagernessToBuildRandomUnits = section.getInt("EagernessToBuildRandomUnits", p.eagernessToBuildRandomUnits);
        readRandomAmount(section, "IdealRefineries", p.idealRefineries);
        p.harvestersPerRefinery = section.getInt("HarvestersPerRefinery", p.harvestersPerRefinery);
        p.idealHarvestersRandomExtra = section.getInt("IdealHarvestersRandomExtra", p.idealHarvestersRandomExtra);
        readRandomAmount(section, "IdealCarryalls", p.idealCarryalls);
        readRandomAmount(section, "IdealOrnithopters", p.idealOrnithopters);
        p.initialEconomyScore = section.getInt("InitialEconomyScore", p.initialEconomyScore);
        p.idealCredits = section.getInt("IdealCredits", p.idealCredits);
        p.economyScoreStepDivisor = section.getInt("EconomyScoreStepDivisor", p.economyScoreStepDivisor);
        p.economyBadBelow = section.getInt("EconomyBadBelow", p.economyBadBelow);
        p.economyImproveBelow = section.getInt("EconomyImproveBelow", p.economyImproveBelow);
        p.economyGoodAbove = section.getInt("EconomyGoodAbove", p.economyGoodAbove);
        p.unitUpgradeMinCredits = section.getInt("UnitUpgradeMinCredits", p.unitUpgradeMinCredits);
        p.badEconomyMinHarvesters = section.getInt("BadEconomyMinHarvesters", p.badEconomyMinHarvesters);
        p.badEconomyMinCarryalls = section.getInt("BadEconomyMinCarryalls", p.badEconomyMinCarryalls);
        p.additionalUnitsChanceBonusGoodEconomy = section.getInt("AdditionalUnitsChanceBonusGoodEconomy", p.additionalUnitsChanceBonusGoodEconomy);
        p.additionalUnitsChanceBonusOtherEconomy = section.getInt("AdditionalUnitsChanceBonusOtherEconomy", p.additionalUnitsChanceBonusOtherEconomy);
        p.additionalUnitsCooldownRandom = section.getInt("AdditionalUnitsCooldownRandom", p.additionalUnitsCooldownRandom);
        p.additionalUnitsMaxSiegeTanks = section.getInt("AdditionalUnitsMaxSiegeTanks", p.additionalUnitsMaxSiegeTanks);
        p.additionalUnitsMaxLaunchers = section.getInt("AdditionalUnitsMaxLaunchers", p.additionalUnitsMaxLaunchers);
        p.additionalUnitsMaxQuads = section.getInt("AdditionalUnitsMaxQuads", p.additionalUnitsMaxQuads);
        p.additionalUnitsMaxTanks = section.getInt("AdditionalUnitsMaxTanks", p.additionalUnitsMaxTanks);
        p.missionDelayRandom = section.getInt("MissionDelayRandom", p.missionDelayRandom);
        p.harvestersPerEconomyMission = section.getInt("HarvestersPerEconomyMission", p.harvestersPerEconomyMission);
        p.carryallsPerEconomyMission = section.getInt("CarryallsPerEconomyMission", p.carryallsPerEconomyMission);
        p.scoutsPerScoutMission = section.getInt("ScoutsPerScoutMission", p.scoutsPerScoutMission);
        p.guardForceQuads = section.getInt("GuardForceQuads", p.guardForceQuads);
        p.guardForceTanks = section.getInt("GuardForceTanks", p.guardForceTanks);
        readRandomAmount(section, "AirAttackOrnithopters", p.airAttackOrnithopters);
        p.groundAttackSmallChance = section.getInt("GroundAttackSmallChance", p.groundAttackSmallChance);
        p.groundAttackNormalChance = section.getInt("GroundAttackNormalChance", p.groundAttackNormalChance);
        p.groundAttackBigChance = section.getInt("GroundAttackBigChance", p.groundAttackBigChance);
        p.groundAttackRichCredits = section.getInt("GroundAttackRichCredits", p.groundAttackRichCredits);
        p.groundAttackRichChanceMultiplier = section.getDouble("GroundAttackRichChanceMultiplier", p.groundAttackRichChanceMultiplier);
        p.groundAttackWealthyCredits = section.getInt("GroundAttackWealthyCredits", p.groundAttackWealthyCredits);
        p.groundAttackWealthyChanceMultiplier = section.getDouble("GroundAttackWealthyChanceMultiplier", p.groundAttackWealthyChanceMultiplier);
        readRandomAmount(section, "GroundAttackSiegeTanks", p.groundAttackSiegeTanks);
        readRandomAmount(section, "GroundAttackTroopers", p.groundAttackTroopers);
        readRandomAmount(section, "GroundAttackInfantry", p.groundAttackInfantry);
        readRandomAmount(section, "GroundAttackLaunchers", p.groundAttackLaunchers);
        readRandomAmount(section, "GroundAttackTanks", p.groundAttackTanks);
        readRandomAmount(section, "GroundAttackScouts", p.groundAttackScouts);
        readRandomAmount(section, "GroundAttackQuads", p.groundAttackQuads);
        readRandomAmount(section, "GroundAttackSpecialUnits", p.groundAttackSpecialUnits);
        p.lowEconomyRefineries = section.getInt("LowEconomyRefineries", p.lowEconomyRefineries);
        p.additionalRefineryChance = section.getInt("AdditionalRefineryChance", p.additionalRefineryChance);
        p.siloWhenStorageAlmostFullChance = section.getInt("SiloWhenStorageAlmostFullChance", p.siloWhenStorageAlmostFullChance);
        p.radarBeforeHeavyFactoryChance = section.getInt("RadarBeforeHeavyFactoryChance", p.radarBeforeHeavyFactoryChance);
        p.earlyInfantryStructureChance = section.getInt("EarlyInfantryStructureChance", p.earlyInfantryStructureChance);
        p.earlyPalaceChance = section.getInt("EarlyPalaceChance", p.earlyPalaceChance);
        p.earlyTurretChance = section.getInt("EarlyTurretChance", p.earlyTurretChance);
        p.earlyTurretsMax = section.getInt("EarlyTurretsMax", p.earlyTurretsMax);
        p.earlyHighTechChance = section.getInt("EarlyHighTechChance", p.earlyHighTechChance);
        p.rocketTurretBaseChance = section.getInt("RocketTurretBaseChance", p.rocketTurretBaseChance);
        p.rocketTurretChancePenaltyPerTurret = section.getInt("RocketTurretChancePenaltyPerTurret", p.rocketTurretChancePenaltyPerTurret);
        p.turretBaseChance = section.getInt("TurretBaseChance", p.turretBaseChance);
        p.turretChancePenaltyPerTurret = section.getInt("TurretChancePenaltyPerTurret", p.turretChancePenaltyPerTurret);
        p.latePalaceChance = section.getInt("LatePalaceChance", p.latePalaceChance);
        p.rocketTurretsMax = section.getInt("RocketTurretsMax", p.rocketTurretsMax);
        p.additionalHeavyFactoryChance = section.getInt("AdditionalHeavyFactoryChance", p.additionalHeavyFactoryChance);
        p.productionStructuresMax = section.getInt("ProductionStructuresMax", p.productionStructuresMax);
        p.endGameAttackUnitChance = section.getInt("EndGameAttackUnitChance", p.endGameAttackUnitChance);
        p.endGamePickUnitChance = section.getInt("EndGamePickUnitChance", p.endGamePickUnitChance);
        p.endGamePickStructureChance = section.getInt("EndGamePickStructureChance", p.endGamePickStructureChance);
        p.moveAsideDistance = section.getInt("MoveAsideDistance", p.moveAsideDistance);
    }
    {
        const cSection section = sectionOrEmpty(file, loaded, "CAMPAIGN");
        auto &p = parameters.campaign;
        p.initialRestTime = section.getInt("InitialRestTime", p.initialRestTime);
        p.scoutMissionDelayRandom = section.getInt("ScoutMissionDelayRandom", p.scoutMissionDelayRandom);
        p.scoutsPerScoutMission = section.getInt("ScoutsPerScoutMission", p.scoutsPerScoutMission);
    }
    {
        const cSection section = sectionOrEmpty(file, loaded, "MISSIONS");
        auto &p = parameters.missions;
        p.buildOrderDefaultWaitTicks = section.getInt("BuildOrderDefaultWaitTicks", p.buildOrderDefaultWaitTicks);
        p.buildOrderWaitBuildTimeFactor = static_cast<float>(section.getDouble("BuildOrderWaitBuildTimeFactor", p.buildOrderWaitBuildTimeFactor));
        p.buildOrderFallbackWaitMs = section.getInt("BuildOrderFallbackWaitMs", p.buildOrderFallbackWaitMs);
        p.buildOrderExtraWaitMs = section.getInt("BuildOrderExtraWaitMs", p.buildOrderExtraWaitMs);
        p.missionWithoutUnitsWaitMs = section.getInt("MissionWithoutUnitsWaitMs", p.missionWithoutUnitsWaitMs);
        p.attackTargetUnitFirstChance = section.getInt("AttackTargetUnitFirstChance", p.attackTargetUnitFirstChance);
        p.attackPickStructureChance = section.getInt("AttackPickStructureChance", p.attackPickStructureChance);
        p.attackPickUnitChance = section.getInt("AttackPickUnitChance", p.attackPickUnitChance);
        p.deathHandPickStructureChance = section.getInt("DeathHandPickStructureChance", p.deathHandPickStructureChance);
        p.deathHandPickUnitChance = section.getInt("DeathHandPickUnitChance", p.deathHandPickUnitChance);
        p.saboteurPickStructureChance = section.getInt("SaboteurPickStructureChance", p.saboteurPickStructureChance);
        p.exploreBorderDistance = section.getInt("ExploreBorderDistance", p.exploreBorderDistance);
        p.exploreArrivedDistanceIdle = section.getInt("ExploreArrivedDistanceIdle", p.exploreArrivedDistanceIdle);
        p.exploreArrivedDistanceMoving = section.getInt("ExploreArrivedDistanceMoving", p.exploreArrivedDistanceMoving);
    }
    {
        const cSection section = sectionOrEmpty(file, loaded, "SANDWORM");
        auto &p = parameters.sandworm;
        p.thinkInterval = section.getInt("ThinkInterval", p.thinkInterval);
    }
    {
        const cSection section = sectionOrEmpty(file, loaded, "FREMEN_SUPERWEAPON");
        auto &p = parameters.fremenSuperWeapon;
        p.skipPlayerAbove = section.getInt("SkipPlayerAbove", p.skipPlayerAbove);
        p.keepUnitTargetAbove = section.getInt("KeepUnitTargetAbove", p.keepUnitTargetAbove);
        p.keepStructureTargetAbove = section.getInt("KeepStructureTargetAbove", p.keepStructureTargetAbove);
    }
    return loaded;
}
