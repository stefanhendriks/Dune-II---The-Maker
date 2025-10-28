#pragma once

#include <stdexcept>
#include <memory>

#include "context/GraphicsContext.hpp"

class cTimeManager;
class cSoundPlayer;

class GameContext {
public:
    GameContext() = default;
    ~GameContext();

    void setGraphicsContext(std::unique_ptr<GraphicsContext> graphicsContext);
    GraphicsContext* getGraphicsContext() const;

    void setTimeManager(std::unique_ptr<cTimeManager> timeManager);
    cTimeManager* getTimeManager() const;

    void setSoundPlayer(std::unique_ptr<cSoundPlayer> soundPlayer);
    cSoundPlayer* getSoundPlayer() const;

private:
    std::unique_ptr<GraphicsContext> m_graphicsContext;
    std::unique_ptr<cTimeManager> m_timeManager;
    std::unique_ptr<cSoundPlayer> m_soundPlayer;
};