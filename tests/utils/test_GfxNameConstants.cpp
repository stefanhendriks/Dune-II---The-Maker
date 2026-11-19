/**
 * @file test_GfxNameConstants.cpp
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
#include "utils/Graphics.hpp"
#include "utils/GraphicsID.hpp"
#include "data/GfxNames.hpp"
#include "gameobjects/map/TerrainTypes.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

// Every name of gfxnames is checked against the shipped pack: the name must resolve to an entry,
// the entry at that index must carry that name, and the pack has no entry without a name constant.

namespace {

void checkNames(const std::string &packFile, const std::vector<std::string_view> &table)
{
    const std::string packPath = std::string(D2TM_SOURCE_DIR) + "/resources/bin/data/" + packFile;
    INFO("graphics pack not found: " << packPath);
    REQUIRE(std::filesystem::exists(packPath));

    auto *fakeRenderer = reinterpret_cast<SDL_Renderer *>(1);
    Graphics graphics(fakeRenderer, packPath);

    REQUIRE(static_cast<std::size_t>(graphics.getNumberOfFiles()) == table.size());

    const std::vector<std::string> names = graphics.getNames();
    for (const std::string_view name : table) {
        INFO(packFile << " " << name);
        const TextureID texture = graphics.getTextureID(name);
        REQUIRE(texture != TextureID::invalid);
        const int index = toPackIndex(texture);
        REQUIRE(static_cast<std::size_t>(index) < names.size());
        REQUIRE(names[index] == name);
        REQUIRE(graphics.getSurfaceID(name) == toSurfaceID(texture));
    }
}

// the map drawer turns a terrain type into a TextureID, so the terrain constants must stay in line with the pack
void checkTerrain(Graphics &graphics, int terrain, std::string_view name)
{
    INFO(name);
    REQUIRE(toTextureID(terrain) == graphics.getTextureID(name));
}
}

TEST_CASE("gfxdata names match the entries of sdl_data.dat", "[graphics]")
{
    const std::vector<std::string_view> table = {
        gfxnames::ATTACK_INDICATOR,
        gfxnames::BUILD_BARRACKS,
        gfxnames::BUILD_BARRACKS_SHADOW,
        gfxnames::BUILD_CONSTYARD,
        gfxnames::BUILD_CONSTYARD_SHADOW,
        gfxnames::BUILD_HEAVYFACTORY,
        gfxnames::BUILD_HEAVYFACTORY_FLASH,
        gfxnames::BUILD_HEAVYFACTORY_SHADOW,
        gfxnames::BUILD_HIGHTECH,
        gfxnames::BUILD_HIGHTECH_SHADOW,
        gfxnames::BUILD_IX,
        gfxnames::BUILD_IX_SHADOW,
        gfxnames::BUILD_LIGHTFACTORY,
        gfxnames::BUILD_LIGHTFACTORY_FLASH,
        gfxnames::BUILD_LIGHTFACTORY_SHADOW,
        gfxnames::BUILD_PALACE,
        gfxnames::BUILD_PALACE_SHADOW,
        gfxnames::BUILD_PRE_1X1,
        gfxnames::BUILD_PRE_1X1_SHADOW,
        gfxnames::BUILD_PRE_2X2,
        gfxnames::BUILD_PRE_2X2_SHADOW,
        gfxnames::BUILD_PRE_3X2,
        gfxnames::BUILD_PRE_3X2_SHADOW,
        gfxnames::BUILD_PRE_3X3,
        gfxnames::BUILD_PRE_3X3_SHADOW,
        gfxnames::BUILD_PRE_CONST,
        gfxnames::BUILD_RADAR,
        gfxnames::BUILD_RADAR_SHADOW,
        gfxnames::BUILD_REFINERY,
        gfxnames::BUILD_REFINERY_SHADOW,
        gfxnames::BUILD_REPAIR,
        gfxnames::BUILD_REPAIR_SHADOW,
        gfxnames::BUILD_RTURRET,
        gfxnames::BUILD_RTURRET_SHADOW,
        gfxnames::BUILD_SILO,
        gfxnames::BUILD_SILO_SHADOW,
        gfxnames::BUILD_STARPORT,
        gfxnames::BUILD_STARPORT_SHADOW,
        gfxnames::BUILD_TURRET,
        gfxnames::BUILD_TURRET_SHADOW,
        gfxnames::BUILD_WINDTRAP,
        gfxnames::BUILD_WINDTRAP_SHADOW,
        gfxnames::BUILD_WOR,
        gfxnames::BUILD_WOR_SHADOW,
        gfxnames::BUILDING_FLAG_LARGE,
        gfxnames::BUILDING_FLAG_SMALL,
        gfxnames::BULLET_DOT_LARGE,
        gfxnames::BULLET_DOT_MEDIUM,
        gfxnames::BULLET_DOT_SMALL,
        gfxnames::BULLET_PUF,
        gfxnames::BULLET_ROCKET_LARGE,
        gfxnames::BULLET_ROCKET_NORMAL,
        gfxnames::BULLET_ROCKET_SMALL,
        gfxnames::CREDITS_0,
        gfxnames::CREDITS_1,
        gfxnames::CREDITS_2,
        gfxnames::CREDITS_3,
        gfxnames::CREDITS_4,
        gfxnames::CREDITS_5,
        gfxnames::CREDITS_6,
        gfxnames::CREDITS_7,
        gfxnames::CREDITS_8,
        gfxnames::CREDITS_9,
        gfxnames::CREDITS_NONE,
        gfxnames::EXPLOSION_BULLET,
        gfxnames::EXPLOSION_FIRE,
        gfxnames::EXPLOSION_GAS,
        gfxnames::EXPLOSION_ORNI,
        gfxnames::EXPLOSION_ROCKET,
        gfxnames::EXPLOSION_ROCKET_SMALL,
        gfxnames::EXPLOSION_SQUISH01,
        gfxnames::EXPLOSION_SQUISH02,
        gfxnames::EXPLOSION_SQUISH03,
        gfxnames::EXPLOSION_STRUCTURE01,
        gfxnames::EXPLOSION_STRUCTURE02,
        gfxnames::EXPLOSION_TANK_ONE,
        gfxnames::EXPLOSION_TANK_TWO,
        gfxnames::EXPLOSION_TRIKE,
        gfxnames::FOCUS,
        gfxnames::MOUSE_ATTACK,
        gfxnames::MOUSE_DOWN,
        gfxnames::MOUSE_FORBIDDEN,
        gfxnames::MOUSE_LEFT,
        gfxnames::MOUSE_MOVE,
        gfxnames::MOUSE_NORMAL,
        gfxnames::MOUSE_PICK,
        gfxnames::MOUSE_RALLY,
        gfxnames::MOUSE_REPAIR,
        gfxnames::MOUSE_RIGHT,
        gfxnames::MOUSE_UP,
        gfxnames::MOVE_INDICATOR,
        gfxnames::OBJECT_BOOM01,
        gfxnames::OBJECT_BOOM02,
        gfxnames::OBJECT_BOOM03,
        gfxnames::OBJECT_BOOM04,
        gfxnames::OBJECT_CARRYPUFF,
        gfxnames::OBJECT_DEADINF01,
        gfxnames::OBJECT_DEADINF02,
        gfxnames::OBJECT_SIEGEDIE,
        gfxnames::OBJECT_SIEGESHOOT,
        gfxnames::OBJECT_SMOKE,
        gfxnames::OBJECT_SMOKE_SHADOW,
        gfxnames::OBJECT_STAR_01,
        gfxnames::OBJECT_STAR_02,
        gfxnames::OBJECT_STAR_03,
        gfxnames::OBJECT_TANKSHOOT,
        gfxnames::OBJECT_WORMEAT,
        gfxnames::OBJECT_WORMTRAIL,
        gfxnames::PALETTE_D2TM,
        gfxnames::PLACE_SLAB1,
        gfxnames::PLACE_SLAB4,
        gfxnames::PLACE_WALL,
        gfxnames::SHOOT_TANK,
        gfxnames::SHROUD,
        gfxnames::SHROUD_SHADOW,
        gfxnames::SMUDGE,
        gfxnames::SYMB_PICKMEUP,
        gfxnames::TERRAIN_BLOOM,
        gfxnames::TERRAIN_HILL,
        gfxnames::TERRAIN_MOUNTAIN,
        gfxnames::TERRAIN_ROCK,
        gfxnames::TERRAIN_SAND,
        gfxnames::TERRAIN_SLAB,
        gfxnames::TERRAIN_SPICE,
        gfxnames::TERRAIN_SPICEHILL,
        gfxnames::TERRAIN_WALL,
        gfxnames::TRACK_DIA,
        gfxnames::TRACK_DIA2,
        gfxnames::TRACK_HOR,
        gfxnames::TRACK_VER,
        gfxnames::UNIT_CARRYALL,
        gfxnames::UNIT_CARRYALL_SHADOW,
        gfxnames::UNIT_DEVASTATOR,
        gfxnames::UNIT_DEVASTATOR_SHADOW,
        gfxnames::UNIT_DEVIATOR,
        gfxnames::UNIT_FRIGATE,
        gfxnames::UNIT_FRIGATE_SHADOW,
        gfxnames::UNIT_HARVESTER,
        gfxnames::UNIT_HARVESTER_SHADOW,
        gfxnames::UNIT_LAUNCHER,
        gfxnames::UNIT_LAUNCHER_SHADOW,
        gfxnames::UNIT_MCV,
        gfxnames::UNIT_MCV_SHADOW,
        gfxnames::UNIT_ORNITHOPTER,
        gfxnames::UNIT_ORNITHOPTER_SHADOW,
        gfxnames::UNIT_QUAD,
        gfxnames::UNIT_QUAD_SHADOW,
        gfxnames::UNIT_RAIDER,
        gfxnames::UNIT_RAIDER_SHADOW,
        gfxnames::UNIT_SABOTEUR,
        gfxnames::UNIT_SANDWORM,
        gfxnames::UNIT_SIEGEBASE,
        gfxnames::UNIT_SIEGEBASE_SHADOW,
        gfxnames::UNIT_SIEGETOP,
        gfxnames::UNIT_SINGLEFREMEN,
        gfxnames::UNIT_SOLDIER,
        gfxnames::UNIT_SOLDIERS,
        gfxnames::UNIT_SONICTANK,
        gfxnames::UNIT_SONICTANK_SHADOW,
        gfxnames::UNIT_TANKBASE,
        gfxnames::UNIT_TANKBASE_SHADOW,
        gfxnames::UNIT_TANKTOP,
        gfxnames::UNIT_TRIKE,
        gfxnames::UNIT_TRIKE_SHADOW,
        gfxnames::UNIT_TRIPLEFREMEN,
        gfxnames::UNIT_TROOPER,
        gfxnames::UNIT_TROOPERS,
        gfxnames::EXPLOSION_CARRYALL,
    };
    checkNames("sdl_data.dat", table);
}

TEST_CASE("gfxinter names match the entries of sdl_inter.dat", "[graphics]")
{
    const std::vector<std::string_view> table = {
        gfxnames::BMP_D2TM,
        gfxnames::BMP_GAME_DUNE,
        gfxnames::BMP_GENERATING,
        gfxnames::BMP_GERALD_CANDYBAR_BALL,
        gfxnames::BMP_GERALD_CANDYBAR_BOTTOM,
        gfxnames::BMP_GERALD_CANDYBAR_PIECE,
        gfxnames::BMP_GERALD_CANDYBAR_TOP,
        gfxnames::BMP_GERALD_ICONLIST_BACKGROUND,
        gfxnames::BMP_GERALD_SIDEBAR_PIECE,
        gfxnames::BMP_GERALD_TOP_BAR,
        gfxnames::BMP_LOSING,
        gfxnames::BMP_SELECT_HOUSE_ATREIDES,
        gfxnames::BMP_SELECT_HOUSE_HARKONNEN,
        gfxnames::BMP_SELECT_HOUSE_ORDOS,
        gfxnames::BMP_SELECT_YOUR_HOUSE,
        gfxnames::BMP_TOPBAR_BACKGROUND,
        gfxnames::BMP_UNKNOWNMAP,
        gfxnames::BMP_WINNING,
        gfxnames::BTN_OPTIONS,
        gfxnames::BTN_ORDER,
        gfxnames::CREDITS_BAR,
        gfxnames::CREDITS_DUNE2_CORE_TEAM,
        gfxnames::D2TM_TITLE,
        gfxnames::GRID_0X0,
        gfxnames::GRID_1X1,
        gfxnames::GRID_2X2,
        gfxnames::GRID_3X2,
        gfxnames::GRID_3X3,
        gfxnames::HORIZONTAL_CANDYBAR,
        gfxnames::ICON_SPECIAL_FREMEN,
        gfxnames::ICON_SPECIAL_MISSILE,
        gfxnames::ICON_SPECIAL_SABOTEUR,
        gfxnames::ICON_STR_1SLAB,
        gfxnames::ICON_STR_4SLAB,
        gfxnames::ICON_STR_BARRACKS,
        gfxnames::ICON_STR_CONSTYARD,
        gfxnames::ICON_STR_HEAVYFACTORY,
        gfxnames::ICON_STR_HIGHTECH,
        gfxnames::ICON_STR_IX,
        gfxnames::ICON_STR_LIGHTFACTORY,
        gfxnames::ICON_STR_PALACE,
        gfxnames::ICON_STR_RADAR,
        gfxnames::ICON_STR_REFINERY,
        gfxnames::ICON_STR_REPAIR,
        gfxnames::ICON_STR_RTURRET,
        gfxnames::ICON_STR_SILO,
        gfxnames::ICON_STR_STARPORT,
        gfxnames::ICON_STR_TURRET,
        gfxnames::ICON_STR_WALL,
        gfxnames::ICON_STR_WINDTRAP,
        gfxnames::ICON_STR_WOR,
        gfxnames::ICON_UNIT_CARRYALL,
        gfxnames::ICON_UNIT_DEVASTATOR,
        gfxnames::ICON_UNIT_DEVIATOR,
        gfxnames::ICON_UNIT_FREMEN,
        gfxnames::ICON_UNIT_HARVESTER,
        gfxnames::ICON_UNIT_INFANTRY,
        gfxnames::ICON_UNIT_LAUNCHER,
        gfxnames::ICON_UNIT_MCV,
        gfxnames::ICON_UNIT_ORNITHOPTER,
        gfxnames::ICON_UNIT_QUAD,
        gfxnames::ICON_UNIT_RAIDER,
        gfxnames::ICON_UNIT_SABOTEUR,
        gfxnames::ICON_UNIT_SANDWORM,
        gfxnames::ICON_UNIT_SARDAUKAR,
        gfxnames::ICON_UNIT_SIEGETANK,
        gfxnames::ICON_UNIT_SOLDIER,
        gfxnames::ICON_UNIT_SONICTANK,
        gfxnames::ICON_UNIT_TANK,
        gfxnames::ICON_UNIT_TRIKE,
        gfxnames::ICON_UNIT_TROOPER,
        gfxnames::ICON_UNIT_TROOPERS,
        gfxnames::LIST_BTN_CONSTYARD,
        gfxnames::LIST_BTN_FACTORY,
        gfxnames::LIST_BTN_INFANTRY,
        gfxnames::LIST_BTN_PALACE,
        gfxnames::LIST_BTN_STARPORT,
        gfxnames::LIST_BTN_UPGRADE,
        gfxnames::MESSAGE_LEFT,
        gfxnames::MESSAGE_MIDDLE,
        gfxnames::MESSAGE_RIGHT,
        gfxnames::PAL_INTER,
        gfxnames::PROGRESS001,
        gfxnames::PROGRESS002,
        gfxnames::PROGRESS003,
        gfxnames::PROGRESS004,
        gfxnames::PROGRESS005,
        gfxnames::PROGRESS006,
        gfxnames::PROGRESS007,
        gfxnames::PROGRESS008,
        gfxnames::PROGRESS009,
        gfxnames::PROGRESS010,
        gfxnames::PROGRESS011,
        gfxnames::PROGRESS012,
        gfxnames::PROGRESS013,
        gfxnames::PROGRESS014,
        gfxnames::PROGRESS015,
        gfxnames::PROGRESS016,
        gfxnames::PROGRESS017,
        gfxnames::PROGRESS018,
        gfxnames::PROGRESS019,
        gfxnames::PROGRESS020,
        gfxnames::PROGRESS021,
        gfxnames::PROGRESS022,
        gfxnames::PROGRESS023,
        gfxnames::PROGRESS024,
        gfxnames::PROGRESS025,
        gfxnames::PROGRESS026,
        gfxnames::PROGRESS027,
        gfxnames::PROGRESS028,
        gfxnames::PROGRESS029,
        gfxnames::PROGRESS030,
        gfxnames::PROGRESS031,
        gfxnames::PROGRESS032,
        gfxnames::PROGRESSFIX,
        gfxnames::PROGRESSNA,
        gfxnames::READY01,
        gfxnames::READY02,
        gfxnames::STAT01,
        gfxnames::STAT02,
        gfxnames::STAT03,
        gfxnames::STAT04,
        gfxnames::STAT05,
        gfxnames::STAT06,
        gfxnames::STAT07,
        gfxnames::STAT08,
        gfxnames::STAT09,
        gfxnames::STAT10,
        gfxnames::STAT11,
        gfxnames::STAT12,
        gfxnames::STAT13,
        gfxnames::STAT14,
        gfxnames::STAT15,
        gfxnames::STAT16,
        gfxnames::STAT17,
        gfxnames::STAT18,
        gfxnames::STAT19,
        gfxnames::STAT20,
        gfxnames::STAT21,
        gfxnames::ICON_POWER,
        gfxnames::ICON_POWER_HIGH,
        gfxnames::ICON_SOLARIS_LOW,
        gfxnames::ICON_SOLARIS,
        gfxnames::ICON_SOLARIS_FULL,
        gfxnames::BMP_SELECT_HOUSE_SARDAUKAR,
        gfxnames::ICON_POWER_LOW,
    };
    checkNames("sdl_inter.dat", table);
}

TEST_CASE("gfxmentat names match the entries of sdl_mentat.dat", "[graphics]")
{
    const std::vector<std::string_view> table = {
        gfxnames::ATR_EYES01,
        gfxnames::ATR_EYES02,
        gfxnames::ATR_EYES03,
        gfxnames::ATR_EYES04,
        gfxnames::ATR_EYES05,
        gfxnames::ATR_MOUTH01,
        gfxnames::ATR_MOUTH02,
        gfxnames::ATR_MOUTH03,
        gfxnames::ATR_MOUTH04,
        gfxnames::ATR_MOUTH05,
        gfxnames::BEN_EYES01,
        gfxnames::BEN_EYES02,
        gfxnames::BEN_EYES03,
        gfxnames::BEN_EYES04,
        gfxnames::BEN_EYES05,
        gfxnames::BEN_MOUTH01,
        gfxnames::BEN_MOUTH02,
        gfxnames::BEN_MOUTH03,
        gfxnames::BEN_MOUTH04,
        gfxnames::BEN_MOUTH05,
        gfxnames::BTN_NO,
        gfxnames::BTN_PROCEED,
        gfxnames::BTN_REPEAT,
        gfxnames::BTN_YES,
        gfxnames::HAR_EYES01,
        gfxnames::HAR_EYES02,
        gfxnames::HAR_EYES03,
        gfxnames::HAR_EYES04,
        gfxnames::HAR_EYES05,
        gfxnames::HAR_MOUTH01,
        gfxnames::HAR_MOUTH02,
        gfxnames::HAR_MOUTH03,
        gfxnames::HAR_MOUTH04,
        gfxnames::HAR_MOUTH05,
        gfxnames::HEN_HARKONNEN,
        gfxnames::MEN_WISH,
        gfxnames::MENTAT,
        gfxnames::MENTATA,
        gfxnames::MENTATH,
        gfxnames::MENTATM,
        gfxnames::MENTATO,
        gfxnames::ORD_EYES01,
        gfxnames::ORD_EYES02,
        gfxnames::ORD_EYES03,
        gfxnames::ORD_EYES04,
        gfxnames::ORD_EYES05,
        gfxnames::ORD_MOUTH01,
        gfxnames::ORD_MOUTH02,
        gfxnames::ORD_MOUTH03,
        gfxnames::ORD_MOUTH04,
        gfxnames::ORD_MOUTH05,
        gfxnames::MENTATS,
    };
    checkNames("sdl_mentat.dat", table);
}

TEST_CASE("gfxworld names match the entries of sdl_world.dat", "[graphics]")
{
    const std::vector<std::string_view> table = {
        gfxnames::BMP_NCATR,
        gfxnames::BMP_NCHAR,
        gfxnames::BMP_NCORD,
        gfxnames::BMP_NEXTCONQ,
        gfxnames::CONQUEST,
        gfxnames::PALETTE_WORLD,
        gfxnames::PIECE_DUNE_001,
        gfxnames::PIECE_DUNE_002,
        gfxnames::PIECE_DUNE_003,
        gfxnames::PIECE_DUNE_004,
        gfxnames::PIECE_DUNE_005,
        gfxnames::PIECE_DUNE_006,
        gfxnames::PIECE_DUNE_007,
        gfxnames::PIECE_DUNE_008,
        gfxnames::PIECE_DUNE_009,
        gfxnames::PIECE_DUNE_010,
        gfxnames::PIECE_DUNE_011,
        gfxnames::PIECE_DUNE_012,
        gfxnames::PIECE_DUNE_013,
        gfxnames::PIECE_DUNE_014,
        gfxnames::PIECE_DUNE_015,
        gfxnames::PIECE_DUNE_016,
        gfxnames::PIECE_DUNE_017,
        gfxnames::PIECE_DUNE_018,
        gfxnames::PIECE_DUNE_019,
        gfxnames::PIECE_DUNE_020,
        gfxnames::PIECE_DUNE_021,
        gfxnames::PIECE_DUNE_022,
        gfxnames::PIECE_DUNE_023,
        gfxnames::PIECE_DUNE_024,
        gfxnames::PIECE_DUNE_025,
        gfxnames::PIECE_DUNE_026,
        gfxnames::PIECE_DUNE_027,
        gfxnames::WORLD_DUNE,
        gfxnames::WORLD_DUNE_CLICK,
        gfxnames::WORLD_DUNE_REGIONS,
    };
    checkNames("sdl_world.dat", table);
}

TEST_CASE("terrain types are the pack indexes of the terrain sprites", "[graphics]")
{
    const std::string packPath = std::string(D2TM_SOURCE_DIR) + "/resources/bin/data/sdl_data.dat";
    REQUIRE(std::filesystem::exists(packPath));
    auto *fakeRenderer = reinterpret_cast<SDL_Renderer *>(1);
    Graphics graphics(fakeRenderer, packPath);
    checkTerrain(graphics, TERRAIN_BLOOM, gfxnames::TERRAIN_BLOOM);
    checkTerrain(graphics, TERRAIN_HILL, gfxnames::TERRAIN_HILL);
    checkTerrain(graphics, TERRAIN_MOUNTAIN, gfxnames::TERRAIN_MOUNTAIN);
    checkTerrain(graphics, TERRAIN_ROCK, gfxnames::TERRAIN_ROCK);
    checkTerrain(graphics, TERRAIN_SAND, gfxnames::TERRAIN_SAND);
    checkTerrain(graphics, TERRAIN_SLAB, gfxnames::TERRAIN_SLAB);
    checkTerrain(graphics, TERRAIN_SPICE, gfxnames::TERRAIN_SPICE);
    checkTerrain(graphics, TERRAIN_SPICEHILL, gfxnames::TERRAIN_SPICEHILL);
    checkTerrain(graphics, TERRAIN_WALL, gfxnames::TERRAIN_WALL);
}
