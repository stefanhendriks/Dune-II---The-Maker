/**
 * @file test_ParticleInfosLoader.cpp
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

#include "context/ParticleInfosLoader.hpp"
#include "definitions.h"
#include "gameobjects/particles/cParticleInfos.h"

namespace {

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_particles_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

// "BITMAP_7" resolves to texture 7, anything else is unknown
ParticleInfosLoader makeLoader()
{
    return ParticleInfosLoader([](std::string_view name) {
        return name == "BITMAP_7" ? toTextureID(7) : TextureID::invalid;
    });
}

} // namespace

TEST_CASE("ParticleInfosLoader reads the values of a particle section")
{
    const auto path = writeTempFile(
        "[OBJECT_BOOM01]\nBitmap=BITMAP_7\nStartAlpha=200\nAdditiveBlending=true\nLayer=BOTTOM\nFrameWidth=64\nFrameHeight=48\n");
    cParticleInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_ParticleInfo& info = infos[D2TM_PARTICLE_OBJECT_BOOM01];
    CHECK(info.bmpIndex == toTextureID(7));
    CHECK(info.startAlpha == 200);
    CHECK(info.usesAdditiveBlending);
    CHECK(info.layer == RenderLayerParticle::BOTTOM);
    CHECK(info.frameWidth == 64);
    CHECK(info.frameHeight == 48);
}

TEST_CASE("ParticleInfosLoader keeps the defaults for absent keys and for particles without a section")
{
    const auto path = writeTempFile("[MOVE]\nBitmap=BITMAP_7\n");
    cParticleInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_ParticleInfo& move = infos[D2TM_PARTICLE_MOVE];
    CHECK(move.bmpIndex == toTextureID(7));
    CHECK(move.startAlpha == -1);
    CHECK_FALSE(move.usesAdditiveBlending);
    CHECK(move.layer == RenderLayerParticle::TOP);
    CHECK(move.frameWidth == 32);
    CHECK(move.frameHeight == 32);

    const s_ParticleInfo& other = infos[D2TM_PARTICLE_ATTACK];
    CHECK(other.bmpIndex == TextureID::invalid);
    CHECK(other.frameWidth == 32);
}

TEST_CASE("ParticleInfosLoader skips unknown sections and a missing file")
{
    const auto path = writeTempFile("[NO_SUCH_PARTICLE]\nStartAlpha=1\n[MOVE]\nStartAlpha=99\n");
    cParticleInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);
    CHECK(infos[D2TM_PARTICLE_MOVE].startAlpha == 99);

    cParticleInfos missing;
    CHECK_FALSE(makeLoader().load("does/not/exist.ini", missing));
    CHECK(missing[D2TM_PARTICLE_MOVE].frameWidth == 32);
}

TEST_CASE("ParticleInfosLoader reads the spawn values of a particle")
{
    const auto path = writeTempFile(
        "[SIEGEDIE]\nFrameTimer=500\nFrameTimerRandom=300\nDeadTimer=40\nDeadTimerRandom=7\nStartAlphaRandom=64\nAnchorBottom=true\n"
        "Spawn1=EXPLOSION_FIRE\nSpawn1OffsetY=-18\nSpawn2=OBJECT_BOOM03\nSpawn2OffsetX=4\nSpawn2InheritHouse=true\nSpawn3=NO_SUCH_PARTICLE\n");
    cParticleInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_ParticleInfo& info = infos[D2TM_PARTICLE_SIEGEDIE];
    CHECK(info.spawnFrameTimer == 500);
    CHECK(info.spawnFrameTimerRandom == 300);
    CHECK(info.spawnDeadTimer == 40);
    CHECK(info.spawnDeadTimerRandom == 7);
    CHECK(info.startAlphaRandom == 64);
    CHECK(info.anchorBottom);
    CHECK(info.spawns[0].type == D2TM_PARTICLE_EXPLOSION_FIRE);
    CHECK(info.spawns[0].offsetX == 0);
    CHECK(info.spawns[0].offsetY == -18);
    CHECK_FALSE(info.spawns[0].inheritHouse);
    CHECK(info.spawns[1].type == D2TM_PARTICLE_OBJECT_BOOM03);
    CHECK(info.spawns[1].offsetX == 4);
    CHECK(info.spawns[1].inheritHouse);
    CHECK(info.spawns[2].type == -1);
}

TEST_CASE("ParticleInfosLoader spawn values default to a plain particle")
{
    const auto path = writeTempFile("[MOVE]\nStartAlpha=99\n");
    cParticleInfos infos;

    REQUIRE(makeLoader().load(path.string(), infos));
    std::filesystem::remove(path);

    const s_ParticleInfo& info = infos[D2TM_PARTICLE_MOVE];
    CHECK(info.spawnFrameTimer == 10);
    CHECK(info.spawnFrameTimerRandom == 0);
    CHECK(info.spawnDeadTimer == 0);
    CHECK(info.spawnDeadTimerRandom == 0);
    CHECK(info.startAlphaRandom == 0);
    CHECK_FALSE(info.anchorBottom);
    for (const auto& spawn : info.spawns) {
        CHECK(spawn.type == -1);
    }
}
