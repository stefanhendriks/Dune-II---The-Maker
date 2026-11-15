/**
 * @file test_GamepadMath.cpp
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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "controls/GamepadMath.hpp"

using Catch::Matchers::WithinAbs;
using namespace GamepadMath;

TEST_CASE("radial deadzone zeroes small input", "[gamepad]")
{
    const auto s = applyRadialDeadzone(0.1f, 0.1f, 0.2f);
    REQUIRE(s.x == 0.0f);
    REQUIRE(s.y == 0.0f);
}

TEST_CASE("radial deadzone is radial, not per axis", "[gamepad]")
{
    // each axis is at or below 0.2 but the magnitude (0.28) is above it
    const auto s = applyRadialDeadzone(0.2f, 0.2f, 0.2f);
    REQUIRE(s.x > 0.0f);
    REQUIRE(s.y > 0.0f);
}

TEST_CASE("radial deadzone reaches full tilt and keeps direction", "[gamepad]")
{
    const auto right = applyRadialDeadzone(1.0f, 0.0f, 0.2f);
    REQUIRE_THAT(right.x, WithinAbs(1.0, 1e-5));
    REQUIRE_THAT(right.y, WithinAbs(0.0, 1e-5));

    const auto upLeft = applyRadialDeadzone(-1.0f, -1.0f, 0.2f);
    REQUIRE(upLeft.x < 0.0f);
    REQUIRE(upLeft.y < 0.0f);
    REQUIRE_THAT(std::sqrt(upLeft.x * upLeft.x + upLeft.y * upLeft.y), WithinAbs(1.0, 1e-5));
}

TEST_CASE("radial deadzone starts at zero at the edge", "[gamepad]")
{
    const auto s = applyRadialDeadzone(0.3f, 0.0f, 0.2f);
    REQUIRE_THAT(s.x, WithinAbs(0.125, 1e-5)); // (0.3 - 0.2) / 0.8
}

TEST_CASE("radial deadzone handles degenerate deadzones", "[gamepad]")
{
    REQUIRE(applyRadialDeadzone(0.0f, 0.0f, 0.0f).x == 0.0f);
    REQUIRE(applyRadialDeadzone(1.0f, 0.0f, 1.0f).x == 0.0f);
    REQUIRE_THAT(applyRadialDeadzone(0.5f, 0.0f, 0.0f).x, WithinAbs(0.5, 1e-5));
}

TEST_CASE("response curve", "[gamepad]")
{
    REQUIRE_THAT(applyResponseCurve(0.5f, 1.0f), WithinAbs(0.5, 1e-5));
    REQUIRE_THAT(applyResponseCurve(0.5f, 2.0f), WithinAbs(0.25, 1e-5));
    REQUIRE_THAT(applyResponseCurve(-0.5f, 2.0f), WithinAbs(-0.25, 1e-5));
    REQUIRE_THAT(applyResponseCurve(2.0f, 2.0f), WithinAbs(1.0, 1e-5));
    REQUIRE_THAT(applyResponseCurve(0.0f, 2.0f), WithinAbs(0.0, 1e-5));
}

TEST_CASE("axis normalisation", "[gamepad]")
{
    REQUIRE_THAT(normalizeAxis(0), WithinAbs(0.0, 1e-6));
    REQUIRE_THAT(normalizeAxis(32767), WithinAbs(1.0, 1e-6));
    REQUIRE_THAT(normalizeAxis(-32768), WithinAbs(-1.0, 1e-6));
}

TEST_CASE("repeat timing", "[gamepad]")
{
    // delay 0.4s, interval 0.1s
    REQUIRE(repeatsBetween(0.0f, 0.3f, 0.4f, 0.1f) == 0);
    REQUIRE(repeatsBetween(0.3f, 0.45f, 0.4f, 0.1f) == 1);  // first repeat at the delay
    REQUIRE(repeatsBetween(0.45f, 0.55f, 0.4f, 0.1f) == 1); // at 0.5
    REQUIRE(repeatsBetween(0.0f, 0.85f, 0.4f, 0.1f) == 5);  // 0.4 .. 0.8
    REQUIRE(repeatsBetween(0.5f, 0.5f, 0.4f, 0.1f) == 0);
    REQUIRE(repeatsBetween(0.0f, 1.0f, 0.4f, 0.0f) == 0);
}
