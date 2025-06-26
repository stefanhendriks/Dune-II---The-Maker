#pragma once
#include <SDL2/SDL.h>

struct Texture {
    SDL_Texture *tex;
    int w;
    int h=0;

    explicit Texture(SDL_Texture *texture, int width, int height)
        : tex(texture), w(width), h(height) {}

    Texture() = delete;

    ~Texture() {
        if (tex) { // Check if tex is not null before destroying
            SDL_DestroyTexture(tex);
            tex = nullptr; // Set to nullptr after destruction to prevent double-free
        }
    }
};