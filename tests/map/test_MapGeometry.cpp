#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "map/MapGeometry.hpp"

// All tests use a standard 64x64 map (the size used by the game)
static constexpr int MAP_W = 64;
static constexpr int MAP_H = 64;

TEST_CASE("MapGeometry - getCellX", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    REQUIRE(geo.getCellX(0)    == 0);
    REQUIRE(geo.getCellX(1)    == 1);
    REQUIRE(geo.getCellX(63)   == 63); // last cell of first row
    REQUIRE(geo.getCellX(64)   == 0);  // first cell of second row
    REQUIRE(geo.getCellX(65)   == 1);
    REQUIRE(geo.getCellX(4095) == 63); // last cell of the map
}

TEST_CASE("MapGeometry - getCellY", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    REQUIRE(geo.getCellY(0)    == 0);
    REQUIRE(geo.getCellY(63)   == 0);  // still first row
    REQUIRE(geo.getCellY(64)   == 1);  // second row
    REQUIRE(geo.getCellY(128)  == 2);
    REQUIRE(geo.getCellY(4095) == 63); // last row
}

TEST_CASE("MapGeometry - makeCell", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    REQUIRE(geo.makeCell(0,  0)  == 0);
    REQUIRE(geo.makeCell(1,  0)  == 1);
    REQUIRE(geo.makeCell(0,  1)  == 64);
    REQUIRE(geo.makeCell(63, 63) == 4095);

    // round-trip: makeCell -> getCellX/Y
    REQUIRE(geo.getCellX(geo.makeCell(5, 7)) == 5);
    REQUIRE(geo.getCellY(geo.makeCell(5, 7)) == 7);
}

TEST_CASE("MapGeometry - getCellWithMapDimensions", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    REQUIRE(geo.getCellWithMapDimensions(0,  0)  == 0);
    REQUIRE(geo.getCellWithMapDimensions(1,  0)  == 1);
    REQUIRE(geo.getCellWithMapDimensions(63, 63) == 4095);

    // out-of-bounds returns -1
    REQUIRE(geo.getCellWithMapDimensions(-1,  0) == -1);
    REQUIRE(geo.getCellWithMapDimensions(0,  -1) == -1);
    REQUIRE(geo.getCellWithMapDimensions(64,  0) == -1);
    REQUIRE(geo.getCellWithMapDimensions(0,  64) == -1);
}

TEST_CASE("MapGeometry - getCellWithMapBorders", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    // Playable area is cells (1,1) to (62,62); the outer ring is invisible border
    REQUIRE(geo.getCellWithMapBorders(1,  1)  == 65);   // (1 * 64) + 1
    REQUIRE(geo.getCellWithMapBorders(62, 62) == geo.makeCell(62, 62));

    // Invisible border cells return -1
    REQUIRE(geo.getCellWithMapBorders(0,  1)  == -1);
    REQUIRE(geo.getCellWithMapBorders(1,  0)  == -1);
    REQUIRE(geo.getCellWithMapBorders(63, 1)  == -1);
    REQUIRE(geo.getCellWithMapBorders(1,  63) == -1);
}

TEST_CASE("MapGeometry - isValidCell", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    REQUIRE(geo.isValidCell(0)    == true);
    REQUIRE(geo.isValidCell(4095) == true);
    REQUIRE(geo.isValidCell(-1)   == false);
    REQUIRE(geo.isValidCell(4096) == false);
}

TEST_CASE("MapGeometry - getAbsoluteXPositionFromCell", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    // Each tile is 32 pixels wide
    REQUIRE(geo.getAbsoluteXPositionFromCell(0)  == 0);
    REQUIRE(geo.getAbsoluteXPositionFromCell(1)  == 32);
    REQUIRE(geo.getAbsoluteXPositionFromCell(64) == 0);  // x wraps back to 0 on new row
    REQUIRE(geo.getAbsoluteXPositionFromCell(65) == 32);
}

TEST_CASE("MapGeometry - getAbsoluteYPositionFromCell", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    // Each tile is 32 pixels tall
    REQUIRE(geo.getAbsoluteYPositionFromCell(0)  == 0);
    REQUIRE(geo.getAbsoluteYPositionFromCell(1)  == 0);   // still row 0
    REQUIRE(geo.getAbsoluteYPositionFromCell(64) == 32);  // row 1
    REQUIRE(geo.getAbsoluteYPositionFromCell(65) == 32);
}

TEST_CASE("MapGeometry - getAbsolutePositionFromCell (centered)", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    // Centered adds half a tile (16px) to each axis
    REQUIRE(geo.getAbsoluteXPositionFromCellCentered(0)  == 16);
    REQUIRE(geo.getAbsoluteYPositionFromCellCentered(0)  == 16);
    REQUIRE(geo.getAbsoluteXPositionFromCellCentered(65) == 48);
    REQUIRE(geo.getAbsoluteYPositionFromCellCentered(65) == 48);
}

TEST_CASE("MapGeometry - distance", "[map][geometry]")
{
    MapGeometry geo(MAP_W, MAP_H);

    // ABS_length() returns 1 when both inputs are the same cell (by design)
    REQUIRE_THAT(geo.distance(0, 0),   Catch::Matchers::WithinAbs(1.0,        1e-9));
    REQUIRE_THAT(geo.distance(0, 1),   Catch::Matchers::WithinAbs(1.0,        1e-9)); // horizontal
    REQUIRE_THAT(geo.distance(0, 64),  Catch::Matchers::WithinAbs(1.0,        1e-9)); // vertical
    REQUIRE_THAT(geo.distance(0, 65),  Catch::Matchers::WithinAbs(std::sqrt(2.0), 1e-9)); // diagonal
}
