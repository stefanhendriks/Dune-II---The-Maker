/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "utils/ini.h"
#include "iniDefine.h"
#include "utils/cIniUtils.h"

#include "data/gfxdata.h"
#include "definitions.h"
#include "include/sDataCampaign.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "managers/cDrawManager.h" // TODO: an ini file reader should not depend on drawing code
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/cMapEditor.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/mentat/AbstractMentat.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "utils/Log.h"
#include "utils/common.h"
#include "utils/cSeedMapGenerator.h"
#include "gameobjects/units/cReinforcements.h"
#include "gameobjects/sTerrainInfo.h"
#include "utils/RNG.hpp"
#include "gameobjects/units/cUnits.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "include/sGameServices.h"
#include "context/GameContext.hpp"
#include "game/cGameInterface.h"
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
int INI_WordType(const std::string& word, int section)
{
    if (section == SEC_REGION) {
        if (cIniUtils::caseInsCompare(word, "Region"))            return WORD_REGION;
        if (cIniUtils::caseInsCompare(word, "RegionConquer"))     return WORD_REGIONCONQUER;
        if (cIniUtils::caseInsCompare(word, "House"))             return WORD_REGIONHOUSE;
        if (cIniUtils::caseInsCompare(word, "Text"))              return WORD_REGIONTEXT;
        if (cIniUtils::caseInsCompare(word, "Select"))            return WORD_REGIONSELECT;
        return WORD_NONE;
    }

    if (section == INI_SCEN ||
            section == INI_DESCRIPTION ||
            section == INI_BRIEFING ||
            section == INI_ADVICE ||
            section == INI_LOSE ||
            section == INI_WIN) {
        if (cIniUtils::caseInsCompare(word, "Number"))            return WORD_NUMBER;
        if (cIniUtils::caseInsCompare(word, "Text"))              return WORD_REGIONTEXT;
        return WORD_NONE;
    }

    if (section == INI_SKIRMISH) {
        if (cIniUtils::caseInsCompare(word, "Title"))             return WORD_MAPNAME;
        if (cIniUtils::caseInsCompare(word, "StartCell"))         return WORD_STARTCELL;
    }

    if (section == INI_GAME) {
        if (cIniUtils::caseInsCompare(word, "ModId"))             return WORD_MODID;
    }
    else if (section == INI_BASIC) {
        if (cIniUtils::caseInsCompare(word, "CursorPos"))         return WORD_FOCUS;
        if (cIniUtils::caseInsCompare(word, "BriefPicture"))      return WORD_BRIEFPICTURE;
        if (cIniUtils::caseInsCompare(word, "WinFlags"))          return WORD_WINFLAGS;
        if (cIniUtils::caseInsCompare(word, "LoseFlags"))         return WORD_LOSEFLAGS;
    }
    else if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY) {
        if (cIniUtils::caseInsCompare(word, "Team"))              return WORD_TEAM;
        if (cIniUtils::caseInsCompare(word, "Credits"))           return WORD_CREDITS;
        if (cIniUtils::caseInsCompare(word, "Quota"))             return WORD_QUOTA;
        if (cIniUtils::caseInsCompare(word, "Brain"))             return WORD_BRAIN;
        if (cIniUtils::caseInsCompare(word, "House"))             return WORD_HOUSE;
        if (cIniUtils::caseInsCompare(word, "Focus"))             return WORD_FOCUS;
    }
    else if (section == INI_MAP) {
        if (cIniUtils::caseInsCompare(word, "Width"))             return WORD_MAPWIDTH;
        if (cIniUtils::caseInsCompare(word, "Height"))            return WORD_MAPHEIGHT;
        if (cIniUtils::caseInsCompare(word, "Seed"))              return WORD_MAPSEED;
        if (cIniUtils::caseInsCompare(word, "Bloom"))             return WORD_MAPBLOOM;
        if (cIniUtils::caseInsCompare(word, "Field"))             return WORD_MAPFIELD;
    }
    else if (section == INI_BULLETS) {
        if (cIniUtils::caseInsCompare(word, "Bitmap"))            return WORD_BITMAP;
        if (cIniUtils::caseInsCompare(word, "BitmapExplosion"))   return WORD_BITMAP_DEAD;
        if (cIniUtils::caseInsCompare(word, "BitmapWidth"))       return WORD_BITMAP_WIDTH;
        if (cIniUtils::caseInsCompare(word, "BitmapFrames"))      return WORD_BITMAP_FRAMES;
        if (cIniUtils::caseInsCompare(word, "BitmapExplFrames"))  return WORD_BITMAP_DEADFRAMES;
        if (cIniUtils::caseInsCompare(word, "DamageVehicle"))     return WORD_DAMAGE_VEHICLE;
        if (cIniUtils::caseInsCompare(word, "DamageInfantry"))    return WORD_DAMAGE_INFANTRY;
        if (cIniUtils::caseInsCompare(word, "Definition"))        return WORD_DEFINITION;
        if (cIniUtils::caseInsCompare(word, "Sound"))             return WORD_SOUND;
        if (cIniUtils::caseInsCompare(word, "ExplosionSize"))     return WORD_EXPLOSION_SIZE;
        if (cIniUtils::caseInsCompare(word, "DeviateProbability"))return WORD_DEVIATE_PROBABILITY;
        if (cIniUtils::caseInsCompare(word, "GroundBullet"))      return WORD_GROUND_BULLET;
    }
    else if (section == INI_STRUCTURES) {
        if (cIniUtils::caseInsCompare(word, "HitPoints"))         return WORD_HITPOINTS;
        if (cIniUtils::caseInsCompare(word, "FixPoints"))         return WORD_FIXHP;
        if (cIniUtils::caseInsCompare(word, "PowerDrain"))        return WORD_POWERDRAIN;
        if (cIniUtils::caseInsCompare(word, "HasConcrete"))       return WORD_HAS_CONCRETE;
        if (cIniUtils::caseInsCompare(word, "PowerGive"))         return WORD_POWERGIVE;
        if (cIniUtils::caseInsCompare(word, "Cost"))              return WORD_COST;
        if (cIniUtils::caseInsCompare(word, "BuildTime"))         return WORD_BUILDTIME;
        if (cIniUtils::caseInsCompare(word, "UponDestructionSpawnUnitAmountMin")) return WORD_UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MIN;
        if (cIniUtils::caseInsCompare(word, "UponDestructionSpawnUnitAmountMax")) return WORD_UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MAX;
        if (cIniUtils::caseInsCompare(word, "UponDestructionSpawnUnitType"))      return WORD_UPON_DESTRUCTION_SPAWN_UNIT_TYPE;
    }
    else if (section == INI_UNITS) {
        if (cIniUtils::caseInsCompare(word, "Bitmap"))            return WORD_BITMAP;
        if (cIniUtils::caseInsCompare(word, "BitmapTop"))         return WORD_BITMAP_TOP;
        if (cIniUtils::caseInsCompare(word, "Icon"))              return WORD_ICON;
        if (cIniUtils::caseInsCompare(word, "BitmapWidth"))       return WORD_BITMAP_WIDTH;
        if (cIniUtils::caseInsCompare(word, "BitmapHeight"))      return WORD_BITMAP_HEIGHT;
        if (cIniUtils::caseInsCompare(word, "HitPoints"))         return WORD_HITPOINTS;
        if (cIniUtils::caseInsCompare(word, "Appetite"))          return WORD_APPETITE;
        if (cIniUtils::caseInsCompare(word, "Cost"))              return WORD_COST;
        if (cIniUtils::caseInsCompare(word, "BulletType"))        return WORD_BULLETTYPE;
        if (cIniUtils::caseInsCompare(word, "MoveSpeed"))         return WORD_MOVESPEED;
        if (cIniUtils::caseInsCompare(word, "TurnSpeed"))         return WORD_TURNSPEED;
        // Attack frequency (todo: wording, it should be more like "delay" or "fireRate")
        if (cIniUtils::caseInsCompare(word, "AttackFrequency"))   return WORD_ATTACKFREQ;
        // Next Attack frequency (if applicable) (todo: wording, it should be more like "delay" or "fireRate")
        if (cIniUtils::caseInsCompare(word, "NextAttackFrequency")) return WORD_NEXTATTACKFREQ;
        if (cIniUtils::caseInsCompare(word, "Sight"))             return WORD_SIGHT;
        if (cIniUtils::caseInsCompare(word, "Range"))             return WORD_RANGE;
        if (cIniUtils::caseInsCompare(word, "BuildTime"))         return WORD_BUILDTIME;
        if (cIniUtils::caseInsCompare(word, "Description"))       return WORD_DESCRIPTION;
        // BOOLEANS
        if (cIniUtils::caseInsCompare(word, "IsHarvester"))       return WORD_ISHARVESTER;
        if (cIniUtils::caseInsCompare(word, "FireTwice"))         return WORD_FIRETWICE;
        if (cIniUtils::caseInsCompare(word, "IsInfantry"))        return WORD_ISINFANTRY;
        if (cIniUtils::caseInsCompare(word, "Squishable"))        return WORD_ISSQUISHABLE;
        if (cIniUtils::caseInsCompare(word, "CanSquish"))         return WORD_CANSQUISH;
        if (cIniUtils::caseInsCompare(word, "IsAirborn"))         return WORD_ISAIRBORN;
        if (cIniUtils::caseInsCompare(word, "AbleToCarry"))       return WORD_ABLETOCARRY;
        if (cIniUtils::caseInsCompare(word, "FreeRoam"))          return WORD_FREEROAM;
        if (cIniUtils::caseInsCompare(word, "Producer"))          return WORD_PRODUCER;
        if (cIniUtils::caseInsCompare(word, "MaxCredits"))        return WORD_HARVESTLIMIT;
        if (cIniUtils::caseInsCompare(word, "HarvestSpeed"))      return WORD_HARVESTSPEED;
        if (cIniUtils::caseInsCompare(word, "HarvestAmount"))     return WORD_HARVESTAMOUNT;
        if (cIniUtils::caseInsCompare(word, "squish"))            return WORD_SQUISH;
        if (cIniUtils::caseInsCompare(word, "SmokeHpFactor"))     return WORD_SMOKEHFACTOR;
        if (cIniUtils::caseInsCompare(word, "CanAttackAirUnits")) return WORD_CANATTACKAIRUNITS;
        if (cIniUtils::caseInsCompare(word, "CanAttackUnits"))    return WORD_CANATTACKUNITS;
    }
    else if (section == INI_STRUCTURES) {
        if (word.length() > 1) {
            if (cIniUtils::caseInsCompare(word, "PreBuild"))      return WORD_PREBUILD;
            if (cIniUtils::caseInsCompare(word, "Description"))   return WORD_DESCRIPTION;
            if (cIniUtils::caseInsCompare(word, "Power"))         return WORD_POWER;
            if (cIniUtils::caseInsCompare(word, "Sight"))         return WORD_SIGHT;
            if (cIniUtils::caseInsCompare(word, "Range"))         return WORD_RANGE;
            if (cIniUtils::caseInsCompare(word, "FireRate"))      return WORD_FIRERATE;
        }
        else
            return WORD_NONE;
    }
    else if (section == INI_HOUSES) {
        if (cIniUtils::caseInsCompare(word, "ColorR"))            return WORD_RED;
        if (cIniUtils::caseInsCompare(word, "ColorG"))            return WORD_GREEN;
        if (cIniUtils::caseInsCompare(word, "ColorB"))            return WORD_BLUE;
        if (cIniUtils::caseInsCompare(word, "FirePower"))         return WORD_FIREPOWER;
        if (cIniUtils::caseInsCompare(word, "FireRate"))          return WORD_FIRERATE;
        if (cIniUtils::caseInsCompare(word, "StructPrice"))       return WORD_STRUCTPRICE;
        if (cIniUtils::caseInsCompare(word, "UnitPrice"))         return WORD_UNITPRICE;
        if (cIniUtils::caseInsCompare(word, "Speed"))             return WORD_SPEED;
        if (cIniUtils::caseInsCompare(word, "BuildSpeed"))        return WORD_BUILDSPEED;
        if (cIniUtils::caseInsCompare(word, "HarvestSpeed"))      return WORD_HARVESTSPEED;
        if (cIniUtils::caseInsCompare(word, "DumpSpeed"))         return WORD_DUMPSPEED;
    }

    Logger::warn(COMP_NONE, "SCEN_INI_WordType", "Could not find word-type for [{}]", word);
    return WORD_NONE;
}


