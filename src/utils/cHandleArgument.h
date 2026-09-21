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