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
#include "gameobjects/map/TerrainTypes.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

// The code owns the names it asks for. These tests check the shipped packs: every entry resolves by its own
// name to its own index, and the terrain constants are the pack indexes of the terrain sprites.

namespace {

void checkPack(const std::string &packFile)
{
    const std::string packPath = std::string(D2TM_SOURCE_DIR) + "/resources/bin/data/" + packFile;
    INFO("graphics pack not found: " << packPath);
    REQUIRE(std::filesystem::exists(packPath));

    auto *fakeRenderer = reinterpret_cast<SDL_Renderer *>(1);
    Graphics graphics(fakeRenderer, packPath);

    const std::vector<std::string> names = graphics.getNames();
    REQUIRE(static_cast<std::size_t>(graphics.getNumberOfFiles()) == names.size());
    for (std::size_t i = 0; i < names.size(); i++) {
        INFO(packFile << " " << names[i]);
        const TextureID texture = graphics.getTextureID(names[i]);
        REQUIRE(texture != TextureID::invalid);
        REQUIRE(toPackIndex(texture) == static_cast<int>(i));
        REQUIRE(graphics.getSurfaceID(names[i]) == toSurfaceID(texture));
    }
}

void checkTerrain(Graphics &graphics, int terrain, std::string_view name)
{
    INFO(name);
    REQUIRE(toTextureID(terrain) == graphics.getTextureID(name));
}
}

TEST_CASE("every entry of sdl_data.dat resolves by its name", "[graphics]")
{
    checkPack("sdl_data.dat");
}

TEST_CASE("every entry of sdl_inter.dat resolves by its name", "[graphics]")
{
    checkPack("sdl_inter.dat");
}

TEST_CASE("every entry of sdl_mentat.dat resolves by its name", "[graphics]")
{
    checkPack("sdl_mentat.dat");
}

TEST_CASE("every entry of sdl_world.dat resolves by its name", "[graphics]")
{
    checkPack("sdl_world.dat");
}

TEST_CASE("terrain types are the pack indexes of the terrain sprites", "[graphics]")
{
    const std::string packPath = std::string(D2TM_SOURCE_DIR) + "/resources/bin/data/sdl_data.dat";
    REQUIRE(std::filesystem::exists(packPath));
    auto *fakeRenderer = reinterpret_cast<SDL_Renderer *>(1);
    Graphics graphics(fakeRenderer, packPath);
    checkTerrain(graphics, TERRAIN_BLOOM, "TERRAIN_BLOOM");
    checkTerrain(graphics, TERRAIN_HILL, "TERRAIN_HILL");
    checkTerrain(graphics, TERRAIN_MOUNTAIN, "TERRAIN_MOUNTAIN");
    checkTerrain(graphics, TERRAIN_ROCK, "TERRAIN_ROCK");
    checkTerrain(graphics, TERRAIN_SAND, "TERRAIN_SAND");
    checkTerrain(graphics, TERRAIN_SLAB, "TERRAIN_SLAB");
    checkTerrain(graphics, TERRAIN_SPICE, "TERRAIN_SPICE");
    checkTerrain(graphics, TERRAIN_SPICEHILL, "TERRAIN_SPICEHILL");
    checkTerrain(graphics, TERRAIN_WALL, "TERRAIN_WALL");
}
