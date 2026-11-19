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

#include "cEnumArray.h"
#include "definitions.h"
#include "iniUnitsParts.h"
#include "utils/Log.h"
#include "utils/ScenarioData.hpp"
#include "utils/cIniUtils.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <span>
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

eIniWord ScenarioIniWordType(const std::string& word, eScenarioIniSection section)
{
    if (section == eScenarioIniSection::REGION) {
        if (cIniUtils::caseInsCompare(word, "Region"))            return eIniWord::REGION;
        if (cIniUtils::caseInsCompare(word, "RegionConquer"))     return eIniWord::REGIONCONQUER;
        if (cIniUtils::caseInsCompare(word, "House"))             return eIniWord::REGIONHOUSE;
        if (cIniUtils::caseInsCompare(word, "Text"))              return eIniWord::REGIONTEXT;
        if (cIniUtils::caseInsCompare(word, "Select"))            return eIniWord::REGIONSELECT;
        return eIniWord::NONE;
    }

    if (section == eScenarioIniSection::SCEN ||
            section == eScenarioIniSection::DESCRIPTION ||
            section == eScenarioIniSection::BRIEFING ||
            section == eScenarioIniSection::ADVICE ||
            section == eScenarioIniSection::LOSE ||
            section == eScenarioIniSection::WIN) {
        if (cIniUtils::caseInsCompare(word, "Number"))            return eIniWord::NUMBER;
        if (cIniUtils::caseInsCompare(word, "Text"))              return eIniWord::REGIONTEXT;
        return eIniWord::NONE;
    }

    if (section == eScenarioIniSection::SKIRMISH) {
        if (cIniUtils::caseInsCompare(word, "Title"))             return eIniWord::MAPNAME;
        if (cIniUtils::caseInsCompare(word, "StartCell"))         return eIniWord::STARTCELL;
    }

    if (section == eScenarioIniSection::BASIC) {
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
    else if (section == eScenarioIniSection::MAP) {
        if (cIniUtils::caseInsCompare(word, "Width"))             return eIniWord::MAPWIDTH;
        if (cIniUtils::caseInsCompare(word, "Height"))            return eIniWord::MAPHEIGHT;
        if (cIniUtils::caseInsCompare(word, "Seed"))              return eIniWord::MAPSEED;
        if (cIniUtils::caseInsCompare(word, "Bloom"))             return eIniWord::MAPBLOOM;
        if (cIniUtils::caseInsCompare(word, "Field"))             return eIniWord::MAPFIELD;
    }
    Logger::warn(COMP_NONE, "ScenarioIniWordType", "Could not find word-type for [{}]", word);
    return eIniWord::NONE;
}

eScenarioIniSection SCEN_INI_SectionType(const std::string& section)
{
    if (cIniUtils::caseInsCompare(section, "UNITS"))          return eScenarioIniSection::UNITS;
    if (cIniUtils::caseInsCompare(section, "SKIRMISH"))       return eScenarioIniSection::SKIRMISH;
    if (cIniUtils::caseInsCompare(section, "STRUCTURES"))     return eScenarioIniSection::STRUCTURES;
    if (cIniUtils::caseInsCompare(section, "REINFORCEMENTS")) return eScenarioIniSection::REINFORCEMENTS;
    if (cIniUtils::caseInsCompare(section, "MAP"))            return eScenarioIniSection::MAP;
    if (cIniUtils::caseInsCompare(section, "BASIC"))          return eScenarioIniSection::BASIC;
    if (cIniUtils::caseInsCompare(section, "Atreides"))       return eScenarioIniSection::HOUSEATREIDES;
    if (cIniUtils::caseInsCompare(section, "Ordos"))          return eScenarioIniSection::HOUSEORDOS;
    if (cIniUtils::caseInsCompare(section, "Harkonnen"))      return eScenarioIniSection::HOUSEHARKONNEN;
    if (cIniUtils::caseInsCompare(section, "Sardaukar"))      return eScenarioIniSection::HOUSESARDAUKAR;
    if (cIniUtils::caseInsCompare(section, "Fremen"))         return eScenarioIniSection::HOUSEFREMEN;
    if (cIniUtils::caseInsCompare(section, "Mercenary"))      return eScenarioIniSection::HOUSEMERCENARY;
    Logger::warn(COMP_NONE, "SCEN_INI_SectionType", "Could not find section type for [{}]", section);
    return eScenarioIniSection::NONE;
}

bool isCommentLine(const std::string& linefeed)
{
    if (linefeed.empty()) return true;
    char first = linefeed[0];
    if (first == ';' || first == '#' || first == '\n' || first == '\0') return true;
    if (first == '/' && linefeed.size() > 1 && linefeed[1] == '/') return true;
    return false;
}

std::string getHouseDirectoryName(int iHouse)
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

} // namespace

