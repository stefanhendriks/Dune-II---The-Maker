#include "game/cScreenShotSaver.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <format>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

unsigned int cScreenShotSaver::screenCount = 0;

bool cScreenShotSaver::saveScreen(SDL_Renderer* renderer, int width, int height)
{
    screenCount++;
    std::string filename = std::format("{}_{}x{}_{:0>4}.png", getBaseFileName() , width, height,screenCount);
    std::cout << filename << std::endl;
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        std::cerr << "Erreur création surface: " << SDL_GetError() << std::endl;
        return false;
    }
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
        std::cerr << "Erreur lecture pixels: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }
    IMG_SavePNG(surface, filename.c_str());
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
            std::cerr << "error creating folder : " << e.what() << '\n';
            return baseName;
        }
    }
    else if (!fs::is_directory(folder)) {
        std::cerr << folder << " exist but it's not a folder !\n";
        return baseName;
    }
    std::cout << folderName << "/" << baseName << std::endl;
    return folderName+"/"+baseName;
}