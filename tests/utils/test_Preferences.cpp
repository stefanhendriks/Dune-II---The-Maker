/**
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 */

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string>
#include "game/Preferences.hpp"

namespace {
std::filesystem::path freshDirectory(const char *name)
{
    const auto dir = std::filesystem::temp_directory_path() / name;
    std::filesystem::remove_all(dir);
    return dir;
}
}

TEST_CASE("Preferences - saved settings load back identical", "[preferences]")
{
    const auto file = freshDirectory("d2tm_prefs_roundtrip") / "preferences.ini";

    InitialGameSettings saved;
    saved.screenW = 1280;
    saved.screenH = 720;
    saved.cameraDragMoveSpeed = 0.75f;
    saved.cameraBorderOrKeyMoveSpeed = 0.25f;
    saved.cameraEdgeMove = false;
    saved.windowed = true;
    saved.scalingMode = eScalingMode::LETTERBOX;
    saved.allowRepeatingReinforcements = true;
    saved.turretsDownOnLowPower = true;
    saved.rocketTurretsDownOnLowPower = true;
    saved.showLowPowerIcon = true;
    saved.playMusic = false;
    saved.playSound = false;
    saved.debugMode = true;
    saved.drawUnitDebug = true;
    saved.disableAI = true;
    saved.oneAi = true;
    saved.disableWormAi = true;
    saved.disableReinforcements = true;
    saved.noAiRest = true;
    saved.drawUsages = true;
    saved.fogOfWarEnabled = true;
    saved.pauseWhenLosingFocus = true;
    saved.gameFilename = "custom.ini";

    std::string error;
    REQUIRE(Preferences::save(file, saved, error));

    InitialGameSettings loaded;
    REQUIRE(Preferences::load(file, loaded, error));
    REQUIRE(loaded.screenW == 1280);
    REQUIRE(loaded.screenH == 720);
    REQUIRE(loaded.cameraDragMoveSpeed == 0.75f);
    REQUIRE(loaded.cameraBorderOrKeyMoveSpeed == 0.25f);
    REQUIRE_FALSE(loaded.cameraEdgeMove);
    REQUIRE(loaded.windowed);
    REQUIRE(loaded.scalingMode == eScalingMode::LETTERBOX);
    REQUIRE(loaded.allowRepeatingReinforcements);
    REQUIRE(loaded.turretsDownOnLowPower);
    REQUIRE(loaded.rocketTurretsDownOnLowPower);
    REQUIRE(loaded.showLowPowerIcon);
    REQUIRE_FALSE(loaded.playMusic);
    REQUIRE_FALSE(loaded.playSound);
    REQUIRE(loaded.debugMode);
    REQUIRE(loaded.drawUnitDebug);
    REQUIRE(loaded.disableAI);
    REQUIRE(loaded.oneAi);
    REQUIRE(loaded.disableWormAi);
    REQUIRE(loaded.disableReinforcements);
    REQUIRE(loaded.noAiRest);
    REQUIRE(loaded.drawUsages);
    REQUIRE(loaded.fogOfWarEnabled);
    REQUIRE(loaded.pauseWhenLosingFocus);
    REQUIRE(loaded.gameFilename == "custom.ini");
}

TEST_CASE("Preferences - loading a missing file leaves the settings untouched", "[preferences]")
{
    InitialGameSettings settings;
    settings.fogOfWarEnabled = true;
    std::string error;
    REQUIRE_FALSE(Preferences::load(freshDirectory("d2tm_prefs_missing") / "preferences.ini", settings, error));
    REQUIRE_FALSE(error.empty());
    REQUIRE(settings.fogOfWarEnabled);
}

TEST_CASE("Preferences - the low power icon is off unless the preferences enable it", "[preferences]")
{
    REQUIRE_FALSE(InitialGameSettings{}.showLowPowerIcon);

    const auto file = freshDirectory("d2tm_prefs_lowpoweroff") / "preferences.ini";
    InitialGameSettings saved;
    std::string error;
    REQUIRE(Preferences::save(file, saved, error));

    InitialGameSettings loaded;
    loaded.showLowPowerIcon = true;
    REQUIRE(Preferences::load(file, loaded, error));
    REQUIRE_FALSE(loaded.showLowPowerIcon);
}
