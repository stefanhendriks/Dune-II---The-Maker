#pragma once

#include <memory>
class Graphics;

class GraphicsContext {

public:
    std::shared_ptr<Graphics> gfxmentat;
    std::shared_ptr<Graphics> gfxworld;
    std::shared_ptr<Graphics> gfxinter;
    std::shared_ptr<Graphics> gfxdata;
};