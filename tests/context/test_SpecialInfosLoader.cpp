/**
 * @file test_SpecialInfosLoader.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "context/SpecialInfosLoader.hpp"
#include "definitions.h"
#include "gameobjects/cSpecialInfos.h"
#include "include/enums/HouseFlags.h"

namespace {

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_specials_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

SpecialInfosLoader makeLoader()
{
    return SpecialInfosLoader([](std::string_view) { return TextureID::invalid; });
}

} // namespace

TEST_CASE("SpecialInfosLoader reads the values of a special section")
{
    const auto path = writeTempFile(
        "[DEATHHAND]\nHouses=Harkonnen, Sardaukar\nBuildTime=100\nProvidesType=BULLET\nProvides=ROCKET_BIG\n"
        "DeployFrom=STRUCTURE\nDeployAtStructure=Palace\nDeployTarget=INACCURATE\nDeployTargetPrecision=3\nUnits=2\n");
    cSpecialInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_SpecialInfo& info = infos[SPECIAL_DEATHHAND];
    CHECK(info.house == (eHouseBitFlag::Harkonnen | eHouseBitFlag::Sardaukar));
    CHECK(info.buildTime == 100);
    CHECK(info.providesType == eBuildType::BULLET);
    CHECK(info.providesTypeId == ROCKET_BIG);
    CHECK(info.deployFrom == eDeployFromType::AT_STRUCTURE);
    CHECK(info.deployAtStructure == PALACE);
    CHECK(info.deployTargetType == eDeployTargetType::TARGET_INACCURATE_CELL);
    CHECK(info.deployTargetPrecision == 3);
    CHECK(info.units == 2);
    CHECK(info.listType == eListType::LIST_PALACE);
}

TEST_CASE("SpecialInfosLoader keeps defaults for missing sections, keys and file")
{
    const auto path = writeTempFile("[FREMEN]\nUnits=3\n[NOTASPECIAL]\nUnits=9\n");
    cSpecialInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    CHECK(infos[SPECIAL_FREMEN].units == 3);
    CHECK(infos[SPECIAL_FREMEN].spawnBorderDistance == 4);
    CHECK(infos[SPECIAL_FREMEN].spawnScatter == 2);
    CHECK(infos[SPECIAL_SABOTEUR].house == eHouseBitFlag::Unknown);
    CHECK(infos[SPECIAL_SABOTEUR].units == 0);

    cSpecialInfos missing;
    CHECK_FALSE(makeLoader().load("this_file_does_not_exist_specials.ini", missing));
    CHECK(missing[SPECIAL_DEATHHAND].buildTime == 0);
}

TEST_CASE("The shipped specials.ini holds the old values")
{
    cSpecialInfos infos;

    REQUIRE(makeLoader().load(std::string(D2TM_SOURCE_DIR) + "/resources/specials.ini.org", infos));

    const s_SpecialInfo& saboteur = infos[SPECIAL_SABOTEUR];
    CHECK(saboteur.house == eHouseBitFlag::Ordos);
    CHECK(saboteur.autoBuild);
    CHECK(saboteur.providesType == eBuildType::UNIT);
    CHECK(saboteur.providesTypeId == SABOTEUR);
    CHECK(saboteur.deployFrom == eDeployFromType::AT_STRUCTURE);
    CHECK(saboteur.deployAtStructure == PALACE);
    CHECK(saboteur.units == 1);
    CHECK(saboteur.buildTime == 2468);
    CHECK(saboteur.description == "Saboteur");

    const s_SpecialInfo& fremen = infos[SPECIAL_FREMEN];
    CHECK(fremen.house == eHouseBitFlag::Atreides);
    CHECK(fremen.providesTypeId == UNIT_FREMEN_THREE);
    CHECK(fremen.deployFrom == eDeployFromType::AT_RANDOM_CELL);
    CHECK(fremen.units == 6);
    CHECK(fremen.buildTime == 1371);
    CHECK(fremen.spawnBorderDistance == 4);
    CHECK(fremen.spawnScatter == 2);

    const s_SpecialInfo& deathHand = infos[SPECIAL_DEATHHAND];
    CHECK(deathHand.house == (eHouseBitFlag::Harkonnen | eHouseBitFlag::Sardaukar));
    CHECK(deathHand.providesType == eBuildType::BULLET);
    CHECK(deathHand.providesTypeId == ROCKET_BIG);
    CHECK(deathHand.deployFrom == eDeployFromType::AT_STRUCTURE);
    CHECK(deathHand.deployTargetType == eDeployTargetType::TARGET_INACCURATE_CELL);
    CHECK(deathHand.deployTargetPrecision == 6);
    CHECK(deathHand.buildTime == 3428);
    CHECK(deathHand.description == "Death Hand");
}
