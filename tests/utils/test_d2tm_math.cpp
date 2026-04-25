#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "utils/d2tm_math.h"

// ── wrapDegrees ──────────────────────────────────────────────────────────────

TEST_CASE("wrapDegrees - values in range pass through", "[math]")
{
    REQUIRE_THAT(wrapDegrees(0.0f),   Catch::Matchers::WithinAbs(0.0f,   1e-5f));
    REQUIRE_THAT(wrapDegrees(180.0f), Catch::Matchers::WithinAbs(180.0f, 1e-5f));
    REQUIRE_THAT(wrapDegrees(360.0f), Catch::Matchers::WithinAbs(360.0f, 1e-5f));
}

TEST_CASE("wrapDegrees - negative value wraps up by 360", "[math]")
{
    REQUIRE_THAT(wrapDegrees(-10.0f), Catch::Matchers::WithinAbs(350.0f, 1e-5f));
    REQUIRE_THAT(wrapDegrees(-1.0f),  Catch::Matchers::WithinAbs(359.0f, 1e-5f));
}

TEST_CASE("wrapDegrees - value over 360 wraps down by 360", "[math]")
{
    REQUIRE_THAT(wrapDegrees(361.0f), Catch::Matchers::WithinAbs(1.0f,   1e-5f));
    REQUIRE_THAT(wrapDegrees(400.0f), Catch::Matchers::WithinAbs(40.0f,  1e-5f));
}

// ── isAngleBetween ────────────────────────────────────────────────────────────

TEST_CASE("isAngleBetween - angle inside normal range", "[math]")
{
    REQUIRE(isAngleBetween(45, 0, 90)  == true);
    REQUIRE(isAngleBetween(0,  0, 90)  == true);  // inclusive lower bound
    REQUIRE(isAngleBetween(90, 0, 90)  == true);  // inclusive upper bound
}

TEST_CASE("isAngleBetween - angle outside normal range", "[math]")
{
    REQUIRE(isAngleBetween(135, 0, 90) == false);
    REQUIRE(isAngleBetween(270, 0, 90) == false);
}

TEST_CASE("isAngleBetween - range wraps through zero", "[math]")
{
    REQUIRE(isAngleBetween(350, 340, 20) == true);
    REQUIRE(isAngleBetween(10,  340, 20) == true);
    REQUIRE(isAngleBetween(180, 340, 20) == false);
}

// ── ABS_length ────────────────────────────────────────────────────────────────

TEST_CASE("ABS_length - same point returns 1", "[math]")
{
    REQUIRE_THAT(ABS_length(0, 0, 0, 0),   Catch::Matchers::WithinAbs(1.0, 1e-9));
    REQUIRE_THAT(ABS_length(5, 3, 5, 3),   Catch::Matchers::WithinAbs(1.0, 1e-9));
}

TEST_CASE("ABS_length - horizontal and vertical distances", "[math]")
{
    REQUIRE_THAT(ABS_length(0, 0, 3, 0),   Catch::Matchers::WithinAbs(3.0, 1e-9));
    REQUIRE_THAT(ABS_length(0, 0, 0, 4),   Catch::Matchers::WithinAbs(4.0, 1e-9));
}

TEST_CASE("ABS_length - diagonal distance (3-4-5 triangle)", "[math]")
{
    REQUIRE_THAT(ABS_length(0, 0, 3, 4),   Catch::Matchers::WithinAbs(5.0, 1e-9));
}

// ── fDegrees ──────────────────────────────────────────────────────────────────

TEST_CASE("fDegrees - cardinal directions from origin", "[math]")
{
    // target directly above (y decreases = up in screen coords)
    REQUIRE_THAT(fDegrees(0, 0, 0, -1), Catch::Matchers::WithinAbs(0.0f,   1e-4f));

    // target directly below
    REQUIRE_THAT(fDegrees(0, 0, 0, 1),  Catch::Matchers::WithinAbs(180.0f, 1e-4f));

    // target to the right
    REQUIRE_THAT(fDegrees(0, 0, 1, 0),  Catch::Matchers::WithinAbs(90.0f,  1e-4f));

    // target to the left
    REQUIRE_THAT(fDegrees(0, 0, -1, 0), Catch::Matchers::WithinAbs(270.0f, 1e-4f));
}

TEST_CASE("fDegrees - result is always in [0, 360)", "[math]")
{
    // spot-check several directions
    auto check = [](int x2, int y2) {
        float d = fDegrees(0, 0, x2, y2);
        return d >= 0.0f && d < 360.0f;
    };
    REQUIRE(check( 1,  1));
    REQUIRE(check(-1,  1));
    REQUIRE(check( 1, -1));
    REQUIRE(check(-1, -1));
}
