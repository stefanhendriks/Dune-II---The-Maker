#include "game/cScreenInit.h"

#include "utils/cLog.h"

#include <SDL3/SDL.h>
#include <format>

#include <array>
#include "include/cAssert.h"
#include <optional>
#include <span>
#include <vector>

void cScreenInit::setFullScreenMode()
{
    SDL_SetWindowFullscreen(window, true);
    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", "Fullscreen desktop");
    int renderW, renderH;
    SDL_GetCurrentRenderOutputSize(renderer, &renderW, &renderH);
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", std::format("Renderer output size : {}x{}",renderW,renderH));
    float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "DPI", std::format("Display content scale : {}", scale));
}

void cScreenInit::setWindowMode()
{
    SDL_SetWindowFullscreen(window, false);
    SDL_SetWindowSize(window, renderResolution.width, renderResolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", "Windowed desktop");
}

void cScreenInit::getWindowResolution()
{
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
    if (mode) {
        cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", std::format("Resolution screen : {}x{}",mode->w,mode->h));
        windowResolution.width = mode->w;
        windowResolution.height = mode->h;
    }
}

void cScreenInit::adaptResolution(int desiredWidth, int desiredHeight)
{
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "Resolution", std::format("Desired : {}x{}",desiredWidth,desiredHeight));
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
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "Resolution", std::format("Adopted : {}x{}",renderResolution.width,renderResolution.height));
}

cScreenInit::cScreenInit(int desiredWidth, int desiredHeight, const std::string &title)
{
    this->getWindowResolution();
    this->adaptResolution(desiredWidth, desiredHeight);

    auto logger = cLogger::getInstance();

    window = SDL_CreateWindow(title.c_str(), renderResolution.width, renderResolution.height, 0);
    if (window == nullptr) {
        const auto msg = std::format("Failed initialized screen with resolution {}x{}", renderResolution.width, renderResolution.height);
        logger->log(LOG_ERROR, COMP_SDL2, "Screen init", msg, OUTC_FAILED);
        SDL_Quit();
        return;
    }
    else {
        const auto msg = std::format("Successfully initialized screen with resolution {}x{}.", renderResolution.width, renderResolution.height);
        logger->log(LOG_INFO, COMP_SDL2, "Screen init", msg, OUTC_SUCCESS);
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        const auto msg = std::format("Failed initialized renderer with resolution {}x{}", renderResolution.width, renderResolution.height);
        logger->log(LOG_ERROR, COMP_SDL2, "Renderer init", msg, OUTC_FAILED);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    else {
        const auto msg = std::format("Successfully initialized renderer");
        logger->log(LOG_INFO, COMP_SDL2, "Renderer init", msg, OUTC_SUCCESS);
    }

    SDL_SetRenderVSync(renderer, 1);
    SDL_SetWindowFullscreen(window, false);
    SDL_SetWindowSize(window, renderResolution.width, renderResolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}
