#pragma once

#include <stdexcept>
#include <memory>

#include "context/EntityContext.hpp"

class GameContext {
public:
    GameContext() = default;
    ~GameContext() = default;

    void setGraphicsContext(std::unique_ptr<GraphicsContext> newGfxCtx);

    GraphicsContext* getGraphicsContext() const;

private:
    std::unique_ptr<GraphicsContext> gCtx=nullptr;
};