// Scenario section types
int SCEN_INI_SectionType(const std::string& section)
{
    if (cIniUtils::caseInsCompare(section, "UNITS"))          return INI_UNITS;
    if (cIniUtils::caseInsCompare(section, "SKIRMISH"))       return INI_SKIRMISH;
    if (cIniUtils::caseInsCompare(section, "STRUCTURES"))     return INI_STRUCTURES;
    if (cIniUtils::caseInsCompare(section, "REINFORCEMENTS")) return INI_REINFORCEMENTS;
    if (cIniUtils::caseInsCompare(section, "MAP"))            return INI_MAP;
    if (cIniUtils::caseInsCompare(section, "BASIC"))          return INI_BASIC;
    if (cIniUtils::caseInsCompare(section, "Atreides"))       return INI_HOUSEATREIDES;
    if (cIniUtils::caseInsCompare(section, "Ordos"))          return INI_HOUSEORDOS;
    if (cIniUtils::caseInsCompare(section, "Harkonnen"))      return INI_HOUSEHARKONNEN;
    if (cIniUtils::caseInsCompare(section, "Sardaukar"))      return INI_HOUSESARDAUKAR;
    if (cIniUtils::caseInsCompare(section, "Fremen"))         return INI_HOUSEFREMEN;
    if (cIniUtils::caseInsCompare(section, "Mercenary"))      return INI_HOUSEMERCENARY;
    Logger::warn(COMP_NONE, "SCEN_INI_SectionType", "Could not find section type for [{}]", section);
    return -1;
}


