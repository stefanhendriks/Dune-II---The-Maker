/**
 * @file cSDLSystem.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <string>
#include <SDL3/SDL.h>

#include "game/eScalingMode.h"

struct DisplayResolution {
    int width;
    int height;
};

class cSDLSystem {
public:
    // Initializes SDL, creates a window and renderer with the given resolution
    cSDLSystem(int desiredWidth, int desiredHeight, const std::string &title, bool windowed, eScalingMode scalingMode);
    ~cSDLSystem();

    void setFullScreenMode();
    void setWindowMode();
    void onPixelSizeChanged();
    void toggleScalingMode();

    eScalingMode getScalingMode() const {
        return m_scalingMode;
    }

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
    void adaptResolution(int desiredWidth, int desiredHeight, bool windowed);
    void fitToUsableBounds();
    void syncRenderResolutionToWindow();
    void applyFullscreenPresentation();
    DisplayResolution renderResolution;
    DisplayResolution windowResolution; //display size of screen
    DisplayResolution usableResolution; //display size minus taskbar/dock/menu bar
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    eScalingMode m_scalingMode;
};