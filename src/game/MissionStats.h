/**
 * @file MissionStats.h
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

#include "include/definitions.h"

#include <array>
#include <cstdint>

struct PlayerMissionStats {
    int unitsBuilt = 0;
    int unitsLost = 0;
    int structuresBuilt = 0;
    int structuresLost = 0;
};

struct MissionStats {
    std::array<PlayerMissionStats, MAX_PLAYERS> players;
    uint64_t elapsedSeconds = 0;
};
