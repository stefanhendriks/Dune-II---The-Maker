/**
 * @file test_Color.cpp
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

#include <catch2/catch_test_macros.hpp>
#include "utils/Color.hpp"

TEST_CASE("Color - default constructor is opaque black", "[color]")
{
    constexpr Color c;
    REQUIRE(c.r == 0);
    REQUIRE(c.g == 0);
    REQUIRE(c.b == 0);
    REQUIRE(c.a == 255);
}

TEST_CASE("Color - value constructor stores channels", "[color]")
{
    Color c(1, 2, 3, 4);
    REQUIRE(c.r == 1);
    REQUIRE(c.g == 2);
    REQUIRE(c.b == 3);
    REQUIRE(c.a == 4);
}

TEST_CASE("Color - toSDL and explicit conversion keep all channels", "[color]")
{
    Color c(10, 20, 30, 40);
    SDL_Color sdl = c.toSDL();
    REQUIRE(sdl.r == 10);
    REQUIRE(sdl.g == 20);
    REQUIRE(sdl.b == 30);
    REQUIRE(sdl.a == 40);

    SDL_Color viaCast = static_cast<SDL_Color>(c);
    REQUIRE(viaCast.r == 10);
    REQUIRE(viaCast.a == 40);
}

TEST_CASE("Color - withAlpha changes only alpha and leaves the original untouched", "[color]")
{
    Color c = Color::Red.withAlpha(128);
    REQUIRE(c.r == 255);
    REQUIRE(c.g == 0);
    REQUIRE(c.b == 0);
    REQUIRE(c.a == 128);
    REQUIRE(Color::Red.a == 255);
}

TEST_CASE("Color - predefined colors", "[color]")
{
    REQUIRE(Color::White.r == 255);
    REQUIRE(Color::White.g == 255);
    REQUIRE(Color::White.b == 255);
    REQUIRE(Color::Black.r == 0);
    REQUIRE(Color::Black.a == 255);
    REQUIRE(Color::Green.g == 255);
    REQUIRE(Color::Blue.b == 255);
    REQUIRE(Color::Purple.r == 255);
    REQUIRE(Color::Purple.b == 255);
}

TEST_CASE("Color - placement colors are translucent", "[color]")
{
    REQUIRE(Color::PlaceNeutral.a == 64);
    REQUIRE(Color::PlaceBad.a == 64);
    REQUIRE(Color::PlaceGood.a == 64);
}
