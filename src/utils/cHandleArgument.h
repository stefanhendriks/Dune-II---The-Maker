#pragma once

#include <map>
#include <string>
#include "utils/GameSettings.hpp"

class cHandleArgument {
public:
    cHandleArgument() = default;
    ~cHandleArgument() = default;

    int handleArguments(int argc, char *argv[], GameSettings &settings);

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
        SCREENX,
        SCREENY,
        HELP
    };

    const std::map<std::string, Options> optionStrings{
        {"-game",             Options::GAME},
        {"-windowed",         Options::WINDOWED},
        {"-nomusic",          Options::NOMUSIC},
        {"-nosound",          Options::NOSOUND},
        {"-debug",            Options::DEBUG},
        {"-debug-units",      Options::DEBUG_UNITS},
        {"-noai",             Options::NOAI},
        {"-oneai",            Options::ONEAI},
        {"-nowormai",         Options::NOWORMAI},
        {"-noreinforcements", Options::NOREINFORCEMENTS},
        {"-noairest",         Options::NOAIREST},
        {"-screenWidth",      Options::SCREENX},
        {"-screenHeight",     Options::SCREENY},
        {"-usages",           Options::USAGES},
        {"--help",            Options::HELP}
    };


    void printInstructions() const;
};