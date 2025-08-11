#pragma once

#include <stdexcept>
#include <memory>

#include "context/EntityContext.hpp"

class cTimeManager;

class GameContext {
public:
    GameContext() = default;
    ~GameContext() = default;

    void setGraphicsContext(std::unique_ptr<GraphicsContext> newGfxCtx);
    GraphicsContext* getGraphicsContext() const;
    void setTimeManager(std::unique_ptr<cTimeManager> newTimeManager);
    cTimeManager* getTimeManager() const;
private:
    std::unique_ptr<GraphicsContext> gCtx = nullptr;
    std::unique_ptr<cTimeManager> timeManager = nullptr;
};