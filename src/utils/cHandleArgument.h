/**
 * @file cHandleArgument.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <map>
#include <string>
#include "utils/InitialGameSettings.hpp"

class cHandleArgument {
public:
    cHandleArgument() = default;
    ~cHandleArgument() = default;

    int handleArguments(int argc, char *argv[], InitialGameSettings *settings);

private:
    enum class Options : char {
        GAME,
        WINDOWED,
        FULLSCREEN,
        INTEGERSCALE,
        LETTERBOX,
        NOMUSIC,
        NOSOUND,
        DEBUG,
        DEBUG_UNITS,
        NOAI,
        ONEAI,
        NOWORMAI,
        NOREINFORCEMENTS,
        NOAIREST,
        USAGES,
        PAUSE_WHEN_LOSING_FOCUS,
        FOG_OF_WAR,
        SCREENX,
        SCREENY,
        HELP
    };

    static const std::map<std::string, Options> optionStrings;

    void printInstructions() const;
};