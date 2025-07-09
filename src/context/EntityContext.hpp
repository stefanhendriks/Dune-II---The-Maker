#pragma once

#include <memory>
class Graphics;

struct GraphicsContext {
    std::shared_ptr<Graphics> gfxmentat;
};