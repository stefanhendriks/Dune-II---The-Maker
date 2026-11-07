/**
 * @file Preferences.cpp
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


#include "game/Preferences.hpp"

#include "utils/cIniFile.h"

namespace Preferences {

namespace {
constexpr const char *SECTION = "SETTINGS";

// cIniFile expects UTF-8 file names, path::string() would give the ANSI code page on Windows
std::string utf8FileName(const std::filesystem::path &path)
{
    const std::u8string name = path.u8string();
    return std::string(name.begin(), name.end());
}

std::string toText(bool value)
{
    return value ? "true" : "false";
}

void set(cIniFile &ini, const char *key, const std::string &value)
{
    ini.setValue(SECTION, key, value);
}
}

void readSettings(const cSection &section, InitialGameSettings &settings)
{
    if (section.hasValue("ScreenWidth")) {
        settings.screenW = section.getInt("ScreenWidth");
    }
    if (section.hasValue("ScreenHeight")) {
        settings.screenH = section.getInt("ScreenHeight");
    }
    if (section.hasValue("CameraDragMoveSpeed")) {
        settings.cameraDragMoveSpeed = section.getDouble("CameraDragMoveSpeed");
    }
    if (section.hasValue("CameraBorderOrKeyMoveSpeed")) {
        settings.cameraBorderOrKeyMoveSpeed = section.getDouble("CameraBorderOrKeyMoveSpeed");
    }
    if (section.hasValue("FullScreen")) {
        settings.windowed = !section.getBoolean("FullScreen");
    }
    if (section.hasValue("CameraEdgeMove")) {
        settings.cameraEdgeMove = section.getBoolean("CameraEdgeMove");
    }
    if (section.hasValue("ScalingMode")) {
        settings.scalingMode = section.getStringValue("ScalingMode") == "Letterbox"
            ? eScalingMode::LETTERBOX
            : eScalingMode::INTEGER_SCALE;
    }
    if (section.hasValue("AllowRepeatingReinforcements")) {
        settings.allowRepeatingReinforcements = section.getBoolean("AllowRepeatingReinforcements");
    }
    if (section.hasValue("AllTurretsDownOnLowPower")) {
        settings.turretsDownOnLowPower = section.getBoolean("AllTurretsDownOnLowPower");
    }
    if (section.hasValue("RocketTurretsDownOnLowPower")) {
        settings.rocketTurretsDownOnLowPower = section.getBoolean("RocketTurretsDownOnLowPower");
    }
    if (section.hasValue("GameRules")) {
        settings.gameFilename = section.getStringValue("GameRules");
    }
    if (section.hasValue("NoAI")) {
        settings.disableAI = section.getBoolean("NoAI");
    }
    if (section.hasValue("Debug")) {
        settings.debugMode = section.getBoolean("Debug");
    }
    if (section.hasValue("playMusic")) {
        settings.playMusic = section.getBoolean("playMusic");
    }
    if (section.hasValue("playSound")) {
        settings.playSound = section.getBoolean("playSound");
    }
    if (section.hasValue("drawUnitDebug")) {
        settings.drawUnitDebug = section.getBoolean("drawUnitDebug");
    }
    if (section.hasValue("oneAi")) {
        settings.oneAi = section.getBoolean("oneAi");
    }
    if (section.hasValue("disableWormAi")) {
        settings.disableWormAi = section.getBoolean("disableWormAi");
    }
    if (section.hasValue("disableReinforcements")) {
        settings.disableReinforcements = section.getBoolean("disableReinforcements");
    }
    if (section.hasValue("noAiRest")) {
        settings.noAiRest = section.getBoolean("noAiRest");
    }
    if (section.hasValue("drawUsages")) {
        settings.drawUsages = section.getBoolean("drawUsages");
    }
    if (section.hasValue("pauseWhenLosingFocus")) {
        settings.pauseWhenLosingFocus = section.getBoolean("pauseWhenLosingFocus");
    }
    if (section.hasValue("fogOfWarEnabled")) {
        settings.fogOfWarEnabled = section.getBoolean("fogOfWarEnabled");
    }
}

bool load(const std::filesystem::path &file, InitialGameSettings &settings, std::string &error)
{
    std::error_code filesystemError;
    if (!std::filesystem::exists(file, filesystemError)) {
        error = "No preferences file found at " + utf8FileName(file);
        return false;
    }

    cIniFile ini;
    if (!ini.load(utf8FileName(file)) || !ini.hasSection(SECTION)) {
        error = "Preferences file is unusable: " + utf8FileName(file);
        return false;
    }
    readSettings(ini.getSection(SECTION), settings);
    return true;
}

bool save(const std::filesystem::path &file, const InitialGameSettings &settings, std::string &error)
{
    std::error_code filesystemError;
    std::filesystem::create_directories(file.parent_path(), filesystemError);
    if (filesystemError) {
        error = "Could not create preferences directory: " + filesystemError.message();
        return false;
    }

    cIniFile ini;
    set(ini, "ScreenWidth", std::to_string(settings.screenW));
    set(ini, "ScreenHeight", std::to_string(settings.screenH));
    set(ini, "CameraDragMoveSpeed", std::to_string(settings.cameraDragMoveSpeed));
    set(ini, "CameraBorderOrKeyMoveSpeed", std::to_string(settings.cameraBorderOrKeyMoveSpeed));
    set(ini, "FullScreen", toText(!settings.windowed));
    set(ini, "CameraEdgeMove", toText(settings.cameraEdgeMove));
    set(ini, "ScalingMode", settings.scalingMode == eScalingMode::LETTERBOX ? "Letterbox" : "IntegerScale");
    set(ini, "AllowRepeatingReinforcements", toText(settings.allowRepeatingReinforcements));
    set(ini, "AllTurretsDownOnLowPower", toText(settings.turretsDownOnLowPower));
    set(ini, "RocketTurretsDownOnLowPower", toText(settings.rocketTurretsDownOnLowPower));
    set(ini, "GameRules", settings.gameFilename);
    set(ini, "NoAI", toText(settings.disableAI));
    set(ini, "Debug", toText(settings.debugMode));
    set(ini, "playMusic", toText(settings.playMusic));
    set(ini, "playSound", toText(settings.playSound));
    set(ini, "drawUnitDebug", toText(settings.drawUnitDebug));
    set(ini, "oneAi", toText(settings.oneAi));
    set(ini, "disableWormAi", toText(settings.disableWormAi));
    set(ini, "disableReinforcements", toText(settings.disableReinforcements));
    set(ini, "noAiRest", toText(settings.noAiRest));
    set(ini, "drawUsages", toText(settings.drawUsages));
    set(ini, "pauseWhenLosingFocus", toText(settings.pauseWhenLosingFocus));
    set(ini, "fogOfWarEnabled", toText(settings.fogOfWarEnabled));

    // write to a temporary file first, so a crash while writing cannot leave a half written file
    std::filesystem::path tempPath = file;
    tempPath += ".tmp";
    if (!ini.save(utf8FileName(tempPath))) {
        error = "Could not write preferences file";
        return false;
    }
    std::filesystem::rename(tempPath, file, filesystemError);
    if (filesystemError) {
        error = "Could not replace preferences file: " + filesystemError.message();
        return false;
    }
    return true;
}

}
