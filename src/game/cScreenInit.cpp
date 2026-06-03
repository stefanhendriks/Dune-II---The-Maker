#include "game/cScreenInit.h"

#include "utils/cLog.h"

#include <SDL3/SDL.h>
#include <format>

#include <array>
#include "include/cAssert.h"
#include <optional>
#include <span>
#include <vector>

void cScreenInit::applyFullscreenPresentation()
{
    int renderW, renderH;
    SDL_GetCurrentRenderOutputSize(renderer, &renderW, &renderH);

    // Always INTEGER_SCALE: every game pixel maps to the same-sized block on
    // screen (1x, 2x, 3x ...). If the screen is larger than an exact multiple,
    // the remainder appears as black borders. This avoids fractional scaling
    // (LETTERBOX at e.g. 1.167x) which produces jagged, uneven pixels even
    // with nearest-neighbour filtering. renderResolution is already capped to
    // the logical screen size in adaptResolution, so the scale is always >= 1.
    int intScale = std::min(renderW / renderResolution.width, renderH / renderResolution.height);

    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", std::format("Renderer output size : {}x{}", renderW, renderH));
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", std::format("Presentation mode : INTEGER_SCALE (integer scale = {})", intScale));
    float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "DPI", std::format("Display content scale : {}", scale));
}

void cScreenInit::setFullScreenMode()
{
    SDL_SetWindowFullscreen(window, true);
    // On macOS, SDL_SetWindowFullscreen is asynchronous — the Spaces animation
    // plays out after the call returns. SDL_SyncWindow blocks until the window
    // state (and renderer output size) is final, so applyFullscreenPresentation
    // measures the correct output size.
    SDL_SyncWindow(window);
    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", "Fullscreen desktop");
    applyFullscreenPresentation();
}

void cScreenInit::onPixelSizeChanged()
{
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
        applyFullscreenPresentation();
    }
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
    // SDL_GetCurrentDisplayMode returns PHYSICAL pixels, but SDL_CreateWindow
    // expects LOGICAL pixels (points). On Retina/HiDPI displays these differ
    // (e.g. 2560x1600 physical vs 1280x800 logical on a 2x Retina Mac).
    // Use SDL_GetDisplayBounds which returns screen coordinates (logical pixels).
    SDL_Rect bounds = {};
    if (SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds)) {
        cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "desktop", std::format("Display bounds : {}x{}", bounds.w, bounds.h));
        windowResolution.width = bounds.w;
        windowResolution.height = bounds.h;
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

    // Cap to the logical screen size so the game never requests a larger
    // coordinate space than the display provides. Without this, fullscreen
    // would have to scale DOWN (e.g. fit 1792x1120 into a 1536x960 logical
    // screen), causing squishing. The cap ensures fullscreen can always show
    // the game at 1:1 or larger integer multiples.
    renderResolution.width = std::min(renderResolution.width, windowResolution.width);
    renderResolution.height = std::min(renderResolution.height, windowResolution.height);

    cLogger::getInstance()->log(LOG_INFO, COMP_SDL2, "Resolution", std::format("Adopted : {}x{}",renderResolution.width,renderResolution.height));
}

cScreenInit::cScreenInit(int desiredWidth, int desiredHeight, const std::string &title)
{
    this->getWindowResolution();
    this->adaptResolution(desiredWidth, desiredHeight);

    // Disable macOS Spaces fullscreen: SDL3 defaults to Spaces (animated
    // Space transition), which changes the renderer output to logical pixels
    // (1536x960 on a 1792x1120 physical display) and fires async size events.
    // Non-Spaces fullscreen matches SDL2 behaviour: immediate, uses physical
    // pixel dimensions, no transition surprises.
    SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0");

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

    // SDL_RENDER_SCALE_QUALITY was removed in SDL3. The correct way to control
    // the logical presentation's internal texture scale is via the renderer's
    // default texture scale mode. Setting it to NEAREST once here ensures
    // every SDL_SetRenderLogicalPresentation call (now and later) creates its
    // internal texture with nearest-neighbour filtering — critical for crisp
    // pixel art at any scale factor.
    SDL_SetDefaultTextureScaleMode(renderer, SDL_SCALEMODE_NEAREST);

    SDL_SetWindowFullscreen(window, false);
    SDL_SetWindowSize(window, renderResolution.width, renderResolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}
