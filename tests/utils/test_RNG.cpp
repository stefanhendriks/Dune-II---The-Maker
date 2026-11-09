/**
 * @file test_RNG.cpp
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
#include "utils/RNG.hpp"

namespace {
constexpr int SAMPLES = 2000;
}

TEST_CASE("RNG - genInt stays within the inclusive range and reaches both ends", "[rng]")
{
    bool sawMin = false;
    bool sawMax = false;
    for (int i = 0; i < SAMPLES; i++) {
        int v = RNG::genInt(1, 3);
        REQUIRE(v >= 1);
        REQUIRE(v <= 3);
        sawMin = sawMin || v == 1;
        sawMax = sawMax || v == 3;
    }
    REQUIRE(sawMin);
    REQUIRE(sawMax);
}

TEST_CASE("RNG - genInt with equal bounds returns that value", "[rng]")
{
    REQUIRE(RNG::genInt(5, 5) == 5);
}

TEST_CASE("RNG - genIntMaxExcl never returns the upper bound", "[rng]")
{
    bool sawMin = false;
    for (int i = 0; i < SAMPLES; i++) {
        int v = RNG::genIntMaxExcl(1, 3);
        REQUIRE(v >= 1);
        REQUIRE(v < 3);
        sawMin = sawMin || v == 1;
    }
    REQUIRE(sawMin);
}

TEST_CASE("RNG - rnd returns values below max", "[rng]")
{
    for (int i = 0; i < SAMPLES; i++) {
        int v = RNG::rnd(4);
        REQUIRE(v >= 0);
        REQUIRE(v < 4);
    }
}

TEST_CASE("RNG - rnd returns 0 when max is below 2", "[rng]")
{
    REQUIRE(RNG::rnd(1) == 0);
    REQUIRE(RNG::rnd(0) == 0);
    REQUIRE(RNG::rnd(-10) == 0);
}

TEST_CASE("RNG - genDouble stays within the range", "[rng]")
{
    for (int i = 0; i < SAMPLES; i++) {
        double v = RNG::genDouble(-1.5, 2.5);
        REQUIRE(v >= -1.5);
        REQUIRE(v <= 2.5);
    }
}

TEST_CASE("RNG - getGenerator always returns the same generator", "[rng]")
{
    REQUIRE(&RNG::getGenerator() == &RNG::getGenerator());
}
