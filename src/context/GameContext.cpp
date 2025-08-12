#include "context/GameContext.hpp"
#include "utils/cTimeManager.h"
#include "utils/cSoundPlayer.h"

GameContext::~GameContext()
{}

void GameContext::setGraphicsContext(std::unique_ptr<GraphicsContext> newGfxCtx) {
    if (!newGfxCtx) {
        throw std::runtime_error("GraphicsContext is not initialized!");
    }
    gCtx = std::move(newGfxCtx);
}

GraphicsContext* GameContext::getGraphicsContext() const {
    if (gCtx) {
        return gCtx.get();
    } else {
        throw std::runtime_error("GraphicsContext not define");
    }
}

void GameContext::setTimeManager(std::unique_ptr<cTimeManager> newTimeManager) {
    if (!newTimeManager) {
        throw std::runtime_error("cTimeManager is not initialized!");
    }
    timeManager = std::move(newTimeManager);
}

cTimeManager* GameContext::getTimeManager() const {
    if (timeManager) {
        return timeManager.get();
    } else {
        throw std::runtime_error("cTimeManager not defined");
    }
}

void GameContext::setSoundPlayer(std::unique_ptr<cSoundPlayer> newSoundPlayer)
{
    if (!newSoundPlayer) {
        throw std::runtime_error("cTimeManager is not initialized!");
    }
    soundPlayer = std::move(newSoundPlayer);  
}


cSoundPlayer* GameContext::getSoundPlayer() const {
    if (soundPlayer) {
        return soundPlayer.get();
    } else {
        throw std::runtime_error("soundPlayer not defined");
    }
}