/**
 * @file BulletInfosLoader.cpp
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

#include "context/BulletInfosLoader.hpp"

#include <utility>

#include "definitions.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "utils/IniParsing.hpp"
#include "utils/Log.h"
#include "utils/ParticleNames.hpp"
#include "utils/cIniFile.h"
#include "utils/cIniUtils.h"

namespace {

// What a bullet gets when bullets.ini does not say otherwise
void setDefaults(s_BulletInfo& info)
{
    info.bmp = nullptr;
    info.moveSpeed = 2;
    info.deathParticle = -1; // this points to a particle type
    info.damage_vehicles = 0;
    info.damage_infantry = 0;
    info.max_frames = 1; // 1 frame animation
    info.max_deadframes = 4; // 4 frame animation
    info.smokeParticle = -1; // by default no smoke particle is spawned
    info.bmp_width = 8 * 2;
    info.sound = audioID::invalid; // no sound
    info.deviateSound = audioID::invalid;
    info.explosionSize = 1; // 1 tile sized explosion
    info.deviateProbability = 0;
    info.groundBullet = false;
    info.canDamageAirUnits = false;
    info.canDamageGround = false;
    info.description = "Unknown";
}

void readInt(const cSection& section, const char* key, int& target)
{
    if (section.hasValue(key)) target = ToInt(section.getStringValue(key));
}

void readBool(const cSection& section, const char* key, bool& target)
{
    if (section.hasValue(key)) target = ToBool(section.getStringValue(key));
}

} // namespace

BulletInfosLoader::BulletInfosLoader(TextureLookup textureLookup, SoundLookup soundLookup)
    : m_textureLookup(std::move(textureLookup)), m_soundLookup(std::move(soundLookup))
{
}

bool BulletInfosLoader::load(const std::string& filename, cBulletInfos& bulletInfos) const
{
    for (auto& info : bulletInfos) {
        setDefaults(info);
    }

    Logger::info(COMP_GAMERULES, "BulletInfosLoader::load", "Opening bullet rules from : {}", filename);
    cIniFile file;
    if (!file.load(filename)) {
        Logger::error(COMP_GAMERULES, "BulletInfosLoader::load", "Could not open file [{}], no bullets are available", filename);
        return false;
    }

    const int bulletInfoCount = static_cast<int>(bulletInfos.size());
    for (const auto& sectionName : file.getSectionNames()) {
        const int id = cIniUtils::getBulletTypeFromString(sectionName);
        if (id < 0 || id >= bulletInfoCount) {
            Logger::error(COMP_GAMERULES, "BulletInfosLoader::load", "[{}] Unknown bullet section [{}], skipping it (expected e.g. [ROCKET_NORMAL])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_BulletInfo& info = bulletInfos[static_cast<std::size_t>(id)];
        info.description = sectionName;
        readInt(section, "DamageVehicle", info.damage_vehicles);
        readInt(section, "DamageInfantry", info.damage_infantry);
        readInt(section, "DeviateProbability", info.deviateProbability);
        readInt(section, "ExplosionSize", info.explosionSize);
        readBool(section, "GroundBullet", info.groundBullet);
        readInt(section, "MoveSpeed", info.moveSpeed);
        readInt(section, "BitmapWidth", info.bmp_width);
        readInt(section, "BitmapFrames", info.max_frames);
        readInt(section, "BitmapExplFrames", info.max_deadframes);
        readBool(section, "CanDamageAirUnits", info.canDamageAirUnits);
        readBool(section, "CanDamageGround", info.canDamageGround);

        if (section.hasValue("Bitmap")) {
            info.bmp = m_textureLookup(section.getStringValue("Bitmap"));
        }
        if (section.hasValue("Sound")) {
            info.sound = m_soundLookup(section.getStringValue("Sound"));
        }
        if (section.hasValue("DeviateSound")) {
            info.deviateSound = m_soundLookup(section.getStringValue("DeviateSound"));
        }
        if (section.hasValue("DeathParticle")) {
            info.deathParticle = particleIdFromName(section.getStringValue("DeathParticle"));
        }
        if (section.hasValue("SmokeParticle")) {
            info.smokeParticle = particleIdFromName(section.getStringValue("SmokeParticle"));
        }
    }
    Logger::info(COMP_GAMERULES, "BulletInfosLoader::load", "[{}] Done", filename);
    return true;
}