ScenarioData parseScenario(std::istream& file)
{
    ScenarioData data;
    eScenarioIniSection section = eScenarioIniSection::NONE;
    int iPlayerID = -1;

    std::string linefeed;
    while (std::getline(file, linefeed)) {
        linefeed.erase(std::remove(linefeed.begin(), linefeed.end(), '\r'), linefeed.end());
        if (linefeed.empty() || isCommentLine(linefeed)) {
            continue;
        }

        auto sectionName = extractSectionName(linefeed);
        if (!sectionName.empty()) {
            eScenarioIniSection sectionType = SCEN_INI_SectionType(sectionName);
            if (sectionType != eScenarioIniSection::NONE) {
                section = sectionType;

                Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] found section '{}', resulting in section id [{}]", sectionName, std::to_underlying(section));

                if (isHouseSection(section)) {
                    iPlayerID = std::min(iPlayerID + 1, MAX_PLAYERS - 1);

                    int house = -1;
                    if (section == eScenarioIniSection::HOUSEATREIDES) house = ATREIDES;
                    if (section == eScenarioIniSection::HOUSEORDOS)    house = ORDOS;
                    if (section == eScenarioIniSection::HOUSEHARKONNEN) house = HARKONNEN;
                    if (section == eScenarioIniSection::HOUSEMERCENARY) house = MERCENARY;
                    if (section == eScenarioIniSection::HOUSEFREMEN)   house = FREMEN;
                    if (section == eScenarioIniSection::HOUSESARDAUKAR) house = SARDAUKAR;

                    data.playerHouse[iPlayerID] = house;

                    Logger::info(COMP_SCENARIOINI, "cIni::INI_Load", "[SCENARIO] Setting house to [{}] for playerId [{}]", data.playerHouse[iPlayerID], iPlayerID);
                }
            }
            continue;
        }

        if (section == eScenarioIniSection::NONE) continue;

        auto [word_left, word_right] = INI_SplitWord(linefeed);
        eIniWord wordtype = ScenarioIniWordType(word_left, section);

        if (section == eScenarioIniSection::BASIC) {
            readScenarioBasicLine(data, wordtype, word_right);
        }

        if (isHouseSection(section)) {
            readScenarioHouseLine(data, wordtype, iPlayerID, word_right);
        }

        if (section == eScenarioIniSection::MAP) {
            readScenarioMapLine(data, wordtype, linefeed);
        }
        else if (section == eScenarioIniSection::UNITS) {
            readScenarioUnitLine(data, linefeed);
        }
        else if (section == eScenarioIniSection::STRUCTURES) {
            readScenarioStructureLine(data, linefeed);
        }
        else if (section == eScenarioIniSection::REINFORCEMENTS) {
            readScenarioReinforcementLine(data, linefeed);
        }
    }
    return data;
}

std::string getBriefingFileName(int iHouse)
{
    switch (iHouse) {
        case ATREIDES:   return "mentata.ini";
        case ORDOS:      return "mentato.ini";
        case HARKONNEN:  return "mentath.ini";
        case SARDAUKAR:  return "mentats.ini";
        default:         return "";
    }
}

std::vector<std::string> parseBriefing(std::istream& file, int iScenarioFind, eScenarioIniSection iSectionFind)
{
    constexpr std::size_t MAX_SENTENCES = 10;
    std::vector<std::string> sentences;

    int iScenario = 0;
    eScenarioIniSection iSection = eScenarioIniSection::NONE;

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

        if (iSection == eScenarioIniSection::SCEN) {
            auto [word_left, word_right] = INI_SplitWord(linefeed);
            eIniWord wordtype = ScenarioIniWordType(word_left, iSection);

            if (wordtype == eIniWord::NUMBER) {
                iScenario = ToInt(word_right);
                continue;
            }
        }

        if (iScenario == iScenarioFind && iSection == iSectionFind) {
            auto [word_left, word_right] = INI_SplitWord(linefeed);
            eIniWord wordtype = ScenarioIniWordType(word_left, iSection);

            if (wordtype == eIniWord::REGIONTEXT) {
                sentences.push_back(removeQuote(word_right));
                if (sentences.size() >= MAX_SENTENCES) break;
            }
        }
    }
    return sentences;
}

RegionFileData parseRegionFile(std::istream& file)
{
    RegionFileData data;

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

        eIniWord wordtype = ScenarioIniWordType(word_left, eScenarioIniSection::REGION);

        if (wordtype == eIniWord::REGION) {
            iRegionConquer = -1;
            iRegionNumber = ToInt(word_right) - 1;
        }
        else if (wordtype == eIniWord::REGIONCONQUER) {
            iRegionNumber = -1;
            iRegionIndex++;
            iRegionConquer = ToInt(word_right) - 1;
            data.conquests.resize(iRegionIndex + 1);
            data.conquests[iRegionIndex].regionConquer = iRegionConquer;
        }

        if (iRegionIndex > -1 || iRegionNumber > -1) {
            if (wordtype == eIniWord::REGIONHOUSE) {
                int iH = cIniUtils::getHouseFromString(word_right);
                if (iRegionNumber > -1) {
                    auto& region = data.worldRegions[iRegionNumber];
                    region.house = iH;
                    region.alpha = 255;
                }
                if (iRegionConquer > -1) {
                    data.conquests[iRegionIndex].house = iH;
                }
            }

            if (wordtype == eIniWord::REGIONTEXT && iRegionConquer > -1 && iRegionIndex > -1) {
                data.conquests[iRegionIndex].text = removeQuote(word_right);
            }

            if (wordtype == eIniWord::REGIONSELECT && iRegionNumber > -1) {
                auto& region = data.worldRegions[iRegionNumber];
                region.selectable = ToBool(word_right);
                region.alpha = 1;
            }
        }
    }
    return data;
}
