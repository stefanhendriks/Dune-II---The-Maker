/**
 * @file UnitInfosLoader.cpp
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

#include "context/UnitInfosLoader.hpp"

#include <array>
#include <utility>

#include "definitions.h"
#include "gameobjects/units/cUnitInfos.h"
#include "sidebar/cSideBar.h"
#include "utils/IniParsing.hpp"
#include "utils/Log.h"
#include "utils/cIniFile.h"
#include "utils/cIniUtils.h"

namespace {

// Used when a unit has no (valid) bitmap: a quad
constexpr std::string_view FALLBACK_BITMAP = "UNIT_QUAD";

struct s_NamedInt {
    std::string_view name;
    int value;
};

constexpr auto SUBLISTS = std::to_array<s_NamedInt>({
    {"SUBLIST_CONSTYARD", SUBLIST_CONSTYARD},
    {"SUBLIST_LIGHTFCTRY", SUBLIST_LIGHTFCTRY},
    {"SUBLIST_HEAVYFCTRY", SUBLIST_HEAVYFCTRY},
    {"SUBLIST_HIGHTECH", SUBLIST_HIGHTECH},
    {"SUBLIST_INFANTRY", SUBLIST_INFANTRY},
    {"SUBLIST_TROOPERS", SUBLIST_TROOPERS},
});

constexpr auto LISTS = std::to_array<std::pair<std::string_view, eListType>>({
    {"LIST_NONE", eListType::LIST_NONE},
    {"LIST_CONSTYARD", eListType::LIST_CONSTYARD},
    {"LIST_FOOT_UNITS", eListType::LIST_FOOT_UNITS},
    {"LIST_UNITS", eListType::LIST_UNITS},
    {"LIST_STARPORT", eListType::LIST_STARPORT},
    {"LIST_PALACE", eListType::LIST_PALACE},
    {"LIST_UPGRADES", eListType::LIST_UPGRADES},
});

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

// A sublist is given by name (SUBLIST_HEAVYFCTRY) or as a plain number
void readSubList(const cSection& section, int& target)
{
    if (!section.hasValue("SubListId")) return;
    const std::string value = section.getStringValue("SubListId");
    for (const auto& [name, id] : SUBLISTS) {
        if (cIniUtils::caseInsCompare(value, std::string(name))) {
            target = id;
            return;
        }
    }
    target = ToInt(value);
}

void readList(const cSection& section, eListType& target)
{
    if (!section.hasValue("ListType")) return;
    const std::string value = section.getStringValue("ListType");
    for (const auto& [name, type] : LISTS) {
        if (cIniUtils::caseInsCompare(value, std::string(name))) {
            target = type;
            return;
        }
    }
    Logger::error(COMP_GAMERULES, "UnitInfosLoader::load", "Unknown ListType [{}], keeping the default", value);
}

void readBullet(const cSection& section, const char* key, int& target)
{
    if (section.hasValue(key)) target = cIniUtils::getBulletTypeFromString(section.getStringValue(key));
}

} // namespace

UnitInfosLoader::UnitInfosLoader(SurfaceLookup surfaceLookup, TextureLookup textureLookup, IconLookup iconLookup, SoundLookup soundLookup)
    : m_surfaceLookup(std::move(surfaceLookup)), m_textureLookup(std::move(textureLookup)), m_iconLookup(std::move(iconLookup)),
      m_soundLookup(std::move(soundLookup))
{
}

bool UnitInfosLoader::load(const std::string& filename, cUnitInfos& unitInfos) const
{
    for (int i = 0; i < MAX_UNITTYPES; i++) {
        s_UnitInfo& info = unitInfos[static_cast<std::size_t>(i)];
        info = s_UnitInfo{};
        info.bmp = m_surfaceLookup(FALLBACK_BITMAP); // default bitmap is a quad!
        info.icon = TextureID::invalid;
        info.hp = -1;
        info.bulletType = -1;
        info.bulletTypeSecondary = -1;
        info.fireSecondaryWithinRange = -1;
        info.attack_frequency = -1;
        info.next_attack_frequency = -1;
        info.buildTime = -1;
        info.fireTwiceHpThresholdFactor = 0.5f; // default 50% hp
        info.squish = true; // most units can squish (infantry)
        info.range = -1;
        info.sight = -1;
        info.queuable = true;
        info.dieWhenLowerThanHP = 1; // lower than 1 means dead
        info.appetite = -1;
        info.deathSound = audioID::invalid;
        info.eatSound = audioID::invalid;
        info.listType = eListType::LIST_NONE;
        info.canDamageStructures = true;
    }

    Logger::info(COMP_GAMERULES, "UnitInfosLoader::load", "Opening unit rules from : {}", filename);
    cIniFile file;
    if (!file.load(filename)) {
        Logger::error(COMP_GAMERULES, "UnitInfosLoader::load", "Could not open file [{}], no units are available", filename);
        return false;
    }

    for (const auto& sectionName : file.getSectionNames()) {
        const int id = cIniUtils::getUnitTypeFromString(sectionName);
        if (id < 0 || id >= MAX_UNITTYPES) {
            Logger::error(COMP_GAMERULES, "UnitInfosLoader::load", "[{}] Unknown unit section [{}], skipping it (expected e.g. [TRIKE])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_UnitInfo& info = unitInfos[static_cast<std::size_t>(id)];
        info.configured = true;
        if (section.hasValue("Name")) {
            info.name = section.getStringValue("Name");
        }
        readInt(section, "Width", info.bmp_width);
        readInt(section, "Height", info.bmp_height);
        readInt(section, "StartPixel", info.bmp_startpixel);
        readInt(section, "Frames", info.bmp_frames);
        readBullet(section, "Bullet", info.bulletType);
        readBullet(section, "BulletSecondary", info.bulletTypeSecondary);
        readInt(section, "FireSecondaryWithinRange", info.fireSecondaryWithinRange);
        readList(section, info.listType);
        readSubList(section, info.subListId);
        readBool(section, "CanGuard", info.canGuard);
        readBool(section, "CanSquishInfantry", info.squish);
        readBool(section, "CanBeSquished", info.canBeSquished);
        readBool(section, "SmokeWhenDamaged", info.renderSmokeOnUnitWhenThresholdMet);
        readBool(section, "CanDamageStructures", info.canDamageStructures);
        readBool(section, "CanEnterAndDamageStructure", info.canEnterAndDamageStructure);
        readBool(section, "AttackIsEnterStructure", info.attackIsEnterStructure);
        readFloat(section, "DamageOnEnterStructure", info.damageOnEnterStructure);
        readInt(section, "DieWhenLowerThanHp", info.dieWhenLowerThanHP);

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
        readBool(section, "FireTwice", info.fireTwice);
        readBool(section, "IsInfantry", info.infantry);
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

        if (section.hasValue("Bitmap")) {
            info.bmp = m_surfaceLookup(section.getStringValue("Bitmap"));
        }
        if (section.hasValue("Top")) {
            info.top = m_surfaceLookup(section.getStringValue("Top"));
        }
        if (section.hasValue("Shadow")) {
            info.shadow = m_textureLookup(section.getStringValue("Shadow"));
        }
        if (section.hasValue("Icon")) {
            info.icon = m_iconLookup(section.getStringValue("Icon"));
        }
        if (section.hasValue("DeathSound")) {
            info.deathSound = m_soundLookup(section.getStringValue("DeathSound"));
        }
        if (section.hasValue("EatSound")) {
            info.eatSound = m_soundLookup(section.getStringValue("EatSound"));
        }
    }
    Logger::info(COMP_GAMERULES, "UnitInfosLoader::load", "[{}] Done", filename);
    return true;
}
