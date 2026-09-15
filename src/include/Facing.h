/**
 * @file Facing.h
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

#include "utils/RNG.hpp"

enum class Facing : int {
    UP = 0,
    UPRIGHT = 1,
    RIGHT = 2,
    DOWNRIGHT = 3,
    DOWN = 4,
    DOWNLEFT = 5,
    LEFT = 6,
    UPLEFT = 7,
    COUNT = 8
};

constexpr int facingToInt(Facing facing) {
    return static_cast<int>(facing);
}

constexpr Facing facingFromInt(int value) {
    const int count = facingToInt(Facing::COUNT);
    int normalized = value % count;
    if (normalized < 0) {
        normalized += count;
    }
    return static_cast<Facing>(normalized);
}

inline Facing randomFacing() {
    return facingFromInt(RNG::rnd(facingToInt(Facing::COUNT)));
}