int GAME_INI_SectionType(const std::string& section, int last)
{
    if (cIniUtils::caseInsCompare(section, "BULLETS"))    return INI_BULLETS;
    if (cIniUtils::caseInsCompare(section, "UNITS"))      return INI_UNITS;
    if (cIniUtils::caseInsCompare(section, "STRUCTURES")) return INI_STRUCTURES;
    if (cIniUtils::caseInsCompare(section, "TERRAINS"))   return INI_TERRAINS;
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
}

void cIni::INI_Load_seed(int seed)
{
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_seed", "Generating seed map with seed {}.", seed);

    auto seedGenerator = cSeedMapGenerator(seed);
    auto seedMap = seedGenerator.generateSeedMap();
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_seed", "Seedmap generated");

    auto mapEditor = cMapEditor(m_objects->getMap());
    for (int mapY = 0; mapY < 64; mapY++) {
        for (int mapX = 0; mapX < 64; mapX++) {
            int type = seedMap.getCellType(mapX, mapY);
            int iCell = m_objects->getMapGeometry()->makeCell(mapX, mapY);
            mapEditor.createCell(iCell, type, 0);
        }
    }

    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_seed", "Seedmap converted into D2TM map.");
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

    int wordtype = WORD_NONE;
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

        wordtype = INI_WordType(word_left, SEC_REGION);

        if (wordtype == WORD_REGION) {
            iRegionNumber = -1;
            iRegionConquer = -1;
            iRegionNumber = ToInt(word_right) - 1;
        }
        else if (wordtype == WORD_REGIONCONQUER) {
            iRegionNumber = -1;
            iRegionConquer = -1;
            iRegionIndex++;
            iRegionConquer = ToInt(word_right) - 1;
            selectYourNextConquestState->setRegionConquer(iRegionIndex, iRegionConquer);
        }

        if (iRegionIndex > -1 || iRegionNumber > -1) {
            if (wordtype == WORD_REGIONHOUSE) {
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

            if (wordtype == WORD_REGIONTEXT && iRegionConquer > -1 && iRegionIndex > -1) {
                std::string text = removeQuote(word_right);
                selectYourNextConquestState->setRegionText(iRegionIndex, text.c_str());
            }

            if (wordtype == WORD_REGIONSELECT) {
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

    std::array<int, 30> blooms, fields;
    blooms.fill(-1);
    fields.fill(-1);

    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::error(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Error, could not open file");
        return;
    }

    int section = INI_NONE;
    int iPlayerID = -1;
    int iHumanID = -1;
    bool bSetUpPlayers = true;

    std::array<int, MAX_PLAYERS> iPl_credits{};
    std::array<int, MAX_PLAYERS> iPl_house;
    std::array<int, MAX_PLAYERS> iPl_quota{};
    iPl_house.fill(-1);

    auto mapEditor = cMapEditor(m_objects->getMap());

    std::string linefeed;
    while (std::getline(file, linefeed)) {
        linefeed.erase(std::remove(linefeed.begin(), linefeed.end(), '\r'), linefeed.end());
        if (linefeed.empty() || isCommentLine(linefeed)) {
            continue;
        }

        auto sectionName = extractSectionName(linefeed);
        if (!sectionName.empty()) {
            int sectionType = SCEN_INI_SectionType(sectionName);
            if (sectionType > -1) {
                section = sectionType;

                Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] found section '{}', resulting in section id [{}]", sectionName, section);

                if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY) {
                    iPlayerID = std::min(iPlayerID + 1, MAX_PLAYERS - 1);

                    int house = -1;
                    if (section == INI_HOUSEATREIDES) house = ATREIDES;
                    if (section == INI_HOUSEORDOS)    house = ORDOS;
                    if (section == INI_HOUSEHARKONNEN) house = HARKONNEN;
                    if (section == INI_HOUSEMERCENARY) house = MERCENARY;
                    if (section == INI_HOUSEFREMEN)   house = FREMEN;
                    if (section == INI_HOUSESARDAUKAR) house = SARDAUKAR;

                    iPl_house[iPlayerID] = house;

                    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Setting house to [{}] for playerId [{}]", iPl_house[iPlayerID], iPlayerID);
                }
            }
            continue;
        }

        if (section == INI_NONE) continue;

        auto [word_left, word_right] = INI_SplitWord(linefeed);
        int wordtype = INI_WordType(word_left, section);

        if (section == INI_BASIC) {
            INI_Scenario_Section_Basic(pMentat, wordtype, word_right);
        }

        if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY) {
            int result = INI_Scenario_Section_House(wordtype, iPlayerID, iPl_credits, iPl_quota, word_right);
            if (result > -1) {
                iHumanID = result;
            }
        }

        if (section == INI_MAP) {
            INI_Scenario_Section_MAP(blooms, fields, wordtype, linefeed);
        }
        else if (section == INI_UNITS) {
            bSetUpPlayers = INI_Scenario_Section_Units(iHumanID, bSetUpPlayers, iPl_credits, iPl_house, iPl_quota, linefeed);
        }
        else if (section == INI_STRUCTURES) {
            bSetUpPlayers = INI_Scenario_Section_Structures(iHumanID, bSetUpPlayers, iPl_credits, iPl_house, iPl_quota, linefeed);
        }
        else if (section == INI_REINFORCEMENTS) {
            INI_Scenario_Section_Reinforcements(iHouse, linefeed, reinforcements);
        }
    }
    file.close();

    mapEditor.smoothMap();

    for (int iB = 0; iB < 30; iB++) {
        if (blooms[iB] > -1) {
            Logger::debug(COMP_SCENARIOINI, "[SCENARIO]", "Placing spice BLOOM at cell : {}", blooms[iB]);
            mapEditor.createCell(blooms[iB], TERRAIN_BLOOM, 0);
        }
    }

    for (int iB = 0; iB < 30; iB++) {
        if (fields[iB] > -1) {
            Logger::debug(COMP_SCENARIOINI, "[SCENARIO]", "Placing spice FIELD at cell : {}", fields[iB]);
            mapEditor.createRandomField(fields[iB], TERRAIN_SPICE, 25 + (RNG::rnd(50)));
        }
    }

    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Done reading");

    mapEditor.smoothMap();
    m_objects->getMap()->setDesiredAmountOfWorms(m_objects->getPlayer(AI_WORM)->getAmountOfUnitsForType(SANDWORM));
}

