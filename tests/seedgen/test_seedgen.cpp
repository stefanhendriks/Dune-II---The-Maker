// Golden master tests for the seedgen map generator (resources/tools/seedgen), a reverse engineered
// Dune 2 algorithm: the 64x64 output must stay byte-identical to the files in resources/tools/seedgen/expected/.
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

constexpr int MAP_WIDTH = 64;
constexpr int MAP_HEIGHT = 64;

char matrix[16 * 17 + 1];
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

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "d2tm_seedgen_test.txt";
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

TEST_CASE("seedgen - map has 64 rows of 64 cells", "[seedgen]")
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
