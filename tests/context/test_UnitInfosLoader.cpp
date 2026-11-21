#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

#include "context/UnitInfosLoader.hpp"
#include "definitions.h"
#include "gameobjects/units/cUnitInfos.h"
#include "sidebar/cSideBar.h"

namespace {

// Assets are only handed around as pointers/ids here, so the length of the name is enough to tell them apart
UnitInfosLoader makeLoader()
{
    return UnitInfosLoader(
        [](std::string_view name) { return reinterpret_cast<SDL_Surface*>(name.size()); },
        [](std::string_view name) { return reinterpret_cast<Texture*>(name.size()); },
        [](std::string_view name) { return toTextureID(static_cast<int>(name.size())); },
        [](std::string_view name) { return toAudioID(static_cast<int>(name.size())); });
}

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_units_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

} // namespace

TEST_CASE("UnitInfosLoader reads values, assets and lists of a unit section")
{
    const auto path = writeTempFile(
        "[SABOTEUR]\n"
        "Name=Sneaky\n"
        "Width=32\n"
        "Height=24\n"
        "Frames=3\n"
        "Bitmap=ABC\n"
        "Top=ABCD\n"
        "Shadow=ABCDE\n"
        "Icon=ABCDEF\n"
        "DeathSound=ABCDEFG\n"
        "EatSound=ABCDEFGH\n"
        "Bullet=BULLET_SMALL\n"
        "ListType=LIST_PALACE\n"
        "SubListId=SUBLIST_HEAVYFCTRY\n"
        "HitPoints=60\n"
        "CanSquishInfantry=false\n"
        "DamageOnEnterStructure=9999.5\n"
        "IsInfantry=true\n");
    cUnitInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_UnitInfo& info = infos[SABOTEUR];
    CHECK(info.configured);
    CHECK(info.name == "Sneaky");
    CHECK(info.bmp_width == 32);
    CHECK(info.bmp_height == 24);
    CHECK(info.bmp_frames == 3);
    CHECK(info.bmp == reinterpret_cast<SDL_Surface*>(3));
    CHECK(info.top == reinterpret_cast<SDL_Surface*>(4));
    CHECK(info.shadow == reinterpret_cast<Texture*>(5));
    CHECK(info.icon == toTextureID(6));
    CHECK(info.deathSound == toAudioID(7));
    CHECK(info.eatSound == toAudioID(8));
    CHECK(info.bulletType == BULLET_SMALL);
    CHECK(info.listType == eListType::LIST_PALACE);
    CHECK(info.subListId == SUBLIST_HEAVYFCTRY);
    CHECK(info.hp == 60);
    CHECK_FALSE(info.squish);
    CHECK(info.damageOnEnterStructure == 9999.5f);
    CHECK(info.infantry);
}

TEST_CASE("UnitInfosLoader gives units without a section the defaults")
{
    const auto path = writeTempFile("[SABOTEUR]\nHitPoints=60\n");
    cUnitInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_UnitInfo& info = infos[TRIKE];
    CHECK_FALSE(info.configured);
    CHECK(info.bmp == reinterpret_cast<SDL_Surface*>(9)); // the quad bitmap
    CHECK(info.top == nullptr);
    CHECK(info.shadow == nullptr);
    CHECK(info.icon == TextureID::invalid);
    CHECK(info.deathSound == audioID::invalid);
    CHECK(info.eatSound == audioID::invalid);
    CHECK(info.hp == -1);
    CHECK(info.bulletType == -1);
    CHECK(info.sight == -1);
    CHECK(info.squish);
    CHECK(info.queuable);
    CHECK(info.dieWhenLowerThanHP == 1);
    CHECK(info.listType == eListType::LIST_NONE);
    CHECK(info.structureTypeItLeavesFrom == 0);
}

TEST_CASE("UnitInfosLoader keeps defaults and reports failure when the file is missing")
{
    cUnitInfos infos;

    CHECK_FALSE(makeLoader().load("this_file_does_not_exist_units.ini", infos));
    CHECK(infos[TRIKE].hp == -1);
}

TEST_CASE("UnitInfosLoader skips unknown sections and accepts a plain number as sublist")
{
    const auto path = writeTempFile("[NO_SUCH_UNIT]\nHitPoints=5\n[TRIKE]\nSubListId=2\nHitPoints=12\n");
    cUnitInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    CHECK(infos[QUAD].hp == -1);
    CHECK(infos[TRIKE].hp == 12);
    CHECK(infos[TRIKE].subListId == 2);
}

TEST_CASE("The shipped units.ini describes every unit")
{
    cUnitInfos infos;

    REQUIRE(makeLoader().load(std::string(D2TM_SOURCE_DIR) + "/resources/units.ini.org", infos));

    for (const int type : {SOLDIER, TROOPER, INFANTRY, TROOPERS, TRIKE, RAIDER, QUAD, TANK, SIEGETANK, LAUNCHER, DEVASTATOR, DEVIATOR,
                           SONICTANK, HARVESTER, MCV, CARRYALL, ORNITHOPTER, SANDWORM, SABOTEUR, UNIT_FREMEN_ONE, UNIT_FREMEN_THREE, FRIGATE}) {
        CHECK(infos[static_cast<std::size_t>(type)].configured);
        CHECK(infos[static_cast<std::size_t>(type)].bmp != nullptr);
    }
    CHECK(infos[TRIKE].deathSound != audioID::invalid);
    CHECK(infos[SOLDIER].canBeSquished);
    CHECK(infos[FRIGATE].airborn);
}
