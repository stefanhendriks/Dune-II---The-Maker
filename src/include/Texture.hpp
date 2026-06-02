#pragma once
#include <SDL3/SDL.h>
#include <memory>

struct SDLTextureDeleter {
    // SDL3 crashes if SDL_DestroyTexture is called after SDL_Quit() (e.g. when
    // Graphics globals outlive the renderer). Guard with SDL_WasInit so the
    // deleter is a no-op once the video subsystem has been torn down.
    void operator()(SDL_Texture* t) const {
        if (t && SDL_WasInit(SDL_INIT_VIDEO)) SDL_DestroyTexture(t);
    }
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