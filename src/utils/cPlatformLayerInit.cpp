#include "cPlatformLayerInit.h"

#include "utils/cLog.h"

#include <allegro/config.h>
#include <allegro/system.h>
#include <allegro/timer.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <cstring>
#include <stdexcept>

static bool allegroInitialized = false;

cPlatformLayerInit::cPlatformLayerInit(const std::string& path_to_config_file, const std::string& window_title) {
    cLogger *logger = cLogger::getInstance();
    logger->logHeader("Allegro");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        logger->log(LOG_FATAL, COMP_SDL2, "SDL2 init", SDL_GetError(), OUTC_FAILED);
        throw std::runtime_error(SDL_GetError());
    } else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL2 init", "Initialized successfully", OUTC_SUCCESS);
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
    {
        logger->log(LOG_FATAL, COMP_SDL2, "SDL2 mixer", Mix_GetError(), OUTC_FAILED);
        throw std::runtime_error(Mix_GetError());
    }  else {
        logger->log(LOG_INFO, COMP_SDL2, "SDL2_mixer", "Initialized successfully", OUTC_SUCCESS);
    }

    if (allegroInitialized) {
        throw std::runtime_error("Allegro initialization was attempted twice.");
    }
    allegroInitialized = true;

    set_config_file(path_to_config_file.c_str());

    // Using the install function wihout registering an at_exit function. The cleanup will be done
    // in the destructor of this object.
    if (install_allegro(SYSTEM_AUTODETECT, &errno, nullptr) != 0) {
        const char* error_message = std::strerror(errno);
        logger->log(LOG_FATAL, COMP_ALLEGRO, "Allegro init", error_message, OUTC_FAILED);
        throw std::runtime_error(error_message);
    }

    logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_SUCCESS);

	set_window_title(window_title.c_str());
	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", window_title, OUTC_SUCCESS);
}

cPlatformLayerInit::~cPlatformLayerInit() {
    cLogger *logger = cLogger::getInstance();
    logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro shutdown", "Shutting down...");
    allegro_exit();
    logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro shutdown", "Thanks for playing!", OUTC_SUCCESS);
    Mix_CloseAudio();
    logger->log(LOG_INFO, COMP_SDL2, "SDL2_mixer shutdown", "Thanks for playing!", OUTC_SUCCESS);
    SDL_Quit();
    logger->log(LOG_INFO, COMP_SDL2, "SDL2 shutdown", "Thanks for playing!", OUTC_SUCCESS);
}