void cIni::INI_Scenario_Section_Basic(AbstractMentat *pMentat, int wordtype, const std::string& value)
{
    if (wordtype == WORD_BRIEFPICTURE) {
        std::string scene = cIniUtils::getSceneFileToScene(value);
        if (!cIniUtils::caseInsCompare(scene, "unknown")) {
            pMentat->loadScene(scene);
        }
    }
    else if (wordtype == WORD_FOCUS) {
        int focusCell = ToInt(value);
        m_objects->getPlayer(0)->setFocusCell(focusCell);
        m_interface->getMapCamera()->centerAndJumpViewPortToCell(focusCell);
    }
    else if (wordtype == WORD_WINFLAGS) {
        m_interface->setWinFlags(ToInt(value));
    }
    else if (wordtype == WORD_LOSEFLAGS) {
        m_interface->setLoseFlags(ToInt(value));
    }
}

int cIni::INI_Scenario_Section_House(int wordtype, int iPlayerID, std::span<int> iPl_credits, std::span<int> iPl_quota, const std::string& value)
{
    int iHumanID = -1;
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Section is between atreides and mercenary, the playerId is [{}]. WordType is [{}]", iPlayerID, wordtype);
    if (iPlayerID > -1) {
        if (wordtype == WORD_BRAIN) {
            Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Brain is [{}]", value);
            if (cIniUtils::caseInsCompare(value, "Human")) {
                Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Found human player for id [{}]", iPlayerID);
                iHumanID = iPlayerID;
            } else {
                Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "This brain is not human...");
            }
        }
        else if (wordtype == WORD_CREDITS) {
            int credits = ToInt(value) - 1;
            Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_House", "Set credits for player id [{}] to [{}]", iPlayerID, credits);
            iPl_credits[iPlayerID] = credits;
        }
        else if (wordtype == WORD_QUOTA) {
            iPl_quota[iPlayerID] = ToInt(value);
        }
    }
    return iHumanID;
}

