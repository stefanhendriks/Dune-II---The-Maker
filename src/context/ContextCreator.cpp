#include "context/ContextCreator.hpp"
#include "utils/Graphics.hpp"
#include "utils/cLog.h"
#include "utils/cFileValidator.h"
#include "context/EntityContext.hpp"

ContextCreator::ContextCreator(SDL_Renderer *renderer, cFileValidator *settingsValidator)
{
    gfxmentat = std::make_shared<Graphics>(renderer,settingsValidator->getFullName(eGameDirFileName::GFXMENTAT));
    if (gfxmentat == nullptr) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Load data", "Could not hook/load datafile:" + settingsValidator->getName(eGameDirFileName::GFXMENTAT), OUTC_FAILED);
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "Load data", "Hooked datafile: " + settingsValidator->getName(eGameDirFileName::GFXMENTAT), OUTC_SUCCESS);
    }
}

std::unique_ptr<GraphicsContext> ContextCreator::createGraphicsContext()
{
    auto gtx = std::make_unique<GraphicsContext>();
    gtx->gfxmentat = gfxmentat;
    return gtx;
}