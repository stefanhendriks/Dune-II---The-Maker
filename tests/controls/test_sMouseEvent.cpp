/**
 * @file test_sMouseEvent.cpp
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

#include "controls/sMouseEvent.h"

#include <string>
#include <utility>

TEST_CASE("s_MouseEvent - default event is MOUSE_NONE at the origin", "[mouseevent]")
{
    const s_MouseEvent event;
    REQUIRE(event.eventType == eMouseEventType::MOUSE_NONE);
    REQUIRE(event.coords.x == 0);
    REQUIRE(event.coords.y == 0);
}

TEST_CASE("s_MouseEvent - every event type has its own name", "[mouseevent]")
{
    const std::pair<eMouseEventType, std::string> expected[] = {
        {MOUSE_NONE, "MOUSE_NONE"},
        {MOUSE_MOVED_TO, "MOUSE_MOVED_TO"},
        {MOUSE_RIGHT_BUTTON_CLICKED, "MOUSE_RIGHT_BUTTON_CLICKED"},
        {MOUSE_LEFT_BUTTON_CLICKED, "MOUSE_LEFT_BUTTON_CLICKED"},
        {MOUSE_LEFT_BUTTON_DOUBLE_CLICKED, "MOUSE_LEFT_BUTTON_DOUBLE_CLICKED"},
        {MOUSE_RIGHT_BUTTON_PRESSED, "MOUSE_RIGHT_BUTTON_PRESSED"},
        {MOUSE_LEFT_BUTTON_PRESSED, "MOUSE_LEFT_BUTTON_PRESSED"},
        {MOUSE_SCROLLED_UP, "MOUSE_SCROLLED_UP"},
        {MOUSE_SCROLLED_DOWN, "MOUSE_SCROLLED_DOWN"},
    };
    for (const auto &[type, name] : expected) {
        REQUIRE(name == s_MouseEvent::toStringMouseEventType(type));
    }
}

TEST_CASE("s_MouseEvent - toString shows type and coordinates", "[mouseevent]")
{
    s_MouseEvent event;
    event.eventType = eMouseEventType::MOUSE_MOVED_TO;
    event.coords = cPoint(12, 34);
    REQUIRE(s_MouseEvent::toString(event) == "s_MouseEvent [type=MOUSE_MOVED_TO], [x=12], [y=34]");
}

TEST_CASE("s_MouseEvent - toString handles negative coordinates", "[mouseevent]")
{
    s_MouseEvent event;
    event.eventType = eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED;
    event.coords = cPoint(-1, -20);
    REQUIRE(s_MouseEvent::toString(event) == "s_MouseEvent [type=MOUSE_LEFT_BUTTON_CLICKED], [x=-1], [y=-20]");
}
