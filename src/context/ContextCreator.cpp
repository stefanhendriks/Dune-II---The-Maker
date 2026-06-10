#include "context/ContextCreator.hpp"
#include "utils/Graphics.hpp"
#include "utils/Log.h"
#include "utils/cFileValidator.h"
#include "context/GraphicsContext.hpp"
#include "context/TextContext.hpp"
#include "drawers/cTextDrawer.h"
#include "drawers/SDLDrawer.hpp"
#include "game/cGameSettings.h"

#include "include/cAssert.h"

ContextCreator::ContextCreator(SDL_Renderer *renderer, cFileValidator *settingsValidator)
{
    d2tm_assert(renderer!=nullptr);
    d2tm_assert(settingsValidator!=nullptr);
    //
    // loading all datapacks
    //
    gfxmentat = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXMENTAT));
    if (gfxmentat == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load datafile: {}", settingsValidator->getName(eGameDirFileName::GFXMENTAT));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked datafile: {}", settingsValidator->getName(eGameDirFileName::GFXMENTAT));
    }

    gfxworld = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXWORLD));
    if (gfxworld == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load datafile: {}", settingsValidator->getName(eGameDirFileName::GFXWORLD));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked datafile: {}", settingsValidator->getName(eGameDirFileName::GFXWORLD));
    }

    gfxinter = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXINTER));
    if (gfxinter == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load datafile: {}", settingsValidator->getName(eGameDirFileName::GFXINTER));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked datafile: {}", settingsValidator->getName(eGameDirFileName::GFXINTER));
    }

    gfxdata = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXDATA));
    if (gfxdata == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load datafile: {}", settingsValidator->getName(eGameDirFileName::GFXDATA));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked datafile: {}", settingsValidator->getName(eGameDirFileName::GFXDATA));
    }

    gfxeditor = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXEDITOR));
    if (gfxeditor == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load datafile: {}", settingsValidator->getName(eGameDirFileName::GFXEDITOR));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked datafile: {}", settingsValidator->getName(eGameDirFileName::GFXEDITOR));
    }

    //
    // loading all fonts
    //
    game_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::ARRAKEEN).c_str(),12);
    if (game_font == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load font: {}", settingsValidator->getName(eGameDirFileName::ARRAKEEN));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked font: {}", settingsValidator->getName(eGameDirFileName::ARRAKEEN));
    }

    bene_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::BENEGESS).c_str(),12);
    if (bene_font == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load font: {}", settingsValidator->getName(eGameDirFileName::BENEGESS));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked font: {}", settingsValidator->getName(eGameDirFileName::BENEGESS));
    }

    small_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::SMALL).c_str(),12);
    if (small_font == nullptr) {
        Logger::error(COMP_INIT, "Load data", "Could not hook/load font: {}", settingsValidator->getName(eGameDirFileName::SMALL));
    }
    else {
        Logger::info(COMP_INIT, "Load data", "Hooked font: {}", settingsValidator->getName(eGameDirFileName::SMALL));
    }
}

ContextCreator::~ContextCreator()
{
    TTF_CloseFont(game_font);
    TTF_CloseFont(bene_font);
    TTF_CloseFont(small_font);
}

std::unique_ptr<GraphicsContext> ContextCreator::createGraphicsContext()
{
    auto gtx = std::make_unique<GraphicsContext>();
    gtx->gfxmentat = gfxmentat;
    gtx->gfxworld = gfxworld;
    gtx->gfxinter = gfxinter;
    gtx->gfxdata = gfxdata;
    gtx->gfxeditor = gfxeditor;
    return gtx;
}

std::unique_ptr<TextContext> ContextCreator::createTextContext(cGameSettings *settings, SDLDrawer *renderer) {
    return std::make_unique<TextContext>(
        std::make_unique<cTextDrawer>(small_font, settings, renderer),
        std::make_unique<cTextDrawer>(bene_font, settings, renderer),
        std::make_unique<cTextDrawer>(game_font, settings, renderer)
    );
}
