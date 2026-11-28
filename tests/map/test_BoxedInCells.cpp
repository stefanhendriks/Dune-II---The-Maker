#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include "gameobjects/map/BoxedInCells.hpp"
#include "gameobjects/map/MapGeometry.hpp"

#include <algorithm>
#include <cstdlib>
#include <set>

static constexpr int MAP_W = 64;
static constexpr int MAP_H = 64;

TEST_CASE("BoxedInCells - a unit in the open is not boxed in", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    REQUIRE_FALSE(BoxedInCells::isBoxedIn(geo, geo.makeCell(10, 10), [](int) { return true; }));
}

TEST_CASE("BoxedInCells - a unit with no free neighbour is boxed in", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    REQUIRE(BoxedInCells::isBoxedIn(geo, geo.makeCell(10, 10), [](int) { return false; }));
}

TEST_CASE("BoxedInCells - a unit that can reach only 1 other cell is boxed in", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    int open = geo.makeCell(11, 10);
    REQUIRE(BoxedInCells::isBoxedIn(geo, geo.makeCell(10, 10), [open](int c) { return c == open; }));
}

TEST_CASE("BoxedInCells - a unit that can reach 2 cells in a row is not boxed in", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    // only a corridor to the east: (11,10) then (12,10)
    std::set<int> open = {geo.makeCell(11, 10), geo.makeCell(12, 10)};
    REQUIRE_FALSE(BoxedInCells::isBoxedIn(geo, geo.makeCell(10, 10), [&open](int c) { return open.contains(c); }));
}

TEST_CASE("BoxedInCells - diagonal steps count as moves", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    std::set<int> open = {geo.makeCell(11, 11), geo.makeCell(12, 12)};
    REQUIRE_FALSE(BoxedInCells::isBoxedIn(geo, geo.makeCell(10, 10), [&open](int c) { return open.contains(c); }));
}

TEST_CASE("BoxedInCells - the invisible map border is never reachable", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    // unit in the top-left playable corner, only border cells around it would be 'open'
    int corner = geo.makeCell(1, 1);
    REQUIRE(BoxedInCells::isBoxedIn(geo, corner, [&geo](int c) { return geo.getCellX(c) == 0 || geo.getCellY(c) == 0; }));
}

TEST_CASE("BoxedInCells - closest usable cells are all usable cells at radius 1", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    auto cells = BoxedInCells::findClosestUsableCells(geo, geo.makeCell(10, 10), [](int) { return true; });
    std::vector<int> expected = {geo.makeCell(9, 10), geo.makeCell(10, 9), geo.makeCell(10, 11), geo.makeCell(11, 10)};
    REQUIRE_THAT(cells, Catch::Matchers::UnorderedEquals(expected));
}

TEST_CASE("BoxedInCells - closest usable cells move outwards when radius 1 has none", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    int unitCell = geo.makeCell(10, 10);
    // usable: a diagonal neighbour (radius 2), and a cell at radius 3
    std::set<int> usable = {geo.makeCell(11, 11), geo.makeCell(13, 10)};
    auto cells = BoxedInCells::findClosestUsableCells(geo, unitCell, [&usable](int c) { return usable.contains(c); });
    REQUIRE(cells == std::vector<int>{geo.makeCell(11, 11)});
}

TEST_CASE("BoxedInCells - every cell at the radius is checked once", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    std::vector<int> checked;
    auto cells = BoxedInCells::findClosestUsableCells(geo, geo.makeCell(10, 10), [&checked](int c) {
        checked.push_back(c);
        return false;
    });
    REQUIRE(cells.empty());
    // radius 2 has 8 cells, each checked once
    std::set<int> atRadius2;
    for (int c : checked) {
        int d = std::abs(geo.getCellX(c) - 10) + std::abs(geo.getCellY(c) - 10);
        if (d == 2) {
            atRadius2.insert(c);
        }
    }
    REQUIRE(atRadius2.size() == 8);
    REQUIRE(std::count_if(checked.begin(), checked.end(), [&atRadius2](int c) { return atRadius2.contains(c); }) == 8);
}

TEST_CASE("BoxedInCells - no usable cell anywhere gives an empty result", "[map][boxedin]")
{
    MapGeometry geo(MAP_W, MAP_H);
    REQUIRE(BoxedInCells::findClosestUsableCells(geo, geo.makeCell(10, 10), [](int) { return false; }).empty());
}
