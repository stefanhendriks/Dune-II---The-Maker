#include "game/cPlatformLayerInit.h"

#include "utils/cLog.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cstring>
#include <stdexcept>

cPlatformLayerInit::cPlatformLayerInit()
{
    cLogger *logger = cLogger::getInstance();
    logger->logHeader("SDL");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        logger->log(LOG_FATAL, COMP_SDL2, "SDL init", SDL_GetError(), OUTC_FAILED);
        throw std::runtime_error(SDL_GetError());
    }
    else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL init", "Initialized successfully", OUTC_SUCCESS);
    }

    if (!MIX_Init()) {
        logger->log(LOG_FATAL, COMP_SDL2, "SDL mixer", SDL_GetError(), OUTC_FAILED);
        throw std::runtime_error(SDL_GetError());
    }
    else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL_mixer", "Initialized successfully", OUTC_SUCCESS);
    }

    if (!TTF_Init()) {
        logger->log(LOG_FATAL, COMP_SDL2, "SDL ttf", SDL_GetError(), OUTC_FAILED);
    }
    else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL_ttf", "Initialized successfully", OUTC_SUCCESS);
    }
}

cPlatformLayerInit::~cPlatformLayerInit()
{
    cLogger *logger = cLogger::getInstance();
    logger->log(LOG_INFO, COMP_SDL2, "SDL shutdown", "Shutting down...");
    TTF_Quit();
    MIX_Quit();
    logger->log(LOG_INFO, COMP_SDL2, "SDL_mixer shutdown", "Thanks for playing!", OUTC_SUCCESS);
    // On Linux, SDL3 registers its own atexit() handler that calls SDL_Quit() before
    // C++ global destructors run. Calling it again here causes a double-free crash.
    // Guard so we only call SDL_Quit() if SDL is still initialised.
    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
    logger->log(LOG_INFO, COMP_SDL2, "SDL shutdown", "Thanks for playing!", OUTC_SUCCESS);
}
