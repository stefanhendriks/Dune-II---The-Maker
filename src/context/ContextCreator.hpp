#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

class cFileValidator;
class GraphicsContext;
class Graphics;
class TextContext;
class cTextDrawer;

class ContextCreator {
public:
    ContextCreator(SDL_Renderer *renderer, cFileValidator *settingsValidator);
    ContextCreator() = delete;
    ~ContextCreator() = default;

    std::unique_ptr<GraphicsContext> createGraphicsContext();

private:
    std::shared_ptr<Graphics> gfxmentat;
    std::shared_ptr<Graphics> gfxworld;
    std::shared_ptr<Graphics> gfxinter;
    std::shared_ptr<Graphics> gfxdata;

    TTF_Font *game_font = nullptr; 	// arrakeen.fon
    TTF_Font *bene_font = nullptr;	// benegesserit font.
    TTF_Font *small_font = nullptr;	// small font.
};