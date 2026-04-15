#pragma once

#include <SDL2/SDL_pixels.h>

class Color {
public:
    uint8_t r, g, b, a;

    constexpr Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    SDL_Color toSDL() const {
        return { r, g, b, a };
    }

    explicit operator SDL_Color() const {
        return toSDL();
    }

    constexpr Color withAlpha(uint8_t alpha) const { 
        return {r, g, b, alpha}; 
    }
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Black;
    static const Color White;
    static const Color Yellow;

    static const Color PlaceNeutral;
    static const Color PlaceBad;
    static const Color PlaceGood;
};

inline constexpr Color Color::Red{255, 0, 0};
inline constexpr Color Color::Green{0, 255, 0};
inline constexpr Color Color::Blue{0, 0, 255};
inline constexpr Color Color::Black{0, 0, 0, 255};
inline constexpr Color Color::White{255, 255, 255, 255};
inline constexpr Color Color::Yellow{255, 207, 41, 255};

inline constexpr Color Color::PlaceNeutral{242, 174, 36, 64};
inline constexpr Color Color::PlaceBad{160, 0, 0, 64};
inline constexpr Color Color::PlaceGood{64, 255, 64, 64};