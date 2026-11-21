#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>

#include "context/BulletInfosLoader.hpp"
#include "definitions.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "utils/ParticleNames.hpp"

namespace {

// Textures are only handed around as pointers here, so a number cast to a pointer is enough
Texture* fakeTexture(std::uintptr_t value)
{
    return reinterpret_cast<Texture*>(value);
}

BulletInfosLoader::TextureLookup textureLookup(std::set<std::string>& asked)
{
    return [&asked](std::string_view name) {
        asked.emplace(name);
        return fakeTexture(name.size());
    };
}

BulletInfosLoader::SoundLookup soundLookup(std::set<std::string>& asked)
{
    return [&asked](std::string_view name) {
        asked.emplace(name);
        return toAudioID(static_cast<int>(name.size()));
    };
}

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_bullets_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

} // namespace

TEST_CASE("BulletInfosLoader reads values, assets and the description of a bullet section")
{
    const auto path = writeTempFile(
        "[ROCKET_NORMAL]\n"
        "BitmapWidth=32\n"
        "DamageVehicle=76\n"
        "GroundBullet=false\n"
        "CanDamageGround=true\n"
        "Bitmap=ABC\n"
        "Sound=SOUND\n"
        "DeviateSound=DEVIATE\n"
        "DeathParticle=EXPLOSION_ROCKET\n"
        "SmokeParticle=BULLET_PUF\n");
    std::set<std::string> askedTextures;
    std::set<std::string> askedSounds;
    const BulletInfosLoader loader(textureLookup(askedTextures), soundLookup(askedSounds));
    cBulletInfos infos;

    REQUIRE(loader.load(path.string(), infos));
    std::filesystem::remove(path);

    const s_BulletInfo& info = infos[ROCKET_NORMAL];
    CHECK(info.description == "ROCKET_NORMAL");
    CHECK(info.bmp_width == 32);
    CHECK(info.damage_vehicles == 76);
    CHECK(info.canDamageGround);
    CHECK(info.bmp == fakeTexture(3));
    CHECK(info.sound == toAudioID(5));
    CHECK(info.deviateSound == toAudioID(7));
    CHECK(info.deathParticle == D2TM_PARTICLE_EXPLOSION_ROCKET);
    CHECK(info.smokeParticle == D2TM_PARTICLE_BULLET_PUF);
    CHECK(askedTextures == std::set<std::string>{"ABC"});
}

TEST_CASE("BulletInfosLoader gives bullets without a section the defaults")
{
    const auto path = writeTempFile("[ROCKET_NORMAL]\nDamageVehicle=76\n");
    std::set<std::string> askedTextures;
    std::set<std::string> askedSounds;
    const BulletInfosLoader loader(textureLookup(askedTextures), soundLookup(askedSounds));
    cBulletInfos infos;

    REQUIRE(loader.load(path.string(), infos));
    std::filesystem::remove(path);

    const s_BulletInfo& info = infos[BULLET_TANK];
    CHECK(info.description == "Unknown");
    CHECK(info.bmp == nullptr);
    CHECK(info.sound == audioID::invalid);
    CHECK(info.deathParticle == -1);
    CHECK(info.smokeParticle == -1);
    CHECK(info.moveSpeed == 2);
    CHECK(info.bmp_width == 16);
    CHECK(info.max_frames == 1);
    CHECK(info.max_deadframes == 4);
    CHECK(info.explosionSize == 1);
}

TEST_CASE("BulletInfosLoader keeps defaults and reports failure when the file is missing")
{
    std::set<std::string> askedTextures;
    std::set<std::string> askedSounds;
    const BulletInfosLoader loader(textureLookup(askedTextures), soundLookup(askedSounds));
    cBulletInfos infos;

    CHECK_FALSE(loader.load("this_file_does_not_exist_bullets.ini", infos));
    CHECK(infos[ROCKET_NORMAL].description == "Unknown");
}

TEST_CASE("BulletInfosLoader skips unknown sections and unknown particle names")
{
    const auto path = writeTempFile("[NO_SUCH_BULLET]\nDamageVehicle=5\n[BULLET_TANK]\nDeathParticle=NO_SUCH_PARTICLE\nDamageVehicle=12\n");
    std::set<std::string> askedTextures;
    std::set<std::string> askedSounds;
    const BulletInfosLoader loader(textureLookup(askedTextures), soundLookup(askedSounds));
    cBulletInfos infos;

    REQUIRE(loader.load(path.string(), infos));
    std::filesystem::remove(path);

    CHECK(infos[BULLET_TANK].damage_vehicles == 12);
    CHECK(infos[BULLET_TANK].deathParticle == -1);
}

TEST_CASE("The shipped bullets.ini describes every bullet")
{
    std::set<std::string> askedTextures;
    std::set<std::string> askedSounds;
    const BulletInfosLoader loader(textureLookup(askedTextures), soundLookup(askedSounds));
    cBulletInfos infos;

    REQUIRE(loader.load(std::string(D2TM_SOURCE_DIR) + "/resources/bullets.ini.org", infos));

    for (const int type : {ROCKET_SMALL, ROCKET_NORMAL, ROCKET_BIG, BULLET_SMALL, BULLET_TRIKE, BULLET_QUAD, BULLET_TANK, BULLET_SIEGE,
                           BULLET_DEVASTATOR, BULLET_GAS, BULLET_TURRET, BULLET_SHIMMER, ROCKET_SMALL_ORNI, ROCKET_RTURRET, ROCKET_SMALL_FREMEN}) {
        CHECK(infos[static_cast<std::size_t>(type)].description != "Unknown");
    }
    CHECK(infos[BULLET_SHIMMER].bmp == nullptr);
    CHECK(infos[ROCKET_BIG].bmp != nullptr);
    CHECK(infos[ROCKET_BIG].deathParticle == D2TM_PARTICLE_EXPLOSION_STRUCTURE01);
    CHECK(infos[BULLET_GAS].deviateProbability == 34);
}

TEST_CASE("particleIdFromName resolves names without the prefix")
{
    CHECK(particleIdFromName("BULLET_PUF") == D2TM_PARTICLE_BULLET_PUF);
    CHECK(particleIdFromName("EXPLOSION_CARRYALL") == D2TM_PARTICLE_EXPLOSION_CARRYALL);
    CHECK(particleIdFromName("NOPE") == -1);
    CHECK(particleIdFromName("") == -1);
}
