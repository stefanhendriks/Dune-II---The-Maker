#include "cHandleArgument.hpp"
#include "cGame.h"
#include <iostream>

cHandleArgument::cHandleArgument(cGame *_game){
    game=_game;
}

cHandleArgument::~cHandleArgument()
{}

int cHandleArgument::handleArguments(int argc, char *argv[]) {

    game->m_disableAI = false;
    game->m_disableReinforcements = false;
    game->m_drawUsages = false;
    game->m_drawUnitDebug = false;
    game->m_oneAi = false;
    game->m_windowed = false;
    game->m_noAiRest = false;
    game->setDebugMode(false);

	if (argc <2) {
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        std::string command = argv[i];
        auto itr = optionStrings.find(command);
        if( itr == optionStrings.end() ) {
            std::cerr << "Unknown option, check with -usages" << std::endl;
            return -1;
        }

        switch (itr->second) {
            case    Options::GAME :  if ((i + 1) < argc) {
					    i++;
					    game->setGameFilename(std::string(argv[i]));
                        } break;
            case    Options::WINDOWED:  // Windowed flag passed, so use that
                        optionToHandleAfter[Options::WINDOWED] = true;
                        break;
            case    Options::NOMUSIC:
                        game->m_playMusic = false;
                        break;
            case    Options::NOSOUND:   // disable all sound effects
			        	game->m_playMusic = false;
			        	game->m_playSound = false;
                        break;
            case    Options::DEBUG:   // generic debugging enabled
			        	game->setDebugMode(true);
                        break;
            case    Options::DEBUG_UNITS:   // unit debugging enabled
			        	game->m_drawUnitDebug = true;
                        break;
            case    Options::NOAI:
			        	game->m_disableAI = true;
                        break;
            case    Options::ONEAI:
			        	game->m_oneAi = true;
                        break;
            case    Options::NOREINFORCEMENTS:
			        	game->m_disableReinforcements = true;
                        break;
            case    Options::NOAIREST:
			        	game->m_noAiRest = true;
                        break;
            case    Options::USAGES:
			default : 
                    game->m_drawUsages = true;
                    break;
        }
	} // arguments passed
	return 0;
}

void cHandleArgument::applyArguments(){
    for (const auto& [key, value] : optionToHandleAfter) {
        switch (key) {
            case    Options::WINDOWED:  game->m_windowed = value;   break;
            default:    break;
        }
    }
}