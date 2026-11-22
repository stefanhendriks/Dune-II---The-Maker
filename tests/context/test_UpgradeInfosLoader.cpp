/**
 * @file test_UpgradeInfosLoader.cpp
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

#include "context/UpgradeInfosLoader.hpp"
#include "definitions.h"
#include "gameobjects/cUpgradeInfo.h"
#include "include/enums/HouseFlags.h"
#include "sidebar/cSideBar.h"

namespace {

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_upgrades_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

UpgradeInfosLoader makeLoader(bool cheatMode = false)
{
    return UpgradeInfosLoader(
        [](std::string_view) { return TextureID::invalid; },
        [](int type) { return type == HEAVYFACTORY ? 600 : 400; },
        [](int type) { return type == SONICTANK ? 300 : 200; },
        cheatMode);
}

} // namespace

TEST_CASE("UpgradeInfosLoader reads the values of an upgrade section")
{
    const auto path = writeTempFile(
        "[HEAVYFCTRY_LAUNCHER]\nDescription=Rockets\nHouses=Atreides, Sardaukar\nTechLevel=5\nCost=123\n"
        "Structure=HEAVYFACTORY\nNeedsStructure=RADAR\nAtUpgradeLevel=1\nDependsOnStructureLevel=false\n"
        "ProvidesType=UNIT\nProvides=LAUNCHER\nList=UNITS\nSubList=HEAVYFCTRY\nBuildTime=77\n");
    cUpgradeInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_UpgradeInfo& info = infos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER];
    CHECK(info.enabled);
    CHECK(info.description == "Rockets");
    CHECK(info.house == (eHouseBitFlag::Atreides | eHouseBitFlag::Sardaukar));
    CHECK(info.techLevel == 5);
    CHECK(info.cost == 123);
    CHECK(info.structureType == HEAVYFACTORY);
    CHECK(info.needsStructureType == RADAR);
    CHECK(info.atUpgradeLevel == 1);
    CHECK_FALSE(info.dependsOnStructureLevel);
    CHECK(info.providesType == eBuildType::UNIT);
    CHECK(info.providesTypeId == LAUNCHER);
    CHECK(info.providesTypeList == eListType::LIST_UNITS);
    CHECK(info.providesTypeSubList == SUBLIST_HEAVYFCTRY);
    CHECK(info.buildTime == 77);
}

TEST_CASE("UpgradeInfosLoader derives the cost from a structure or unit")
{
    const auto path = writeTempFile(
        "[HEAVYFCTRY_MVC]\nCostOfStructure=HEAVYFACTORY\n"
        "[HEAVYFCTRY_SAR_SONICTANK]\nCostOfUnit=SONICTANK\nCostPercentage=10\n");
    cUpgradeInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost == 300);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].cost == 30);
}

TEST_CASE("UpgradeInfosLoader uses BuildTimeCheatMode only in cheat mode")
{
    const auto path = writeTempFile("[WOR_TROOPERS]\nBuildTime=150\nBuildTimeCheatMode=5\n");
    cUpgradeInfos normal;
    cUpgradeInfos cheat;

    REQUIRE(makeLoader(false).load(path.string(), normal));
    REQUIRE(makeLoader(true).load(path.string(), cheat));
    std::filesystem::remove(path);

    CHECK(normal[UPGRADE_TYPE_WOR_TROOPERS].buildTime == 150);
    CHECK(cheat[UPGRADE_TYPE_WOR_TROOPERS].buildTime == 5);
}

TEST_CASE("UpgradeInfosLoader keeps defaults for missing sections, keys and file")
{
    const auto path = writeTempFile("[HITECH_ORNI]\nTechLevel=8\n[NOT_AN_UPGRADE]\nTechLevel=1\n");
    cUpgradeInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    CHECK(infos[UPGRADE_TYPE_HITECH_ORNI].enabled);
    CHECK(infos[UPGRADE_TYPE_HITECH_ORNI].techLevel == 8);
    CHECK(infos[UPGRADE_TYPE_HITECH_ORNI].cost == 100);
    CHECK(infos[UPGRADE_TYPE_HITECH_ORNI].buildTime == 5);
    CHECK(infos[UPGRADE_TYPE_HITECH_ORNI].dependsOnStructureLevel);
    CHECK_FALSE(infos[UPGRADE_TYPE_WOR_TROOPERS].enabled);

    cUpgradeInfos missing;
    CHECK_FALSE(makeLoader().load("does_not_exist_upgrades.ini", missing));
    CHECK_FALSE(missing[UPGRADE_TYPE_HITECH_ORNI].enabled);
}

TEST_CASE("The shipped upgrades.ini holds the old values")
{
    cUpgradeInfos infos;
    REQUIRE(makeLoader().load(std::string(D2TM_SOURCE_DIR) + "/resources/upgrades.ini.org", infos));

    const auto allHouses = eHouseBitFlag::Atreides | eHouseBitFlag::Harkonnen | eHouseBitFlag::Ordos | eHouseBitFlag::Sardaukar;

    const s_UpgradeInfo& slab = infos[UPGRADE_TYPE_CONSTYARD_SLAB4];
    CHECK(slab.enabled);
    CHECK(slab.house == allHouses);
    CHECK(slab.techLevel == 4);
    CHECK(slab.cost == 200);
    CHECK(slab.structureType == CONSTYARD);
    CHECK(slab.needsStructureType == -1);
    CHECK(slab.atUpgradeLevel == 0);
    CHECK(slab.providesType == eBuildType::STRUCTURE);
    CHECK(slab.providesTypeId == SLAB4);
    CHECK(slab.providesTypeList == eListType::LIST_CONSTYARD);
    CHECK(slab.providesTypeSubList == SUBLIST_CONSTYARD);
    CHECK(slab.buildTime == 50);
    CHECK(slab.description == "Build 4 concrete slabs at once");

    const s_UpgradeInfo& turret = infos[UPGRADE_TYPE_CONSTYARD_RTURRET];
    CHECK(turret.techLevel == 6);
    CHECK(turret.needsStructureType == RADAR);
    CHECK(turret.atUpgradeLevel == 1);
    CHECK(turret.providesTypeId == RTURRET);
    CHECK(turret.buildTime == 150);

    const s_UpgradeInfo& quad = infos[UPGRADE_TYPE_LIGHTFCTRY_QUAD];
    CHECK(quad.house == (eHouseBitFlag::Atreides | eHouseBitFlag::Ordos | eHouseBitFlag::Sardaukar));
    CHECK(quad.cost == 200); // half of the structure cost given by the test lookup (400)
    CHECK(quad.structureType == LIGHTFACTORY);
    CHECK(quad.needsStructureType == LIGHTFACTORY);
    CHECK(quad.providesTypeId == QUAD);
    CHECK(quad.providesTypeSubList == SUBLIST_LIGHTFCTRY);

    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_MVC].house == allHouses);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost == 300);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeId == MCV);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].atUpgradeLevel == 1);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].atUpgradeLevel == 2);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].house == eHouseBitFlag::Ordos);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].atUpgradeLevel == 1);

    const s_UpgradeInfo& orni = infos[UPGRADE_TYPE_HITECH_ORNI];
    CHECK(orni.techLevel == 8);
    CHECK(orni.structureType == HIGHTECH);
    CHECK(orni.needsStructureType == -1);
    CHECK(orni.providesTypeId == ORNITHOPTER);
    CHECK(orni.providesTypeSubList == SUBLIST_HIGHTECH);

    const s_UpgradeInfo& troopers = infos[UPGRADE_TYPE_WOR_TROOPERS];
    CHECK(troopers.house == (eHouseBitFlag::Harkonnen | eHouseBitFlag::Ordos | eHouseBitFlag::Sardaukar));
    CHECK(troopers.providesTypeList == eListType::LIST_FOOT_UNITS);
    CHECK(troopers.providesTypeSubList == SUBLIST_TROOPERS);
    CHECK(troopers.buildTime == 150);
    CHECK(infos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeSubList == SUBLIST_INFANTRY);

    const s_UpgradeInfo& sonic = infos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK];
    CHECK(sonic.house == eHouseBitFlag::Sardaukar);
    CHECK(sonic.cost == 150);
    CHECK(sonic.structureType == IX);
    CHECK(sonic.needsStructureType == HEAVYFACTORY);
    CHECK_FALSE(sonic.dependsOnStructureLevel);
    CHECK(sonic.providesTypeId == SONICTANK);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].providesTypeId == DEVIATOR);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].providesTypeId == DEVASTATOR);

    cUpgradeInfos cheatInfos;
    REQUIRE(makeLoader(true).load(std::string(D2TM_SOURCE_DIR) + "/resources/upgrades.ini.org", cheatInfos));
    CHECK(cheatInfos[UPGRADE_TYPE_WOR_TROOPERS].buildTime == 5);
    CHECK(cheatInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].buildTime == 5);
    CHECK(cheatInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].buildTime == 50);
}

TEST_CASE("UpgradeInfosLoader keeps the defaults when a structure or unit name is unknown")
{
    const auto path = writeTempFile(
        "[HEAVYFCTRY_MVC]\nCostOfStructure=NOPE\nStructure=NOPE\n"
        "[HEAVYFCTRY_SAR_SONICTANK]\nCostOfUnit=NOPE\n");
    cUpgradeInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost == 100);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_MVC].structureType == CONSTYARD);
    CHECK(infos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].cost == 100);
}
