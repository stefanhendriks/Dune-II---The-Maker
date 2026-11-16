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

TEST_CASE("cursor delta scales with speed and frame time", "[gamepad]")
{
    const auto d = cursorDelta({1.0f, 0.0f}, 600.0f, 2.0f, 0.1f);
    REQUIRE_THAT(d.x, WithinAbs(60.0, 1e-3));
    REQUIRE_THAT(d.y, WithinAbs(0.0, 1e-3));
}

TEST_CASE("cursor delta is slower near the center and keeps direction", "[gamepad]")
{
    const auto d = cursorDelta({-0.3f, -0.4f}, 100.0f, 2.0f, 1.0f); // magnitude 0.5 -> 0.25
    REQUIRE_THAT(d.x, WithinAbs(-15.0, 1e-3));
    REQUIRE_THAT(d.y, WithinAbs(-20.0, 1e-3));
}

TEST_CASE("cursor delta is zero without input", "[gamepad]")
{
    const auto d = cursorDelta({0.0f, 0.0f}, 600.0f, 2.0f, 0.1f);
    REQUIRE(d.x == 0.0f);
    REQUIRE(d.y == 0.0f);
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

TEST_CASE("stick scroll directions", "[gamepad]")
{
    const auto centered = scrollDirections({0.0f, 0.0f}, 0.5f);
    REQUIRE_FALSE((centered.left || centered.right || centered.up || centered.down));

    const auto east = scrollDirections({0.9f, 0.1f}, 0.5f);
    REQUIRE(east.right);
    REQUIRE_FALSE((east.left || east.up || east.down));

    const auto northWest = scrollDirections({-0.8f, -0.8f}, 0.5f);
    REQUIRE(northWest.left);
    REQUIRE(northWest.up);
    REQUIRE_FALSE((northWest.right || northWest.down));

    // below the threshold nothing scrolls, exactly on it neither
    const auto weak = scrollDirections({0.5f, 0.3f}, 0.5f);
    REQUIRE_FALSE((weak.left || weak.right || weak.up || weak.down));

    REQUIRE(scrollDirections({0.0f, 1.0f}, 0.5f).down);
}

TEST_CASE("setting values outside their range fall back", "[gamepad]")
{
    REQUIRE(valueOrDefault(0.3f, 0.0f, 0.9f, 0.2f) == 0.3f);
    REQUIRE(valueOrDefault(0.0f, 0.0f, 0.9f, 0.2f) == 0.0f);
    REQUIRE(valueOrDefault(0.9f, 0.0f, 0.9f, 0.2f) == 0.9f);
    REQUIRE(valueOrDefault(-0.1f, 0.0f, 0.9f, 0.2f) == 0.2f);
    REQUIRE(valueOrDefault(1.0f, 0.0f, 0.9f, 0.2f) == 0.2f);
    REQUIRE(valueOrDefault(std::nanf(""), 0.0f, 0.9f, 0.2f) == 0.2f);
}

TEST_CASE("radial deadzone exactly on the edge is zero, just outside is not", "[gamepad]")
{
    REQUIRE(applyRadialDeadzone(0.25f, 0.0f, 0.25f).x == 0.0f);
    REQUIRE(applyRadialDeadzone(0.0f, -0.25f, 0.25f).y == 0.0f);
    REQUIRE(applyRadialDeadzone(0.26f, 0.0f, 0.25f).x > 0.0f);
}

TEST_CASE("radial deadzone keeps the sign per axis", "[gamepad]")
{
    const auto s = applyRadialDeadzone(-0.8f, 0.6f, 0.2f);
    REQUIRE(s.x < 0.0f);
    REQUIRE(s.y > 0.0f);
    // direction (ratio) is kept
    REQUIRE_THAT(s.x / s.y, WithinAbs(-0.8 / 0.6, 1e-4));
}

TEST_CASE("radial deadzone clamps input beyond the unit circle", "[gamepad]")
{
    const auto s = applyRadialDeadzone(3.0f, 4.0f, 0.2f);
    REQUIRE_THAT(std::sqrt(s.x * s.x + s.y * s.y), WithinAbs(1.0, 1e-5));
    // a deadzone above 1 behaves like 1: nothing gets through
    REQUIRE(applyRadialDeadzone(1.0f, 1.0f, 1.5f).x == 0.0f);
}

TEST_CASE("radial deadzone output never exceeds the unit circle", "[gamepad]")
{
    for (float x = -1.0f; x <= 1.0f; x += 0.125f) {
        for (float y = -1.0f; y <= 1.0f; y += 0.125f) {
            const auto s = applyRadialDeadzone(x, y, 0.15f);
            REQUIRE(std::sqrt(s.x * s.x + s.y * s.y) <= 1.0f + 1e-5f);
        }
    }
}

TEST_CASE("response curve with exponent below one speeds up the center", "[gamepad]")
{
    REQUIRE_THAT(applyResponseCurve(0.25f, 0.5f), WithinAbs(0.5, 1e-5));
    REQUIRE_THAT(applyResponseCurve(-0.25f, 0.5f), WithinAbs(-0.5, 1e-5));
}

TEST_CASE("response curve at the extremes and with negative input", "[gamepad]")
{
    for (float exponent : {0.5f, 1.0f, 2.0f, 3.0f}) {
        REQUIRE_THAT(applyResponseCurve(1.0f, exponent), WithinAbs(1.0, 1e-5));
        REQUIRE_THAT(applyResponseCurve(-1.0f, exponent), WithinAbs(-1.0, 1e-5));
        REQUIRE_THAT(applyResponseCurve(-2.0f, exponent), WithinAbs(-1.0, 1e-5)); // clamped
        REQUIRE_THAT(applyResponseCurve(0.0f, exponent), WithinAbs(0.0, 1e-5));
    }
}

TEST_CASE("response curve is monotonic", "[gamepad]")
{
    float previous = -1.0f;
    for (float v = -1.0f; v <= 1.0f; v += 0.05f) {
        const float curved = applyResponseCurve(v, 2.0f);
        REQUIRE(curved >= previous);
        previous = curved;
    }
}

TEST_CASE("cursor delta follows the response curve exponent", "[gamepad]")
{
    // magnitude 0.5: exponent 1 -> 0.5, exponent 3 -> 0.125
    REQUIRE_THAT(cursorDelta({0.5f, 0.0f}, 100.0f, 1.0f, 1.0f).x, WithinAbs(50.0, 1e-3));
    REQUIRE_THAT(cursorDelta({0.5f, 0.0f}, 100.0f, 3.0f, 1.0f).x, WithinAbs(12.5, 1e-3));
}

TEST_CASE("cursor delta with zero speed or zero frame time does not move", "[gamepad]")
{
    REQUIRE(cursorDelta({1.0f, 0.0f}, 0.0f, 2.0f, 0.1f).x == 0.0f);
    REQUIRE(cursorDelta({1.0f, 1.0f}, 600.0f, 2.0f, 0.0f).y == 0.0f);
}

TEST_CASE("cursor delta distance is proportional to frame time", "[gamepad]")
{
    const auto oneFrame = cursorDelta({0.0f, 0.8f}, 500.0f, 2.0f, 0.016f);
    const auto twoFrames = cursorDelta({0.0f, 0.8f}, 500.0f, 2.0f, 0.032f);
    REQUIRE_THAT(twoFrames.y, WithinAbs(oneFrame.y * 2.0f, 1e-4));
}

TEST_CASE("axis normalisation is symmetric around zero and clamped", "[gamepad]")
{
    REQUIRE_THAT(normalizeAxis(16384), WithinAbs(0.5, 1e-3));
    REQUIRE_THAT(normalizeAxis(-16384), WithinAbs(-0.5, 1e-3));
    REQUIRE_THAT(normalizeAxis(1), WithinAbs(1.0 / 32767.0, 1e-9));
    REQUIRE_THAT(normalizeAxis(-1), WithinAbs(-1.0 / 32768.0, 1e-9));
    // out of SDL range values are clamped
    REQUIRE(normalizeAxis(100000) == 1.0f);
    REQUIRE(normalizeAxis(-100000) == -1.0f);
}

TEST_CASE("repeat timing edges", "[gamepad]")
{
    // exactly on the initial delay fires, just before does not
    REQUIRE(repeatsBetween(0.0f, 0.4f, 0.4f, 0.1f) == 1);
    REQUIRE(repeatsBetween(0.0f, 0.39f, 0.4f, 0.1f) == 0);
    // time going backwards never repeats
    REQUIRE(repeatsBetween(1.0f, 0.5f, 0.4f, 0.1f) == 0);
    // negative interval is invalid
    REQUIRE(repeatsBetween(0.0f, 1.0f, 0.4f, -0.1f) == 0);
    // a big frame hiccup fires every missed repeat
    REQUIRE(repeatsBetween(0.0f, 2.45f, 0.4f, 0.1f) == 21);
    // many small steps give about the same total as one big step (float rounding allows one off)
    int total = 0;
    float before = 0.0f;
    for (int i = 1; i <= 100; i++) {
        const float now = static_cast<float>(i) * 0.01f;
        total += repeatsBetween(before, now, 0.4f, 0.1f);
        before = now;
    }
    REQUIRE(std::abs(total - repeatsBetween(0.0f, before, 0.4f, 0.1f)) <= 1);
}

TEST_CASE("stick scroll threshold edges", "[gamepad]")
{
    REQUIRE_FALSE(scrollDirections({0.5f, 0.0f}, 0.5f).right);
    REQUIRE(scrollDirections({0.5001f, 0.0f}, 0.5f).right);
    REQUIRE_FALSE(scrollDirections({-0.5f, 0.0f}, 0.5f).left);
    REQUIRE(scrollDirections({-0.5001f, 0.0f}, 0.5f).left);
    REQUIRE_FALSE(scrollDirections({0.0f, -0.5f}, 0.5f).up);
    REQUIRE(scrollDirections({0.0f, -0.5001f}, 0.5f).up);
    REQUIRE_FALSE(scrollDirections({0.0f, 0.5f}, 0.5f).down);
    REQUIRE(scrollDirections({0.0f, 0.5001f}, 0.5f).down);
}

TEST_CASE("stick scroll directions never give opposite directions at once", "[gamepad]")
{
    for (float x = -1.0f; x <= 1.0f; x += 0.1f) {
        for (float y = -1.0f; y <= 1.0f; y += 0.1f) {
            const auto d = scrollDirections({x, y}, 0.4f);
            REQUIRE_FALSE((d.left && d.right));
            REQUIRE_FALSE((d.up && d.down));
        }
    }
}

TEST_CASE("stick scroll threshold extremes", "[gamepad]")
{
    // threshold 0: any tilt scrolls, centered does not
    REQUIRE(scrollDirections({0.01f, 0.0f}, 0.0f).right);
    REQUIRE_FALSE(scrollDirections({0.0f, 0.0f}, 0.0f).right);
    // threshold 1: full tilt is not strictly above it, so nothing scrolls
    const auto full = scrollDirections({1.0f, -1.0f}, 1.0f);
    REQUIRE_FALSE((full.left || full.right || full.up || full.down));
}

TEST_CASE("setting value fallback handles infinities and a swapped range", "[gamepad]")
{
    REQUIRE(valueOrDefault(INFINITY, 0.0f, 1.0f, 0.5f) == 0.5f);
    REQUIRE(valueOrDefault(-INFINITY, 0.0f, 1.0f, 0.5f) == 0.5f);
    // min above max: nothing fits
    REQUIRE(valueOrDefault(0.5f, 1.0f, 0.0f, 0.25f) == 0.25f);
}
