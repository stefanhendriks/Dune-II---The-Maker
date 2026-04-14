#pragma once
#include <SDL2/SDL.h>
#include <memory>

struct SDLTextureDeleter {
    void operator()(SDL_Texture* t) const { if (t) SDL_DestroyTexture(t); }
};

class Texture {

public:
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> tex;
    int w=0;
    int h=0;
    bool isRenderTarget=false;

    explicit Texture(SDL_Texture *texture, int width, int height, bool isTarget = false)
        : tex(texture), w(width), h(height), isRenderTarget(isTarget) {}

    Texture() = delete;

    ~Texture() = default; // Smart pointer handles cleanup
};