#pragma once

#include <string>
#include <SDL3/SDL.h>

struct DisplayResolution {
    int width;
    int height;
};

class cSDLSystem {
public:
    // Initializes SDL, creates a window and renderer with the given resolution
    cSDLSystem(int desiredWidth, int desiredHeight, const std::string &title);
    ~cSDLSystem();

    void setFullScreenMode();
    void setWindowMode();
    void onPixelSizeChanged();

    cSDLSystem &operator=(const cSDLSystem &) = delete;
    cSDLSystem &operator=(cSDLSystem &&) = delete;
    cSDLSystem(const cSDLSystem &) = delete;
    cSDLSystem(cSDLSystem &&) = delete;

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
    void getWindowResolution();
    void adaptResolution(int desiredWidth, int desiredHeight);
    void applyFullscreenPresentation();
    DisplayResolution renderResolution;
    DisplayResolution windowResolution; //display size of screen
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};