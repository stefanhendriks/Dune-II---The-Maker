#include <catch2/catch_test_macros.hpp>
#include "game/KillDeathRatio.hpp"

TEST_CASE("killDeathRatioText - destroyed divided by lost, one decimal", "[kdr]")
{
    REQUIRE(killDeathRatioText(30, 5) == "6.0");
    REQUIRE(killDeathRatioText(7, 2) == "3.5");
    REQUIRE(killDeathRatioText(1, 3) == "0.3");
}

TEST_CASE("killDeathRatioText - no units lost shows the number of kills", "[kdr]")
{
    REQUIRE(killDeathRatioText(12, 0) == "12.0");
}

TEST_CASE("killDeathRatioText - no kills gives zero", "[kdr]")
{
    REQUIRE(killDeathRatioText(0, 4) == "0.0");
}

TEST_CASE("killDeathRatioText - no kills and no losses shows a dash", "[kdr]")
{
    REQUIRE(killDeathRatioText(0, 0) == "-");
}
