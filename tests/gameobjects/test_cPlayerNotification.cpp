/**
 * @file test_cPlayerNotification.cpp
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
#include "gameobjects/players/cPlayerNotification.h"

namespace {
bool sameColor(const Color &a, const Color &b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
}

TEST_CASE("eNotificationTypeString - maps every type to its name", "[notification]")
{
    REQUIRE(eNotificationTypeString(eNotificationType::NEUTRAL) == "NEUTRAL");
    REQUIRE(eNotificationTypeString(eNotificationType::PRIORITY) == "PRIORITY");
    REQUIRE(eNotificationTypeString(eNotificationType::BAD) == "BAD");
    REQUIRE(eNotificationTypeString(eNotificationType::OTHER) == "OTHER");
}

TEST_CASE("cPlayerNotification - starts visible with its message", "[notification]")
{
    cPlayerNotification notification("Harvester lost", eNotificationType::BAD);
    REQUIRE(notification.isVisible());
    REQUIRE(notification.getMessage() == "Harvester lost");
    REQUIRE(notification.getTimer() > 0);
}

TEST_CASE("cPlayerNotification - thinkFast counts down until it is no longer visible", "[notification]")
{
    cPlayerNotification notification("msg", eNotificationType::NEUTRAL);
    const int start = notification.getTimer();
    notification.thinkFast();
    REQUIRE(notification.getTimer() == start - 1);

    for (int i = 1; i < start; i++) {
        notification.thinkFast();
    }
    REQUIRE(notification.getTimer() == 0);
    REQUIRE_FALSE(notification.isVisible());
}

TEST_CASE("cPlayerNotification - each type has its own color", "[notification]")
{
    REQUIRE(sameColor(cPlayerNotification("a", eNotificationType::NEUTRAL).getColor(), Color::White));
    REQUIRE(sameColor(cPlayerNotification("a", eNotificationType::PRIORITY).getColor(), Color::Yellow));
    REQUIRE(sameColor(cPlayerNotification("a", eNotificationType::BAD).getColor(), Color::Red));
    REQUIRE(sameColor(cPlayerNotification("a", eNotificationType::OTHER).getColor(), Color::Green));
}
