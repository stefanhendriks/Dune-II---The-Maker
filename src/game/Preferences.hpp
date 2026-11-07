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

// Saving the player's options to a preferences file, in the same [SETTINGS] format as settings.ini.
namespace Preferences {

// Writes every value of the settings. On failure returns false and sets error.
bool save(const std::filesystem::path &file, const InitialGameSettings &settings, std::string &error);

}
