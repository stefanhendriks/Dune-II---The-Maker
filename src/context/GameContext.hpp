#pragma once

#include <stdexcept>
#include <memory>

#include "context/EntityContext.hpp"
//#include "utils/cSoundPlayer.h"

class cTimeManager;
class cSoundPlayer;

class GameContext {
public:
    GameContext() = default;
    ~GameContext();

    void setGraphicsContext(std::unique_ptr<GraphicsContext> newGfxCtx);
    GraphicsContext* getGraphicsContext() const;
    void setTimeManager(std::unique_ptr<cTimeManager> newTimeManager);
    cTimeManager* getTimeManager() const;
    void setSoundPlayer(std::unique_ptr<cSoundPlayer> newSoundPlayer);
    cSoundPlayer* getSoundPlayer() const;
private:
    std::unique_ptr<GraphicsContext> gCtx;
    std::unique_ptr<cTimeManager> timeManager;
    std::unique_ptr<cSoundPlayer> soundPlayer;
};