#include <catch2/catch_test_macros.hpp>

#include "data/gfxdata.h"
#include "gameobjects/map/cSeedMap.h"
#include "gameobjects/map/cSeedMapGenerator.h"

#include <array>
#include <cstdint>
#include <string>
#include <utility>

namespace {

// FNV-1a over all cell types, row by row
uint64_t hashOf(cSeedMap &map)
{
    uint64_t hash = 14695981039346656037ULL;
    for (int y = 0; y < cSeedMap::kMapHeight; y++) {
        for (int x = 0; x < cSeedMap::kMapWidth; x++) {
            hash ^= static_cast<uint64_t>(map.getCellType(x, y));
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}

std::string rowOf(cSeedMap &map, int y)
{
    std::string row;
    for (int x = 0; x < cSeedMap::kMapWidth; x++) {
        row += map.getCellTypeCharacter(x, y);
    }
    return row;
}

struct Golden {
    unsigned long seed;
    uint64_t hash;
};

// Golden values captured from the generator as it behaved before any refactoring.
// A mismatch means the generated terrain for that seed changed.
const Golden kGolden[] = {
    {0, 5860565007229673054ULL},
    {1, 10951012138447265671ULL},
    {2, 7410813960815597406ULL},
    {5, 815355382236517750ULL},
    {12, 13176126056764479605ULL},
    {42, 4900174247775374652ULL},
    {100, 17436169594819454557ULL},
    {500, 17381084056648620080ULL},
    {1234, 165553064185987318ULL},
    {3161, 915452899041332573ULL},
    {4242, 1211598214954311012ULL},
    {65535, 17961194730901492244ULL},
    {123456, 6044483416949189825ULL},
};

} // namespace

TEST_CASE("cSeedMapGenerator - fixed seeds give the golden map", "[map][seedmap]")
{
    for (const Golden &golden : kGolden) {
        CAPTURE(golden.seed);
        cSeedMap map = cSeedMapGenerator(golden.seed).generateSeedMap();
        REQUIRE(hashOf(map) == golden.hash);
    }
}

TEST_CASE("cSeedMapGenerator - same seed gives the same map", "[map][seedmap]")
{
    cSeedMap first = cSeedMapGenerator(1234).generateSeedMap();
    cSeedMap second = cSeedMapGenerator(1234).generateSeedMap();
    REQUIRE(hashOf(first) == hashOf(second));
}

TEST_CASE("cSeedMapGenerator - different seeds give different maps", "[map][seedmap]")
{
    cSeedMap first = cSeedMapGenerator(12).generateSeedMap();
    cSeedMap second = cSeedMapGenerator(13).generateSeedMap();
    REQUIRE(hashOf(first) != hashOf(second));
}

TEST_CASE("cSeedMapGenerator - only known terrain types are generated", "[map][seedmap]")
{
    for (unsigned long seed : {0UL, 1UL, 42UL, 500UL, 3161UL}) {
        cSeedMap map = cSeedMapGenerator(seed).generateSeedMap();
        for (int y = 0; y < cSeedMap::kMapHeight; y++) {
            for (int x = 0; x < cSeedMap::kMapWidth; x++) {
                REQUIRE(map.getCellTypeCharacter(x, y) != 'X');
            }
        }
    }
}

TEST_CASE("cSeedMapGenerator - seed 42 exact rows", "[map][seedmap]")
{
    cSeedMap map = cSeedMapGenerator(42).generateSeedMap();
    const std::array<std::pair<int, const char *>, 4> expected = {{
        {0, ".......---+-..............%%%%%........HH-..............%%%%...."},
        {20, ".--+----....%.-+-...........%..............-++--..............-+"},
        {40, "--.%%%.............-+-.........%%%%%%%....%%%%.....%%%%%%%...---"},
        {63, ".........HHHH--+-................HHHHHH-++++-+-................."},
    }};
    for (const auto &[y, row] : expected) {
        CAPTURE(y);
        REQUIRE(rowOf(map, y) == row);
    }
}

TEST_CASE("cSeedMapGenerator - seed 0 and seed 4294967295 give the same map", "[map][seedmap]")
{
    cSeedMap first = cSeedMapGenerator(0).generateSeedMap();
    cSeedMap second = cSeedMapGenerator(4294967295UL).generateSeedMap();
    REQUIRE(hashOf(first) == hashOf(second));
}
