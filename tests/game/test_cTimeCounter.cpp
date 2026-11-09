/**
 * @file test_cTimeCounter.cpp
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
#include "game/cTimeCounter.h"

TEST_CASE("cTimeCounter - starts at zero and does not count before start", "[timecounter]")
{
    cTimeCounter counter;
    counter.addTime(100);
    REQUIRE(counter.getTime() == 0);
}

TEST_CASE("cTimeCounter - start makes added time accumulate", "[timecounter]")
{
    cTimeCounter counter;
    counter.start();
    counter.addTime(100);
    counter.addTime(50);
    REQUIRE(counter.getTime() == 150);
}

TEST_CASE("cTimeCounter - pause stops counting and keeps the time", "[timecounter]")
{
    cTimeCounter counter;
    counter.start();
    counter.addTime(100);
    counter.pause();
    counter.addTime(500);
    REQUIRE(counter.getTime() == 100);
}

TEST_CASE("cTimeCounter - restart continues from the paused time", "[timecounter]")
{
    cTimeCounter counter;
    counter.start();
    counter.addTime(100);
    counter.pause();
    counter.restart();
    counter.addTime(25);
    REQUIRE(counter.getTime() == 125);
}

TEST_CASE("cTimeCounter - start again resets the time", "[timecounter]")
{
    cTimeCounter counter;
    counter.start();
    counter.addTime(100);
    counter.start();
    REQUIRE(counter.getTime() == 0);
    counter.addTime(7);
    REQUIRE(counter.getTime() == 7);
}
