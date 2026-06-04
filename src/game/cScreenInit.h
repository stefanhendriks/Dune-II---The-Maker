#pragma once

#include <string>
#include <SDL3/SDL.h>

struct DisplayResolution {
    int width;
    int height;
};

class cScreenInit {
public:
    // Initializes SDL, creates a window and renderer with the given resolution
    cScreenInit(int desiredWidth, int desiredHeight, const std::string &title);
    ~cScreenInit();

    void setFullScreenMode();
    void setWindowMode();
    void onPixelSizeChanged();

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
    void getWindowResolution();
    void adaptResolution(int desiredWidth, int desiredHeight);
    void applyFullscreenPresentation();
    DisplayResolution renderResolution;
    DisplayResolution windowResolution; //display size of screen
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};