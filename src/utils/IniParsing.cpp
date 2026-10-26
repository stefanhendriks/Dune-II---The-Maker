/**
 * @file IniParsing.cpp
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

#include "utils/IniParsing.hpp"

#include "utils/Log.h"
#include "utils/cIniUtils.h"

#include <cctype>
#include <charconv>
#include <sstream>

int ToInt(const std::string& str)
{
#if D2TM_CLANG
    try {
        size_t idx = 0;
        int value = std::stoi(str, &idx);

        if (idx == str.size()) {
            return value;
        } else {
            Logger::warn(COMP_NONE, "ToInt", "Extra characters in '{}', 0 returned.", str);
            return 0;
        }
    } catch (const std::invalid_argument&) {
        Logger::warn(COMP_NONE, "ToInt", "Failed to convert '{}' to int, 0 returned.", str);
        return 0;
    } catch (const std::out_of_range&) {
        Logger::warn(COMP_NONE, "ToInt", "Value out of range in '{}', 0 returned.", str);
        return 0;
    }
#else
    int value = 0;
    auto result = std::from_chars(str.data(), str.data() + str.size(), value);
    if (result.ec == std::errc() && result.ptr == str.data() + str.size()) {
        return value;
    } else {
        Logger::warn(COMP_NONE, "ToInt", "Failed to convert '{}' to int, 0 returned.", str);
        return 0;
    }
#endif
}

float ToFloat(const std::string& str)
{
#if D2TM_CLANG
    try {
        size_t idx = 0;
        float value = std::stof(str, &idx);

        if (idx == str.size()) {
            return value;
        } else {
            Logger::warn(COMP_NONE, "ToFloat", "Extra characters in '{}', 0.0f returned.", str);
            return 0.0f;
        }
    } catch (const std::invalid_argument&) {
        Logger::warn(COMP_NONE, "ToFloat", "Failed to convert '{}' to float, 0.0f returned.", str);
        return 0.0f;
    } catch (const std::out_of_range&) {
        Logger::warn(COMP_NONE, "ToFloat", "Value out of range in '{}', 0.0f returned.", str);
        return 0.0f;
    }
#else
    float value = 0.0f;
    auto result = std::from_chars(str.data(), str.data() + str.size(), value);
    if (result.ec == std::errc() && result.ptr == str.data() + str.size()) {
        return value;
    } else {
        Logger::warn(COMP_NONE, "ToFloat", "Failed to convert '{}' to float, 0.0f returned.", str);
        return 0.0f;
    }
#endif
}

bool ToBool(const std::string& str)
{
    return str == "1" || cIniUtils::caseInsCompare(str, "true") || cIniUtils::caseInsCompare(str, "yes");
}

std::string ToTrim(const std::string& input)
{
    std::string result = input;
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.front()))) {
        result.erase(0, 1);
    }
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.back()))) {
        result.pop_back();
    }
    return result;
}

std::string removeQuote(const std::string& input)
{
    std::string result = input;
    if (!result.empty() && result.front() == '"') result.erase(0, 1);
    if (!result.empty() && result.back() == '"')  result.pop_back();
    return result;
}

std::vector<std::string> splitByComma(const std::string& input)
{
    std::vector<std::string> result;
    std::istringstream stream(input);
    std::string token;
    while (std::getline(stream, token, ',')) {
        result.push_back(std::move(token));
    }
    return result;
}

std::string extractTagValue(const std::string& line, std::string_view prefix)
{
    auto pos = line.find(prefix);
    if (pos == std::string::npos) return {};
    auto start = pos + prefix.size();
    auto end = line.find(']', start);
    return ToTrim(line.substr(start, end == std::string::npos ? std::string::npos : end - start));
}

std::string extractSectionName(const std::string& line)
{
    auto start = line.find('[');
    auto end = line.find(']', start);
    if (start != std::string::npos && end != std::string::npos && end > start + 1) {
        std::string section = ToTrim(line.substr(start + 1, end - start - 1));
        if (!section.empty())
            return section;
    }
    return "";
}

std::pair<std::string, std::string> INI_SplitWord(const std::string& input)
{
    auto pos = input.find('=');
    if (pos == std::string::npos) {
        return {input, ""};
    }
    return {input.substr(0, pos), input.substr(pos + 1)};
}

eIniWord INI_WordType(const std::string& word, eIniSection section)
{
    if (section == eIniSection::REGION) {
        if (cIniUtils::caseInsCompare(word, "Region"))            return eIniWord::REGION;
        if (cIniUtils::caseInsCompare(word, "RegionConquer"))     return eIniWord::REGIONCONQUER;
        if (cIniUtils::caseInsCompare(word, "House"))             return eIniWord::REGIONHOUSE;
        if (cIniUtils::caseInsCompare(word, "Text"))              return eIniWord::REGIONTEXT;
        if (cIniUtils::caseInsCompare(word, "Select"))            return eIniWord::REGIONSELECT;
        return eIniWord::NONE;
    }

    if (section == eIniSection::SCEN ||
            section == eIniSection::DESCRIPTION ||
            section == eIniSection::BRIEFING ||
            section == eIniSection::ADVICE ||
            section == eIniSection::LOSE ||
            section == eIniSection::WIN) {
        if (cIniUtils::caseInsCompare(word, "Number"))            return eIniWord::NUMBER;
        if (cIniUtils::caseInsCompare(word, "Text"))              return eIniWord::REGIONTEXT;
        return eIniWord::NONE;
    }

    if (section == eIniSection::SKIRMISH) {
        if (cIniUtils::caseInsCompare(word, "Title"))             return eIniWord::MAPNAME;
        if (cIniUtils::caseInsCompare(word, "StartCell"))         return eIniWord::STARTCELL;
    }

    if (section == eIniSection::GAME) {
        if (cIniUtils::caseInsCompare(word, "ModId"))             return eIniWord::MODID;
    }
    else if (section == eIniSection::BASIC) {
        if (cIniUtils::caseInsCompare(word, "CursorPos"))         return eIniWord::FOCUS_POS;
        if (cIniUtils::caseInsCompare(word, "BriefPicture"))      return eIniWord::BRIEFPICTURE;
        if (cIniUtils::caseInsCompare(word, "WinFlags"))          return eIniWord::WINFLAGS;
        if (cIniUtils::caseInsCompare(word, "LoseFlags"))         return eIniWord::LOSEFLAGS;
    }
    else if (isHouseSection(section)) {
        if (cIniUtils::caseInsCompare(word, "Team"))              return eIniWord::TEAM;
        if (cIniUtils::caseInsCompare(word, "Credits"))           return eIniWord::CREDITS;
        if (cIniUtils::caseInsCompare(word, "Quota"))             return eIniWord::QUOTA;
        if (cIniUtils::caseInsCompare(word, "Brain"))             return eIniWord::BRAIN;
        if (cIniUtils::caseInsCompare(word, "House"))             return eIniWord::HOUSE;
        if (cIniUtils::caseInsCompare(word, "Focus"))             return eIniWord::FOCUS_POS;
    }
    else if (section == eIniSection::MAP) {
        if (cIniUtils::caseInsCompare(word, "Width"))             return eIniWord::MAPWIDTH;
        if (cIniUtils::caseInsCompare(word, "Height"))            return eIniWord::MAPHEIGHT;
        if (cIniUtils::caseInsCompare(word, "Seed"))              return eIniWord::MAPSEED;
        if (cIniUtils::caseInsCompare(word, "Bloom"))             return eIniWord::MAPBLOOM;
        if (cIniUtils::caseInsCompare(word, "Field"))             return eIniWord::MAPFIELD;
    }
    else if (section == eIniSection::BULLETS) {
        if (cIniUtils::caseInsCompare(word, "Bitmap"))            return eIniWord::BITMAP;
        if (cIniUtils::caseInsCompare(word, "BitmapExplosion"))   return eIniWord::BITMAP_DEAD;
        if (cIniUtils::caseInsCompare(word, "BitmapWidth"))       return eIniWord::BITMAP_WIDTH;
        if (cIniUtils::caseInsCompare(word, "BitmapFrames"))      return eIniWord::BITMAP_FRAMES;
        if (cIniUtils::caseInsCompare(word, "BitmapExplFrames"))  return eIniWord::BITMAP_DEADFRAMES;
        if (cIniUtils::caseInsCompare(word, "DamageVehicle"))     return eIniWord::DAMAGE_VEHICLE;
        if (cIniUtils::caseInsCompare(word, "DamageInfantry"))    return eIniWord::DAMAGE_INFANTRY;
        if (cIniUtils::caseInsCompare(word, "Definition"))        return eIniWord::DEFINITION;
        if (cIniUtils::caseInsCompare(word, "Sound"))             return eIniWord::SOUND;
        if (cIniUtils::caseInsCompare(word, "ExplosionSize"))     return eIniWord::EXPLOSION_SIZE;
        if (cIniUtils::caseInsCompare(word, "DeviateProbability"))return eIniWord::DEVIATE_PROBABILITY;
        if (cIniUtils::caseInsCompare(word, "GroundBullet"))      return eIniWord::GROUND_BULLET;
    }
    else if (section == eIniSection::STRUCTURES) {
        if (cIniUtils::caseInsCompare(word, "HitPoints"))         return eIniWord::HITPOINTS;
        if (cIniUtils::caseInsCompare(word, "FixPoints"))         return eIniWord::FIXHP;
        if (cIniUtils::caseInsCompare(word, "PowerDrain"))        return eIniWord::POWERDRAIN;
        if (cIniUtils::caseInsCompare(word, "HasConcrete"))       return eIniWord::HAS_CONCRETE;
        if (cIniUtils::caseInsCompare(word, "PowerGive"))         return eIniWord::POWERGIVE;
        if (cIniUtils::caseInsCompare(word, "Cost"))              return eIniWord::COST;
        if (cIniUtils::caseInsCompare(word, "BuildTime"))         return eIniWord::BUILDTIME;
        if (cIniUtils::caseInsCompare(word, "UponDestructionSpawnUnitAmountMin")) return eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MIN;
        if (cIniUtils::caseInsCompare(word, "UponDestructionSpawnUnitAmountMax")) return eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MAX;
        if (cIniUtils::caseInsCompare(word, "UponDestructionSpawnUnitType"))      return eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_TYPE;
    }
    else if (section == eIniSection::UNITS) {
        if (cIniUtils::caseInsCompare(word, "Bitmap"))            return eIniWord::BITMAP;
        if (cIniUtils::caseInsCompare(word, "BitmapTop"))         return eIniWord::BITMAP_TOP;
        if (cIniUtils::caseInsCompare(word, "Icon"))              return eIniWord::ICON;
        if (cIniUtils::caseInsCompare(word, "BitmapWidth"))       return eIniWord::BITMAP_WIDTH;
        if (cIniUtils::caseInsCompare(word, "BitmapHeight"))      return eIniWord::BITMAP_HEIGHT;
        if (cIniUtils::caseInsCompare(word, "HitPoints"))         return eIniWord::HITPOINTS;
        if (cIniUtils::caseInsCompare(word, "Appetite"))          return eIniWord::APPETITE;
        if (cIniUtils::caseInsCompare(word, "Cost"))              return eIniWord::COST;
        if (cIniUtils::caseInsCompare(word, "BulletType"))        return eIniWord::BULLETTYPE;
        if (cIniUtils::caseInsCompare(word, "MoveSpeed"))         return eIniWord::MOVESPEED;
        if (cIniUtils::caseInsCompare(word, "TurnSpeed"))         return eIniWord::TURNSPEED;
        // Attack frequency (todo: wording, it should be more like "delay" or "fireRate")
        if (cIniUtils::caseInsCompare(word, "AttackFrequency"))   return eIniWord::ATTACKFREQ;
        // Next Attack frequency (if applicable) (todo: wording, it should be more like "delay" or "fireRate")
        if (cIniUtils::caseInsCompare(word, "NextAttackFrequency")) return eIniWord::NEXTATTACKFREQ;
        if (cIniUtils::caseInsCompare(word, "Sight"))             return eIniWord::SIGHT;
        if (cIniUtils::caseInsCompare(word, "Range"))             return eIniWord::RANGE;
        if (cIniUtils::caseInsCompare(word, "BuildTime"))         return eIniWord::BUILDTIME;
        if (cIniUtils::caseInsCompare(word, "Description"))       return eIniWord::DESCRIPTION;
        // BOOLEANS
        if (cIniUtils::caseInsCompare(word, "IsHarvester"))       return eIniWord::ISHARVESTER;
        if (cIniUtils::caseInsCompare(word, "FireTwice"))         return eIniWord::FIRETWICE;
        if (cIniUtils::caseInsCompare(word, "IsInfantry"))        return eIniWord::ISINFANTRY;
        if (cIniUtils::caseInsCompare(word, "Squishable"))        return eIniWord::ISSQUISHABLE;
        if (cIniUtils::caseInsCompare(word, "CanSquish"))         return eIniWord::CANSQUISH;
        if (cIniUtils::caseInsCompare(word, "IsAirborn"))         return eIniWord::ISAIRBORN;
        if (cIniUtils::caseInsCompare(word, "AbleToCarry"))       return eIniWord::ABLETOCARRY;
        if (cIniUtils::caseInsCompare(word, "FreeRoam"))          return eIniWord::FREEROAM;
        if (cIniUtils::caseInsCompare(word, "Producer"))          return eIniWord::PRODUCER;
        if (cIniUtils::caseInsCompare(word, "MaxCredits"))        return eIniWord::HARVESTLIMIT;
        if (cIniUtils::caseInsCompare(word, "HarvestSpeed"))      return eIniWord::HARVESTSPEED;
        if (cIniUtils::caseInsCompare(word, "HarvestAmount"))     return eIniWord::HARVESTAMOUNT;
        if (cIniUtils::caseInsCompare(word, "squish"))            return eIniWord::SQUISH;
        if (cIniUtils::caseInsCompare(word, "SmokeHpFactor"))     return eIniWord::SMOKEHFACTOR;
        if (cIniUtils::caseInsCompare(word, "CanAttackAirUnits")) return eIniWord::CANATTACKAIRUNITS;
        if (cIniUtils::caseInsCompare(word, "CanAttackUnits"))    return eIniWord::CANATTACKUNITS;
        if (cIniUtils::caseInsCompare(word, "AttackIsEnterStructure")) return eIniWord::ATTACK_IS_ENTER_STRUCTURE;
        if (cIniUtils::caseInsCompare(word, "DamageOnEnterStructure")) return eIniWord::DAMAGE_ON_ENTER_STRUCTURE;
    }
    else if (section == eIniSection::STRUCTURES) {
        if (word.length() > 1) {
            if (cIniUtils::caseInsCompare(word, "PreBuild"))      return eIniWord::PREBUILD;
            if (cIniUtils::caseInsCompare(word, "Description"))   return eIniWord::DESCRIPTION;
            if (cIniUtils::caseInsCompare(word, "Power"))         return eIniWord::POWER;
            if (cIniUtils::caseInsCompare(word, "Sight"))         return eIniWord::SIGHT;
            if (cIniUtils::caseInsCompare(word, "Range"))         return eIniWord::RANGE;
            if (cIniUtils::caseInsCompare(word, "FireRate"))      return eIniWord::FIRERATE;
        }
        else
            return eIniWord::NONE;
    }
    else if (section == eIniSection::HOUSES) {
        if (cIniUtils::caseInsCompare(word, "ColorR"))            return eIniWord::RED;
        if (cIniUtils::caseInsCompare(word, "ColorG"))            return eIniWord::GREEN;
        if (cIniUtils::caseInsCompare(word, "ColorB"))            return eIniWord::BLUE;
        if (cIniUtils::caseInsCompare(word, "FirePower"))         return eIniWord::FIREPOWER;
        if (cIniUtils::caseInsCompare(word, "FireRate"))          return eIniWord::FIRERATE;
        if (cIniUtils::caseInsCompare(word, "StructPrice"))       return eIniWord::STRUCTPRICE;
        if (cIniUtils::caseInsCompare(word, "UnitPrice"))         return eIniWord::UNITPRICE;
        if (cIniUtils::caseInsCompare(word, "Speed"))             return eIniWord::SPEED;
        if (cIniUtils::caseInsCompare(word, "BuildSpeed"))        return eIniWord::BUILDSPEED;
        if (cIniUtils::caseInsCompare(word, "HarvestSpeed"))      return eIniWord::HARVESTSPEED;
        if (cIniUtils::caseInsCompare(word, "DumpSpeed"))         return eIniWord::DUMPSPEED;
    }

    Logger::warn(COMP_NONE, "SCEN_INI_WordType", "Could not find word-type for [{}]", word);
    return eIniWord::NONE;
}

eIniSection SCEN_INI_SectionType(const std::string& section)
{
    if (cIniUtils::caseInsCompare(section, "UNITS"))          return eIniSection::UNITS;
    if (cIniUtils::caseInsCompare(section, "SKIRMISH"))       return eIniSection::SKIRMISH;
    if (cIniUtils::caseInsCompare(section, "STRUCTURES"))     return eIniSection::STRUCTURES;
    if (cIniUtils::caseInsCompare(section, "REINFORCEMENTS")) return eIniSection::REINFORCEMENTS;
    if (cIniUtils::caseInsCompare(section, "MAP"))            return eIniSection::MAP;
    if (cIniUtils::caseInsCompare(section, "BASIC"))          return eIniSection::BASIC;
    if (cIniUtils::caseInsCompare(section, "Atreides"))       return eIniSection::HOUSEATREIDES;
    if (cIniUtils::caseInsCompare(section, "Ordos"))          return eIniSection::HOUSEORDOS;
    if (cIniUtils::caseInsCompare(section, "Harkonnen"))      return eIniSection::HOUSEHARKONNEN;
    if (cIniUtils::caseInsCompare(section, "Sardaukar"))      return eIniSection::HOUSESARDAUKAR;
    if (cIniUtils::caseInsCompare(section, "Fremen"))         return eIniSection::HOUSEFREMEN;
    if (cIniUtils::caseInsCompare(section, "Mercenary"))      return eIniSection::HOUSEMERCENARY;
    Logger::warn(COMP_NONE, "SCEN_INI_SectionType", "Could not find section type for [{}]", section);
    return eIniSection::NONE;
}

eIniSection GAME_INI_SectionType(const std::string& section, eIniSection last)
{
    if (cIniUtils::caseInsCompare(section, "BULLETS"))    return eIniSection::BULLETS;
    if (cIniUtils::caseInsCompare(section, "UNITS"))      return eIniSection::UNITS;
    if (cIniUtils::caseInsCompare(section, "STRUCTURES")) return eIniSection::STRUCTURES;
    if (cIniUtils::caseInsCompare(section, "TERRAINS"))   return eIniSection::TERRAINS;
    // When nothing found; we assume its just a new ID tag for some unit or structure
    // Therefor we return the last known SECTION ID so we can assign the proper WORD ID's
    return last;
}

bool isCommentLine(const std::string& linefeed)
{
    if (linefeed.empty()) return true;
    char first = linefeed[0];
    if (first == ';' || first == '#' || first == '\n' || first == '\0') return true;
    if (first == '/' && linefeed.size() > 1 && linefeed[1] == '/') return true;
    return false;
}
