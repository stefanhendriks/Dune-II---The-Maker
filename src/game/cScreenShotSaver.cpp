#include "game/cScreenShotSaver.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <format>
#include <chrono>

unsigned int cScreenShotSaver::screenCount = 0;

bool cScreenShotSaver::saveScreen(SDL_Renderer* renderer, int width, int height)
{
    screenCount++;
    std::string filename = std::format("screen_{}_{}x{}_{:0>4}.png", getBaseFileName() , width, height,screenCount);
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
        std::chrono::time_point_cast<std::chrono::days>(std::chrono::system_clock::now())
    };
    return std::format("{:%F}", now);
}