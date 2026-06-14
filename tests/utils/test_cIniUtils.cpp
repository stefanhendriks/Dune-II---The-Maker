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

TEST_CASE("colorFromString - extra whitespace around values is trimmed", "[color_parsing]")
{
    Color c = cIniUtils::colorFromString(" 27 , 224 , 204 ");
    REQUIRE(c.r == 27);
    REQUIRE(c.g == 224);
    REQUIRE(c.b == 204);
}

TEST_CASE("houseColorFromString - parses 'r, g, b' string into HouseColor", "[color_parsing]")
{
    HouseColor hc = cIniUtils::houseColorFromString("27, 224, 204");
    REQUIRE(hc.r == 27);
    REQUIRE(hc.g == 224);
    REQUIRE(hc.b == 204);
}

TEST_CASE("houseColorFromString - zero values", "[color_parsing]")
{
    HouseColor hc = cIniUtils::houseColorFromString("0, 0, 0");
    REQUIRE(hc.r == 0);
    REQUIRE(hc.g == 0);
    REQUIRE(hc.b == 0);
}

TEST_CASE("houseColorFromString - max channel values", "[color_parsing]")
{
    HouseColor hc = cIniUtils::houseColorFromString("255, 255, 255");
    REQUIRE(hc.r == 255);
    REQUIRE(hc.g == 255);
    REQUIRE(hc.b == 255);
}

TEST_CASE("colorFromString - non-numeric value throws invalid_argument", "[color_parsing]")
{
    REQUIRE_THROWS_AS(cIniUtils::colorFromString("23, a, 2"), std::invalid_argument);
}

TEST_CASE("colorFromString - value above 255 throws out_of_range", "[color_parsing]")
{
    REQUIRE_THROWS_AS(cIniUtils::colorFromString("256, 0, 0"), std::out_of_range);
}

TEST_CASE("colorFromString - negative value throws out_of_range", "[color_parsing]")
{
    REQUIRE_THROWS_AS(cIniUtils::colorFromString("-1, 0, 0"), std::out_of_range);
}

TEST_CASE("houseColorFromString - non-numeric value throws invalid_argument", "[color_parsing]")
{
    REQUIRE_THROWS_AS(cIniUtils::houseColorFromString("23, a, 2"), std::invalid_argument);
}

TEST_CASE("houseColorFromString - value above 255 throws out_of_range", "[color_parsing]")
{
    REQUIRE_THROWS_AS(cIniUtils::houseColorFromString("0, 256, 0"), std::out_of_range);
}

TEST_CASE("houseColorFromString - negative value throws out_of_range", "[color_parsing]")
{
    REQUIRE_THROWS_AS(cIniUtils::houseColorFromString("0, -1, 0"), std::out_of_range);
}
