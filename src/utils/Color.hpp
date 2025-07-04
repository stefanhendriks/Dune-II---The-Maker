#pragma once

#include <SDL2/SDL_pixels.h>

class Color {
public:
    uint8_t r, g, b, a;

    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    SDL_Color toSDL() const {
        return SDL_Color{ r, g, b, a };
    }

    explicit operator SDL_Color() const {
        return toSDL();
    }

    static Color red() { return Color(255, 0, 0); }
    static Color green() { return Color(0, 255, 0); }
    static Color blue() { return Color(0, 0, 255); }
    static Color black() { return Color(0,0,0,255);}
    static Color white() { return Color(255,255,255,255);}
    static Color yellow() { return Color{255, 207, 41,255};}

    Color withAlpha(uint8_t alpha) const { return Color(r, g, b, alpha); }
};
