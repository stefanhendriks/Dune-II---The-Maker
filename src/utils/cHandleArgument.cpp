#include "cHandleArgument.h"
#include "utils/GameSettings.hpp"
#include <iostream>
#include <string>
#include <format>

int cHandleArgument::handleArguments(int argc, char *argv[], GameSettings *settings)
{
    if (argc < 2) {
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        std::string command = argv[i];
        auto itr = optionStrings.find(command);
        if (itr == optionStrings.end()) {
            std::cerr << "Unknown option " << command << ", use --help for instructions\n";
            throw std::runtime_error(std::format("invokes game with unknown option {}",command));
        }

        switch (itr->second) {
            case Options::HELP:
                printInstructions();
                return -2;
            case Options::GAME:
                if ((i + 1) < argc) {
                    i++;
                    settings->gameFilename = std::string(argv[i]);
                } else {
                    throw std::runtime_error("invokes game but no value assigned");
                }
                break;
            case Options::WINDOWED:
                settings->windowed = true;
                break;
            case Options::NOMUSIC:
                settings->playMusic = false;
                break;
            case Options::NOSOUND:
                settings->playSound = false;
                settings->playMusic = false;
                break;
            case Options::DEBUG:
                settings->debugMode = true;
                break;
            case Options::DEBUG_UNITS:
                settings->drawUnitDebug = true;
                break;
            case Options::NOAI:
                settings->disableAI = true;
                break;
            case Options::ONEAI:
                settings->oneAi = true;
                break;
            case Options::NOWORMAI:
                settings->disableWormAi = true;
                break;
            case Options::NOREINFORCEMENTS:
                settings->disableReinforcements = true;
                break;
            case Options::NOAIREST:
                settings->noAiRest = true;
                break;
            case Options::SCREENX:
                if ((i + 1) < argc) {
                    i++;
                    settings->screenW = std::stoi(argv[i]);
                } else {
                    throw std::runtime_error("invokes screenX but no value assigned");
                }
                break;
            case Options::SCREENY:
                if ((i + 1) < argc) {
                    i++;
                    settings->screenH = std::stoi(argv[i]);
                } else {
                    throw std::runtime_error("invokes screenY but no value assigned");
                }
                break;
            case Options::USAGES:
                settings->drawUsages = true;
                break;
        }
    }
    return 0;
}

void cHandleArgument::printInstructions() const
{
    std::cout << "\n";
    std::cout << "Usage: d2tm[.exe] [-windowed] [-screenWidth <width>] ...\n\n";
    std::cout << "Graphics\n";
    std::cout << "--------\n\n";
    std::cout << "-windowed              - Run game in window instead of fullscreen\n";
    std::cout << "-screenWidth <value>   - Width of screen / window (minimum 800)\n";
    std::cout << "-screenHeight <value>  - Height of screen / window (minimum 600)\n";
    std::cout << "\n\n";
    std::cout << "Sound/Music\n";
    std::cout << "-----------\n\n";
    std::cout << "-nosound               - Run game without any sounds or music\n";
    std::cout << "-nomusic               - Use sound effects only, no music\n";
    std::cout << "\n\n";
    std::cout << "Experimental\n";
    std::cout << "------------\n\n";
    std::cout << "-game <filename>       - Specify which file to use instead of game.ini\n";
    std::cout << "\n\n";
    std::cout << "Debugging\n";
    std::cout << "---------\n\n";
    std::cout << "-debug                 - Run in debug mode\n";
    std::cout << "-debug-units           - Draw additional debug info about units\n";
    std::cout << "-noai                  - Disable player AI's\n";
    std::cout << "-nowormai              - Disable worm AI\n";
    std::cout << "-oneai                 - Disable all but one player AI\n";
    std::cout << "-noreinforcements      - Disable reinforcements (Campaign mode)\n";
    std::cout << "-noairest              - Disable initial delay of player AI before becoming active\n";
    std::cout << "-usages                - Draw used units, structures, particles, etc.\n";
    std::cout << "\n\n";
    std::cout << "Examples (Windows)\n";
    std::cout << "------------------\n\n";
    std::cout << "Run windowed with resolution 1024x768:\n";
    std::cout << "d2tm.exe -windowed -screenWidth 1024 -screenHeight 768\n";
    std::cout << "\n\n";
    std::cout << "Run without any sounds and debugging enabled:\n";
    std::cout << "d2tm.exe -nosound -debug\n";
    std::cout << "\n\n";
}
