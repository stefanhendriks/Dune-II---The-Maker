#pragma once

#include <map>
#include <string>

class cGame;

class cHandleArgument
{
public:
    cHandleArgument(cGame *_game);
    ~cHandleArgument();
    int handleArguments(int argc, char *argv[]);
    void applyArguments();
private:
    enum class Options : char {GAME, WINDOWED, NOMUSIC, NOSOUND, DEBUG, DEBUG_UNITS, NOAI, ONEAI, NOREINFORCEMENTS, NOAIREST, USAGES};
    const std::map<std::string, Options> optionStrings {
        { "-game", Options::GAME },
        { "-windowed", Options::WINDOWED },
        { "-nomusic", Options::NOMUSIC },
        { "-nosound", Options::NOSOUND },
        { "-debug", Options::DEBUG },
        { "-debug-units", Options::DEBUG_UNITS },
        { "-noai", Options::NOAI },
        { "-oneai", Options::ONEAI },
        { "-noreinforcements", Options::NOREINFORCEMENTS },
        { "-noairest", Options::NOAIREST },
        { "-usages", Options::USAGES }
    };

    cGame *game;
    std::map<Options, bool> optionToHandle;
};