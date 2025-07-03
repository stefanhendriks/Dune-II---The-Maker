#include "cPlatformLayerInit.h"

#include "utils/cLog.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <cstring>
#include <stdexcept>

cPlatformLayerInit::cPlatformLayerInit()
{
    cLogger *logger = cLogger::getInstance();
    logger->logHeader("SDL");

    if (SDL_Init(SDL_INIT_EVERYTHING)< 0) {
        logger->log(LOG_FATAL, COMP_SDL2, "SDL2 init", SDL_GetError(), OUTC_FAILED);
        throw std::runtime_error(SDL_GetError());
    }
    else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL2 init", "Initialized successfully", OUTC_SUCCESS);
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) { //Initialisation de l'API Mixer
        logger->log(LOG_FATAL, COMP_SDL2, "SDL2 mixer", Mix_GetError(), OUTC_FAILED);
        throw std::runtime_error(Mix_GetError());
    }
    else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL2_mixer", "Initialized successfully", OUTC_SUCCESS);
    }

    if (TTF_Init() < 0) {
        logger->log(LOG_FATAL, COMP_SDL2, "SDL2 ttf", TTF_GetError(), OUTC_FAILED);
    }
    else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL2_ttf", "Initialized successfully", OUTC_SUCCESS);
    }
}

cPlatformLayerInit::~cPlatformLayerInit()
{
    cLogger *logger = cLogger::getInstance();
    logger->log(LOG_INFO, COMP_ALLEGRO, "SDL shutdown", "Shutting down...");
    TTF_Quit();
    Mix_CloseAudio();
    logger->log(LOG_INFO, COMP_SDL2, "SDL2_mixer shutdown", "Thanks for playing!", OUTC_SUCCESS);
    SDL_Quit();
    logger->log(LOG_INFO, COMP_SDL2, "SDL2 shutdown", "Thanks for playing!", OUTC_SUCCESS);
}
