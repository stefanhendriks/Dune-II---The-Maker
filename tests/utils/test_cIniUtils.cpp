#include <catch2/catch_test_macros.hpp>
#include "utils/cIniUtils.h"

TEST_CASE("colorFromString - parses 'r, g, b' string into Color", "[color_parsing]")
{
    Color c = cIniUtils::colorFromString("27, 224, 204");
    REQUIRE(c.r == 27);
    REQUIRE(c.g == 224);
    REQUIRE(c.b == 204);
    REQUIRE(c.a == 255);
}

TEST_CASE("colorFromString - zero values produce black with full alpha", "[color_parsing]")
{
    Color c = cIniUtils::colorFromString("0, 0, 0");
    REQUIRE(c.r == 0);
    REQUIRE(c.g == 0);
    REQUIRE(c.b == 0);
    REQUIRE(c.a == 255);
}

TEST_CASE("colorFromString - max channel values", "[color_parsing]")
{
    Color c = cIniUtils::colorFromString("255, 255, 255");
    REQUIRE(c.r == 255);
    REQUIRE(c.g == 255);
    REQUIRE(c.b == 255);
    REQUIRE(c.a == 255);
}

TEST_CASE("houseColorFromString - parses 'r, g, b' string into HouseColor", "[color_parsing]")
{
    HouseColor hc = cIniUtils::houseColorFromString("27, 224, 204");
    REQUIRE(hc[0] == 27);
    REQUIRE(hc[1] == 224);
    REQUIRE(hc[2] == 204);
}

TEST_CASE("houseColorFromString - zero values", "[color_parsing]")
{
    HouseColor hc = cIniUtils::houseColorFromString("0, 0, 0");
    REQUIRE(hc[0] == 0);
    REQUIRE(hc[1] == 0);
    REQUIRE(hc[2] == 0);
}

TEST_CASE("houseColorFromString - max channel values", "[color_parsing]")
{
    HouseColor hc = cIniUtils::houseColorFromString("255, 255, 255");
    REQUIRE(hc[0] == 255);
    REQUIRE(hc[1] == 255);
    REQUIRE(hc[2] == 255);
}
