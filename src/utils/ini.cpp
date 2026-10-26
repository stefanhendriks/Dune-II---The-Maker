/**
 * @file ini.cpp
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

#include "utils/ini.h"
#include "utils/ScenarioData.hpp"
#include "iniSections.h"
#include "iniWords.h"
#include "iniUnitsParts.h"
#include "cEnumArray.h"
#include "utils/cIniUtils.h"

#include "data/gfxdata.h"
#include "definitions.h"
#include "include/sDataCampaign.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "managers/cDrawManager.h" // TODO: an ini file reader should not depend on drawing code
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/cMapEditor.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/map/cPreviewMaps.h"
#include "gameobjects/mentat/AbstractMentat.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "utils/Log.h"
#include "utils/common.h"
#include "gameobjects/map/SeedMapLoader.hpp"
#include "gameobjects/units/cReinforcements.h"
#include "gameobjects/sTerrainInfo.h"
#include "utils/RNG.hpp"
#include "gameobjects/units/cUnits.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "include/sGameServices.h"
#include "context/GameContext.hpp"
#include "game/cGameInterface.h"
#include "game/cGameConditionChecker.h"
#include "game/cGameSettings.h"
#include <format>
#include <filesystem>
namespace fs=std::filesystem;

#include <algorithm>
#include <array>
#include <charconv>
#include <sstream>
#include <span>
#include <string>
#include <unordered_map>
#include <optional>
#include <utility>
#include <vector>
#include <platform.h>

class cReinforcements;

static int ToInt(const std::string& str)
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

static float ToFloat(const std::string& str)
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

static bool ToBool(const std::string& str)
{
    return str == "1" || cIniUtils::caseInsCompare(str, "true") || cIniUtils::caseInsCompare(str, "yes");
}

static std::string ToTrim(const std::string& input)
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

static std::string removeQuote(const std::string& input)
{
    std::string result = input;
    if (!result.empty() && result.front() == '"') result.erase(0, 1);
    if (!result.empty() && result.back() == '"')  result.pop_back();
    return result;
}

// Split a comma-separated string into a vector of tokens (not trimmed)
static std::vector<std::string> splitByComma(const std::string& input)
{
    std::vector<std::string> result;
    std::istringstream stream(input);
    std::string token;
    while (std::getline(stream, token, ',')) {
        result.push_back(std::move(token));
    }
    return result;
}

// Extract the trimmed value from a section tag like "[UNIT: Devastator]" given the prefix "[UNIT:"
static std::string extractTagValue(const std::string& line, std::string_view prefix)
{
    auto pos = line.find(prefix);
    if (pos == std::string::npos) return {};
    auto start = pos + prefix.size();
    auto end = line.find(']', start);
    return ToTrim(line.substr(start, end == std::string::npos ? std::string::npos : end - start));
}

/*********************************************************************************/
// [SECTION] → "SECTION"
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

// "key=value" → {"key", "value"}
std::pair<std::string, std::string> INI_SplitWord(const std::string& input)
{
    auto pos = input.find('=');
    if (pos == std::string::npos) {
        return {input, ""};
    }
    return {input.substr(0, pos), input.substr(pos + 1)};
}


// Reads out word[], does a string compare and returns type id
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


// Scenario section types
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

cIni::cIni(sGameServices* services)
{
    m_settings = services->settings;
    m_objects = services->objects;
    m_infos = services->info;
    m_interface = services->ctx->getGameInterface();
    m_services = services;
    m_gameConditionChecker = services->gameConditionChecker;
}

bool isCommentLine(const std::string& linefeed)
{
    if (linefeed.empty()) return true;
    char first = linefeed[0];
    if (first == ';' || first == '#' || first == '\n' || first == '\0') return true;
    if (first == '/' && linefeed.size() > 1 && linefeed[1] == '/') return true;
    return false;
}

std::string INI_GetHouseDirectoryName(int iHouse)
{
    switch (iHouse) {
        case ATREIDES:   return "atreides";
        case HARKONNEN:  return "harkonnen";
        case SARDAUKAR:  return "sardaukar";
        case ORDOS:      return "ordos";
        case FREMEN:     return "fremen";
        case MERCENARY:  return "mercenary";
        default:         return "unknown";
    }
}

