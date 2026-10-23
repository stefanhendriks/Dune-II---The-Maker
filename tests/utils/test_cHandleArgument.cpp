/**
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 */

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include "utils/cHandleArgument.h"

namespace {
int run(std::vector<const char *> args, InitialGameSettings &settings)
{
    args.insert(args.begin(), "d2tm");
    cHandleArgument handler;
    return handler.handleArguments(static_cast<int>(args.size()), const_cast<char **>(args.data()), &settings);
}

// names of the fields that differ from a default constructed InitialGameSettings
std::vector<std::string> changedFields(const InitialGameSettings &s)
{
    const InitialGameSettings d;
    std::vector<std::string> changed;
    const auto check = [&](const char *name, bool differs) {
        if (differs) changed.emplace_back(name);
    };
    check("screenW", s.screenW != d.screenW);
    check("screenH", s.screenH != d.screenH);
    check("windowed", s.windowed != d.windowed);
    check("scalingMode", s.scalingMode != d.scalingMode);
    check("playMusic", s.playMusic != d.playMusic);
    check("playSound", s.playSound != d.playSound);
    check("debugMode", s.debugMode != d.debugMode);
    check("drawUnitDebug", s.drawUnitDebug != d.drawUnitDebug);
    check("disableAI", s.disableAI != d.disableAI);
    check("oneAi", s.oneAi != d.oneAi);
    check("disableWormAi", s.disableWormAi != d.disableWormAi);
    check("disableReinforcements", s.disableReinforcements != d.disableReinforcements);
    check("noAiRest", s.noAiRest != d.noAiRest);
    check("drawUsages", s.drawUsages != d.drawUsages);
    check("fogOfWarEnabled", s.fogOfWarEnabled != d.fogOfWarEnabled);
    check("pauseWhenLosingFocus", s.pauseWhenLosingFocus != d.pauseWhenLosingFocus);
    check("gameFilename", s.gameFilename != d.gameFilename);
    return changed;
}
}

TEST_CASE("cHandleArgument - no arguments leaves every setting at its default", "[arguments]")
{
    InitialGameSettings settings;
    REQUIRE(run({}, settings) == 0);
    REQUIRE(settings.screenW == 800);
    REQUIRE(settings.screenH == 600);
    REQUIRE_FALSE(settings.windowed);
    REQUIRE(settings.scalingMode == eScalingMode::INTEGER_SCALE);
    REQUIRE(settings.playMusic);
    REQUIRE(settings.playSound);
    REQUIRE_FALSE(settings.debugMode);
    REQUIRE_FALSE(settings.drawUnitDebug);
    REQUIRE_FALSE(settings.disableAI);
    REQUIRE_FALSE(settings.oneAi);
    REQUIRE_FALSE(settings.disableWormAi);
    REQUIRE_FALSE(settings.disableReinforcements);
    REQUIRE_FALSE(settings.noAiRest);
    REQUIRE_FALSE(settings.drawUsages);
    REQUIRE_FALSE(settings.fogOfWarEnabled);
    REQUIRE_FALSE(settings.pauseWhenLosingFocus);
    REQUIRE(settings.gameFilename == "game.ini");
    REQUIRE(changedFields(settings).empty());
}

