#include "context/GameContext.hpp"
#include "game/cTimeManager.h"
#include "game/cTimeCounter.h"
#include "utils/cSoundPlayer.h"
#include "drawers/cTextDrawer.h"
#include "drawers/SDLDrawer.hpp"
#include "game/cGameInterface.h"

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
    }
    throw std::runtime_error("GraphicsContext not defined");
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
    }
    throw std::runtime_error("TextContext not defined");
}

void GameContext::resetCache() const {
    if (m_textContext == nullptr) {
        return;
    }
    m_textContext->resetCache();
}

void GameContext::setSDLDrawer(std::unique_ptr<SDLDrawer> SDLDrawer) {
    if (!SDLDrawer) {
        throw std::runtime_error("SDLDrawer is not initialized!");
    }
    m_SDLDrawer = std::move(SDLDrawer);
}

SDLDrawer *GameContext::getSDLDrawer() const {
    if (m_SDLDrawer) {
        return m_SDLDrawer.get();
    }
    throw std::runtime_error("SDLDrawer not defined");
}

void GameContext::setGameInterface(std::unique_ptr<cGameInterface> gameInterface) {
    if (!gameInterface) {
        throw std::runtime_error("cGameInterface is not initialized!");
    }
    m_gameInterface = std::move(gameInterface);
}

cGameInterface *GameContext::getGameInterface() const {
    if (m_gameInterface) {
        return m_gameInterface.get();
    }
    throw std::runtime_error("cGameInterface not defined");
}

void GameContext::setLog(cLog *log) {
    if (!log) {
        throw std::runtime_error("cLog is not initialized!");
    }
    m_log = log;
}

cLog *GameContext::getLog() const {
    if (m_log) {
        return m_log;
    }
    throw std::runtime_error("cLog not defined");
}