void cIni::loadRegionfile(std::span<cRegion> world, int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState)
{
    auto filename = std::format("campaign/{}/mission{}.ini", INI_GetHouseDirectoryName(iHouse), iMission);
    Logger::info(COMP_REGIONINI, "cIni::loadRegionfile", "Opening mission file: {}", filename);

    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::error(COMP_REGIONINI, "cIni::loadRegionfile", "[CAMPAIGN] Error, could not open file");
        return;
    }

    eIniWord wordtype = eIniWord::NONE;
    int iRegionIndex = -1;
    int iRegionNumber = -1;
    int iRegionConquer = -1;

    std::string line;
    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if (line.empty() || isCommentLine(line)) {
            continue;
        }

        auto [word_left, word_right] = INI_SplitWord(line);
        if (word_left.empty() || word_right.empty()) {
            Logger::warn(COMP_REGIONINI, "cIni::loadRegionfile", "Skipping invalid line: '{}'", line);
            continue;
        }

        wordtype = INI_WordType(word_left, eIniSection::REGION);

        if (wordtype == eIniWord::REGION) {
            iRegionNumber = -1;
            iRegionConquer = -1;
            iRegionNumber = ToInt(word_right) - 1;
        }
        else if (wordtype == eIniWord::REGIONCONQUER) {
            iRegionNumber = -1;
            iRegionConquer = -1;
            iRegionIndex++;
            iRegionConquer = ToInt(word_right) - 1;
            selectYourNextConquestState->setRegionConquer(iRegionIndex, iRegionConquer);
        }

        if (iRegionIndex > -1 || iRegionNumber > -1) {
            if (wordtype == eIniWord::REGIONHOUSE) {
                Logger::info(COMP_REGIONINI, "cIni::loadRegionfile", "Region house");
                int iH = cIniUtils::getHouseFromString(word_right);
                if (iRegionNumber > -1) {
                    world[iRegionNumber].iHouse = iH;
                    world[iRegionNumber].iAlpha = 255;
                }
                if (iRegionConquer > -1) {
                    selectYourNextConquestState->setRegionHouse(iRegionIndex, iH);
                }
            }

            if (wordtype == eIniWord::REGIONTEXT && iRegionConquer > -1 && iRegionIndex > -1) {
                std::string text = removeQuote(word_right);
                selectYourNextConquestState->setRegionText(iRegionIndex, text.c_str());
            }

            if (wordtype == eIniWord::REGIONSELECT) {
                if (iRegionNumber > -1) {
                    world[iRegionNumber].bSelectable = ToBool(word_right);
                    world[iRegionNumber].iAlpha = 1;
                }
            }
        }
    }
    file.close();
    Logger::info(COMP_REGIONINI, "cIni::loadRegionfile", "[CAMPAIGN] Done");
}

int getTechLevelByRegion(int iRegion)
{
    if (iRegion == 1) return 1;
    if (iRegion == 2 || iRegion == 3 || iRegion == 4) return 2;
    if (iRegion == 5 || iRegion == 6 || iRegion == 7) return 3;
    if (iRegion == 8 || iRegion == 9 || iRegion == 10) return 4;
    if (iRegion == 11 || iRegion == 12 || iRegion == 13) return 5;
    if (iRegion == 14 || iRegion == 15 || iRegion == 16) return 6;
    if (iRegion == 17 || iRegion == 18 || iRegion == 19) return 7;
    if (iRegion == 20 || iRegion == 21) return 8;
    return 9;
}


namespace {

void readScenarioHouseLine(ScenarioData& data, eIniWord wordtype, int iPlayerID, const std::string& value)
{
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Section is between atreides and mercenary, the playerId is [{}]. WordType is [{}]", iPlayerID, std::to_underlying(wordtype));
    if (iPlayerID < 0) return;

    if (wordtype == eIniWord::BRAIN) {
        Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Brain is [{}]", value);
        if (cIniUtils::caseInsCompare(value, "Human")) {
            Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Found human player for id [{}]", iPlayerID);
            data.humanPlayerId = iPlayerID;
        } else {
            Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "This brain is not human...");
        }
    }
    else if (wordtype == eIniWord::CREDITS) {
        int credits = ToInt(value) - 1;
        Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Set credits for player id [{}] to [{}]", iPlayerID, credits);
        data.playerCredits[iPlayerID] = credits;
    }
    else if (wordtype == eIniWord::QUOTA) {
        data.playerQuota[iPlayerID] = ToInt(value);
    }
}

void readScenarioMapLine(ScenarioData& data, eIniWord wordtype, const std::string& slinefeed)
{
    std::optional<int> seed;
    if (wordtype == eIniWord::MAPSEED) {
        Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_MAP", "[SCENARIO] -> [MAP] Seed=");
        auto [word, seek] = INI_SplitWord(slinefeed);
        seed = ToInt(seek);
    }
    data.mapLines.push_back(seed);

    auto parseCells = [&](std::span<int> dest, const char* label) {
        Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_MAP", "[SCENARIO] -> [MAP] {}=", label);
        auto [key, values] = INI_SplitWord(slinefeed);
        int idx = 0;
        for (const auto& token : splitByComma(values)) {
            if (idx >= static_cast<int>(dest.size())) break;
            auto trimmed = ToTrim(token);
            if (trimmed.empty()) continue;
            dest[idx++] = ToInt(trimmed);
        }
    };

    if (wordtype == eIniWord::MAPBLOOM) {
        parseCells(data.blooms, "Bloom");
    }
    else if (wordtype == eIniWord::MAPFIELD) {
        parseCells(data.fields, "Field");
    }
}

