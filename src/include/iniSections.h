/**
 * @file iniSections.h
 *
 * Section identifiers used by the INI parser.
 */

#pragma once

enum class eIniSection {
    NONE = -1,
    HOUSES,
    UNITS,
    STRUCTURES,
    MAP,
    TERRAINS,
    GAME,
    BULLETS,
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

constexpr bool isHouseSection(eIniSection section)
{
    return section >= eIniSection::HOUSEATREIDES && section <= eIniSection::HOUSEMERCENARY;
}
