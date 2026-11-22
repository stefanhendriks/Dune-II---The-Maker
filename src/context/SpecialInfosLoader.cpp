/**
 * @file SpecialInfosLoader.cpp
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

#include "context/SpecialInfosLoader.hpp"

#include <sstream>
#include <utility>

#include "definitions.h"
#include "include/enums/HouseFlags.h"
#include "gameobjects/cSpecialInfos.h"
#include "utils/IniParsing.hpp"
#include "utils/Log.h"
#include "utils/cIniFile.h"
#include "utils/cIniUtils.h"

namespace {

void setDefaults(s_SpecialInfo& info)
{
    info.icon = TextureID::invalid;
    info.providesType = eBuildType::UNIT;
    info.providesTypeId = -1;
    info.buildTime = 0;
    info.deployFrom = eDeployFromType::AT_RANDOM_CELL;
    info.deployAtStructure = -1;
    info.units = 0;
    info.house = eHouseBitFlag::Unknown;
    info.autoBuild = false;
    info.deployTargetType = eDeployTargetType::TARGET_NONE;
    info.deployTargetPrecision = 0;
    info.spawnBorderDistance = 4;
    info.spawnScatter = 2;
    info.listType = eListType::LIST_NONE;
    info.subListId = -1;
    info.description = "";
}

int specialIdFromName(const std::string& name)
{
    if (cIniUtils::caseInsCompare(name, "SABOTEUR")) return SPECIAL_SABOTEUR;
    if (cIniUtils::caseInsCompare(name, "FREMEN")) return SPECIAL_FREMEN;
    if (cIniUtils::caseInsCompare(name, "DEATHHAND")) return SPECIAL_DEATHHAND;
    return -1;
}

std::string trim(const std::string& text)
{
    const auto first = text.find_first_not_of(" 	");
    if (first == std::string::npos) return "";
    return text.substr(first, text.find_last_not_of(" 	") - first + 1);
}

// "Harkonnen, Sardaukar" becomes the combined house bit flags
unsigned char houseFlagsFromString(const std::string& text)
{
    unsigned char flags = 0;
    std::stringstream stream(text);
    std::string token;
    while (std::getline(stream, token, ',')) {
        token = trim(token);
        if (cIniUtils::caseInsCompare(token, "Atreides")) flags |= eHouseBitFlag::Atreides;
        else if (cIniUtils::caseInsCompare(token, "Ordos")) flags |= eHouseBitFlag::Ordos;
        else if (cIniUtils::caseInsCompare(token, "Harkonnen")) flags |= eHouseBitFlag::Harkonnen;
        else if (cIniUtils::caseInsCompare(token, "Sardaukar")) flags |= eHouseBitFlag::Sardaukar;
        else if (cIniUtils::caseInsCompare(token, "Fremen")) flags |= eHouseBitFlag::Fremen;
        else Logger::error(COMP_GAMERULES, "SpecialInfosLoader::load", "Unknown house [{}] in Houses, ignoring it", token);
    }
    return flags;
}

void readInt(const cSection& section, const char* key, int& target)
{
    if (section.hasValue(key)) target = ToInt(section.getStringValue(key));
}

} // namespace

SpecialInfosLoader::SpecialInfosLoader(IconLookup iconLookup) : m_iconLookup(std::move(iconLookup))
{
}

bool SpecialInfosLoader::load(const std::string& filename, cSpecialInfos& specialInfos) const
{
    for (auto& info : specialInfos) {
        setDefaults(info);
    }

    Logger::info(COMP_GAMERULES, "SpecialInfosLoader::load", "Opening special rules from : {}", filename);
    cIniFile file;
    if (!file.load(filename)) {
        Logger::error(COMP_GAMERULES, "SpecialInfosLoader::load", "Could not open file [{}], no specials are available", filename);
        return false;
    }

    for (const auto& sectionName : file.getSectionNames()) {
        const int id = specialIdFromName(sectionName);
        if (id < 0) {
            Logger::error(COMP_GAMERULES, "SpecialInfosLoader::load", "[{}] Unknown special section [{}], skipping it (expected [SABOTEUR], [FREMEN] or [DEATHHAND])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_SpecialInfo& info = specialInfos[static_cast<std::size_t>(id)];
        // all specials are shown in the palace list
        info.listType = eListType::LIST_PALACE;
        info.subListId = 0;

        if (section.hasValue("Description")) info.description = section.getStringValue("Description");
        if (section.hasValue("Icon")) info.icon = m_iconLookup(section.getStringValue("Icon"));
        if (section.hasValue("Houses")) info.house = houseFlagsFromString(section.getStringValue("Houses"));
        if (section.hasValue("AutoBuild")) info.autoBuild = ToBool(section.getStringValue("AutoBuild"));
        readInt(section, "BuildTime", info.buildTime);
        readInt(section, "Units", info.units);
        readInt(section, "DeployTargetPrecision", info.deployTargetPrecision);
        readInt(section, "SpawnBorderDistance", info.spawnBorderDistance);
        readInt(section, "SpawnScatter", info.spawnScatter);

        if (section.hasValue("Provides")) {
            const std::string provides = section.getStringValue("Provides");
            if (cIniUtils::caseInsCompare(section.getStringValue("ProvidesType", "UNIT"), "BULLET")) {
                info.providesType = eBuildType::BULLET;
                info.providesTypeId = cIniUtils::getBulletTypeFromString(provides);
            } else {
                info.providesType = eBuildType::UNIT;
                info.providesTypeId = cIniUtils::getUnitTypeFromString(provides);
            }
        }
        if (section.hasValue("DeployFrom")) {
            info.deployFrom = cIniUtils::caseInsCompare(section.getStringValue("DeployFrom"), "STRUCTURE")
                              ? eDeployFromType::AT_STRUCTURE : eDeployFromType::AT_RANDOM_CELL;
        }
        if (section.hasValue("DeployAtStructure")) {
            info.deployAtStructure = cIniUtils::getStructureTypeFromString(section.getStringValue("DeployAtStructure"));
        }
        if (section.hasValue("DeployTarget")) {
            const std::string target = section.getStringValue("DeployTarget");
            if (cIniUtils::caseInsCompare(target, "SPECIFIC")) info.deployTargetType = eDeployTargetType::TARGET_SPECIFIC_CELL;
            else if (cIniUtils::caseInsCompare(target, "INACCURATE")) info.deployTargetType = eDeployTargetType::TARGET_INACCURATE_CELL;
            else info.deployTargetType = eDeployTargetType::TARGET_NONE;
        }
    }
    Logger::info(COMP_GAMERULES, "SpecialInfosLoader::load", "[{}] Done", filename);
    return true;
}
