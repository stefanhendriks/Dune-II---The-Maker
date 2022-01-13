#pragma once

#include "cPlatformLayerInit.h"

#include <string>

struct ScreenResolution {
    int width;
    int height;
};

class cScreenInit {
  public:
    // Creates a window or initializes full-screen with the given resolution at desktop color depth
    cScreenInit(const cPlatformLayerInit& platform, const std::string& window_title, bool windowed, int width, int height);

    // Initializes full-screen at desktop color depth, autodetecting a supported resolution.
    cScreenInit(const cPlatformLayerInit& platform, const std::string& window_title);

    // No cleanup required
    ~cScreenInit() = default;

    cScreenInit& operator=(const cScreenInit&) = delete;
    cScreenInit& operator=(cScreenInit&&) = delete;
    cScreenInit(const cScreenInit&) = delete;
    cScreenInit(cScreenInit&&) = delete;

    int Width() const { return m_screenResolution.width; }
    int Height() const { return m_screenResolution.height; }

  private:
    void TitleAndColorDepthInit(const cPlatformLayerInit& platform, const std::string& window_title);
    void AutoDetectFullScreen();

    int m_colorDepth;
    ScreenResolution m_screenResolution;
};