#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <format>
#include <chrono>
#include <filesystem>

#include "game/cScreenShotSaver.h"
#include "utils/Log.h"

namespace fs = std::filesystem;

unsigned int cScreenShotSaver::screenCount = 0;

bool cScreenShotSaver::saveScreen(SDL_Renderer* renderer, int width, int height)
{
    if (renderer == nullptr) {
        Logger::error(COMP_SDL2, "saveScreen", "renderer is null, cannot take screenshot");
        return false;
    }
    try {
        screenCount++;
        std::string filename = std::format("{}_{}x{}_{:0>4}.png", getBaseFileName(), width, height, screenCount);
        SDL_Surface* surface = SDL_RenderReadPixels(renderer, NULL);
        if (!surface) {
            Logger::error(COMP_SDL2, "saveScreen", "Error reading pixels: {}", SDL_GetError());
            return false;
        }
        if (IMG_SavePNG(surface, filename.c_str()) != 0) {
            Logger::error(COMP_SDL2, "saveScreen", "IMG_SavePNG error: {}", SDL_GetError());
        }
        SDL_DestroySurface(surface);
        return true;
    }
    catch (const std::exception& e) {
        Logger::error(COMP_SDL2, "saveScreen", "Unexpected exception: {}", e.what());
        return false;
    }
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
            Logger::error(COMP_SDL2, "cScreenShotSaver", "error creating folder: {}", e.what());
            return baseName;
        }
    }
    else if (!fs::is_directory(folder)) {
        Logger::error(COMP_SDL2, "cScreenShotSaver", "{} exist but it's not a folder !", folderName);
        return baseName;
    }
    return folderName+"/"+baseName;
}
