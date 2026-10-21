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
#include <vector>
#include "utils/cHandleArgument.h"

namespace {
int run(std::vector<const char *> args, InitialGameSettings &settings)
{
    args.insert(args.begin(), "d2tm");
    cHandleArgument handler;
    return handler.handleArguments(static_cast<int>(args.size()), const_cast<char **>(args.data()), &settings);
}
}

TEST_CASE("cHandleArgument - no arguments leaves settings untouched", "[arguments]")
{
    InitialGameSettings settings;
    REQUIRE(run({}, settings) == 0);
    REQUIRE(settings.playMusic);
    REQUIRE(settings.playSound);
    REQUIRE(settings.gameFilename == "game.ini");
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
