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
#include <set>
#include <sstream>
#include <string>

#include "controls/cKeyBindings.h"
#include "game/Preferences.hpp"
#include "utils/cIniFile.h"
#include "utils/InitialGameSettings.hpp"

// D2TM_SOURCE_DIR is set in tests/CMakeLists.txt

namespace {
// A key is the quoted name between the prefix and the suffix, e.g. hasValue("Windowed")
const char *const READ_PREFIX = "hasValue(\"";
const char *const READ_SUFFIX = "\")";
const char *const SAVE_PREFIX = "set(ini, \"";
const char *const SAVE_SUFFIX = "\"";

bool isKeyChar(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

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
    std::set<std::string> keys;
    std::string line;
    bool inSection = false;
    while (std::getline(in, line)) {
        if (!line.empty() && line[0] == '[') {
            inSection = line.rfind("[" + section + "]", 0) == 0;
            continue;
        }
        if (!inSection) continue;
        // optional '#', then a name, then '='
        std::size_t pos = line.find_first_not_of(" \t");
        if (pos == std::string::npos) continue;
        if (line[pos] == '#') {
            pos = line.find_first_not_of(" \t", pos + 1);
            if (pos == std::string::npos) continue;
        }
        std::size_t end = pos;
        while (end < line.size() && isKeyChar(line[end])) {
            end++;
        }
        if (end == pos) continue;
        const std::size_t equals = line.find_first_not_of(" \t", end);
        if (equals == std::string::npos || line[equals] != '=') continue;
        const std::string key = lower(line.substr(pos, end - pos));
        // "#true = ..." is a sentence in a comment, not a setting
        if (key != "true" && key != "false") {
            keys.insert(key);
        }
    }
    return keys;
}

// Keys (lower case) in Preferences.cpp written as <prefix>Name<suffix>
std::set<std::string> keysInPreferences(const std::string &prefix, const std::string &suffix)
{
    const std::string source = readFile("src/game/Preferences.cpp");
    std::set<std::string> keys;
    std::size_t pos = source.find(prefix);
    while (pos != std::string::npos) {
        const std::size_t start = pos + prefix.size();
        std::size_t end = start;
        while (end < source.size() && isKeyChar(source[end])) {
            end++;
        }
        if (end > start && source.compare(end, suffix.size(), suffix) == 0) {
            keys.insert(lower(source.substr(start, end - start)));
        }
        pos = source.find(prefix, start);
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
    CHECK(gs.gameFilename == "houses.ini");
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
    CHECK(gs.gameFilename == "houses.ini");
    // everything that is commented out in the shipped file keeps its default
    const InitialGameSettings defaults;
    CHECK(gs.playMusic == defaults.playMusic);
    CHECK(gs.playSound == defaults.playSound);
    CHECK(gs.debugMode == defaults.debugMode);
    CHECK(gs.fogOfWarEnabled == defaults.fogOfWarEnabled);
}

TEST_CASE("Settings ini - every [SETTINGS] key read in code is in settings.ini", "[settings]")
{
    const auto readInCode = keysInPreferences(READ_PREFIX, READ_SUFFIX);
    REQUIRE(readInCode.size() > 10);
    const auto inIni = keysInSection("SETTINGS");
    // On failure: add the key to resources/bin/settings.ini (commented out when off by default)
    INFO("read in Preferences.cpp, missing in settings.ini:" << missingFrom(readInCode, inIni));
    CHECK(missingFrom(readInCode, inIni).empty());
}

TEST_CASE("Settings ini - every [SETTINGS] key in settings.ini is read in code", "[settings]")
{
    // GameDir is read when the data files are loaded, not by Preferences
    std::set<std::string> readInCode = keysInPreferences(READ_PREFIX, READ_SUFFIX);
    readInCode.insert("gamedir");
    const auto inIni = keysInSection("SETTINGS");
    INFO("in settings.ini, not read in code:" << missingFrom(inIni, readInCode));
    CHECK(missingFrom(inIni, readInCode).empty());
}

TEST_CASE("Settings ini - every saved option is also read back", "[settings]")
{
    const auto readInCode = keysInPreferences(READ_PREFIX, READ_SUFFIX);
    const auto savedInCode = keysInPreferences(SAVE_PREFIX, SAVE_SUFFIX);
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
    const auto keys = keysInSection("KEYS");
    const auto actions = actionNames();
    INFO("action without a [KEYS] entry in settings.ini:" << missingFrom(actions, keys));
    CHECK(missingFrom(actions, keys).empty());
}
