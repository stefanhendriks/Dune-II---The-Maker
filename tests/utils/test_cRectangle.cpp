#include <catch2/catch_test_macros.hpp>
#include "utils/cRectangle.h"

// Rectangle at (10, 20) with width=30, height=40 — used across all test cases
static const cRectangle RECT(10, 20, 30, 40);

// ── isPointWithin (int, int) ──────────────────────────────────────────────────

TEST_CASE("cRectangle::isPointWithin - point inside", "[rectangle]")
{
    REQUIRE(RECT.isPointWithin(15, 30) == true);
}

TEST_CASE("cRectangle::isPointWithin - corners are within (inclusive)", "[rectangle]")
{
    REQUIRE(RECT.isPointWithin(10, 20) == true);  // top-left
    REQUIRE(RECT.isPointWithin(40, 20) == true);  // top-right  (10+30)
    REQUIRE(RECT.isPointWithin(10, 60) == true);  // bottom-left (20+40)
    REQUIRE(RECT.isPointWithin(40, 60) == true);  // bottom-right
}

TEST_CASE("cRectangle::isPointWithin - point outside", "[rectangle]")
{
    REQUIRE(RECT.isPointWithin(9,  30) == false);  // left of rect
    REQUIRE(RECT.isPointWithin(41, 30) == false);  // right of rect
    REQUIRE(RECT.isPointWithin(15, 19) == false);  // above rect
    REQUIRE(RECT.isPointWithin(15, 61) == false);  // below rect
}

// ── isWithin (static) ────────────────────────────────────────────────────────

TEST_CASE("cRectangle::isWithin - static helper matches instance behaviour", "[rectangle]")
{
    REQUIRE(cRectangle::isWithin(15, 30, 10, 20, 30, 40) == true);
    REQUIRE(cRectangle::isWithin(9,  30, 10, 20, 30, 40) == false);
}

// ── isOverlapping ─────────────────────────────────────────────────────────────

TEST_CASE("cRectangle::isOverlapping - fully overlapping (same rect)", "[rectangle]")
{
    cRectangle same(10, 20, 30, 40);
    REQUIRE(RECT.isOverlapping(same)  == true);
    REQUIRE(RECT.isOverlapping(&same) == true);
}

TEST_CASE("cRectangle::isOverlapping - partial overlap", "[rectangle]")
{
    cRectangle partial(35, 50, 20, 20);  // top-left corner (35,50) is inside RECT
    REQUIRE(RECT.isOverlapping(partial) == true);
}

TEST_CASE("cRectangle::isOverlapping - no overlap", "[rectangle]")
{
    cRectangle far(100, 100, 10, 10);
    REQUIRE(RECT.isOverlapping(far) == false);
}

TEST_CASE("cRectangle::isOverlapping - nullptr returns false", "[rectangle]")
{
    REQUIRE(RECT.isOverlapping(nullptr) == false);
}
