/**
 * @file LowPowerBlink.hpp
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

#include <cstdint>

namespace LowPowerBlink {

constexpr int MIN_OPACITY = 64;
constexpr int MAX_OPACITY = 255;
constexpr std::uint64_t PERIOD_MS = 1000;

// Opacity of the low power icon at the given time: a triangle wave that fades
// from MIN_OPACITY up to MAX_OPACITY and back again, once per PERIOD_MS.
constexpr int opacityAt(std::uint64_t timeMs)
{
    const std::uint64_t half = PERIOD_MS / 2;
    const std::uint64_t phase = timeMs % PERIOD_MS;
    const std::uint64_t rising = phase < half ? phase : PERIOD_MS - phase;
    return MIN_OPACITY + static_cast<int>((MAX_OPACITY - MIN_OPACITY) * rising / half);
}

}
