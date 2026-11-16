/**
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 */

#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <string>

#include "controls/cKeyBindings.h"
#include "game/Preferences.hpp"
#include "utils/cIniFile.h"
#include "utils/InitialGameSettings.hpp"

// D2TM_SOURCE_DIR is set in tests/CMakeLists.txt

namespace {
const char *const READ_PATTERN = R"re(hasValue\("([A-Za-z0-9_]+)"\))re";
const char *const SAVE_PATTERN = R"re(set\(ini, "([A-Za-z0-9_]+)")re";

std::string lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

std::string readFile(const std::string &relativePath)
{
    std::ifstream in(std::filesystem::path(D2TM_SOURCE_DIR) / relativePath);
    REQUIRE(in.good());
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

// Keys (lower case) in a section of settings.ini. A key counts when it is set, or is present as a
// commented out example ("#key=value" or "# key = value"): both are documented for the player.
std::set<std::string> keysInSection(const std::string &section)
{
    std::istringstream in(readFile("resources/bin/settings.ini"));
    const std::regex keyLine(R"(^\s*#?\s*([A-Za-z0-9_]+)\s*=)");
    std::set<std::string> keys;
    std::string line;
    bool inSection = false;
    while (std::getline(in, line)) {
        if (!line.empty() && line[0] == '[') {
            inSection = line.rfind("[" + section + "]", 0) == 0;
            continue;
        }
        std::smatch match;
        if (inSection && std::regex_search(line, match, keyLine)) {
            const std::string key = lower(match[1]);
            // "#true = ..." is a sentence in a comment, not a setting
            if (key != "true" && key != "false") {
                keys.insert(key);
            }
        }
    }
    return keys;
}

// Keys (lower case) that a pattern with one capture group finds in Preferences.cpp
std::set<std::string> keysInPreferences(const std::string &pattern)
{
    const std::string source = readFile("src/game/Preferences.cpp");
    std::set<std::string> keys;
    const std::regex re(pattern);
    for (auto it = std::sregex_iterator(source.begin(), source.end(), re); it != std::sregex_iterator(); ++it) {
        keys.insert(lower((*it)[1]));
    }
    return keys;
}

std::set<std::string> actionNames()
{
    std::set<std::string> actions;
    for (const auto &entry : cKeyBindings::getActionTable()) {
        actions.insert(lower(entry.first));
    }
    return actions;
}

// The keys of 'wanted' that 'have' does not contain, as text for a failure message
std::string missingFrom(const std::set<std::string> &wanted, const std::set<std::string> &have)
{
    std::string missing;
    for (const auto &key : wanted) {
        if (have.count(key) == 0) {
            missing += " " + key;
        }
    }
    return missing;
}
}

TEST_CASE("Settings defaults - InitialGameSettings values without any settings.ini", "[settings]")
{
    const InitialGameSettings gs;
    CHECK(gs.screenW == 800);
    CHECK(gs.screenH == 600);
    CHECK(gs.cameraDragMoveSpeed == 0.5f);
    CHECK(gs.cameraBorderOrKeyMoveSpeed == 0.5f);
    CHECK(gs.cameraEdgeMove);
    CHECK(gs.gamepadCursorSpeed == 600.0f);
    CHECK(gs.gamepadDeadzone == 0.2f);
    CHECK(gs.gamepadCursorCurve == 2.0f);
    CHECK(gs.gamepadScrollThreshold == 0.5f);
    CHECK_FALSE(gs.windowed);
    CHECK(gs.scalingMode == eScalingMode::INTEGER_SCALE);
    CHECK_FALSE(gs.allowRepeatingReinforcements);
    CHECK_FALSE(gs.turretsDownOnLowPower);
    CHECK_FALSE(gs.rocketTurretsDownOnLowPower);
    CHECK_FALSE(gs.showLowPowerIcon);
    CHECK(gs.playMusic);
    CHECK_FALSE(gs.customMusic);
    CHECK(gs.playSound);
    CHECK(gs.playCreditsSound);
    CHECK_FALSE(gs.debugMode);
    CHECK_FALSE(gs.drawUnitDebug);
    CHECK_FALSE(gs.disableAI);
    CHECK_FALSE(gs.oneAi);
    CHECK_FALSE(gs.disableWormAi);
    CHECK_FALSE(gs.disableReinforcements);
    CHECK_FALSE(gs.noAiRest);
    CHECK_FALSE(gs.drawUsages);
    CHECK_FALSE(gs.fogOfWarEnabled);
    CHECK_FALSE(gs.pauseWhenLosingFocus);
    CHECK(gs.gameFilename == "game.ini");
}

TEST_CASE("Settings defaults - a [SETTINGS] section without known keys changes nothing", "[settings]")
{
    cIniFile ini;
    ini.setValue("SETTINGS", "Unrelated", "1");
    InitialGameSettings read;
    Preferences::readSettings(ini.getSection("SETTINGS"), read);

    const InitialGameSettings defaults;
    CHECK(read.screenW == defaults.screenW);
    CHECK(read.screenH == defaults.screenH);
    CHECK(read.cameraEdgeMove == defaults.cameraEdgeMove);
    CHECK(read.windowed == defaults.windowed);
    CHECK(read.playMusic == defaults.playMusic);
    CHECK(read.playSound == defaults.playSound);
    CHECK(read.gameFilename == defaults.gameFilename);
}

TEST_CASE("Settings ini - the shipped settings.ini loads into the documented values", "[settings]")
{
    cIniFile ini;
    REQUIRE(ini.load((std::filesystem::path(D2TM_SOURCE_DIR) / "resources/bin/settings.ini").string()));
    REQUIRE(ini.hasSection("SETTINGS"));
    REQUIRE(ini.hasSection("KEYS"));
    REQUIRE(ini.hasSection("PAD"));

    InitialGameSettings gs;
    Preferences::readSettings(ini.getSection("SETTINGS"), gs);

    CHECK(gs.screenW == 1024);
    CHECK(gs.screenH == 768);
    CHECK(gs.windowed);
    CHECK(gs.scalingMode == eScalingMode::INTEGER_SCALE);
    CHECK_FALSE(gs.allowRepeatingReinforcements);
    CHECK_FALSE(gs.turretsDownOnLowPower);
    CHECK(gs.rocketTurretsDownOnLowPower);
    CHECK_FALSE(gs.showLowPowerIcon);
    CHECK(gs.gameFilename == "game.ini");
    // everything that is commented out in the shipped file keeps its default
    const InitialGameSettings defaults;
    CHECK(gs.playMusic == defaults.playMusic);
    CHECK(gs.playSound == defaults.playSound);
    CHECK(gs.debugMode == defaults.debugMode);
    CHECK(gs.fogOfWarEnabled == defaults.fogOfWarEnabled);
}

TEST_CASE("Settings ini - every [SETTINGS] key read in code is in settings.ini", "[settings]")
{
    const auto readInCode = keysInPreferences(READ_PATTERN);
    REQUIRE(readInCode.size() > 10);
    const auto inIni = keysInSection("SETTINGS");
    // On failure: add the key to resources/bin/settings.ini (commented out when off by default)
    INFO("read in Preferences.cpp, missing in settings.ini:" << missingFrom(readInCode, inIni));
    CHECK(missingFrom(readInCode, inIni).empty());
}

TEST_CASE("Settings ini - every [SETTINGS] key in settings.ini is read in code", "[settings]")
{
    // GameDir is read when the data files are loaded, not by Preferences
    std::set<std::string> readInCode = keysInPreferences(READ_PATTERN);
    readInCode.insert("gamedir");
    const auto inIni = keysInSection("SETTINGS");
    INFO("in settings.ini, not read in code:" << missingFrom(inIni, readInCode));
    CHECK(missingFrom(inIni, readInCode).empty());
}

TEST_CASE("Settings ini - every saved option is also read back", "[settings]")
{
    const auto readInCode = keysInPreferences(READ_PATTERN);
    const auto savedInCode = keysInPreferences(SAVE_PATTERN);
    REQUIRE(savedInCode.size() > 10);
    INFO("saved but never read:" << missingFrom(savedInCode, readInCode));
    CHECK(missingFrom(savedInCode, readInCode).empty());
}

TEST_CASE("Settings ini - [KEYS] and [PAD] only name actions the game knows", "[settings]")
{
    const auto actions = actionNames();
    REQUIRE(actions.size() > 10);

    const auto keys = keysInSection("KEYS");
    INFO("in [KEYS], unknown action:" << missingFrom(keys, actions));
    CHECK(missingFrom(keys, actions).empty());

    // the modifier buttons are not actions, they only exist in [PAD]
    std::set<std::string> padNames = actions;
    padNames.insert({"modifier_shift", "modifier_ctrl", "modifier_alt"});
    const auto pad = keysInSection("PAD");
    INFO("in [PAD], unknown action:" << missingFrom(pad, padNames));
    CHECK(missingFrom(pad, padNames).empty());
}

TEST_CASE("Settings ini - every key action can be remapped in [KEYS]", "[settings]")
{
    // TODO(#1522) - these actions are not in settings.ini yet; remove them here when they are added
    const std::set<std::string> knownMissing = {
        "editor_copy", "editor_cut", "editor_display_axes", "editor_display_grid", "editor_paste",
        "editor_redo", "editor_undo", "group_6", "select_attacking_on_map", "select_harvesters_on_map",
        "select_harvesters_on_screen", "select_infantry_on_map", "select_infantry_on_screen",
        "select_launchers_on_map", "select_launchers_on_screen", "select_same_type_on_map",
        "select_tanks_on_map", "select_tanks_on_screen", "select_wheels_on_map", "select_wheels_on_screen",
    };
    auto keys = keysInSection("KEYS");
    keys.insert(knownMissing.begin(), knownMissing.end());
    const auto actions = actionNames();
    INFO("action without a [KEYS] entry in settings.ini:" << missingFrom(actions, keys));
    CHECK(missingFrom(actions, keys).empty());
}
