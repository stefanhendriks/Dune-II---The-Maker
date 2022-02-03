#include "cScreenInit.h"

#include "cLog.h"

// Need to include all of Allegro, otherwise the graphic mode defines are not included...
#include <allegro.h>
#include <fmt/core.h>

#include <array>
#include <cassert>
#include <optional>
#include <span>
#include <vector>

namespace {

// Note from Peter Gaal: This code is untested on UNIX platfrom - GFX_XWINDOWS_FULLSCREEN should be the right
// constant to detect available video modes on UNIX system. The code is tested on Windows system with
// GFX_DIRECTX_ACCEL and GFX_DIRECTX constants and it works.
#ifdef WIN32
constexpr int kNonMagicVideoDriver = GFX_DIRECTX_ACCEL;
#else
constexpr int kNonMagicVideoDriver = GFX_XWINDOWS_FULLSCREEN;
#endif

// tries them in this order (1st come first serve)
// do note, when DPI is > 100% in OS (atleast in Windows) things seem to get wonky.
// see also: https://github.com/stefanhendriks/Dune-II---The-Maker/issues/314
constexpr std::array<ScreenResolution, 9> kResolutionsToTry = {
    ScreenResolution{3840, 2160}, // 4k
    ScreenResolution{2048, 1536}, // 4:3 aspect ratio
    ScreenResolution{1920, 1080}, // Full HD
    ScreenResolution{1920, 1440}, // 4:3 aspect ratio
    ScreenResolution{1600, 900},
    ScreenResolution{1280, 1024},
    ScreenResolution{1366, 768},  // widescreen
    ScreenResolution{1024, 768},
    ScreenResolution{800, 600}
};

const GFX_MODE* FindGfxMode(const GFX_MODE_LIST& list, ScreenResolution res, int colorDepth) {
    for (int n = 0; n < list.num_modes; ++n) {
        const GFX_MODE& mode = list.mode[n];
        if (mode.width == res.width && mode.height == res.height && mode.bpp == colorDepth) {
            return &list.mode[n];
        }
    }
    return nullptr;
}

std::optional<ScreenResolution> TryAndSetFirstValidResolution(std::span<const ScreenResolution> resolutionsToTry) {
    auto logger = cLogger::getInstance();

    for (ScreenResolution res : resolutionsToTry) {
        const auto msg = fmt::format("Trying to init full-screen at {}x{}.", res.width, res.height);
        logger->log(LOG_INFO, COMP_ALLEGRO, "Screen init", msg);

        const int r = set_gfx_mode(GFX_AUTODETECT, res.width, res.height, res.width, res.height);

        if (r > -1) {
            return res;
        }
    }
    return {};
}

std::optional<ScreenResolution> SetBestScreenResolution(int colorDepth) {
    // Allegro supports the following color depths
    assert(colorDepth == 8 || colorDepth == 15 || colorDepth == 16 ||
           colorDepth == 24 || colorDepth == 32);

    auto logger = cLogger::getInstance();

    // Note: do not use "magic" drivers like GFX_AUTODETECT here, that doesn't work according to the Allegro
    // documentation.
    GFX_MODE_LIST* modeList = get_gfx_mode_list(kNonMagicVideoDriver);

    std::optional<ScreenResolution> setResolution;
    if (modeList) {
        // Make a shortlist of resolutions to try by filtering unsupported ones.
        std::vector<ScreenResolution> supportedResolutions;
        for (ScreenResolution res : kResolutionsToTry) {
            auto mode = FindGfxMode(*modeList, res, colorDepth);
            if (mode) {
                supportedResolutions.emplace_back(res);
            }
        }

        destroy_gfx_mode_list(modeList);

        return TryAndSetFirstValidResolution(supportedResolutions);
    } else {
        logger->log(LOG_WARN, COMP_ALLEGRO, "Screen auto init",
                    "Allegro could not determine suppored resolutions. Trying out several of them.");
        return TryAndSetFirstValidResolution(kResolutionsToTry);
    }
}

}

cScreenInit::cScreenInit(const cPlatformLayerInit& platform, bool windowed, int width, int height) {
    TitleAndColorDepthInit(platform);

    auto logger = cLogger::getInstance();

    if (windowed) {
        logger->log(LOG_INFO, COMP_ALLEGRO, "Screen init", "Windowed mode requested.");

        const int r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, width, height, width, height);
        if (r > -1) {
            const auto msg = fmt::format("Successfully created window with resolution {}x{} and color depth {}.", width, height, m_colorDepth);
            logger->log(LOG_INFO, COMP_ALLEGRO, "Screen init", msg, OUTC_SUCCESS);
            m_screenResolution.width = width;
            m_screenResolution.height = height;
        } else {
            const auto msg = fmt::format("Failed to create window with resolution {}x{} and color depth {}.", width, height, m_colorDepth);
            logger->log(LOG_ERROR, COMP_ALLEGRO, "Screen init", msg, OUTC_FAILED);
            throw std::runtime_error("Allegro could not create the game window.");
        }
    } else {
        logger->log(LOG_INFO, COMP_ALLEGRO, "Screen init", "Fullscreen mode requested.");

        const int r = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, width, height, width, height);

        if (r > -1) {
            const auto msg = fmt::format("Successfully initialized full-screen with resolution {}x{} and color depth {}.", width, height, m_colorDepth);
            logger->log(LOG_INFO, COMP_ALLEGRO, "Screen init", msg, OUTC_SUCCESS);

            m_screenResolution.width = width;
            m_screenResolution.height = height;
        } else {
            const auto msg = fmt::format("Failed to initialize full-screen with resolution {}x{} and color depth {}.", width, height, m_colorDepth);
            logger->log(LOG_WARN, COMP_ALLEGRO, "Screen init", msg, OUTC_FAILED);

            AutoDetectFullScreen();
        }
    }
}

cScreenInit::cScreenInit(const cPlatformLayerInit& platform) {
    TitleAndColorDepthInit(platform);
    AutoDetectFullScreen();
}

void cScreenInit::TitleAndColorDepthInit(const cPlatformLayerInit&) {
    auto logger = cLogger::getInstance();

    m_colorDepth = desktop_color_depth();
    set_color_depth(m_colorDepth);
    logger->log(LOG_INFO, COMP_ALLEGRO, "Screen init", fmt::format("Desktop color dept is {}.", m_colorDepth));
}

void cScreenInit::AutoDetectFullScreen() {
    auto logger = cLogger::getInstance();

    auto setResolution = SetBestScreenResolution(m_colorDepth);

    if (setResolution) {
        const auto msg = fmt::format("Successfully initialized full-screen with resolution {}x{} and color depth {}.",
                                     setResolution->width, setResolution->height, m_colorDepth);
        logger->log(LOG_INFO, COMP_ALLEGRO, "Screen init", msg, OUTC_SUCCESS);
        m_screenResolution = *setResolution;
    } else {
        const auto msg = fmt::format("Failed initialized full-screen with resolution {}x{} and color depth {}.",
                                     setResolution->width, setResolution->height, m_colorDepth);
        logger->log(LOG_ERROR, COMP_ALLEGRO, "Screen init", msg, OUTC_FAILED);
        throw std::runtime_error("Allegro could not initialize full-screen.");
    }
}
