#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "definitions.h"
#include "gameobjects/players/cHousesInfo.h"
#include "utils/cIniFile.h"

namespace {

std::shared_ptr<cIniFile> loadIni(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_houses_" + std::to_string(timestamp) + ".ini");
    {
        std::ofstream output(path);
        output << contents;
    }
    auto ini = std::make_shared<cIniFile>();
    REQUIRE(ini->load(path.string()));
    std::filesystem::remove(path);
    return ini;
}

} // namespace

TEST_CASE("House difficulty factors are read from the house section")
{
    cHousesInfo houses;
    houses.installHouses(loadIni("[ORDOS]\nMoveSpeedFactor=0.5\nBuildSpeedFactor=0.6\nHarvestSpeedFactor=0.7\nDumpSpeedFactor=0.8\nInflictDamageFactor=0.9\n"));

    const auto& difficulty = houses.getDifficulty(ORDOS);
    CHECK(difficulty.getMoveSpeedFactor() == 0.5f);
    CHECK(difficulty.getBuildSpeedFactor() == 0.6f);
    CHECK(difficulty.getHarvestSpeedFactor() == 0.7f);
    CHECK(difficulty.getDumpSpeedFactor() == 0.8f);
    CHECK(difficulty.getInflictDamageFactor() == 0.9f);
}

TEST_CASE("House difficulty factors keep the old values for missing keys, sections and files")
{
    cHousesInfo houses;
    houses.installHouses(loadIni("[SARDAUKAR]\nBuildSpeedFactor=2.0\n[HARKONNEN]\nMINIMAPCOLOR = 255,0,0\n"));

    CHECK(houses.getDifficulty(SARDAUKAR).getBuildSpeedFactor() == 2.0f);
    CHECK(houses.getDifficulty(SARDAUKAR).getMoveSpeedFactor() == 1.2f);
    CHECK(houses.getDifficulty(SARDAUKAR).getInflictDamageFactor() == 1.25f);
    CHECK(houses.getDifficulty(HARKONNEN).getHarvestSpeedFactor() == 1.1f);
    CHECK(houses.getDifficulty(ORDOS).getMoveSpeedFactor() == 0.8f);
    CHECK(houses.getDifficulty(ATREIDES).getBuildSpeedFactor() == 1.0f);
    CHECK(houses.getDifficulty(FREMEN).getDumpSpeedFactor() == 1.0f);

    const cHousesInfo untouched;
    CHECK(untouched.getDifficulty(HARKONNEN).getMoveSpeedFactor() == 1.2f);
}

TEST_CASE("The shipped houses.ini.org holds the old house difficulty factors")
{
    cHousesInfo shipped;
    cHousesInfo defaults;
    auto ini = std::make_shared<cIniFile>();
    REQUIRE(ini->load(std::string(D2TM_SOURCE_DIR) + "/resources/houses.ini.org"));
    shipped.installHouses(ini);

    for (const int house : {ATREIDES, ORDOS, HARKONNEN, SARDAUKAR}) {
        CHECK(shipped.getDifficulty(house).getMoveSpeedFactor() == defaults.getDifficulty(house).getMoveSpeedFactor());
        CHECK(shipped.getDifficulty(house).getBuildSpeedFactor() == defaults.getDifficulty(house).getBuildSpeedFactor());
        CHECK(shipped.getDifficulty(house).getHarvestSpeedFactor() == defaults.getDifficulty(house).getHarvestSpeedFactor());
        CHECK(shipped.getDifficulty(house).getDumpSpeedFactor() == defaults.getDifficulty(house).getDumpSpeedFactor());
        CHECK(shipped.getDifficulty(house).getInflictDamageFactor() == defaults.getDifficulty(house).getInflictDamageFactor());
    }
}
