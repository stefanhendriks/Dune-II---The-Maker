#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

#include "context/StructureInfosLoader.hpp"
#include "definitions.h"
#include "gameobjects/structures/cStructureInfo.h"

namespace {

// Assets are only handed around as pointers/ids here, so the length of the name is enough to tell them apart
StructureInfosLoader makeLoader()
{
    return StructureInfosLoader(
        [](std::string_view name) { return reinterpret_cast<SDL_Surface*>(name.size()); },
        [](std::string_view name) { return reinterpret_cast<Texture*>(name.size()); },
        [](std::string_view name) { return toTextureID(static_cast<int>(name.size())); });
}

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_structures_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

} // namespace

TEST_CASE("StructureInfosLoader reads values, assets and flags of a structure section")
{
    const auto path = writeTempFile(
        "[RADAR]\n"
        "Name=Outpost\n"
        "Width=64\n"
        "Height=96\n"
        "Bitmap=ABC\n"
        "Shadow=ABCD\n"
        "Flash=ABCDE\n"
        "Icon=ABCDEF\n"
        "Flag=big,17,38\n"
        "Flag2=small,12,46\n"
        "Queuable=true\n"
        "HitPoints=900\n"
        "Sight=12\n"
        "UponDestructionSpawnUnitType=SOLDIER\n");
    cStructureInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_StructureInfo& info = infos[RADAR];
    CHECK(info.name == "Outpost");
    CHECK(info.configured);
    CHECK(info.bmp_width == 64);
    CHECK(info.bmp_height == 96);
    CHECK(info.bmp == reinterpret_cast<SDL_Surface*>(3));
    CHECK(info.shadow == reinterpret_cast<Texture*>(4));
    CHECK(info.flash == reinterpret_cast<SDL_Surface*>(5));
    CHECK(info.icon == toTextureID(6));
    CHECK(info.queuable);
    CHECK(info.hp == 900);
    CHECK(info.sight == 12);
    CHECK(info.uponDestructionSpawnUnitType == SOLDIER);
    REQUIRE(info.flags.size() == 2);
    CHECK(info.flags[0].big);
    CHECK(info.flags[0].relX == 17);
    CHECK(info.flags[0].relY == 38);
    CHECK_FALSE(info.flags[1].big);
    CHECK(info.flags[1].relX == 12);
    CHECK(info.flags[1].relY == 46);
}

TEST_CASE("StructureInfosLoader gives structures without a section the defaults")
{
    const auto path = writeTempFile("[RADAR]\nHitPoints=900\n");
    cStructureInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_StructureInfo& info = infos[WOR];
    CHECK(info.name == "Unknown");
    CHECK_FALSE(info.configured);
    CHECK(info.bmp == reinterpret_cast<SDL_Surface*>(14)); // the windtrap bitmap
    CHECK(info.shadow == nullptr);
    CHECK(info.flash == nullptr);
    CHECK(info.icon == TextureID::invalid);
    CHECK(info.flags.empty());
    CHECK(info.hp == 1);
    CHECK(info.fixhp == -1);
    CHECK(info.sight == 1);
    CHECK(info.bmp_width == 64);
    CHECK(info.hasConcrete);
    CHECK(info.list == -1);
    CHECK(info.uponDestructionSpawnUnitType == -1);
}

TEST_CASE("StructureInfosLoader keeps defaults and reports failure when the file is missing")
{
    cStructureInfos infos;

    CHECK_FALSE(makeLoader().load("this_file_does_not_exist_structures.ini", infos));
    CHECK(infos[RADAR].name == "Unknown");
}

TEST_CASE("StructureInfosLoader skips unknown sections and malformed flags")
{
    const auto path = writeTempFile("[NO_SUCH_STRUCTURE]\nHitPoints=5\n[WOR]\nFlag=big,1\nHitPoints=12\n");
    cStructureInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    CHECK(infos[PALACE].hp == 1);
    CHECK(infos[WOR].hp == 12);
    CHECK(infos[WOR].flags.empty());
}

TEST_CASE("The shipped structures.ini describes every structure")
{
    cStructureInfos infos;

    REQUIRE(makeLoader().load(std::string(D2TM_SOURCE_DIR) + "/resources/structures.ini.org", infos));

    for (const int type : {PALACE, WINDTRAP, HEAVYFACTORY, LIGHTFACTORY, CONSTYARD, SILO, HIGHTECH, IX, REPAIR, RADAR, REFINERY, WOR, BARRACKS,
                           STARPORT, TURRET, RTURRET, SLAB1, SLAB4, WALL}) {
        CHECK(infos[static_cast<std::size_t>(type)].configured);
        CHECK(infos[static_cast<std::size_t>(type)].icon != TextureID::invalid);
    }
    CHECK(infos[BARRACKS].flags.size() == 2);
    CHECK(infos[HEAVYFACTORY].flash != nullptr);
    CHECK(infos[SLAB1].hp == 25);
    CHECK(infos[WALL].hp == 200);
}
