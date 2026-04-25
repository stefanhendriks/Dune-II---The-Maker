#include <catch2/catch_test_macros.hpp>
#include "utils/cPoint.h"
#include "utils/cRectangle.h"

// ── construction ──────────────────────────────────────────────────────────────

TEST_CASE("cPoint - default constructor initialises to (0, 0)", "[point]")
{
    cPoint p;
    REQUIRE(p.x == 0);
    REQUIRE(p.y == 0);
}

TEST_CASE("cPoint - value constructor stores coordinates", "[point]")
{
    cPoint p(3, 7);
    REQUIRE(p.x == 3);
    REQUIRE(p.y == 7);
}

// ── isWithinRectangle ─────────────────────────────────────────────────────────

TEST_CASE("cPoint::isWithinRectangle - point inside rectangle", "[point]")
{
    cPoint p(15, 30);
    cRectangle r(10, 20, 30, 40);
    REQUIRE(p.isWithinRectangle(&r) == true);
}

TEST_CASE("cPoint::isWithinRectangle - point outside rectangle", "[point]")
{
    cRectangle r(10, 20, 30, 40);
    REQUIRE(cPoint(5,  30).isWithinRectangle(&r) == false);
    REQUIRE(cPoint(50, 30).isWithinRectangle(&r) == false);
    REQUIRE(cPoint(15, 10).isWithinRectangle(&r) == false);
    REQUIRE(cPoint(15, 70).isWithinRectangle(&r) == false);
}

TEST_CASE("cPoint::isWithinRectangle - nullptr returns false", "[point]")
{
    cPoint p(5, 5);
    REQUIRE(p.isWithinRectangle(nullptr) == false);
}

// ── split helper ──────────────────────────────────────────────────────────────

TEST_CASE("cPoint::split - unpacks point coordinates into separate variables", "[point]")
{
    int x = 0, y = 0;
    cPoint::split(x, y) = cPoint(4, 9);
    REQUIRE(x == 4);
    REQUIRE(y == 9);
}
