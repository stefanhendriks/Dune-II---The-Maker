/**
 * @file main.cpp
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

#include "game/cGame.h"

#include "config.h"
#include "utils/Log.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "utils/cHandleArgument.h"
#include "utils/common.h"


#include <string>

#include <iostream>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

int main(int argc, char **argv)
{
    std::unique_ptr<cLog> m_log = std::make_unique<cLog>("nlog.txt");
    Logger::init(m_log.get());
    Logger::info(COMP_NONE, "main", "=== Dune II - The Maker ===");
    Logger::info(COMP_VERSION, "main", "=== Version information ===");
    Logger::info(COMP_VERSION, "Initializing", "Version {}, Compiled at {} , {}", D2TM_VERSION, __DATE__, __TIME__);

    cGame game;

    std::unique_ptr<InitialGameSettings> settings = loadSettingsFromIni("settings.ini");
    
    // read command-line arguments, can override settings.ini
    cHandleArgument handleArg;
    try {
        int parseResult = handleArg.handleArguments(argc, argv, settings.get());
        if (parseResult < 0) {
            m_log->error(COMP_GAMERULES, "HandleArgument", "Incorrect arguments");
            return 1;
        }
    }
    catch (std::runtime_error &e) {
        m_log->error(eLogComponent::COMP_GAMERULES, "HandleArgument", "Runtime_error on parsing : {}", e.what());
        std::cerr << std::format("Error: {}\n\n", e.what());
        return 1;
    }

    m_log->setDebug(settings->debugMode);
    
    game.applySettings(std::move(settings));

    try {
        if (game.setupGame()) {
            game.run();
        }

        game.shutdown();
    }
    catch (std::runtime_error &e) {
        m_log->error(eLogComponent::COMP_NONE, "Unknown", "Error: {}", e.what());
        std::cerr << std::format("Error: {}\n\n", e.what());
    }

    std::cout << std::format("Thank you for playing Dune 2 - The Maker\n");
    return 0;
}