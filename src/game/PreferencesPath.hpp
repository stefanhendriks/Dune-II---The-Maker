/**
 * @file PreferencesPath.hpp
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

#include <filesystem>
#include <optional>

// The one place that knows where per-user data (campaign progress, preferences) lives.
// This is the only code that calls SDL_GetPrefPath.
namespace PreferencesPath {

// The per-user data directory, or nullopt when the platform could not provide one.
std::optional<std::filesystem::path> directory();

// directory() / fileName, or nullopt when there is no directory.
std::optional<std::filesystem::path> file(const char *fileName);

}