void readScenarioReinforcementLine(ScenarioData& data, const std::string& slinefeed)
{
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Reinforcements", "[SCENARIO] -> REINFORCEMENTS");

    auto eqPos = slinefeed.find('=');
    if (eqPos == std::string::npos) return;

    auto parts = splitByComma(slinefeed.substr(eqPos + 1));
    if (parts.size() < 4) {
        Logger::warn(COMP_SCENARIOINI, "INI_Scenario_Section_Reinforcements", "Invalid reinforcement line: '{}'", slinefeed);
        return;
    }

    ScenarioData::Reinforcement reinforcement;

    // Part 0: house
    reinforcement.house = cIniUtils::getHouseFromString(ToTrim(parts[0]));

    // Part 1: unit type
    reinforcement.unitType = cIniUtils::getUnitTypeFromString(ToTrim(parts[1]));

    // Part 2: target cell (Homebase / Enemybase / numeric cell)
    auto location = ToTrim(parts[2]);
    if (cIniUtils::caseInsCompare(location, "Homebase")) {
        reinforcement.location = ScenarioData::Reinforcement::Location::HOMEBASE;
    }
    else if (cIniUtils::caseInsCompare(location, "enemybase")) {
        reinforcement.location = ScenarioData::Reinforcement::Location::ENEMYBASE;
    }
    else if (!location.empty() &&
             std::all_of(location.begin(), location.end(), [](unsigned char c) { return std::isdigit(c); })) {
        reinforcement.location = ScenarioData::Reinforcement::Location::CELL;
        reinforcement.cell = ToInt(location);
    }
    else {
        Logger::warn(COMP_SCENARIOINI, "INI_Scenario_Section_Reinforcements", "Invalid target cell for reinforcement: '{}'", location);
    }

    // Part 3: delay in minutes, optional '+' suffix means repeat
    auto delayStr = ToTrim(parts[3]);
    if (!delayStr.empty() && delayStr.back() == '+') {
        reinforcement.repeat = true;
        delayStr.pop_back();
    }
    // D2TM does not interpret the delay as real minutes; multiplier chosen to feel correct in-game
    reinforcement.delay = ToInt(delayStr) * 20;
    data.entries.push_back(reinforcement);
}

void readScenarioStructureLine(ScenarioData& data, const std::string& slinefeed)
{
    auto [key, valueStr] = INI_SplitWord(slinefeed);
    auto parts = splitByComma(valueStr);

    ScenarioData::Structure structure;
    structure.generated = key.rfind("GEN", 0) == 0; // line is a GENxxx= entry

    if (!structure.generated) {
        // Format: ID=House,StructureType,HP,Cell
        if (parts.size() >= 4) {
            structure.houseName = parts[0];
            structure.house = cIniUtils::getHouseFromString(ToTrim(parts[0]));
            structure.type = cIniUtils::getStructureTypeFromString(ToTrim(parts[1]));
            structure.cell = ToInt(ToTrim(parts[3]));
        }
    }
    else {
        // Format: GENcell=House,StructureType
        structure.cell = ToInt(key.substr(3)); // cell number follows "GEN"
        if (parts.size() >= 2) {
            structure.houseName = parts[0];
            structure.house = cIniUtils::getHouseFromString(ToTrim(parts[0]));
            auto typeName = ToTrim(parts[1]);
            if (cIniUtils::caseInsCompare(typeName, "Wall"))     structure.type = WALL;
            if (cIniUtils::caseInsCompare(typeName, "Concrete")) structure.type = SLAB1;
        }
    }
    data.entries.push_back(structure);
}

void readScenarioUnitLine(ScenarioData& data, const std::string& linefeed)
{
    // Format: ID=House,UnitType,HP,Cell,FacingBody,FacingHead
    auto [key, valueStr] = INI_SplitWord(linefeed);
    auto parts = splitByComma(valueStr);

    EnumArray<std::optional<std::string>, eIniUnitsPart> fields;
    for (std::size_t i = 0; i < parts.size() && i < fields.size(); i++) {
        fields.begin()[i] = parts[i];
    }

    ScenarioData::Unit unit;
    if (const auto& controller = fields[eIniUnitsPart::CONTROLLER]) {
        unit.houseName = *controller;
        unit.house = cIniUtils::getHouseFromString(ToTrim(*controller));
    }
    if (const auto& type = fields[eIniUnitsPart::TYPE]) {
        unit.type = cIniUtils::getUnitTypeFromString(ToTrim(*type));
    }
    // eIniUnitsPart::HP skipped
    if (const auto& cell = fields[eIniUnitsPart::CELL]) {
        unit.cell = ToInt(ToTrim(*cell));
    }
    // eIniUnitsPart::FACING_BODY and beyond are not used
    data.entries.push_back(unit);
}

