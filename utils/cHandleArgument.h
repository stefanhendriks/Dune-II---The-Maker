#pragma once

#include <map>
#include <string>

class cGame;

class cHandleArgument {

public:
    explicit cHandleArgument(cGame * game);

    ~cHandleArgument();

    int handleArguments(int argc, char *argv[]);

    void applyArguments();

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
            {"-noreinforcements", Options::NOREINFORCEMENTS},
            {"-noairest",         Options::NOAIREST},
            {"-screenWidth",      Options::SCREENX},
            {"-screenHeight",     Options::SCREENY},
            {"-usages",           Options::USAGES},
            {"--help",            Options::HELP}
    };

    cGame *m_game = nullptr;

    std::map<Options, bool> m_optionToHandleAfter;

    int m_argumentScreenX, m_argumentScreenY;

    void printInstructions() const;
};