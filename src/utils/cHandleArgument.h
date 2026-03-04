#pragma once

#include <map>
#include <string>
#include "utils/GameSettings.hpp"

class cHandleArgument {
public:
    cHandleArgument() = default;
    ~cHandleArgument() = default;

    int handleArguments(int argc, char *argv[], GameSettings *settings);

private:
    enum class Options : char {
        GAME,
        WINDOWED,
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
        SCREENX,
        SCREENY,
        HELP
    };

    static const std::map<std::string, Options> optionStrings;

    void printInstructions() const;
};