#pragma once

#include <stdexcept>
#include <memory>

class Graphics;

struct GraphicsContext {
    std::unique_ptr<Graphics> gfxmentat;
};

class GameContext {
public:
    GameContext();
    ~GameContext() = default;

    void setGraphicsContext(std::unique_ptr<GraphicsContext> newGfxCtx) {
        if (!newGfxCtx) {
            throw std::runtime_error("GraphicsContext is not initialized!");
        }
        gCtx = std::move(newGfxCtx);
    }

    GraphicsContext* getGraphicsContext() {
        if (gCtx) {
            return gCtx.get();
        } else {
            throw std::runtime_error("GraphicsContext not define");
        }
    }

private:
    std::unique_ptr<GraphicsContext> gCtx;
};