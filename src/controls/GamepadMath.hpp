/**
 * @file GamepadMath.hpp
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

#include <algorithm>
#include <cmath>

/**
 * Pure helpers for gamepad input. No SDL dependency so they can be unit tested.
 */
namespace GamepadMath {

struct s_Stick {
    float x = 0.0f; // -1..1, positive is right
    float y = 0.0f; // -1..1, positive is down
};

/**
 * Radial deadzone: inside the deadzone the result is (0,0). Outside it the magnitude is
 * rescaled so it starts at 0 at the deadzone edge and reaches 1 at full tilt, direction is kept.
 * Input is clamped to the unit circle.
 */
inline s_Stick applyRadialDeadzone(float x, float y, float deadzone)
{
    const float magnitude = std::sqrt(x * x + y * y);
    if (magnitude <= deadzone || magnitude <= 0.0f) return {};
    if (deadzone >= 1.0f) return {};
    const float clamped = std::min(magnitude, 1.0f);
    const float scaled = (clamped - deadzone) / (1.0f - deadzone);
    const float factor = scaled / magnitude;
    return {x * factor, y * factor};
}

/**
 * Response curve for a normalised value (-1..1): exponent 1 is linear, higher is slower near
 * the center (precision) and faster at full tilt. Sign is kept.
 */
inline float applyResponseCurve(float value, float exponent)
{
    const float clamped = std::clamp(value, -1.0f, 1.0f);
    return std::copysign(std::pow(std::fabs(clamped), exponent), clamped);
}

/**
 * Cursor movement for one frame from a stick that already has its deadzone applied: the magnitude is
 * run through the response curve (direction is kept), then scaled to pixels with speed (pixels per
 * second at full tilt) and the frame time in seconds.
 */
inline s_Stick cursorDelta(s_Stick stick, float speed, float exponent, float dtSeconds)
{
    const float magnitude = std::sqrt(stick.x * stick.x + stick.y * stick.y);
    if (magnitude <= 0.0f) return {};
    const float curved = applyResponseCurve(magnitude, exponent);
    const float factor = curved / magnitude * speed * dtSeconds;
    return {stick.x * factor, stick.y * factor};
}

/**
 * Which scroll directions a stick (deadzone already applied) asks for, like the mouse at a screen
 * edge: a direction is on when its axis is tilted further than threshold, so a diagonal gives two.
 */
struct s_ScrollDirections {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
};

inline s_ScrollDirections scrollDirections(s_Stick stick, float threshold)
{
    return {stick.x < -threshold, stick.x > threshold, stick.y < -threshold, stick.y > threshold};
}

/**
 * Converts a raw SDL axis value (-32768..32767) to -1..1.
 */
inline float normalizeAxis(int raw)
{
    const float divisor = raw < 0 ? 32768.0f : 32767.0f;
    return std::clamp(static_cast<float>(raw) / divisor, -1.0f, 1.0f);
}

/**
 * Key-repeat style timing for a held button. Returns how many repeats fire when the held time
 * advances from heldBefore to heldNow (seconds): none before initialDelay, then one at the delay
 * and one per interval after that.
 */
inline int repeatsBetween(float heldBefore, float heldNow, float initialDelay, float interval)
{
    if (interval <= 0.0f || heldNow < initialDelay || heldNow <= heldBefore) return 0;
    const auto repeatsUpTo = [&](float held) {
        return held < initialDelay ? 0 : static_cast<int>(std::floor((held - initialDelay) / interval)) + 1;
    };
    return repeatsUpTo(heldNow) - repeatsUpTo(heldBefore);
}

} // namespace GamepadMath
