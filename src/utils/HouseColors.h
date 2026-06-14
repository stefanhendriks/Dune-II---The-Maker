#pragma once

#include <array>
#include <SDL3/SDL.h>

struct HouseColor {
    Uint8 r,g,b;
};

using HouseColors = std::array<HouseColor, 7>;