void readScenarioBasicLine(ScenarioData& data, eIniWord wordtype, const std::string& value)
{
    if (wordtype == eIniWord::BRIEFPICTURE) {
        std::string scene = cIniUtils::getSceneFileToScene(value);
        if (!cIniUtils::caseInsCompare(scene, "unknown")) {
            data.briefScene = scene;
        }
    }
    else if (wordtype == eIniWord::FOCUS_POS) {
        data.focusCell = ToInt(value);
    }
    else if (wordtype == eIniWord::WINFLAGS) {
        data.winFlags = ToInt(value);
    }
    else if (wordtype == eIniWord::LOSEFLAGS) {
        data.loseFlags = ToInt(value);
    }
}

// Reads the scenario file into ScenarioData. Does not touch any game object.
std::optional<ScenarioData> readScenarioFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::error(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Error, could not open file");
        return std::nullopt;
    }

    ScenarioData data;
    eIniSection section = eIniSection::NONE;
    int iPlayerID = -1;

    std::string linefeed;
    while (std::getline(file, linefeed)) {
        linefeed.erase(std::remove(linefeed.begin(), linefeed.end(), '\r'), linefeed.end());
        if (linefeed.empty() || isCommentLine(linefeed)) {
            continue;
        }

        auto sectionName = extractSectionName(linefeed);
        if (!sectionName.empty()) {
            eIniSection sectionType = SCEN_INI_SectionType(sectionName);
            if (sectionType != eIniSection::NONE) {
                section = sectionType;

                Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] found section '{}', resulting in section id [{}]", sectionName, std::to_underlying(section));

                if (isHouseSection(section)) {
                    iPlayerID = std::min(iPlayerID + 1, MAX_PLAYERS - 1);

                    int house = -1;
                    if (section == eIniSection::HOUSEATREIDES) house = ATREIDES;
                    if (section == eIniSection::HOUSEORDOS)    house = ORDOS;
                    if (section == eIniSection::HOUSEHARKONNEN) house = HARKONNEN;
                    if (section == eIniSection::HOUSEMERCENARY) house = MERCENARY;
                    if (section == eIniSection::HOUSEFREMEN)   house = FREMEN;
                    if (section == eIniSection::HOUSESARDAUKAR) house = SARDAUKAR;

                    data.playerHouse[iPlayerID] = house;

                    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Setting house to [{}] for playerId [{}]", data.playerHouse[iPlayerID], iPlayerID);
                }
            }
            continue;
        }

        if (section == eIniSection::NONE) continue;

        auto [word_left, word_right] = INI_SplitWord(linefeed);
        eIniWord wordtype = INI_WordType(word_left, section);

        if (section == eIniSection::BASIC) {
            readScenarioBasicLine(data, wordtype, word_right);
        }

        if (isHouseSection(section)) {
            readScenarioHouseLine(data, wordtype, iPlayerID, word_right);
        }

        if (section == eIniSection::MAP) {
            readScenarioMapLine(data, wordtype, linefeed);
        }
        else if (section == eIniSection::UNITS) {
            readScenarioUnitLine(data, linefeed);
        }
        else if (section == eIniSection::STRUCTURES) {
            readScenarioStructureLine(data, linefeed);
        }
        else if (section == eIniSection::REINFORCEMENTS) {
            readScenarioReinforcementLine(data, linefeed);
        }
    }
    return data;
}

} // namespace

void cIni::loadScenario(AbstractMentat *pMentat, cReinforcements *reinforcements, s_DataCampaign *dataCampaign)
{
    m_settings->setSkirmish(false);
    m_interface->missionInit();
    int iHouse = dataCampaign->housePlayer;
    int iRegion = dataCampaign->region;

    std::string filename = cIniUtils::getScenarioFileName(iHouse, iRegion);

    dataCampaign->mission = getTechLevelByRegion(iRegion);

    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] '{}' (Mission {})", filename, dataCampaign->mission);
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Opening file");

    auto data = readScenarioFile(filename);
    if (!data) return;

    populateScenario(*data, pMentat, reinforcements, iHouse);
}

