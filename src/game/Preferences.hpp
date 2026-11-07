/**
 * @file Preferences.hpp
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

#include "utils/InitialGameSettings.hpp"

#include <filesystem>
#include <string>

class cSection;

// Saving and loading the player's options, in the same [SETTINGS] format as settings.ini.
namespace Preferences {

// Sets the settings for every key present in the section, keys that are absent keep their current value.
void readSettings(const cSection &section, InitialGameSettings &settings);

// Loads the preferences file over the settings. Returns false (settings untouched) when there is
// no usable file, and sets error.
bool load(const std::filesystem::path &file, InitialGameSettings &settings, std::string &error);

// Writes every value of the settings. On failure returns false and sets error.
bool save(const std::filesystem::path &file, const InitialGameSettings &settings, std::string &error);

}
