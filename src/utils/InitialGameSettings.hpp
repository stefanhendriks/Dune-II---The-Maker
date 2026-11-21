/**
 * @file InitialGameSettings.hpp
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
#include <string>

#include "game/eScalingMode.h"

struct InitialGameSettings {
    int screenW = 800;
    int screenH = 600;
    float cameraDragMoveSpeed = 0.5f;
    float cameraBorderOrKeyMoveSpeed = 0.5f;
    bool cameraEdgeMove = true;
    float gamepadCursorSpeed = 600.0f; // pixels per second at full stick tilt
    float gamepadDeadzone = 0.2f;        // stick tilt (0..1) that is ignored
    float gamepadCursorCurve = 2.0f;     // response curve of the cursor stick, 1 is linear, higher is more precise near the center
    float gamepadScrollThreshold = 0.5f; // right stick tilt (after the deadzone) at which the map scrolls
    bool windowed = false;
    eScalingMode scalingMode = eScalingMode::INTEGER_SCALE;
    bool allowRepeatingReinforcements = false;
    bool turretsDownOnLowPower = false;
    bool rocketTurretsDownOnLowPower = false;
    bool showLowPowerIcon = false;
    bool playMusic = true;
    bool customMusic = false;
    bool playSound = true;
    bool playCreditsSound = true;
    bool debugMode = false;
    bool drawUnitDebug = false;
    bool disableAI = false;
    bool oneAi = false;
    bool disableWormAi = false;
    bool disableReinforcements = false;
    bool noAiRest = false;
    bool drawUsages = false;
    bool fogOfWarEnabled = false;
    bool pauseWhenLosingFocus = false;
    std::string gameFilename = "houses.ini";
};