void cIni::populateScenario(const ScenarioData& data, AbstractMentat *pMentat, cReinforcements *reinforcements, int iHouse)
{
    if (data.briefScene) {
        pMentat->loadScene(*data.briefScene);
    }
    if (data.focusCell) {
        m_objects->getPlayer(0)->setFocusCell(*data.focusCell);
        m_services->mapCamera->centerAndJumpViewPortToCell(*data.focusCell);
    }
    if (data.winFlags) {
        m_gameConditionChecker->setWinFlags(*data.winFlags);
    }
    if (data.loseFlags) {
        m_gameConditionChecker->setLoseFlags(*data.loseFlags);
    }

    auto mapEditor = cMapEditor(m_objects->getMap());

    for (const auto& seed : data.mapLines) {
        m_objects->getMap()->init(64, 64);
        if (seed) {
            SeedMapLoader::loadIntoMap(m_objects->getMap(), m_objects->getMapGeometry(), *seed);
        }
    }

    // setupPlayers is required, because we do matching based on name of house, hence, once
    // we encounter either a UNITS or STRUCTURES entry, the assumption is made that all HOUSE information
    // has been processed.
    bool bSetUpPlayers = true;
    auto setupPlayersOnce = [&]() {
        if (!bSetUpPlayers) return;
        INI_Scenario_SetupPlayers(data.humanPlayerId, data.playerCredits, data.playerHouse, data.playerQuota);
        bSetUpPlayers = false;
    };

    auto populateReinforcement = [&](const ScenarioData::Reinforcement& reinforcement) {
        int playerId = -1;
        if (reinforcement.house > -1) {
            playerId = findPlayerIdByHouse(reinforcement.house);
        }

        int targetCell = -1;
        switch (reinforcement.location) {
            case ScenarioData::Reinforcement::Location::HOMEBASE:
                targetCell = m_objects->getPlayer(playerId)->getFocusCell();
                break;
            case ScenarioData::Reinforcement::Location::ENEMYBASE:
                if (playerId == 0) {
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        if (m_objects->getPlayer(i)->getHouse() == iHouse && i != playerId) {
                            targetCell = m_objects->getPlayer(i)->getFocusCell();
                            break;
                        }
                    }
                }
                else {
                    targetCell = m_objects->getPlayer(0)->getFocusCell();
                }
                break;
            case ScenarioData::Reinforcement::Location::CELL:
                targetCell = reinforcement.cell;
                break;
            case ScenarioData::Reinforcement::Location::INVALID:
                break;
        }

        bool repeat = reinforcement.repeat && m_settings->isAllowRepeatingReinforcements();
        reinforcements->addReinforcement(playerId, reinforcement.unitType, targetCell, reinforcement.delay, repeat);
    };

    for (const auto& entry : data.entries) {
        if (const auto* unit = std::get_if<ScenarioData::Unit>(&entry)) {
            setupPlayersOnce();
            int iController = -1;
            if (unit->house) {
                iController = findPlayerIdByHouse(*unit->house);
                if (iController < 0) {
                    Logger::warn(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Units", "WARNING: Cannot identify house/controller -> STRING '{}'", unit->houseName);
                }
            }
            if (iController > -1) {
                cUnits::unitCreate(m_objects, m_infos, m_interface, unit->cell, unit->type, iController, true);
            }
        }
        else if (const auto* structure = std::get_if<ScenarioData::Structure>(&entry)) {
            setupPlayersOnce();
            int iController = -1;
            if (structure->house) {
                iController = findPlayerIdByHouse(*structure->house);
                if (iController < 0 && !structure->generated) {
                    Logger::warn(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Structures", "WARNING: Identifying house/controller of structure (typo?): {}", structure->houseName);
                }
            }
            if (iController > -1) {
                m_objects->getPlayer(iController)->placeStructure(structure->cell, structure->type, 100);
            }
            else {
                Logger::warn(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Structures", "WARNING: Identifying house/controller of structure (typo?)");
            }
        }
        else if (const auto* reinforcement = std::get_if<ScenarioData::Reinforcement>(&entry)) {
            populateReinforcement(*reinforcement);
        }
    }

    mapEditor.smoothMap();

    auto toMapCell = [&](int original) {
        return m_objects->getMapGeometry()->makeCell(original % 64, original / 64);
    };

    for (int bloom : data.blooms) {
        if (bloom > -1) {
            int cell = toMapCell(bloom);
            if (cell > -1) {
                Logger::debug(COMP_SCENARIOINI, "[SCENARIO]", "Placing spice BLOOM at cell : {}", cell);
                mapEditor.createCell(cell, TERRAIN_BLOOM, 0);
            }
        }
    }

    for (int field : data.fields) {
        if (field > -1) {
            int cell = toMapCell(field);
            if (cell > -1) {
                Logger::debug(COMP_SCENARIOINI, "[SCENARIO]", "Placing spice FIELD at cell : {}", cell);
                mapEditor.createRandomField(cell, TERRAIN_SPICE, 25 + (RNG::rnd(50)));
            }
        }
    }

    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Done reading");

    mapEditor.smoothMap();
    m_objects->getMap()->setDesiredAmountOfWorms(m_objects->getPlayer(AI_WORM)->getAmountOfUnitsForType(SANDWORM));
}

int cIni::findPlayerIdByHouse(int house) const
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_objects->getPlayer(i)->getHouse() == house) {
            return i;
        }
    }
    return -1;
}

