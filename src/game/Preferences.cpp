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