void cIni::INI_Scenario_Section_MAP(std::span<int> blooms, std::span<int> fields, int wordtype, const std::string& slinefeed)
{
    m_objects->getMap()->init(64, 64);

    if (wordtype == WORD_MAPSEED) {
        Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_MAP", "[SCENARIO] -> [MAP] Seed=");
        auto [word, seek] = INI_SplitWord(slinefeed);
        INI_Load_seed(ToInt(seek));
    }

    auto parseCells = [&](std::span<int> dest, const char* label) {
        Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_MAP", "[SCENARIO] -> [MAP] {}=", label);
        auto [key, values] = INI_SplitWord(slinefeed);
        int idx = 0;
        for (const auto& token : splitByComma(values)) {
            if (idx >= static_cast<int>(dest.size())) break;
            auto trimmed = ToTrim(token);
            if (trimmed.empty()) continue;
            int original = ToInt(trimmed);
            int iCellX = original % 64;
            int iCellY = original / 64;
            dest[idx++] = m_objects->getMapGeometry()->makeCell(iCellX, iCellY);
        }
    };

    if (wordtype == WORD_MAPBLOOM) {
        parseCells(blooms, "Bloom");
    }
    else if (wordtype == WORD_MAPFIELD) {
        parseCells(fields, "Field");
    }
}

void cIni::INI_Scenario_Section_Reinforcements(int iHouse, const std::string& slinefeed, cReinforcements *reinforcements)
{
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Reinforcements", "[SCENARIO] -> REINFORCEMENTS");

    auto eqPos = slinefeed.find('=');
    if (eqPos == std::string::npos) return;

    auto parts = splitByComma(slinefeed.substr(eqPos + 1));
    if (parts.size() < 4) {
        Logger::warn(COMP_SCENARIOINI, "INI_Scenario_Section_Reinforcements", "Invalid reinforcement line: '{}'", slinefeed);
        return;
    }

    // Part 0: house → player id
    int playerId = -1;
    int reinforcingHouse = cIniUtils::getHouseFromString(ToTrim(parts[0]));
    if (reinforcingHouse > -1) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (m_objects->getPlayer(i)->getHouse() == reinforcingHouse) {
                playerId = i;
                break;
            }
        }
    }

    // Part 1: unit type
    int unitType = cIniUtils::getUnitTypeFromString(ToTrim(parts[1]));

    // Part 2: target cell (Homebase / Enemybase / numeric cell)
    int targetCell = -1;
    auto location = ToTrim(parts[2]);
    if (cIniUtils::caseInsCompare(location, "Homebase")) {
        targetCell = m_objects->getPlayer(playerId)->getFocusCell();
    }
    else if (cIniUtils::caseInsCompare(location, "enemybase")) {
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
    }
    else if (!location.empty() &&
             std::all_of(location.begin(), location.end(), [](unsigned char c) { return std::isdigit(c); })) {
        targetCell = ToInt(location);
    }
    else {
        Logger::warn(COMP_SCENARIOINI, "INI_Scenario_Section_Reinforcements", "Invalid target cell for reinforcement: '{}'", location);
    }

    // Part 3: delay in minutes, optional '+' suffix means repeat
    auto delayStr = ToTrim(parts[3]);
    bool repeat = false;
    if (!delayStr.empty() && delayStr.back() == '+') {
        repeat = m_settings->isAllowRepeatingReinforcements();
        delayStr.pop_back();
    }
    int delayInMinutes = ToInt(delayStr);
    // D2TM does not interpret the delay as real minutes; multiplier chosen to feel correct in-game
    int delayD2TM = delayInMinutes * 20;
    reinforcements->addReinforcement(playerId, unitType, targetCell, delayD2TM, repeat);
}