void cIni::INI_Scenario_SetupPlayers(int iHumanID, std::span<const int> iPl_credits, std::span<const int> iPl_house, std::span<const int> iPl_quota)
{
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_SetupPlayers", "INI: Going to setup players");
    int iCPUId = 1; // index for CPU's, starts at 1 because ID 0 is HUMAN player

    int teamIndexAI = 1;
    bool fremenIsHumanAlly = false;

    for (int playerIndex = 0; playerIndex < MAX_PLAYERS; playerIndex++) {
        int houseForPlayer = iPl_house[playerIndex];
        Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_SetupPlayers", "House for id [{}] is [{}] - human id is [{}]", playerIndex, houseForPlayer, iHumanID);
        if (houseForPlayer > -1) {
            int creditsPlayer = iPl_credits[playerIndex];
            int quota = iPl_quota[playerIndex];

            if (playerIndex == iHumanID) {
                Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_SetupPlayers", "INI: Setting up human player, credits to [{}], house [{}] and team [{}]", creditsPlayer, houseForPlayer, 0);
                m_objects->getPlayer(HUMAN)->setCredits(creditsPlayer);
                m_objects->getPlayer(HUMAN)->setHouse(houseForPlayer);
                m_objects->getPlayer(HUMAN)->setTeam(0);
                m_objects->getPlayer(HUMAN)->setAutoSlabStructures(false);

                m_objects->getPlayer(AI_CPU5)->setHouse(FREMEN);
                if (houseForPlayer == ATREIDES) {
                    fremenIsHumanAlly = true;
                }

                d2tm_assert(m_services->drawManager);
                m_services->drawManager->missionInit();

                if (quota > 0) {
                    m_objects->getPlayer(HUMAN)->setQuota(quota);
                }
            }
            else {
                m_objects->getPlayer(iCPUId)->setAutoSlabStructures(true);

                if (quota > 0) {
                    m_objects->getPlayer(iCPUId)->setQuota(quota);
                }

                if (houseForPlayer == FREMEN) {
                    d2tm_assert(false && "No FREMEN supported in INI files yet");
                }

                m_objects->getPlayer(iCPUId)->setTeam(teamIndexAI);

                Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_SetupPlayers", "INI: Setting up CPU player, credits to [{}], house to [{}] and team [{}]", creditsPlayer, houseForPlayer, teamIndexAI);

                m_objects->getPlayer(iCPUId)->setCredits(creditsPlayer);
                m_objects->getPlayer(iCPUId)->setHouse(houseForPlayer);
                iCPUId++;
            }
        }
    }

    // CPU slots this scenario doesn't use may still hold a house from the region screen
    // (which sets house = player index); clear them so they don't count as enemies
    for (int unusedCPUId = iCPUId; unusedCPUId < AI_CPU5; unusedCPUId++) {
        m_objects->getPlayer(unusedCPUId)->setHouse(GENERALHOUSE);
    }

    if (fremenIsHumanAlly) {
        m_objects->getPlayer(AI_CPU5)->setTeam(0);
    }
    else {
        m_objects->getPlayer(AI_CPU5)->setTeam(teamIndexAI);
    }

    m_objects->getPlayer(AI_WORM)->setTeam(2); // the WORM player is nobody's ally, ever
}

void cIni::loadBriefing(int iHouse, int iScenarioFind, eIniSection iSectionFind, AbstractMentat *pMentat)
{
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_Briefing", "[BRIEFING] Opening file");

    std::string filename;
    if (iHouse == ATREIDES)  filename = "mentata.ini";
    if (iHouse == ORDOS)     filename = "mentato.ini";
    if (iHouse == HARKONNEN) filename = "mentath.ini";
    if (iHouse == SARDAUKAR) filename = "mentats.ini";

    pMentat->initSentences();
    auto path = std::string("campaign/briefings/") + filename;

    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_Briefing", "{}", path);
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_Briefing", "Going to find SCEN ID #{} and SectionID {}", iScenarioFind, std::to_underlying(iSectionFind));

    int iScenario = 0;
    eIniSection iSection = eIniSection::HOUSES;
    int iLine = 0;

    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::error(COMP_SCENARIOINI, "cIni::INI_Load_Briefing", "[BRIEFING] Error, could not open file");
        return;
    }

    std::string linefeed;
    while (std::getline(file, linefeed)) {
        linefeed.erase(std::remove(linefeed.begin(), linefeed.end(), '\r'), linefeed.end());
        if (linefeed.empty() || isCommentLine(linefeed)) {
            continue;
        }

        auto sectionName = extractSectionName(linefeed);
        if (!sectionName.empty()) {
            iSection = cIniUtils::getSectionType(sectionName, iSection);
        }

        if (iSection == eIniSection::SCEN) {
            auto [word_left, word_right] = INI_SplitWord(linefeed);
            eIniWord wordtype = INI_WordType(word_left, iSection);

            if (wordtype == eIniWord::NUMBER) {
                iScenario = ToInt(word_right);
                continue;
            }
        }

        if (iScenario == iScenarioFind) {
            if (iSection == iSectionFind) {
                auto [word_left, word_right] = INI_SplitWord(linefeed);
                eIniWord wordtype = INI_WordType(word_left, iSection);

                if (wordtype == eIniWord::REGIONTEXT) {
                    word_right = removeQuote(word_right);
                    pMentat->setSentence(iLine, word_right.c_str());
                    iLine++;
                    if (iLine > 9) break;
                }
            }
        }
    }
    file.close();
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_Briefing", "[BRIEFING] File opened");
}


