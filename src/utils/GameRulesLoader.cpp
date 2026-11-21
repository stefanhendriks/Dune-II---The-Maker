/**
 * @file GameRulesLoader.cpp
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

#include "utils/GameRulesLoader.hpp"

#include <filesystem>
#include <string>

#include "definitions.h"
#include "context/cInfoContext.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/units/cUnitInfos.h"
#include "include/sGameServices.h"
#include "utils/IniParsing.hpp"
#include "utils/Log.h"
#include "utils/cIniFile.h"
#include "utils/cIniUtils.h"

GameRulesLoader::GameRulesLoader(sGameServices* services)
{
    m_infos = services->info;
}

namespace {

// Each helper overwrites the target only when the key is present, so defaults survive a missing key
void readInt(const cSection& section, const char* key, int& target)
{
    if (section.hasValue(key)) target = ToInt(section.getStringValue(key));
}

void readBool(const cSection& section, const char* key, bool& target)
{
    if (section.hasValue(key)) target = ToBool(section.getStringValue(key));
}

void readFloat(const cSection& section, const char* key, float& target)
{
    if (section.hasValue(key)) target = ToFloat(section.getStringValue(key));
}

bool loadRulesFile(cIniFile& file, const std::string& filename)
{
    Logger::info(COMP_GAMERULES, "GameRulesLoader::installGame", "Opening game settings from : {}", filename);
    if (!file.load(filename)) {
        Logger::error(COMP_GAMERULES, "GameRulesLoader::installGame", "[GameRules] Error, could not open file [{}], its rules are missing and defaults are used", filename);
        return false;
    }
    return true;
}

} // namespace

// Game rules loader: structures.ini and units.ini, in the same directory as the given game.ini
void GameRulesLoader::installGame(const std::string& filename)
{
    const std::filesystem::path directory = std::filesystem::path(filename).parent_path();
    installStructures((directory / "structures.ini").string());
    installUnits((directory / "units.ini").string());
}

// Every section is a structure, named by the section ([WOR])
void GameRulesLoader::installStructures(const std::string& filename)
{
    cIniFile file;
    if (!loadRulesFile(file, filename)) return;

    for (const auto& sectionName : file.getSectionNames()) {
        const int id = cIniUtils::getStructureType(sectionName);
        // getStructureType falls back to 0 for unknown names, which would pour this section's values into the Palace
        if (id < 0 || id >= MAX_STRUCTURETYPES || (id == PALACE && !cIniUtils::caseInsCompare(sectionName, "PALACE"))) {
            Logger::error(COMP_GAMERULES, "GameRulesLoader::installGame", "[{}] Unknown structure section [{}], skipping it (expected e.g. [WOR])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_StructureInfo& info = m_infos->getStructureInfo(id);
        readInt(section, "HitPoints", info.hp);
        readInt(section, "FixPoints", info.fixhp);
        readInt(section, "PowerDrain", info.power_drain);
        readBool(section, "HasConcrete", info.hasConcrete);
        readBool(section, "NeedPowerToWork", info.needPowerToWork);
        readInt(section, "LowPowerIconX", info.lowPowerIconX);
        readInt(section, "LowPowerIconY", info.lowPowerIconY);
        readInt(section, "PowerGive", info.power_give);
        readInt(section, "Cost", info.cost);
        readInt(section, "BuildTime", info.buildTime);
        readBool(section, "CanAttackAirUnits", info.canAttackAirUnits);
        readBool(section, "CanAttackUnits", info.canAttackGroundUnits);
        readInt(section, "Sight", info.sight);
        readInt(section, "Range", info.range);
        readInt(section, "FireRate", info.fireRate);
        readInt(section, "UponDestructionSpawnUnitAmountMin", info.uponDestructionSpawnUnitAmountMin);
        readInt(section, "UponDestructionSpawnUnitAmountMax", info.uponDestructionSpawnUnitAmountMax);
        if (section.hasValue("UponDestructionSpawnUnitType")) {
            info.uponDestructionSpawnUnitType = cIniUtils::getUnitTypeFromString(section.getStringValue("UponDestructionSpawnUnitType"));
        }
    }
    Logger::info(COMP_GAMERULES, "GameRulesLoader::installGame", "[{}] Done", filename);
}

// Every section is a unit, named by the section ([TRIKE])
void GameRulesLoader::installUnits(const std::string& filename)
{
    cIniFile file;
    if (!loadRulesFile(file, filename)) return;

    for (const auto& sectionName : file.getSectionNames()) {
        const int id = cIniUtils::getUnitTypeFromString(sectionName);
        if (id < 0 || id >= MAX_UNITTYPES) {
            Logger::error(COMP_GAMERULES, "GameRulesLoader::installGame", "[{}] Unknown unit section [{}], skipping it (expected e.g. [TRIKE])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_UnitInfo& info = m_infos->getUnitInfo(id);
        readInt(section, "HitPoints", info.hp);
        readInt(section, "Appetite", info.appetite);
        readInt(section, "Cost", info.cost);
        readInt(section, "MoveSpeed", info.speed);
        readInt(section, "TurnSpeed", info.turnspeed);
        readInt(section, "AttackFrequency", info.attack_frequency);
        readInt(section, "NextAttackFrequency", info.next_attack_frequency);
        readInt(section, "Sight", info.sight);
        readInt(section, "Range", info.range);
        readInt(section, "BuildTime", info.buildTime);
        if (section.hasValue("Description")) {
            info.name = section.getStringValue("Description").substr(0, 63); // the old char buffer held at most 63 characters
        }
        readBool(section, "FireTwice", info.fireTwice);
        readBool(section, "IsInfantry", info.infantry);
        readBool(section, "Squishable", info.canBeSquished);
        readBool(section, "CanSquish", info.canBeSquished);
        readBool(section, "squish", info.canBeSquished);
        readBool(section, "FreeRoam", info.free_roam);
        readBool(section, "IsAirborn", info.airborn);
        readInt(section, "MaxCredits", info.credit_capacity);
        readInt(section, "HarvestSpeed", info.harvesting_speed);
        readInt(section, "HarvestAmount", info.harvesting_amount);
        readFloat(section, "SmokeHpFactor", info.smokeHpFactor);
        readBool(section, "CanAttackAirUnits", info.canAttackAirUnits);
        readBool(section, "CanAttackUnits", info.canAttackUnits);
        if (section.hasValue("Producer")) {
            info.structureTypeItLeavesFrom = cIniUtils::getStructureType(section.getStringValue("Producer"));
        }
    }
    Logger::info(COMP_GAMERULES, "GameRulesLoader::installGame", "[{}] Done", filename);
}
