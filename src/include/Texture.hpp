#pragma once
#include <SDL2/SDL.h>

class Texture {

public:
    SDL_Texture *tex;
    int w=0;
    int h=0;
    bool isRenderTarget=false;

    explicit Texture(SDL_Texture *texture, int width, int height, bool isTarget = false)
        : tex(texture), w(width), h(height), isRenderTarget(isTarget) {}

    Texture() = delete;

    ~Texture() {
        if (tex) {
            SDL_DestroyTexture(tex);
            tex = nullptr;
        }
    }
};