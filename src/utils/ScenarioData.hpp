/**
 * @file ScenarioData.hpp
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

#pragma once

#include "definitions.h"

#include <array>
#include <optional>
#include <string>
#include <variant>
#include <vector>

/**
 * The content of a campaign scenario INI file, as read from disk and before any game object is touched.
 * cIni reads a file into this structure first, and populates the game objects from it afterwards.
 */
struct ScenarioData {
    static constexpr int MAX_BLOOMS_AND_FIELDS = 30;

    struct Unit {
        std::optional<int> house;   // empty when the line has no house part at all
        std::string houseName;      // as written in the file, for logging
        int type = -1;
        int cell = -1;
    };

    struct Structure {
        std::optional<int> house;   // empty when the line has no house part at all
        std::string houseName;      // as written in the file, for logging
        bool generated = false;     // GENxxx= entry (wall/slab) instead of a regular entry
        int type = -1;
        int cell = -1;
    };

    struct Reinforcement {
        enum class Location { HOMEBASE, ENEMYBASE, CELL, INVALID };

        int house = -1;
        int unitType = -1;
        Location location = Location::INVALID;
        int cell = -1;              // only for Location::CELL
        int delay = 0;              // already in D2TM time
        bool repeat = false;        // '+' suffix; only effective when the game settings allow repeating
    };

    // [UNITS], [STRUCTURES] and [REINFORCEMENTS] entries, in file order
    using Entry = std::variant<Unit, Structure, Reinforcement>;

    // [BASIC], the last value in the file wins
    std::optional<std::string> briefScene;
    std::optional<int> focusCell;
    std::optional<int> winFlags;
    std::optional<int> loseFlags;

    // [HOUSE] sections, indexed by player id (order of the sections in the file)
    std::array<int, MAX_PLAYERS> playerHouse{};
    std::array<int, MAX_PLAYERS> playerCredits{};
    std::array<int, MAX_PLAYERS> playerQuota{};
    int humanPlayerId = -1;

    // [MAP]: one element per line in the section (each resets the map), holding the seed when it is a Seed= line
    std::vector<std::optional<int>> mapLines;
    // map cell numbers as written in the file (64 wide), -1 when unused
    std::array<int, MAX_BLOOMS_AND_FIELDS> blooms{};
    std::array<int, MAX_BLOOMS_AND_FIELDS> fields{};

    std::vector<Entry> entries;

    ScenarioData()
    {
        playerHouse.fill(-1);
        playerCredits.fill(0);
        playerQuota.fill(0);
        blooms.fill(-1);
        fields.fill(-1);
    }
};
