#pragma once

#include <cstdint>

struct MissionStats {
    int unitsBuilt = 0;
    int unitsLost = 0;
    int structuresBuilt = 0;
    int structuresLost = 0;
    uint64_t elapsedSeconds = 0;
};
