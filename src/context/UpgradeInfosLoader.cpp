/**
 * @file UpgradeInfosLoader.cpp
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

#include "context/UpgradeInfosLoader.hpp"

#include <array>
#include <utility>

#include "definitions.h"
#include "gameobjects/cUpgradeInfo.h"
#include "include/enums/HouseFlags.h"
#include "sidebar/cSideBar.h"
#include "utils/Log.h"
#include "utils/cIniFile.h"
#include "utils/cIniUtils.h"

namespace {

struct NamedId {
    const char* name;
    int id;
};

constexpr std::array<NamedId, 13> kUpgradeNames = {{
    {"CONSTYARD_SLAB4", UPGRADE_TYPE_CONSTYARD_SLAB4},
    {"CONSTYARD_RTURRET", UPGRADE_TYPE_CONSTYARD_RTURRET},
    {"LIGHTFCTRY_QUAD", UPGRADE_TYPE_LIGHTFCTRY_QUAD},
    {"HEAVYFCTRY_MVC", UPGRADE_TYPE_HEAVYFCTRY_MVC},
    {"HEAVYFCTRY_LAUNCHER", UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER},
    {"HEAVYFCTRY_SIEGETANK", UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK},
    {"HEAVYFCTRY_SIEGETANK_ORD", UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD},
    {"HITECH_ORNI", UPGRADE_TYPE_HITECH_ORNI},
    {"WOR_TROOPERS", UPGRADE_TYPE_WOR_TROOPERS},
    {"BARRACKS_INFANTRY", UPGRADE_TYPE_BARRACKS_INFANTRY},
    {"HEAVYFCTRY_SAR_SONICTANK", UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK},
    {"HEAVYFCTRY_SAR_DEVIATOR", UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR},
    {"HEAVYFCTRY_SAR_DEVASTATOR", UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR},
}};

constexpr std::array<NamedId, 6> kSubLists = {{
    {"CONSTYARD", SUBLIST_CONSTYARD},
    {"LIGHTFCTRY", SUBLIST_LIGHTFCTRY},
    {"HEAVYFCTRY", SUBLIST_HEAVYFCTRY},
    {"HIGHTECH", SUBLIST_HIGHTECH},
    {"INFANTRY", SUBLIST_INFANTRY},
    {"TROOPERS", SUBLIST_TROOPERS},
}};

int upgradeIdFromName(const std::string& name)
{
    for (const auto& upgrade : kUpgradeNames) {
        if (cIniUtils::caseInsCompare(name, upgrade.name)) return upgrade.id;
    }
    return -1;
}

int subListFromName(const std::string& name)
{
    for (const auto& subList : kSubLists) {
        if (cIniUtils::caseInsCompare(name, subList.name)) return subList.id;
    }
    Logger::error(COMP_GAMERULES, "UpgradeInfosLoader::load", "Unknown SubList [{}]", name);
    return -1;
}

eListType listFromName(const std::string& name)
{
    if (cIniUtils::caseInsCompare(name, "CONSTYARD")) return eListType::LIST_CONSTYARD;
    if (cIniUtils::caseInsCompare(name, "FOOT_UNITS")) return eListType::LIST_FOOT_UNITS;
    if (cIniUtils::caseInsCompare(name, "UNITS")) return eListType::LIST_UNITS;
    Logger::error(COMP_GAMERULES, "UpgradeInfosLoader::load", "Unknown List [{}]", name);
    return eListType::LIST_NONE;
}

// A structure name as used in structures.ini, -1 when unknown (getStructureType falls back to the Palace)
int structureFromName(const std::string& name)
{
    const int id = cIniUtils::getStructureType(name);
    if (id == PALACE && !cIniUtils::caseInsCompare(name, "PALACE")) {
        Logger::error(COMP_GAMERULES, "UpgradeInfosLoader::load", "Unknown structure [{}]", name);
        return -1;
    }
    return id;
}

std::string trim(const std::string& text)
{
    const std::string whitespace = " \t";
    const auto first = text.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    return text.substr(first, text.find_last_not_of(whitespace) - first + 1);
}

// "Atreides, Ordos" becomes the combined house bit flags
unsigned char houseFlagsFromString(const std::string& text)
{
    unsigned char flags = 0;
    std::string rest = text;
    while (!rest.empty()) {
        const auto comma = rest.find(',');
        const std::string token = trim(rest.substr(0, comma));
        rest = comma == std::string::npos ? "" : rest.substr(comma + 1);
        if (cIniUtils::caseInsCompare(token, "Atreides")) flags |= eHouseBitFlag::Atreides;
        else if (cIniUtils::caseInsCompare(token, "Ordos")) flags |= eHouseBitFlag::Ordos;
        else if (cIniUtils::caseInsCompare(token, "Harkonnen")) flags |= eHouseBitFlag::Harkonnen;
        else if (cIniUtils::caseInsCompare(token, "Sardaukar")) flags |= eHouseBitFlag::Sardaukar;
        else if (cIniUtils::caseInsCompare(token, "Fremen")) flags |= eHouseBitFlag::Fremen;
        else Logger::error(COMP_GAMERULES, "UpgradeInfosLoader::load", "Unknown house [{}] in Houses, ignoring it", token);
    }
    return flags;
}

} // namespace

UpgradeInfosLoader::UpgradeInfosLoader(IconLookup iconLookup, CostLookup structureCost, CostLookup unitCost, bool cheatMode)
    : m_iconLookup(std::move(iconLookup)), m_structureCost(std::move(structureCost)), m_unitCost(std::move(unitCost)), m_cheatMode(cheatMode)
{
}

bool UpgradeInfosLoader::load(const std::string& filename, cUpgradeInfos& upgradeInfos) const
{
    for (std::size_t i = 0; i < MAX_UPGRADETYPES; i++) {
        s_UpgradeInfo& info = upgradeInfos[i];
        info.enabled = false;
        info.techLevel = -1;
        info.house = 0;
        info.needsStructureType = -1;
        info.icon = m_iconLookup("ICON_STR_PALACE");
        info.cost = 100;
        info.atUpgradeLevel = -1;
        info.dependsOnStructureLevel = true;
        info.structureType = CONSTYARD;
        info.providesTypeId = -1;
        info.providesType = STRUCTURE;
        info.providesTypeList = eListType::LIST_NONE;
        info.providesTypeSubList = -1;
        info.buildTime = 5;
        info.description = "Upgrade";
    }

    Logger::info(COMP_GAMERULES, "UpgradeInfosLoader::load", "Opening upgrade rules from : {}", filename);
    cIniFile file;
    if (!file.load(filename)) {
        Logger::error(COMP_GAMERULES, "UpgradeInfosLoader::load", "Could not open file [{}], no upgrades are available", filename);
        return false;
    }

    for (const auto& sectionName : file.getSectionNames()) {
        const int id = upgradeIdFromName(sectionName);
        if (id < 0) {
            Logger::error(COMP_GAMERULES, "UpgradeInfosLoader::load", "[{}] Unknown upgrade section [{}], skipping it (expected e.g. [HEAVYFCTRY_MVC])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_UpgradeInfo& info = upgradeInfos[static_cast<std::size_t>(id)];
        info.enabled = section.getBoolean("Enabled", true);
        info.description = section.getStringValue("Description", info.description);
        if (section.hasValue("Icon")) info.icon = m_iconLookup(section.getStringValue("Icon"));
        if (section.hasValue("Houses")) info.house = houseFlagsFromString(section.getStringValue("Houses"));
        info.techLevel = section.getInt("TechLevel", info.techLevel);
        info.atUpgradeLevel = section.getInt("AtUpgradeLevel", info.atUpgradeLevel);
        info.dependsOnStructureLevel = section.getBoolean("DependsOnStructureLevel", info.dependsOnStructureLevel);
        if (section.hasValue("Structure")) info.structureType = structureFromName(section.getStringValue("Structure"));
        if (section.hasValue("NeedsStructure")) info.needsStructureType = structureFromName(section.getStringValue("NeedsStructure"));

        // either a fixed Cost, or a percentage of the cost of a structure or unit
        info.cost = section.getInt("Cost", info.cost);
        const int percentage = section.getInt("CostPercentage", 50);
        if (section.hasValue("CostOfStructure")) {
            info.cost = m_structureCost(structureFromName(section.getStringValue("CostOfStructure"))) * percentage / 100;
        } else if (section.hasValue("CostOfUnit")) {
            info.cost = m_unitCost(cIniUtils::getUnitTypeFromString(section.getStringValue("CostOfUnit"))) * percentage / 100;
        }

        if (section.hasValue("ProvidesType")) {
            info.providesType = cIniUtils::caseInsCompare(section.getStringValue("ProvidesType"), "UNIT") ? eBuildType::UNIT : eBuildType::STRUCTURE;
        }
        if (section.hasValue("Provides")) {
            const std::string provides = section.getStringValue("Provides");
            info.providesTypeId = info.providesType == eBuildType::UNIT ? cIniUtils::getUnitTypeFromString(provides) : structureFromName(provides);
        }
        if (section.hasValue("List")) info.providesTypeList = listFromName(section.getStringValue("List"));
        if (section.hasValue("SubList")) info.providesTypeSubList = subListFromName(section.getStringValue("SubList"));

        info.buildTime = section.getInt("BuildTime", info.buildTime);
        if (m_cheatMode) info.buildTime = section.getInt("BuildTimeCheatMode", info.buildTime);
    }
    Logger::info(COMP_GAMERULES, "UpgradeInfosLoader::load", "[{}] Done", filename);
    return true;
}
