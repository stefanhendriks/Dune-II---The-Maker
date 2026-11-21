/**
 * @file StructureInfosLoader.cpp
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

#include "context/StructureInfosLoader.hpp"

#include <utility>

#include "definitions.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "utils/IniParsing.hpp"
#include "utils/Log.h"
#include "utils/cIniFile.h"
#include "utils/cIniUtils.h"

namespace {

// Used when a structure has no (valid) bitmap: a windtrap
constexpr std::string_view FALLBACK_BITMAP = "BUILD_WINDTRAP";

void readInt(const cSection& section, const char* key, int& target)
{
    if (section.hasValue(key)) target = ToInt(section.getStringValue(key));
}

void readBool(const cSection& section, const char* key, bool& target)
{
    if (section.hasValue(key)) target = ToBool(section.getStringValue(key));
}

// "big,53,37" or "small,25,3": flag size, then x and y relative to the top-left of the structure
bool parseFlag(const std::string& value, s_FlagInfo& flag)
{
    const auto first = value.find(',');
    if (first == std::string::npos) return false;
    const auto second = value.find(',', first + 1);
    if (second == std::string::npos) return false;

    flag.big = cIniUtils::caseInsCompare(value.substr(0, first), "big");
    flag.relX = ToInt(value.substr(first + 1, second - first - 1));
    flag.relY = ToInt(value.substr(second + 1));
    return true;
}

std::string flagKey(int number)
{
    return number == 1 ? "Flag" : "Flag" + std::to_string(number);
}

} // namespace

StructureInfosLoader::StructureInfosLoader(SurfaceLookup surfaceLookup, TextureLookup textureLookup, IconLookup iconLookup)
    : m_surfaceLookup(std::move(surfaceLookup)), m_textureLookup(std::move(textureLookup)), m_iconLookup(std::move(iconLookup))
{
}

bool StructureInfosLoader::load(const std::string& filename, cStructureInfos& structureInfos) const
{
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        s_StructureInfo& info = structureInfos[static_cast<std::size_t>(i)];
        info = s_StructureInfo{};
        info.bmp = m_surfaceLookup(FALLBACK_BITMAP); // in case an invalid bitmap, we are a windtrap
        info.sight = 1;
        info.bmp_width = 32 * 2;
        info.bmp_height = 32 * 2;
        info.hp = 1; // low health
        info.fixhp = -1; // no fixing hp yet
        info.fadecol = -1;
        info.fademax = -1;
        info.icon = TextureID::invalid;
        info.list = -1; // no list attached
        info.hasConcrete = true;
        info.uponDestructionSpawnUnitType = -1;
        info.name = "Unknown";
    }

    Logger::info(COMP_GAMERULES, "StructureInfosLoader::load", "Opening structure rules from : {}", filename);
    cIniFile file;
    if (!file.load(filename)) {
        Logger::error(COMP_GAMERULES, "StructureInfosLoader::load", "Could not open file [{}], no structures are available", filename);
        return false;
    }

    for (const auto& sectionName : file.getSectionNames()) {
        const int id = cIniUtils::getStructureType(sectionName);
        // getStructureType falls back to 0 for unknown names, which would pour this section's values into the Palace
        if (id < 0 || id >= MAX_STRUCTURETYPES || (id == PALACE && !cIniUtils::caseInsCompare(sectionName, "PALACE"))) {
            Logger::error(COMP_GAMERULES, "StructureInfosLoader::load", "[{}] Unknown structure section [{}], skipping it (expected e.g. [WOR])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_StructureInfo& info = structureInfos[static_cast<std::size_t>(id)];
        info.configured = true;
        if (section.hasValue("Name")) {
            info.name = section.getStringValue("Name");
        }
        readInt(section, "Width", info.bmp_width);
        readInt(section, "Height", info.bmp_height);
        readInt(section, "FadeColor", info.fadecol);
        readInt(section, "FadeMax", info.fademax);
        readBool(section, "Queuable", info.queuable);
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

        // Flag, Flag2, Flag3, ... (a section can only hold a key once)
        for (int number = 1; section.hasValue(flagKey(number)); number++) {
            s_FlagInfo flag;
            if (parseFlag(section.getStringValue(flagKey(number)), flag)) {
                info.flags.push_back(flag);
            } else {
                Logger::error(COMP_GAMERULES, "StructureInfosLoader::load", "[{}] [{}] {} should look like 'big,53,37', skipping it", filename, sectionName, flagKey(number));
            }
        }

        if (section.hasValue("Bitmap")) {
            info.bmp = m_surfaceLookup(section.getStringValue("Bitmap"));
        }
        if (section.hasValue("Shadow")) {
            info.shadow = m_textureLookup(section.getStringValue("Shadow"));
        }
        if (section.hasValue("Flash")) {
            info.flash = m_surfaceLookup(section.getStringValue("Flash"));
        }
        if (section.hasValue("Icon")) {
            info.icon = m_iconLookup(section.getStringValue("Icon"));
        }
    }
    Logger::info(COMP_GAMERULES, "StructureInfosLoader::load", "[{}] Done", filename);
    return true;
}