// Game.ini loader
void cIni::installGame(const std::string& filename)
{
    Logger::info(COMP_GAMERULES, "cIni::installGame", "[GameRules] Opening file");

    eIniSection section = eIniSection::GAME;
    eIniWord wordtype = eIniWord::NONE;
    int id = -1;

    Logger::info(COMP_GAMERULES, "cIni::installGame", "Opening game settings from : {}", filename);

    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::error(COMP_GAMERULES, "cIni::installGame", "[GameRules] Error, could not open file");
        return;
    }

    std::string linefeed;
    while (std::getline(file, linefeed)) {
        linefeed.erase(std::remove(linefeed.begin(), linefeed.end(), '\r'), linefeed.end());
        if (linefeed.empty() || isCommentLine(linefeed)) {
            continue;
        }

        wordtype = eIniWord::NONE;

        auto sectionName = extractSectionName(linefeed);
        if (!sectionName.empty()) {
            eIniSection last = section;
            section = GAME_INI_SectionType(sectionName, section);

            if (last != section) {
                id = -1;
                if (section == eIniSection::UNITS)      Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] -> [UNITS]");
                if (section == eIniSection::STRUCTURES) Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] -> [STRUCTURES]");
                if (section == eIniSection::BULLETS)    Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] -> [BULLETS]");
            }

            if (section == eIniSection::UNITS && linefeed.find("[UNIT:") != std::string::npos) {
                id = cIniUtils::getUnitTypeFromString(extractTagValue(linefeed, "[UNIT:"));
                if (id >= MAX_UNITTYPES) id--;
            }

            if (section == eIniSection::STRUCTURES && linefeed.find("[STRUCTURE:") != std::string::npos) {
                id = cIniUtils::getStructureType(extractTagValue(linefeed, "[STRUCTURE:"));
                if (id >= MAX_STRUCTURETYPES) id--;
            }

            if (section == eIniSection::BULLETS && linefeed.find("[BULLET:") != std::string::npos) {
                id = cIniUtils::getBulletTypeFromString(extractTagValue(linefeed, "[BULLET:"));
                const int bulletInfoCount = m_infos->getBulletInfos()->size();
                if (id >= 0 && id >= bulletInfoCount) {
                    id--;
                }
            }

            continue;
        }

        if (section == eIniSection::GAME) continue;

        auto [word_left, word_right] = INI_SplitWord(linefeed);
        wordtype = INI_WordType(word_left, section);

        /**** [UNITS] ****/
        if (section == eIniSection::UNITS && id > -1) {
            s_UnitInfo &unitInfo = m_infos->getUnitInfo(id);

            if (wordtype == eIniWord::HITPOINTS)     unitInfo.hp = ToInt(word_right);
            if (wordtype == eIniWord::APPETITE)      unitInfo.appetite = ToInt(word_right);
            if (wordtype == eIniWord::COST)          unitInfo.cost = ToInt(word_right);
            if (wordtype == eIniWord::MOVESPEED)     unitInfo.speed = ToInt(word_right);
            if (wordtype == eIniWord::TURNSPEED)     unitInfo.turnspeed = ToInt(word_right);
            if (wordtype == eIniWord::ATTACKFREQ)    unitInfo.attack_frequency = ToInt(word_right);
            if (wordtype == eIniWord::NEXTATTACKFREQ) unitInfo.next_attack_frequency = ToInt(word_right);
            if (wordtype == eIniWord::SIGHT)         unitInfo.sight = ToInt(word_right);
            if (wordtype == eIniWord::RANGE)         unitInfo.range = ToInt(word_right);
            if (wordtype == eIniWord::BUILDTIME)     unitInfo.buildTime = ToInt(word_right);

            if (wordtype == eIniWord::DESCRIPTION) {
                // unitInfo.name is a legacy char[] field
                strncpy(unitInfo.name, word_right.c_str(), sizeof(unitInfo.name) - 1);
                unitInfo.name[sizeof(unitInfo.name) - 1] = '\0';
            }

            if (wordtype == eIniWord::FIRETWICE)       unitInfo.fireTwice = ToBool(word_right);
            if (wordtype == eIniWord::ISINFANTRY)      unitInfo.infantry = ToBool(word_right);
            if (wordtype == eIniWord::ISSQUISHABLE)    unitInfo.canBeSquished = ToBool(word_right);
            if (wordtype == eIniWord::CANSQUISH)       unitInfo.canBeSquished = ToBool(word_right);
            if (wordtype == eIniWord::FREEROAM)        unitInfo.free_roam = ToBool(word_right);
            if (wordtype == eIniWord::ISAIRBORN)       unitInfo.airborn = ToBool(word_right);
            if (wordtype == eIniWord::HARVESTLIMIT)    unitInfo.credit_capacity = ToInt(word_right);
            if (wordtype == eIniWord::HARVESTSPEED)    unitInfo.harvesting_speed = ToInt(word_right);
            if (wordtype == eIniWord::HARVESTAMOUNT)   unitInfo.harvesting_amount = ToInt(word_right);
            if (wordtype == eIniWord::SMOKEHFACTOR)    unitInfo.smokeHpFactor = ToFloat(word_right);
            if (wordtype == eIniWord::SQUISH)          unitInfo.canBeSquished = ToBool(word_right);
            if (wordtype == eIniWord::CANATTACKAIRUNITS) unitInfo.canAttackAirUnits = ToBool(word_right);
            if (wordtype == eIniWord::CANATTACKUNITS)  unitInfo.canAttackUnits = ToBool(word_right);

            if (wordtype == eIniWord::PRODUCER) {
                unitInfo.structureTypeItLeavesFrom = cIniUtils::getStructureType(word_right);
            }
        }

        if (section == eIniSection::TERRAINS && id > -1) {
            if (wordtype == eIniWord::BLOOMTIMERDURATION)    m_infos->getTerrainInfo()->bloomTimerDuration = ToInt(word_right);
            if (wordtype == eIniWord::TERRAIN_MINSPICE)      m_infos->getTerrainInfo()->terrainSpiceMinSpice = ToInt(word_right);
            if (wordtype == eIniWord::TERRAIN_MAXSPICE)      m_infos->getTerrainInfo()->terrainSpiceMaxSpice = ToInt(word_right);
            if (wordtype == eIniWord::TERRAINHILL_MINSPICE)  m_infos->getTerrainInfo()->terrainSpiceHillMinSpice = ToInt(word_right);
            if (wordtype == eIniWord::TERRAINHILL_MAXSPICE)  m_infos->getTerrainInfo()->terrainSpiceHillMaxSpice = ToInt(word_right);
            if (wordtype == eIniWord::TERRAINWALL_HP)        m_infos->getTerrainInfo()->terrainWallHp = ToInt(word_right);
        }

        if (section == eIniSection::STRUCTURES && id > -1) {
            if (wordtype == eIniWord::HITPOINTS)    m_infos->getStructureInfo(id).hp = ToInt(word_right);
            if (wordtype == eIniWord::FIXHP)        m_infos->getStructureInfo(id).fixhp = ToInt(word_right);
            if (wordtype == eIniWord::POWERDRAIN)   m_infos->getStructureInfo(id).power_drain = ToInt(word_right);
            if (wordtype == eIniWord::HAS_CONCRETE) m_infos->getStructureInfo(id).hasConcrete = ToBool(word_right);
            if (wordtype == eIniWord::POWERGIVE)    m_infos->getStructureInfo(id).power_give = ToInt(word_right);
            if (wordtype == eIniWord::COST)         m_infos->getStructureInfo(id).cost = ToInt(word_right);
            if (wordtype == eIniWord::BUILDTIME)    m_infos->getStructureInfo(id).buildTime = ToInt(word_right);
            if (wordtype == eIniWord::CANATTACKAIRUNITS) m_infos->getStructureInfo(id).canAttackAirUnits = ToBool(word_right);
            if (wordtype == eIniWord::CANATTACKUNITS)    m_infos->getStructureInfo(id).canAttackGroundUnits = ToBool(word_right);
            if (wordtype == eIniWord::SIGHT)        m_infos->getStructureInfo(id).sight = ToInt(word_right);
            if (wordtype == eIniWord::RANGE)        m_infos->getStructureInfo(id).range = ToInt(word_right);
            if (wordtype == eIniWord::FIRERATE)     m_infos->getStructureInfo(id).fireRate = ToInt(word_right);
            if (wordtype == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MIN) m_infos->getStructureInfo(id).uponDestructionSpawnUnitAmountMin = ToInt(word_right);
            if (wordtype == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MAX) m_infos->getStructureInfo(id).uponDestructionSpawnUnitAmountMax = ToInt(word_right);
            if (wordtype == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_TYPE)       m_infos->getStructureInfo(id).uponDestructionSpawnUnitType = cIniUtils::getUnitTypeFromString(word_right);
        }

        if (section == eIniSection::BULLETS && id > -1) {
            if (wordtype == eIniWord::DAMAGE_VEHICLE)     m_infos->getBulletInfo(id).damage_vehicles = ToInt(word_right);
            if (wordtype == eIniWord::DAMAGE_INFANTRY)    m_infos->getBulletInfo(id).damage_infantry = ToInt(word_right);
            if (wordtype == eIniWord::DEVIATE_PROBABILITY) m_infos->getBulletInfo(id).deviateProbability = ToInt(word_right);
            if (wordtype == eIniWord::EXPLOSION_SIZE)     m_infos->getBulletInfo(id).explosionSize = ToInt(word_right);
            if (wordtype == eIniWord::GROUND_BULLET)      m_infos->getBulletInfo(id).groundBullet = ToBool(word_right);
        }

    } // while
    file.close();

    Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] Done");
}
