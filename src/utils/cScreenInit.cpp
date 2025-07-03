#include "cScreenInit.h"

#include "cLog.h"

#include <SDL2/SDL.h>
#include <fmt/core.h>

#include <array>
#include <cassert>
#include <optional>
#include <span>
#include <vector>

void cScreenInit::setFullScreenMode()
{
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_RenderSetLogicalSize(renderer, renderResolution.width, renderResolution.height);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    std::cout << "Mode : SDL_WINDOW_FULLSCREEN_DESKTOP\n";

    int renderW, renderH;
    SDL_GetRendererOutputSize(renderer, &renderW, &renderH);
    std::cout << "Renderer output size : " << renderW << " x " << renderH << "\n";

    // Affiche le DPI
    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) == 0) {
        std::cout << "DPI : " << ddpi << " (H: " << hdpi << ", V: " << vdpi << ")\n";
    } else {
        std::cerr << "Error DPI: " << SDL_GetError() << "\n";
    }
}

void cScreenInit::setWindowMode()
{
    SDL_SetWindowFullscreen(window, 0);
    SDL_SetWindowSize(window, renderResolution.width, renderResolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_RenderSetLogicalSize(renderer, renderResolution.width, renderResolution.height);
}

void cScreenInit::getWindowResolution()
{
    SDL_DisplayMode mode;
    if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
        std::cout << "Résolution display : " << mode.w << " x " << mode.h << "\n";
    }
    windowResolution.width = mode.w;
    windowResolution.height = mode.h;
}

void cScreenInit::adaptResolution(int desiredWidth, int desiredHeight)
{
    if (desiredWidth<800)
        desiredWidth = 800;
    if (desiredHeight<600)
        desiredHeight = 600;

    int tmpWidth =  windowResolution.width * desiredHeight / windowResolution.height;
    if (tmpWidth > desiredWidth) {
        renderResolution.height = desiredHeight;
        renderResolution.width = tmpWidth;
    } else {
        renderResolution.width = desiredWidth;
        renderResolution.height = windowResolution.height * desiredWidth / windowResolution.width;
    }
}

cScreenInit::cScreenInit(int desiredWidth, int desiredHeight, const std::string &title)
{
    this->getWindowResolution();
    this->adaptResolution(desiredWidth, desiredHeight);

    auto logger = cLogger::getInstance();

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, renderResolution.width , renderResolution.height, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == nullptr) {
        const auto msg = fmt::format("Failed initialized screen with resolution {}x{}", renderResolution.width, renderResolution.height);
        logger->log(LOG_ERROR, COMP_SDL2, "Screen init", msg, OUTC_FAILED);
        SDL_Quit();
        return;
    }
    else {
        const auto msg = fmt::format("Successfully initialized screen with resolution {}x{}.", renderResolution.width, renderResolution.height);
        std::cout << msg << std::endl;
        logger->log(LOG_INFO, COMP_SDL2, "Screen init", msg, OUTC_SUCCESS);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        const auto msg = fmt::format("Failed initialized renderer with resolution {}x{}", renderResolution.width, renderResolution.height);
        logger->log(LOG_ERROR, COMP_SDL2, "Renderer init", msg, OUTC_FAILED);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    else {
        const auto msg = fmt::format("Successfully initialized renderer");
        logger->log(LOG_INFO, COMP_SDL2, "Renderer init", msg, OUTC_SUCCESS);
    }

    SDL_SetWindowFullscreen(window, 0);
    SDL_SetWindowSize(window, renderResolution.width, renderResolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_RenderSetLogicalSize(renderer, renderResolution.width, renderResolution.height);
}