TEST_CASE("cHandleArgument - each boolean flag sets its own setting and nothing else", "[arguments]")
{
    InitialGameSettings settings;

    SECTION("-noai") {
        run({"-noai"}, settings);
        REQUIRE(settings.disableAI);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"disableAI"});
    }
    SECTION("-oneai") {
        run({"-oneai"}, settings);
        REQUIRE(settings.oneAi);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"oneAi"});
    }
    SECTION("-nowormai") {
        run({"-nowormai"}, settings);
        REQUIRE(settings.disableWormAi);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"disableWormAi"});
    }
    SECTION("-noreinforcements") {
        run({"-noreinforcements"}, settings);
        REQUIRE(settings.disableReinforcements);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"disableReinforcements"});
    }
    SECTION("-noairest") {
        run({"-noairest"}, settings);
        REQUIRE(settings.noAiRest);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"noAiRest"});
    }
    SECTION("-debug") {
        run({"-debug"}, settings);
        REQUIRE(settings.debugMode);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"debugMode"});
    }
    SECTION("-debug-units") {
        run({"-debug-units"}, settings);
        REQUIRE(settings.drawUnitDebug);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"drawUnitDebug"});
    }
    SECTION("-usages") {
        run({"-usages"}, settings);
        REQUIRE(settings.drawUsages);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"drawUsages"});
    }
    SECTION("-autopause") {
        run({"-autopause"}, settings);
        REQUIRE(settings.pauseWhenLosingFocus);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"pauseWhenLosingFocus"});
    }
    SECTION("-fogofwar") {
        run({"-fogofwar"}, settings);
        REQUIRE(settings.fogOfWarEnabled);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"fogOfWarEnabled"});
    }
    SECTION("-windowed") {
        run({"-windowed"}, settings);
        REQUIRE(settings.windowed);
        REQUIRE(changedFields(settings) == std::vector<std::string>{"windowed"});
    }
}

TEST_CASE("cHandleArgument - -nomusic only disables music", "[arguments]")
{
    InitialGameSettings settings;
    run({"-nomusic"}, settings);
    REQUIRE_FALSE(settings.playMusic);
    REQUIRE(settings.playSound);
}

TEST_CASE("cHandleArgument - -nosound disables sound and music", "[arguments]")
{
    InitialGameSettings settings;
    run({"-nosound"}, settings);
    REQUIRE_FALSE(settings.playMusic);
    REQUIRE_FALSE(settings.playSound);
}

TEST_CASE("cHandleArgument - multiple flags are all applied", "[arguments]")
{
    InitialGameSettings settings;
    run({"-noai", "-windowed", "-fogofwar"}, settings);
    REQUIRE(settings.disableAI);
    REQUIRE(settings.windowed);
    REQUIRE(settings.fogOfWarEnabled);
    REQUIRE(changedFields(settings).size() == 3);
}

TEST_CASE("cHandleArgument - -windowed and -fullscreen: last one wins", "[arguments]")
{
    InitialGameSettings settings;
    run({"-windowed", "-fullscreen"}, settings);
    REQUIRE_FALSE(settings.windowed);
}

TEST_CASE("cHandleArgument - scaling mode flags", "[arguments]")
{
    InitialGameSettings settings;
    run({"-letterbox"}, settings);
    REQUIRE(settings.scalingMode == eScalingMode::LETTERBOX);
    run({"-integerscale"}, settings);
    REQUIRE(settings.scalingMode == eScalingMode::INTEGER_SCALE);
}

TEST_CASE("cHandleArgument - options with a value consume it", "[arguments]")
{
    InitialGameSettings settings;
    run({"-screenWidth", "1280", "-screenHeight", "720", "-game", "custom.ini"}, settings);
    REQUIRE(settings.screenW == 1280);
    REQUIRE(settings.screenH == 720);
    REQUIRE(settings.gameFilename == "custom.ini");
}

TEST_CASE("cHandleArgument - option missing its value throws", "[arguments]")
{
    InitialGameSettings settings;
    REQUIRE_THROWS_AS(run({"-game"}, settings), std::runtime_error);
    REQUIRE_THROWS_AS(run({"-screenWidth"}, settings), std::runtime_error);
    REQUIRE_THROWS_AS(run({"-screenHeight"}, settings), std::runtime_error);
}

TEST_CASE("cHandleArgument - unknown option throws", "[arguments]")
{
    InitialGameSettings settings;
    REQUIRE_THROWS_AS(run({"-bogus"}, settings), std::runtime_error);
}

TEST_CASE("cHandleArgument - help returns HELP_REQUESTED and stops parsing", "[arguments]")
{
    InitialGameSettings settings;
    REQUIRE(run({"-h", "-nosound"}, settings) == cHandleArgument::HELP_REQUESTED);
    REQUIRE(settings.playSound);
}
