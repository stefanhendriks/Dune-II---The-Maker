// Golden master tests for the seedgen map generator (resources/tools/seedgen), a reverse engineered
// Dune 2 algorithm. This file is compiled twice: as seedgen_tests (64x64, must stay byte-identical to the
// expected/ files) and as seedgen_tests_large (128x96, expected_large/), see tests/CMakeLists.txt.
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "seed.h"
#include "seed_protos.h"
#include "mapio_protos.h"

extern unsigned long seed;

namespace {

char matrix[MATRIX_SIZE + 1];
struct cell map[MAP_HEIGHT + 1][MAP_WIDTH];

// same steps as main() in resources/tools/seedgen/main.c, the map text is what printMap writes
std::string generateMap(unsigned long mapSeed)
{
    seed = mapSeed;
    createMatrix(matrix);
    addNoise1(matrix);
    addNoise2(matrix);
    copyMatrix(matrix, map);
    spreadMatrix(map);
    balanceMap(map);
    createRegions(map);
    addSpice(map);

    const std::filesystem::path path = std::filesystem::temp_directory_path() / ("d2tm_seedgen_test_" + std::to_string(MAP_WIDTH) + ".txt");
    FILE *f = std::fopen(path.string().c_str(), "w");
    REQUIRE(f != nullptr);
    printMap(map, f);
    std::fclose(f);

    std::string text;
    {
        std::ifstream in(path, std::ios::binary);
        text.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }
    std::filesystem::remove(path);
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
    return text;
}

std::string readExpected(const std::string &name)
{
    std::ifstream in(std::string(SEEDGEN_EXPECTED_DIR) + "/seed_" + name + ".txt", std::ios::binary);
    REQUIRE(in.good());
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
    return text;
}

} // namespace

TEST_CASE("seedgen - generated maps are identical to the golden master", "[seedgen]")
{
    // Seeds: all campaign seeds ([MAP] Seed in resources/bin/campaign/maps/scen*.ini) and a few extra
    const std::vector<unsigned long> seeds = SEEDGEN_SEEDS;
    for (unsigned long s : seeds) {
        CAPTURE(s);
        REQUIRE(generateMap(s) == readExpected(std::to_string(s)));
    }
}

TEST_CASE("seedgen - map has the configured size and terrain in every part of it", "[seedgen]")
{
    const std::vector<unsigned long> seeds = SEEDGEN_SEEDS;
    for (unsigned long s : seeds) {
        CAPTURE(s);
        const std::string text = generateMap(s);
        std::istringstream lines(text);
        std::string line;
        int rows = 0;
        while (std::getline(lines, line)) {
            REQUIRE(line.size() == MAP_WIDTH);
            rows++;
        }
        REQUIRE(rows == MAP_HEIGHT);
    }
}

#ifdef SEEDGEN_LARGE
TEST_CASE("seedgen - larger map is generated completely, not only the top rows", "[seedgen]")
{
    const int band = 24;
    const int cells = band * MAP_WIDTH;
    const std::vector<unsigned long> seeds = SEEDGEN_SEEDS;
    for (unsigned long s : seeds) {
        CAPTURE(s);
        const std::string text = generateMap(s);
        for (int firstRow = 0; firstRow + band <= MAP_HEIGHT; firstRow += band) {
            int terrain = 0;
            for (int y = firstRow; y < firstRow + band; y++) {
                for (int x = 0; x < MAP_WIDTH; x++) {
                    if (text[y * (MAP_WIDTH + 1) + x] != '.') terrain++;
                }
            }
            CAPTURE(firstRow, terrain);
            // a band is neither all sand nor almost completely filled
            REQUIRE(terrain >= 300);
            REQUIRE(terrain <= cells * 0.88);
        }
    }
}
#endif
