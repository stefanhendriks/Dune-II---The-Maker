#include "context/ContextCreator.hpp"
#include "utils/Graphics.hpp"
#include "utils/cLog.h"
#include "utils/cFileValidator.h"
#include "context/GraphicsContext.hpp"
#include "context/TextContext.hpp"
#include "drawers/cTextDrawer.h"

ContextCreator::ContextCreator(SDL_Renderer *renderer, cFileValidator *settingsValidator)
{
    // 
    // loading all datapacks
    //
    gfxmentat = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXMENTAT));
    if (gfxmentat == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXMENTAT), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXMENTAT), OUTC_SUCCESS);
    }

    gfxworld = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXWORLD));
    if (gfxworld == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXWORLD), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXWORLD), OUTC_SUCCESS);
    }

    gfxinter = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXINTER));
    if (gfxinter == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXINTER), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXINTER), OUTC_SUCCESS);
    }

    gfxdata = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXDATA));
    if (gfxdata == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXDATA), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXDATA), OUTC_SUCCESS);
    }

    //
    // loading all fonts
    //
    game_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::ARRAKEEN).c_str(),12);
    if (game_font == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load font:" + settingsValidator->getName(eGameDirFileName::ARRAKEEN), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked font: " + settingsValidator->getName(eGameDirFileName::ARRAKEEN), OUTC_SUCCESS);
    }  

    bene_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::BENEGESS).c_str(),12);
    if (game_font == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load font:" + settingsValidator->getName(eGameDirFileName::BENEGESS), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked font: " + settingsValidator->getName(eGameDirFileName::BENEGESS), OUTC_SUCCESS);
    }  

    small_font = TTF_OpenFont(settingsValidator->getFullName(eGameDirFileName::SMALL).c_str(),12);
    if (game_font == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load font:" + settingsValidator->getName(eGameDirFileName::SMALL), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked font: " + settingsValidator->getName(eGameDirFileName::SMALL), OUTC_SUCCESS);
    }  
}

std::unique_ptr<GraphicsContext> ContextCreator::createGraphicsContext()
{
    auto gtx = std::make_unique<GraphicsContext>();
    gtx->gfxmentat = gfxmentat;
    gtx->gfxworld = gfxworld;
    gtx->gfxinter = gfxinter;
    gtx->gfxdata = gfxdata;
    return gtx;
}

std::unique_ptr<TextContext> ContextCreator::createTextContext()
{
    auto ttx = std::make_unique<TextContext>();
    ttx->smallTextDrawer = std::make_unique<cTextDrawer>(small_font);
    ttx->beneTextDrawer = std::make_unique<cTextDrawer>(bene_font);
    ttx->gameTextDrawer = std::make_unique<cTextDrawer>(game_font);
    return ttx;
}
