/**
 * @file iniSections.h
 *
 * Section identifiers used by the INI parser.
 */

#pragma once

// Sections of game.ini
enum class eGameIniSection {
    NONE = -1,
    HOUSES,
    UNITS,
    STRUCTURES,
    TERRAINS,
    GAME,
    BULLETS
};

// Sections of scenario, briefing and region files
enum class eScenarioIniSection {
    NONE = -1,
    UNITS,
    STRUCTURES,
    MAP,
    BASIC,
    SKIRMISH,
    HOUSEATREIDES,
    HOUSEORDOS,
    HOUSEHARKONNEN,
    HOUSESARDAUKAR,
    HOUSEFREMEN,
    HOUSEMERCENARY,
    REINFORCEMENTS,
    SCEN,
    BRIEFING,
    ADVICE,
    WIN,
    LOSE,
    DESCRIPTION,
    REGION
};

constexpr bool isHouseSection(eScenarioIniSection section)
{
    return section >= eScenarioIniSection::HOUSEATREIDES && section <= eScenarioIniSection::HOUSEMERCENARY;
}
