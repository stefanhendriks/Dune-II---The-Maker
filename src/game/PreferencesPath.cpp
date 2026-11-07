/**
 * @file PreferencesPath.cpp
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


#include "game/PreferencesPath.hpp"

#include "utils/Log.h"

#include <SDL3/SDL.h>

#include <string>
#include <string_view>

namespace PreferencesPath {

std::optional<std::filesystem::path> directory()
{
    char *preferencePath = SDL_GetPrefPath("D2TM", "Dune II - The Maker");
    if (preferencePath == nullptr) {
        Logger::warn(COMP_INIT, "PreferencesPath", "Could not get the user data directory: {}", SDL_GetError());
        return std::nullopt;
    }
    // SDL_GetPrefPath returns UTF-8, a plain char* path would be read in the ANSI code page on Windows
    const std::string_view utf8(preferencePath);
    std::filesystem::path result(std::u8string(utf8.begin(), utf8.end()));
    SDL_free(preferencePath);
    return result;
}

std::optional<std::filesystem::path> file(const char *fileName)
{
    const auto dir = directory();
    if (!dir) return std::nullopt;
    return *dir / fileName;
}

}
