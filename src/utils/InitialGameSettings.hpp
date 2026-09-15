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

struct InitialGameSettings {
    int screenW = 800;
    int screenH = 600;
    float cameraDragMoveSpeed = 0.5f;
    float cameraBorderOrKeyMoveSpeed = 0.5f;
    bool cameraEdgeMove = true;
    bool windowed = false;
    bool allowRepeatingReinforcements = false;
    bool turretsDownOnLowPower = false;
    bool rocketTurretsDownOnLowPower = false;
    bool playMusic = true;
    bool playSound = true;
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
    std::string gameFilename = "game.ini";
};