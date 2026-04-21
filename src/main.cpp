/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "game/cGame.h"

#include "utils/cLog.h"
#include "utils/Log.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "utils/cHandleArgument.h"


#include <string>

#include <iostream>
#include <SDL2/SDL_ttf.h>

// the ultimate game variable(s)
cGame          				game;
SDLDrawer                   *global_renderDrawer = nullptr;
std::shared_ptr<Graphics>   gfxdata;


int main(int argc, char **argv)
{
    std::unique_ptr<cLog> m_log = std::make_unique<cLog>("nlog.txt");

    std::unique_ptr<InitialGameSettings> settings = loadSettingsFromIni("settings.ini");
    
    // read command-line arguments, can override settings.ini
    cHandleArgument handleArg;
    try {
        int parseResult = handleArg.handleArguments(argc, argv, settings.get());
        if (parseResult < 0) {
            cLogger::getInstance()->log(LOG_ERROR, COMP_GAMERULES, "HandleArgument", "Incorrect arguments");
            return 1;
        }
    }
    catch (std::runtime_error &e) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_GAMERULES, "HandleArgument", std::format("Runtime_error on parsing : {}", e.what()));
        std::cerr << std::format("Error: {}\n\n", e.what());
        return 1;
    }

    game.getLog(m_log.get());
    game.applySettings(std::move(settings));

    try {
        if (game.setupGame()) {
            game.run();
        }

        game.shutdown();
    }
    catch (std::runtime_error &e) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_NONE, "Unknown", std::format("Error: {}", e.what()));
        std::cerr << std::format("Error: {}\n\n", e.what());
    }

    std::cout << std::format("Thank you for playing Dune 2 - The Maker\n");
    return 0;
}