bool cIni::INI_Scenario_Section_Structures(int iHumanID, bool bSetUpPlayers, std::span<const int> iPl_credits,
                                            std::span<const int> iPl_house, std::span<const int> iPl_quota,
                                            const std::string& slinefeed)
{
    if (bSetUpPlayers) {
        INI_Scenario_SetupPlayers(iHumanID, iPl_credits, iPl_house, iPl_quota);
        bSetUpPlayers = false;
    }

    auto [key, valueStr] = INI_SplitWord(slinefeed);
    bool bGen = key.rfind("GEN", 0) == 0; // line is a GENxxx= entry

    auto parts = splitByComma(valueStr);

    int iController = -1;
    int iType = -1;
    int iCell = -1;

    if (!bGen) {
        // Format: ID=House,StructureType,HP,Cell
        if (parts.size() >= 4) {
            int house = cIniUtils::getHouseFromString(ToTrim(parts[0]));
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (m_objects->getPlayer(i)->getHouse() == house) {
                    iController = i;
                    break;
                }
            }
            if (iController < 0) {
                Logger::warn(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Structures", "WARNING: Identifying house/controller of structure (typo?): {}", parts[0]);
            }
            iType = cIniUtils::getStructureTypeFromString(ToTrim(parts[1]));
            iCell = ToInt(ToTrim(parts[3]));
        }
    }
    else {
        // Format: GENcell=House,StructureType
        iCell = ToInt(key.substr(3)); // cell number follows "GEN"
        if (parts.size() >= 2) {
            int house = cIniUtils::getHouseFromString(ToTrim(parts[0]));
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (m_objects->getPlayer(i)->getHouse() == house) {
                    iController = i;
                    break;
                }
            }
            auto typeName = ToTrim(parts[1]);
            if (cIniUtils::caseInsCompare(typeName, "Wall"))     iType = WALL;
            if (cIniUtils::caseInsCompare(typeName, "Concrete")) iType = SLAB1;
        }
    }

    if (iController > -1) {
        m_objects->getPlayer(iController)->placeStructure(iCell, iType, 100);
    }
    else {
        Logger::warn(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Structures", "WARNING: Identifying house/controller of structure (typo?)");
    }
    return bSetUpPlayers;
}

bool cIni::INI_Scenario_Section_Units(int iHumanID, bool bSetUpPlayers, std::span<const int> iPl_credits,
                                       std::span<const int> iPl_house, std::span<const int> iPl_quota,
                                       const std::string& linefeed)
{
    // setupPlayers is required, because we do matching based on name of house, hence, once
    // we encounter either a UNITS or STRUCTURES section, the assumption is made that all HOUSE information
    // has been processed.
    if (bSetUpPlayers) {
        INI_Scenario_SetupPlayers(iHumanID, iPl_credits, iPl_house, iPl_quota);
        bSetUpPlayers = false;
    }

    // Format: ID=House,UnitType,HP,Cell,FacingBody,FacingHead
    auto [key, valueStr] = INI_SplitWord(linefeed);
    auto parts = splitByComma(valueStr);

    int iController = -1;
    int iType = -1;
    int iCell = -1;

    if (parts.size() > INI_UNITS_PART_CONTROLLER) {
        int house = cIniUtils::getHouseFromString(ToTrim(parts[INI_UNITS_PART_CONTROLLER]));
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (m_objects->getPlayer(i)->getHouse() == house) {
                iController = i;
                break;
            }
        }
        if (iController < 0) {
            Logger::warn(COMP_SCENARIOINI, "cIni::INI_Scenario_Section_Units", "WARNING: Cannot identify house/controller -> STRING '{}'", parts[INI_UNITS_PART_CONTROLLER]);
        }
    }
    if (parts.size() > INI_UNITS_PART_TYPE) {
        iType = cIniUtils::getUnitTypeFromString(ToTrim(parts[INI_UNITS_PART_TYPE]));
    }
    // parts[INI_UNITS_PART_HP] skipped
    if (parts.size() > INI_UNITS_PART_CELL) {
        iCell = ToInt(ToTrim(parts[INI_UNITS_PART_CELL]));
    }
    // parts[INI_UNITS_PART_FACING_BODY] and beyond are not used

    if (iController > -1) {
        cUnits::unitCreate(m_objects, m_infos, m_interface, iCell, iType, iController, true);
    }
    return bSetUpPlayers;
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

                d2tm_assert(m_interface->getDrawManager());
                m_interface->getDrawManager()->missionInit();

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

    if (fremenIsHumanAlly) {
        m_objects->getPlayer(AI_CPU5)->setTeam(0);
    }
    else {
        m_objects->getPlayer(AI_CPU5)->setTeam(teamIndexAI);
    }

    m_objects->getPlayer(AI_WORM)->setTeam(2); // the WORM player is nobody's ally, ever
}

