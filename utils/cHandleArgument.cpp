#include "cHandleArgument.h"
#include "cGame.h"
#include <iostream>

cHandleArgument::cHandleArgument(cGame *game) :
m_game(game),
m_optionToHandleAfter(),
m_argumentScreenX(800),
m_argumentScreenY(600)
{

}

cHandleArgument::~cHandleArgument() {}

/**
 * Parses arguments given. Returns 0 on success or < 0 on failure.
 *
 * @param argc
 * @param argv
 * @return
 */
int cHandleArgument::handleArguments(int argc, char *argv[]) {
    // TODO: make this return a config object, and not be tightly coupled to cGame
    m_game->m_disableAI = false;
    m_game->m_disableReinforcements = false;
    m_game->m_drawUsages = false;
    m_game->m_drawUnitDebug = false;
    m_game->m_oneAi = false;
    m_game->m_windowed = false;
    m_game->m_noAiRest = false;
    m_game->setDebugMode(false);

    if (argc < 2) {
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        std::string command = argv[i];
        auto itr = optionStrings.find(command);
        if (itr == optionStrings.end()) {
            std::cerr << "Unknown option \"" << command << "\", check with -usages" << std::endl;
            return -1;
        }

        switch (itr->second) {
            case Options::GAME :
                if ((i + 1) < argc) {
                    i++;
                    m_game->setGameFilename(std::string(argv[i]));
                }
                break;
            case Options::WINDOWED:  // Windowed flag passed, so use that
                m_optionToHandleAfter[Options::WINDOWED] = true;
                break;
            case Options::NOMUSIC:
                m_game->m_playMusic = false;
                break;
            case Options::NOSOUND:   // disable all sound effects
                m_game->m_playMusic = false;
                m_game->m_playSound = false;
                break;
            case Options::DEBUG:   // generic debugging enabled
                m_game->setDebugMode(true);
                break;
            case Options::DEBUG_UNITS:   // unit debugging enabled
                m_game->m_drawUnitDebug = true;
                break;
            case Options::NOAI:
                m_game->m_disableAI = true;
                break;
            case Options::ONEAI:
                m_game->m_oneAi = true;
                break;
            case Options::NOREINFORCEMENTS:
                m_game->m_disableReinforcements = true;
                break;
            case Options::NOAIREST:
                m_game->m_noAiRest = true;
                break;
            case Options::SCREENX:
                if ((i + 1) < argc) {
                    i++;
                    m_optionToHandleAfter[Options::SCREENX] = true;
                    m_argumentScreenX = atoi(argv[i]);
                }
                break;
            case Options::SCREENY:
                if ((i + 1) < argc) {
                    i++;
                    m_optionToHandleAfter[Options::SCREENY] = true;
                    m_argumentScreenY = atoi(argv[i]);
                }
                break;
            case Options::USAGES:
            default :
                m_game->m_drawUsages = true;
                break;
        }
    } // arguments passed
    return 0;
}

void cHandleArgument::applyArguments() {
    for (const auto&[key, value]: m_optionToHandleAfter) {
        switch (key) {
            case Options::WINDOWED:
                m_game->m_windowed = value;
                break;
            case Options::SCREENX:
                m_game->m_screenX = m_argumentScreenX;
                break;
            case Options::SCREENY:
                m_game->m_screenY = m_argumentScreenY;
                break;
            default:
                break;
        }
    }
}