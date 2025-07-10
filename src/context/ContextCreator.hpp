#pragma once

#include <SDL2/SDL.h>
#include <memory>

class cFileValidator;
class GraphicsContext;
class Graphics;

class ContextCreator {
public:
    ContextCreator(SDL_Renderer *renderer, cFileValidator *settingsValidator);
    ~ContextCreator();

    std::unique_ptr<GraphicsContext> createGraphicsContext();

private:
    std::shared_ptr<Graphics> gfxmentat = nullptr;
};