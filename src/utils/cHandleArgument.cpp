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
int cHandleArgument::handleArguments(int argc, char *argv[])
{
    // TODO: make this return a config object, and not be tightly coupled to cGame
    m_game->m_disableAI = false;
    m_game->m_disableReinforcements = false;
    m_game->m_drawUsages = false;
    m_game->m_drawUnitDebug = false;
    m_game->m_oneAi = false;
    m_game->m_disableWormAi = false;
    m_game->m_windowed = false;
    m_game->m_allowRepeatingReinforcements = false;
    m_game->m_noAiRest = false;
    m_game->setDebugMode(false);
    m_game->setTurretsDownOnLowPower(false);
    m_game->setRocketTurretsDownOnLowPower(true);

    if (argc < 2) {
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        std::string command = argv[i];
        auto itr = optionStrings.find(command);
        if (itr == optionStrings.end()) {
            std::cerr << std::format("Unknown option {}, use --help for instructions", command);
            return -1;
        }

        switch (itr->second) {
            case Options::HELP:
                printInstructions();
                return -2;
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
            case Options::NOWORMAI:
                m_game->m_disableWormAi = true;
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

void cHandleArgument::printInstructions() const
{
    std::cout << std::format("\n");
    std::cout << std::format("Usage: d2tm[.exe] [-windowed] [-screenWidth <width>] ...\n\n");
    std::cout << std::format("Graphics\n");
    std::cout << std::format("--------\n\n");
    std::cout << std::format("-windowed              - Run game in window instead of fullscreen\n");
    std::cout << std::format("-screenWidth <value>   - Width of screen / window (minimum 800)\n");
    std::cout << std::format("-screenHeight <value>  - Height of screen / window (minimum 600)\n");
    std::cout << std::format("\n\n");
    std::cout << std::format("Sound/Music\n");
    std::cout << std::format("-----------\n\n");
    std::cout << std::format("-no-sound              - Run game without any sounds or music\n");
    std::cout << std::format("-no-music              - Use sound effects only, no music\n");
    std::cout << std::format("\n\n");
    std::cout << std::format("Experimental\n");
    std::cout << std::format("------------\n\n");
    std::cout << std::format("-game <filename>       - Specify which file to use instead of game.ini\n");
    std::cout << std::format("\n\n");
    std::cout << std::format("Debugging\n");
    std::cout << std::format("---------\n\n");
    std::cout << std::format("-debug                 - Run in debug mode\n");
    std::cout << std::format("-debug-units           - Draw additional debug info about units\n");
    std::cout << std::format("-noai                  - Disable player AI's\n");
    std::cout << std::format("-nowormai              - Disable worm AI\n");
    std::cout << std::format("-oneai                 - Disable all but one player AI\n");
    std::cout << std::format("-noreinforcements      - Disable reinforcements (Campaign mode)\n");
    std::cout << std::format("-noairest              - Disable initial delay of player AI before becoming active\n");
    std::cout << std::format("-usages                - Draw used units, structures, particles, etc.\n");
    std::cout << std::format("\n\n");
    std::cout << std::format("Examples (Windows)\n");
    std::cout << std::format("------------------\n\n");
    std::cout << std::format("Run windowed with resolution 1024x768:\n");
    std::cout << std::format("d2tm.exe -windowed -screenWidth 1024 -screenHeight 768\n");
    std::cout << std::format("\n\n");
    std::cout << std::format("Run without any sounds and debugging enabled:\n");
    std::cout << std::format("d2tm.exe -nosound -debug\n");
    std::cout << std::format("\n\n");
}

void cHandleArgument::applyArguments()
{
    for (const auto&[key, value]: m_optionToHandleAfter) {
        switch (key) {
            case Options::WINDOWED:
                m_game->m_windowed = value;
                break;
            case Options::SCREENX:
                m_game->m_screenW = m_argumentScreenX;
                break;
            case Options::SCREENY:
                m_game->m_screenH = m_argumentScreenY;
                break;
            default:
                break;
        }
    }
}