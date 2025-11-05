#include "context/GameContext.hpp"
#include "utils/cTimeManager.h"
#include "utils/cSoundPlayer.h"

GameContext::~GameContext() {
}

void GameContext::setGraphicsContext(std::unique_ptr<GraphicsContext> graphicsContext) {
    if (!graphicsContext) {
        throw std::runtime_error("GraphicsContext is not initialized!");
    }
    m_graphicsContext = std::move(graphicsContext);
}

GraphicsContext *GameContext::getGraphicsContext() const {
    if (m_graphicsContext) {
        return m_graphicsContext.get();
    } else {
        throw std::runtime_error("GraphicsContext not defined");
    }
}

void GameContext::setTimeManager(std::unique_ptr<cTimeManager> timeManager) {
    if (!timeManager) {
        throw std::runtime_error("cTimeManager is not initialized!");
    }
    m_timeManager = std::move(timeManager);
}

cTimeManager *GameContext::getTimeManager() const {
    if (m_timeManager) {
        return m_timeManager.get();
    }
    throw std::runtime_error("cTimeManager not defined");
}

void GameContext::setSoundPlayer(std::unique_ptr<cSoundPlayer> soundPlayer) {
    if (!soundPlayer) {
        throw std::runtime_error("cSoundPlayer is not initialized!");
    }
    m_soundPlayer = std::move(soundPlayer);
}


cSoundPlayer *GameContext::getSoundPlayer() const {
    if (m_soundPlayer) {
        return m_soundPlayer.get();
    }
    throw std::runtime_error("cSoundPlayer not defined");
}

void GameContext::setTextContext(std::unique_ptr<TextContext> textContext) {
    if (!textContext) {
        throw std::runtime_error("textContext is not initialized!");
    }
    m_textContext = std::move(textContext);
}

TextContext *GameContext::getTextContext() const {
    if (m_textContext) {
        return m_textContext.get();
    } else {
        throw std::runtime_error("TextContext not defined");
    }
}
