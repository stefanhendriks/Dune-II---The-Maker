#pragma once

#include "cPlatformLayerInit.h"

#include <string>
#include <SDL2/SDL.h>

struct DisplayResolution {
    int width;
    int height;
};

class cScreenInit {
public:
    // Creates a window or initializes full-screen with the given resolution at desktop color depth
    // cScreenInit(const cPlatformLayerInit &platform, bool windowed, int width, int height);

    // Initializes full-screen at desktop color depth, autodetecting a supported resolution.
    // cScreenInit(const cPlatformLayerInit &platform);

    cScreenInit(int desiredWidth, int desiredHeight, const std::string &title);
    // No cleanup required
    ~cScreenInit() = default;

    cScreenInit &operator=(const cScreenInit &) = delete;
    cScreenInit &operator=(cScreenInit &&) = delete;
    cScreenInit(const cScreenInit &) = delete;
    cScreenInit(cScreenInit &&) = delete;

    int Width() const {
        return renderResolution.width;
    }
    int Height() const {
        return renderResolution.height;
    }

    SDL_Window *getWindows() const {
        return window;
    }

    SDL_Renderer *getRenderer() const {
        return renderer;
    }

private:
    // void TitleAndColorDepthInit(const cPlatformLayerInit &platform);
    void getWindowResolution();
    void adaptResolution(int desiredWidth, int desiredHeight);

    // int m_colorDepth;
    DisplayResolution renderResolution;
    DisplayResolution windowResolution; //display size of screen

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};