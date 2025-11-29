#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <format>
#include <chrono>
#include <filesystem>

#include "game/cScreenShotSaver.h"
#include "utils/cLog.h"

namespace fs = std::filesystem;

unsigned int cScreenShotSaver::screenCount = 0;

bool cScreenShotSaver::saveScreen(SDL_Renderer* renderer, int width, int height)
{
    screenCount++;
    std::string filename = std::format("{}_{}x{}_{:0>4}.png", getBaseFileName() , width, height,screenCount);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_SDL2, "saveScreen", std::format("Error creating surface: {}",SDL_GetError()), OUTC_FAILED);
        return false;
    }
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_SDL2, "saveScreen", std::format("Error reading pixels: {}",SDL_GetError()), OUTC_FAILED);
        SDL_FreeSurface(surface);
        return false;
    }
    if (IMG_SavePNG(surface, filename.c_str()) != 0) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_SDL2, "saveScreen", std::format("IMG_SavePNG error: {}",SDL_GetError()), OUTC_FAILED);
    }        
    SDL_FreeSurface(surface);
    return true;
}

std::string cScreenShotSaver::getBaseFileName()
{
    auto now = std::chrono::year_month_day{
        std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())
    };
    std::string baseName = std::format("{:%F}", now);
    std::string folderName = std::format("screen_{:%Y-%m}", now);

    fs::path folder = folderName;
    if (!fs::exists(folder)) {
        try {
            fs::create_directory(folder);
        }
        catch (const fs::filesystem_error& e) {
            cLogger::getInstance()->log(LOG_ERROR, COMP_SDL2, "cScreenShotSaver", std::format("error creating folder: {}",e.what()), OUTC_FAILED);
            return baseName;
        }
    }
    else if (!fs::is_directory(folder)) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_SDL2, "cScreenShotSaver", std::format("{} exist but it's not a folder !", folderName), OUTC_FAILED);
        return baseName;
    }
    return folderName+"/"+baseName;
}