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
