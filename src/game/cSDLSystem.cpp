#include "game/cSDLSystem.h"

#include "utils/Log.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <format>
#include <stdexcept>

#include <array>
#include "include/cAssert.h"
#include <optional>
#include <span>
#include <vector>

void cSDLSystem::applyFullscreenPresentation()
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

    Logger::info(COMP_SDL2, "desktop", "Renderer output size : {}x{}", renderW, renderH);
    Logger::info(COMP_SDL2, "desktop", "Presentation mode : INTEGER_SCALE (integer scale = {})", intScale);
    float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    Logger::info(COMP_SDL2, "DPI", "Display content scale : {}", scale);
}

void cSDLSystem::setFullScreenMode()
{
    SDL_SetWindowFullscreen(window, true);
    // On macOS, SDL_SetWindowFullscreen is asynchronous — the Spaces animation
    // plays out after the call returns. SDL_SyncWindow blocks until the window
    // state (and renderer output size) is final, so applyFullscreenPresentation
    // measures the correct output size.
    SDL_SyncWindow(window);
    Logger::info(COMP_SDL2, "desktop", "Fullscreen desktop");
    applyFullscreenPresentation();
}

void cSDLSystem::onPixelSizeChanged()
{
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
        applyFullscreenPresentation();
    }
}

void cSDLSystem::setWindowMode()
{
    SDL_SetWindowFullscreen(window, false);
    SDL_SetWindowSize(window, renderResolution.width, renderResolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    Logger::info(COMP_SDL2, "desktop", "Windowed desktop");
}

void cSDLSystem::getWindowResolution()
{
    // SDL_GetCurrentDisplayMode returns PHYSICAL pixels, but SDL_CreateWindow
    // expects LOGICAL pixels (points). On Retina/HiDPI displays these differ
    // (e.g. 2560x1600 physical vs 1280x800 logical on a 2x Retina Mac).
    // Use SDL_GetDisplayBounds which returns screen coordinates (logical pixels).
    SDL_Rect bounds = {};
    if (SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds)) {
        Logger::info(COMP_SDL2, "desktop", "Display bounds : {}x{}", bounds.w, bounds.h);
        windowResolution.width = bounds.w;
        windowResolution.height = bounds.h;
    }
}

void cSDLSystem::adaptResolution(int desiredWidth, int desiredHeight)
{
    Logger::info(COMP_SDL2, "Resolution", "Desired : {}x{}", desiredWidth, desiredHeight);
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

    Logger::info(COMP_SDL2, "Resolution", "Adopted : {}x{}", renderResolution.width, renderResolution.height);
}

cSDLSystem::~cSDLSystem()
{
    Logger::info(COMP_SDL2, "SDL shutdown", "Shutting down...");
    TTF_Quit();
    MIX_Quit();
    Logger::info(COMP_SDL2, "SDL_mixer shutdown", "Thanks for playing!");
    // On Linux, SDL3 registers its own atexit() handler that calls SDL_Quit() before
    // C++ global destructors run. Calling it again here causes a double-free crash.
    // Guard so we only call SDL_Quit() if SDL is still initialised.
    if (SDL_WasInit(0)==0) {
        SDL_Quit();
    }
    Logger::info(COMP_SDL2, "SDL shutdown", "Thanks for playing!");
}

cSDLSystem::cSDLSystem(int desiredWidth, int desiredHeight, const std::string &title)
{
    Logger::info(COMP_SDL2, "cSDLSystem", "=== SDL ===");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        Logger::fatal(COMP_SDL2, "SDL init", "{}", SDL_GetError());
        throw std::runtime_error(SDL_GetError());
    }
    else {
        Logger::info(COMP_SDL2, "SDL init", "Initialized successfully");
    }

    if (!MIX_Init()) {
        Logger::fatal(COMP_SDL2, "SDL mixer", "{}", SDL_GetError());
        throw std::runtime_error(SDL_GetError());
    }
    else {
        Logger::info(COMP_SDL2, "SDL_mixer", "Initialized successfully");
        for (auto i =0; i < MIX_GetNumAudioDecoders();i++) {
            Logger::info(COMP_SDL2, "SDL_mixer", "Audio decoder {} : {}", i, MIX_GetAudioDecoder(i));
        }
    }

    if (!TTF_Init()) {
        Logger::fatal(COMP_SDL2, "SDL ttf", "{}", SDL_GetError());
    }
    else {
        Logger::info(COMP_SDL2, "SDL_ttf", "Initialized successfully");
    }

    this->getWindowResolution();
    this->adaptResolution(desiredWidth, desiredHeight);

    // Disable macOS Spaces fullscreen: SDL3 defaults to Spaces (animated
    // Space transition), which changes the renderer output to logical pixels
    // (1536x960 on a 1792x1120 physical display) and fires async size events.
    // Non-Spaces fullscreen matches SDL2 behaviour: immediate, uses physical
    // pixel dimensions, no transition surprises.
    SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0");

    window = SDL_CreateWindow(title.c_str(), renderResolution.width, renderResolution.height, 0);
    if (window == nullptr) {
        Logger::error(COMP_SDL2, "Screen init", "Failed initialized screen with resolution {}x{}", renderResolution.width, renderResolution.height);
        SDL_Quit();
        return;
    }
    else {
        Logger::info(COMP_SDL2, "Screen init", "Successfully initialized screen with resolution {}x{}.", renderResolution.width, renderResolution.height);
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        Logger::error(COMP_SDL2, "Renderer init", "Failed initialized renderer with resolution {}x{}", renderResolution.width, renderResolution.height);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    else {
        Logger::info(COMP_SDL2, "Renderer init", "Successfully initialized renderer");
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
