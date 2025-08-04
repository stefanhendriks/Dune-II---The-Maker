#include "context/GameContext.hpp"


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