#include <catch2/catch_test_macros.hpp>
#include "utils/cExpandingRectangle.h"

// ── initial state ─────────────────────────────────────────────────────────────

TEST_CASE("cExpandingRectangle - starts uninitialized", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    REQUIRE(r.isInitialized() == false);
}

// ── expand with a single point ────────────────────────────────────────────────

TEST_CASE("cExpandingRectangle - single point gives zero size", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    r.expand(10.0f, 20.0f);
    REQUIRE(r.isInitialized() == true);
    REQUIRE(r.getWidth()  == 0);
    REQUIRE(r.getHeight() == 0);
    REQUIRE(r.getX() == 10);
    REQUIRE(r.getY() == 20);
}

// ── expand with multiple points ───────────────────────────────────────────────

TEST_CASE("cExpandingRectangle - two points produce correct size", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    r.expand(10.0f, 20.0f);
    r.expand(50.0f, 80.0f);
    REQUIRE(r.getWidth()  == 40);
    REQUIRE(r.getHeight() == 60);
    REQUIRE(r.getX() == 10);
    REQUIRE(r.getY() == 20);
}

TEST_CASE("cExpandingRectangle - point inside current bounds does not change size", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    r.expand(0.0f,  0.0f);
    r.expand(100.0f, 100.0f);
    r.expand(50.0f, 50.0f);  // inside — no change expected
    REQUIRE(r.getWidth()  == 100);
    REQUIRE(r.getHeight() == 100);
}

TEST_CASE("cExpandingRectangle - expands in all four directions", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    r.expand(50.0f, 50.0f);
    r.expand(0.0f,  100.0f);  // extends left and down
    REQUIRE(r.getX() == 0);
    REQUIRE(r.getY() == 50);
    REQUIRE(r.getWidth()  == 50);
    REQUIRE(r.getHeight() == 50);
}

// ── expandBy ─────────────────────────────────────────────────────────────────

TEST_CASE("cExpandingRectangle - expandBy adds padding symmetrically", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    r.expand(10.0f, 20.0f);
    r.expand(50.0f, 80.0f);  // width=40, height=60, origin=(10,20)
    r.expandBy(5.0f);
    REQUIRE(r.getWidth()  == 50);   // 40 + 2*5
    REQUIRE(r.getHeight() == 70);   // 60 + 2*5
    REQUIRE(r.getX() == 5);         // 10 - 5
    REQUIRE(r.getY() == 15);        // 20 - 5
}

TEST_CASE("cExpandingRectangle - expandBy on single point gives square padding", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    r.expand(32.0f, 64.0f);
    r.expandBy(32.0f);
    REQUIRE(r.getWidth()  == 64);
    REQUIRE(r.getHeight() == 64);
    REQUIRE(r.getX() == 0);
    REQUIRE(r.getY() == 32);
}

// ── float precision ───────────────────────────────────────────────────────────

TEST_CASE("cExpandingRectangle - float positions are rounded correctly", "[expanding_rectangle]")
{
    cExpandingRectangle r;
    r.expand(0.4f, 0.4f);
    r.expand(9.6f, 9.6f);
    REQUIRE(r.getWidth()  == 9);   // round(9.6 - 0.4) = round(9.2) = 9
    REQUIRE(r.getHeight() == 9);
}
