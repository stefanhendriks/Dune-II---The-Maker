/**
 * @file Color.hpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <SDL3/SDL.h>

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
    static const Color Purple;

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
inline constexpr Color Color::Purple{255, 0, 255, 255};