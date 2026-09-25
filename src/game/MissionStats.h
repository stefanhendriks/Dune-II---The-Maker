#pragma once

#include "include/definitions.h"
#include "utils/Color.hpp"

#include <array>
#include <cstdint>

struct PlayerMissionStats {
    int house = GENERALHOUSE;
    Color minimapColor;
    int unitsBuilt = 0;
    int unitsLost = 0;
    int unitsDestroyed = 0;
    int structuresBuilt = 0;
    int structuresLost = 0;
    int structuresDestroyed = 0;
    int damageDealt = 0;
    int damageReceived = 0;
};

struct MissionStats {
    std::array<PlayerMissionStats, MAX_PLAYERS> players;
    uint64_t elapsedSeconds = 0;
};
