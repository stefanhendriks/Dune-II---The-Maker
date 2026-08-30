#include "game/cSDLSystem.h"

#include "utils/Log.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <format>
#include <stdexcept>

#include <algorithm>
#include <array>
#include "include/cAssert.h"
#include <optional>
#include <span>
#include <vector>

// smallest coordinate space the game UI is laid out for
static constexpr int MIN_RENDER_WIDTH = 800;
static constexpr int MIN_RENDER_HEIGHT = 600;

void cSDLSystem::applyFullscreenPresentation()
{
    int renderW, renderH;
    // GetRenderOutputSize gives the true screen size; GetCurrentRenderOutputSize would be adjusted
    // by the logical presentation already in effect, and always report a scale of 1.
    SDL_GetRenderOutputSize(renderer, &renderW, &renderH);

    // LETTERBOX, not INTEGER_SCALE: fullscreen has to fill the screen. INTEGER_SCALE only allows
    // whole factors (1x, 2x, 3x ...), so playing at 1280x720 on a 1920x1080 screen is stuck at 1x
    // and leaves the game as a small rectangle in a black frame - a 1.5x scale is exactly what is
    // needed there. adaptResolution gives renderResolution the aspect ratio of the display, so
    // LETTERBOX fills the screen edge to edge instead of adding bars.
    // The price is that at a fractional scale the game pixels are no longer all the same size
    // (at 1.5x, one game pixel covers 1 or 2 screen pixels). That is the behaviour players had
    // before, and it beats losing most of the screen.
    float scaleToFillScreen = std::min((float)renderW / (float)renderResolution.width,
                                       (float)renderH / (float)renderResolution.height);

    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    Logger::info(COMP_SDL2, "desktop", "Renderer output size : {}x{}", renderW, renderH);
    Logger::info(COMP_SDL2, "desktop", "Presentation mode : LETTERBOX (scale = {})", scaleToFillScreen);
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

    // The area the desktop actually leaves free: the full bounds minus the taskbar, dock or menu
    // bar. A window asking for more than this gets shrunk by the desktop, and then no longer
    // matches the render resolution. Fall back to the full bounds when the platform has no notion
    // of a usable area.
    usableResolution = windowResolution;
    SDL_Rect usableBounds = {};
    if (SDL_GetDisplayUsableBounds(SDL_GetPrimaryDisplay(), &usableBounds)) {
        Logger::info(COMP_SDL2, "desktop", "Usable bounds : {}x{}", usableBounds.w, usableBounds.h);
        usableResolution.width = usableBounds.w;
        usableResolution.height = usableBounds.h;
    }
}

/**
 * Shrink the render resolution until the window fits in the usable desktop area, keeping the
 * aspect ratio intact. Without this the desktop resizes the window for us, which changes the
 * aspect ratio and makes SDL letterbox the game with black bars.
 */
void cSDLSystem::fitToUsableBounds()
{
    if (usableResolution.width < MIN_RENDER_WIDTH || usableResolution.height < MIN_RENDER_HEIGHT) {
        return; // desktop too small to honour, keep what we have
    }

    if (renderResolution.width <= usableResolution.width &&
            renderResolution.height <= usableResolution.height) {
        return; // already fits
    }

    int fittedWidth = std::min(renderResolution.width, usableResolution.width);
    int fittedHeight = renderResolution.height * fittedWidth / renderResolution.width;

    if (fittedHeight > usableResolution.height) {
        fittedHeight = usableResolution.height;
        fittedWidth = renderResolution.width * fittedHeight / renderResolution.height;
    }

    renderResolution.width = std::max(fittedWidth, MIN_RENDER_WIDTH);
    renderResolution.height = std::max(fittedHeight, MIN_RENDER_HEIGHT);
}

/**
 * The desktop can hand us a smaller client area than we asked for (a title bar that has to fit on
 * screen, a tiled or maximised window). SDL then letterboxes the render resolution inside it,
 * which shows up as black bars and a wrong aspect ratio. Adopt the size we really got instead:
 * the game reads Width()/Height() right after this constructor, so the whole UI follows along.
 */
void cSDLSystem::syncRenderResolutionToWindow()
{
    SDL_SyncWindow(window);

    int outputWidth = 0;
    int outputHeight = 0;
    // GetRenderOutputSize, not GetCurrentRenderOutputSize: the latter is adjusted by the logical
    // presentation, which would just hand back the render resolution we are trying to check.
    if (!SDL_GetRenderOutputSize(renderer, &outputWidth, &outputHeight)) return;

    if (outputWidth == renderResolution.width && outputHeight == renderResolution.height) return;

    if (outputWidth < MIN_RENDER_WIDTH || outputHeight < MIN_RENDER_HEIGHT) {
        // below what the UI is laid out for: keep the render resolution and let SDL letterbox
        Logger::info(COMP_SDL2, "Resolution", "Window is {}x{}, too small to adopt, letterboxing",
                     outputWidth, outputHeight);
        return;
    }

    Logger::info(COMP_SDL2, "Resolution", "Desktop gave us {}x{} instead of {}x{}, adopting it",
                 outputWidth, outputHeight, renderResolution.width, renderResolution.height);

    renderResolution.width = outputWidth;
    renderResolution.height = outputHeight;
}

void cSDLSystem::adaptResolution(int desiredWidth, int desiredHeight, bool windowed)
{
    Logger::info(COMP_SDL2, "Resolution", "Desired : {}x{}", desiredWidth, desiredHeight);
    if (desiredWidth<MIN_RENDER_WIDTH)
        desiredWidth = MIN_RENDER_WIDTH;
    if (desiredHeight<MIN_RENDER_HEIGHT)
        desiredHeight = MIN_RENDER_HEIGHT;

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

    // Only a real window has to share the desktop with a taskbar and a title bar. In fullscreen the
    // resolution must stay untouched, otherwise a player asking for the exact size of their screen
    // would lose the perfect 1:1 scale.
    if (windowed) {
        fitToUsableBounds();
    }

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

cSDLSystem::cSDLSystem(int desiredWidth, int desiredHeight, const std::string &title, bool windowed)
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
    this->adaptResolution(desiredWidth, desiredHeight, windowed);

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
    if (windowed) {
        // Only when we stay windowed: the caller switches to fullscreen right after this, and the
        // render resolution must not be pinned to the size of this temporary window.
        syncRenderResolutionToWindow();
    }
    SDL_SetRenderLogicalPresentation(renderer, renderResolution.width, renderResolution.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}
