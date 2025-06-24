#pragma once
#include <SDL2/SDL.h>

struct Texture {
    SDL_Texture *tex;
    int w;
    int h=0;
    
    explicit Texture(SDL_Texture* texture, int width, int height)
        : tex(texture), w(width), h(height) {}
    
    Texture() = delete;
};