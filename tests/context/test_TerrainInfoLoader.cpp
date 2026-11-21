#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "context/TerrainInfoLoader.hpp"
#include "gameobjects/sTerrainInfo.h"

namespace {

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_terrain_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

} // namespace

TEST_CASE("TerrainInfoLoader reads the values of the TERRAIN section")
{
    const auto path = writeTempFile("[TERRAIN]\nBloomTimerDuration=300\nWallHitPoints=7\n");
    s_TerrainInfo info{};

    REQUIRE(TerrainInfoLoader().load(path.string(), info));
    std::filesystem::remove(path);

    CHECK(info.bloomTimerDuration == 300);
    CHECK(info.terrainWallHp == 7);
}

TEST_CASE("TerrainInfoLoader applies the old defaults for keys that are missing")
{
    const auto path = writeTempFile("[TERRAIN]\nBloomTimerDuration=300\n");
    s_TerrainInfo info{};

    REQUIRE(TerrainInfoLoader().load(path.string(), info));
    std::filesystem::remove(path);

    CHECK(info.terrainSpiceMinSpice == 50);
    CHECK(info.terrainSpiceMaxSpice == 125);
    CHECK(info.terrainSpiceHillMinSpice == 75);
    CHECK(info.terrainSpiceHillMaxSpice == 150);
    CHECK(info.terrainWallHp == 100);
}

TEST_CASE("TerrainInfoLoader keeps the defaults without a TERRAIN section or file")
{
    const auto path = writeTempFile("[OTHER]\nBloomTimerDuration=1\n");
    s_TerrainInfo info{};

    CHECK(TerrainInfoLoader().load(path.string(), info));
    std::filesystem::remove(path);
    CHECK(info.bloomTimerDuration == 200);

    s_TerrainInfo missing{};
    CHECK_FALSE(TerrainInfoLoader().load("this_file_does_not_exist_terrain.ini", missing));
    CHECK(missing.bloomTimerDuration == 200);
    CHECK(missing.terrainWallHp == 100);
}

TEST_CASE("The shipped terrain.ini holds the old values")
{
    s_TerrainInfo info{};

    REQUIRE(TerrainInfoLoader().load(std::string(D2TM_SOURCE_DIR) + "/resources/terrain.ini.org", info));

    CHECK(info.bloomTimerDuration == 200);
    CHECK(info.terrainSpiceMinSpice == 50);
    CHECK(info.terrainSpiceMaxSpice == 125);
    CHECK(info.terrainSpiceHillMinSpice == 75);
    CHECK(info.terrainSpiceHillMaxSpice == 150);
    CHECK(info.terrainWallHp == 100);
}