void cIni::loadBriefing(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat)
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
    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load_Briefing", "Going to find SCEN ID #{} and SectionID {}", iScenarioFind, iSectionFind);

    int iScenario = 0;
    int iSection = 0;
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

        if (iSection == INI_SCEN) {
            auto [word_left, word_right] = INI_SplitWord(linefeed);
            int wordtype = INI_WordType(word_left, iSection);

            if (wordtype == WORD_NUMBER) {
                iScenario = ToInt(word_right);
                continue;
            }
        }

        if (iScenario == iScenarioFind) {
            if (iSection == iSectionFind) {
                auto [word_left, word_right] = INI_SplitWord(linefeed);
                int wordtype = INI_WordType(word_left, iSection);

                if (wordtype == WORD_REGIONTEXT) {
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

    int section = INI_GAME;
    int wordtype = WORD_NONE;
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

        wordtype = WORD_NONE;

        auto sectionName = extractSectionName(linefeed);
        if (!sectionName.empty()) {
            int last = section;
            section = GAME_INI_SectionType(sectionName, section);

            if (last != section) {
                id = -1;
                if (section == INI_UNITS)      Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] -> [UNITS]");
                if (section == INI_STRUCTURES) Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] -> [STRUCTURES]");
                if (section == INI_BULLETS)    Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] -> [BULLETS]");
            }

            if (section == INI_UNITS && linefeed.find("[UNIT:") != std::string::npos) {
                id = cIniUtils::getUnitTypeFromString(extractTagValue(linefeed, "[UNIT:"));
                if (id >= MAX_UNITTYPES) id--;
            }

            if (section == INI_STRUCTURES && linefeed.find("[STRUCTURE:") != std::string::npos) {
                id = cIniUtils::getStructureType(extractTagValue(linefeed, "[STRUCTURE:"));
                if (id >= MAX_STRUCTURETYPES) id--;
            }

            if (section == INI_BULLETS && linefeed.find("[BULLET:") != std::string::npos) {
                id = cIniUtils::getBulletTypeFromString(extractTagValue(linefeed, "[BULLET:"));
                const int bulletInfoCount = m_infos->getBulletInfos()->size();
                if (id >= 0 && id >= bulletInfoCount) {
                    id--;
                }
            }

            continue;
        }

        if (section == INI_GAME) continue;

        auto [word_left, word_right] = INI_SplitWord(linefeed);
        wordtype = INI_WordType(word_left, section);

        /**** [UNITS] ****/
        if (section == INI_UNITS && id > -1) {
            s_UnitInfo &unitInfo = m_infos->getUnitInfo(id);

            if (wordtype == WORD_HITPOINTS)     unitInfo.hp = ToInt(word_right);
            if (wordtype == WORD_APPETITE)      unitInfo.appetite = ToInt(word_right);
            if (wordtype == WORD_COST)          unitInfo.cost = ToInt(word_right);
            if (wordtype == WORD_MOVESPEED)     unitInfo.speed = ToInt(word_right);
            if (wordtype == WORD_TURNSPEED)     unitInfo.turnspeed = ToInt(word_right);
            if (wordtype == WORD_ATTACKFREQ)    unitInfo.attack_frequency = ToInt(word_right);
            if (wordtype == WORD_NEXTATTACKFREQ) unitInfo.next_attack_frequency = ToInt(word_right);
            if (wordtype == WORD_SIGHT)         unitInfo.sight = ToInt(word_right);
            if (wordtype == WORD_RANGE)         unitInfo.range = ToInt(word_right);
            if (wordtype == WORD_BUILDTIME)     unitInfo.buildTime = ToInt(word_right);

            if (wordtype == WORD_DESCRIPTION) {
                // unitInfo.name is a legacy char[] field
                strncpy(unitInfo.name, word_right.c_str(), sizeof(unitInfo.name) - 1);
                unitInfo.name[sizeof(unitInfo.name) - 1] = '\0';
            }

            if (wordtype == WORD_FIRETWICE)       unitInfo.fireTwice = ToBool(word_right);
            if (wordtype == WORD_ISINFANTRY)      unitInfo.infantry = ToBool(word_right);
            if (wordtype == WORD_ISSQUISHABLE)    unitInfo.canBeSquished = ToBool(word_right);
            if (wordtype == WORD_CANSQUISH)       unitInfo.canBeSquished = ToBool(word_right);
            if (wordtype == WORD_FREEROAM)        unitInfo.free_roam = ToBool(word_right);
            if (wordtype == WORD_ISAIRBORN)       unitInfo.airborn = ToBool(word_right);
            if (wordtype == WORD_HARVESTLIMIT)    unitInfo.credit_capacity = ToInt(word_right);
            if (wordtype == WORD_HARVESTSPEED)    unitInfo.harvesting_speed = ToInt(word_right);
            if (wordtype == WORD_HARVESTAMOUNT)   unitInfo.harvesting_amount = ToInt(word_right);
            if (wordtype == WORD_SMOKEHFACTOR)    unitInfo.smokeHpFactor = ToFloat(word_right);
            if (wordtype == WORD_SQUISH)          unitInfo.canBeSquished = ToBool(word_right);
            if (wordtype == WORD_CANATTACKAIRUNITS) unitInfo.canAttackAirUnits = ToBool(word_right);
            if (wordtype == WORD_CANATTACKUNITS)  unitInfo.canAttackUnits = ToBool(word_right);

            if (wordtype == WORD_PRODUCER) {
                unitInfo.structureTypeItLeavesFrom = cIniUtils::getStructureType(word_right);
            }
        }

        if (section == INI_TERRAINS && id > -1) {
            if (wordtype == WORD_BLOOMTIMERDURATION)    m_infos->getTerrainInfo()->bloomTimerDuration = ToInt(word_right);
            if (wordtype == WORD_TERRAIN_MINSPICE)      m_infos->getTerrainInfo()->terrainSpiceMinSpice = ToInt(word_right);
            if (wordtype == WORD_TERRAIN_MAXSPICE)      m_infos->getTerrainInfo()->terrainSpiceMaxSpice = ToInt(word_right);
            if (wordtype == WORD_TERRAINHILL_MINSPICE)  m_infos->getTerrainInfo()->terrainSpiceHillMinSpice = ToInt(word_right);
            if (wordtype == WORD_TERRAINHILL_MAXSPICE)  m_infos->getTerrainInfo()->terrainSpiceHillMaxSpice = ToInt(word_right);
            if (wordtype == WORD_TERRAINWALL_HP)        m_infos->getTerrainInfo()->terrainWallHp = ToInt(word_right);
        }

        if (section == INI_STRUCTURES && id > -1) {
            if (wordtype == WORD_HITPOINTS)    m_infos->getStructureInfo(id).hp = ToInt(word_right);
            if (wordtype == WORD_FIXHP)        m_infos->getStructureInfo(id).fixhp = ToInt(word_right);
            if (wordtype == WORD_POWERDRAIN)   m_infos->getStructureInfo(id).power_drain = ToInt(word_right);
            if (wordtype == WORD_HAS_CONCRETE) m_infos->getStructureInfo(id).hasConcrete = ToBool(word_right);
            if (wordtype == WORD_POWERGIVE)    m_infos->getStructureInfo(id).power_give = ToInt(word_right);
            if (wordtype == WORD_COST)         m_infos->getStructureInfo(id).cost = ToInt(word_right);
            if (wordtype == WORD_BUILDTIME)    m_infos->getStructureInfo(id).buildTime = ToInt(word_right);
            if (wordtype == WORD_CANATTACKAIRUNITS) m_infos->getStructureInfo(id).canAttackAirUnits = ToBool(word_right);
            if (wordtype == WORD_CANATTACKUNITS)    m_infos->getStructureInfo(id).canAttackGroundUnits = ToBool(word_right);
            if (wordtype == WORD_SIGHT)        m_infos->getStructureInfo(id).sight = ToInt(word_right);
            if (wordtype == WORD_RANGE)        m_infos->getStructureInfo(id).range = ToInt(word_right);
            if (wordtype == WORD_FIRERATE)     m_infos->getStructureInfo(id).fireRate = ToInt(word_right);
            if (wordtype == WORD_UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MIN) m_infos->getStructureInfo(id).uponDestructionSpawnUnitAmountMin = ToInt(word_right);
            if (wordtype == WORD_UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MAX) m_infos->getStructureInfo(id).uponDestructionSpawnUnitAmountMax = ToInt(word_right);
            if (wordtype == WORD_UPON_DESTRUCTION_SPAWN_UNIT_TYPE)       m_infos->getStructureInfo(id).uponDestructionSpawnUnitType = cIniUtils::getUnitTypeFromString(word_right);
        }

        if (section == INI_BULLETS && id > -1) {
            if (wordtype == WORD_DAMAGE_VEHICLE)     m_infos->getBulletInfo(id).damage_vehicles = ToInt(word_right);
            if (wordtype == WORD_DAMAGE_INFANTRY)    m_infos->getBulletInfo(id).damage_infantry = ToInt(word_right);
            if (wordtype == WORD_DEVIATE_PROBABILITY) m_infos->getBulletInfo(id).deviateProbability = ToInt(word_right);
            if (wordtype == WORD_EXPLOSION_SIZE)     m_infos->getBulletInfo(id).explosionSize = ToInt(word_right);
            if (wordtype == WORD_GROUND_BULLET)      m_infos->getBulletInfo(id).groundBullet = ToBool(word_right);
        }

    } // while
    file.close();

    Logger::info(COMP_GAMERULES, "cIni::installGame", "[GAME.INI] Done");
}
