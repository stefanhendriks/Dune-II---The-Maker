/**
 * @file test_CreditsMath.cpp
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

#include "drawers/CreditsMath.hpp"

TEST_CASE("CreditsMath::getCreditDrawId maps digits to consecutive sprites", "[credits]")
{
    for (char c = '0'; c <= '9'; c++) {
        REQUIRE(CreditsMath::getCreditDrawId(c) == CREDITS_0 + (c - '0'));
    }
    REQUIRE(CreditsMath::getCreditDrawId('0') == CREDITS_0);
    REQUIRE(CreditsMath::getCreditDrawId('9') == CREDITS_9);
}

TEST_CASE("CreditsMath::getCreditDrawId returns CREDITS_NONE for non digits", "[credits]")
{
    REQUIRE(CreditsMath::getCreditDrawId('\0') == CREDITS_NONE);
    REQUIRE(CreditsMath::getCreditDrawId(' ') == CREDITS_NONE);
    REQUIRE(CreditsMath::getCreditDrawId('-') == CREDITS_NONE);
    REQUIRE(CreditsMath::getCreditDrawId('a') == CREDITS_NONE);
    REQUIRE(CreditsMath::getCreditDrawId('/') == CREDITS_NONE);
    REQUIRE(CreditsMath::getCreditDrawId(':') == CREDITS_NONE);
}

TEST_CASE("CreditsMath::getXDrawingOffset right aligns the number in 6 slots", "[credits]")
{
    REQUIRE(CreditsMath::getXDrawingOffset(0) == 5);
    REQUIRE(CreditsMath::getXDrawingOffset(9) == 5);
    REQUIRE(CreditsMath::getXDrawingOffset(10) == 4);
    REQUIRE(CreditsMath::getXDrawingOffset(99) == 4);
    REQUIRE(CreditsMath::getXDrawingOffset(100) == 3);
    REQUIRE(CreditsMath::getXDrawingOffset(999) == 3);
    REQUIRE(CreditsMath::getXDrawingOffset(1000) == 2);
    REQUIRE(CreditsMath::getXDrawingOffset(9999) == 2);
    REQUIRE(CreditsMath::getXDrawingOffset(10000) == 1);
    REQUIRE(CreditsMath::getXDrawingOffset(99999) == 1);
    REQUIRE(CreditsMath::getXDrawingOffset(100000) == 0);
}

TEST_CASE("CreditsMath::getXDrawingOffset of a negative amount is 5", "[credits]")
{
    REQUIRE(CreditsMath::getXDrawingOffset(-50) == 5);
}

TEST_CASE("CreditsMath::nextDisplayedCredits", "[credits]")
{
    SECTION("no change when displayed equals target") {
        REQUIRE(CreditsMath::nextDisplayedCredits(500, 500) == 500);
    }

    SECTION("small differences jump straight to the target") {
        REQUIRE(CreditsMath::nextDisplayedCredits(100, 101) == 101);
        REQUIRE(CreditsMath::nextDisplayedCredits(100, 120) == 120);
        REQUIRE(CreditsMath::nextDisplayedCredits(100, 80) == 80);
    }

    SECTION("medium differences (more than 20) move a quarter of the way") {
        REQUIRE(CreditsMath::nextDisplayedCredits(100, 121) == 100 + (21 / 4));
        REQUIRE(CreditsMath::nextDisplayedCredits(0, 200) == 50);
    }

    SECTION("big differences (more than 200) move an eighth of the way") {
        REQUIRE(CreditsMath::nextDisplayedCredits(0, 201) == 201 / 8);
        REQUIRE(CreditsMath::nextDisplayedCredits(0, 1000) == 125);
    }

    SECTION("decreases are never sliced, whatever the size") {
        REQUIRE(CreditsMath::nextDisplayedCredits(5000, 0) == 0);
    }

    SECTION("repeated steps always reach the target") {
        int displayed = 0;
        for (int i = 0; i < 100 && displayed != 3000; i++) {
            displayed = CreditsMath::nextDisplayedCredits(displayed, 3000);
        }
        REQUIRE(displayed == 3000);
    }
}
