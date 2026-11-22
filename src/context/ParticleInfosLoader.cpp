/**
 * @file ParticleInfosLoader.cpp
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

#include "context/ParticleInfosLoader.hpp"

#include <string>
#include <utility>

#include "gameobjects/particles/cParticleInfos.h"
#include "utils/Log.h"
#include "utils/ParticleNames.hpp"
#include "utils/cIniFile.h"
#include "utils/cIniUtils.h"

namespace {

// What a particle gets when particles.ini does not say otherwise
void setDefaults(s_ParticleInfo& info)
{
    info.bmpIndex = TextureID::invalid;
    info.startAlpha = -1; // anything outside 0-255 means opaque
    info.usesAdditiveBlending = false;
    info.layer = RenderLayerParticle::TOP;
    info.frameWidth = 32;
    info.frameHeight = 32;
    info.oldParticle = true; // keep the old behavior for now (will be removed)
    info.spawnFrameTimer = 10;
    info.spawnFrameTimerRandom = 0;
    info.spawnDeadTimer = 0;
    info.spawnDeadTimerRandom = 0;
    info.startAlphaRandom = 0;
    info.anchorBottom = false;
    for (auto& spawn : info.spawns) {
        spawn = s_ParticleSpawn{-1, 0, 0, false};
    }
}

RenderLayerParticle layerFromName(const std::string& name, RenderLayerParticle fallback)
{
    if (cIniUtils::caseInsCompare(name, "TOP")) return RenderLayerParticle::TOP;
    if (cIniUtils::caseInsCompare(name, "BOTTOM")) return RenderLayerParticle::BOTTOM;
    Logger::error(COMP_GAMERULES, "ParticleInfosLoader::load", "Unknown Layer [{}], expected TOP or BOTTOM", name);
    return fallback;
}

// Spawn<n>=PARTICLE with the optional Spawn<n>OffsetX, Spawn<n>OffsetY and Spawn<n>InheritHouse
void readSpawn(const cSection& section, int number, s_ParticleSpawn& spawn)
{
    const std::string key = "Spawn" + std::to_string(number);
    if (!section.hasValue(key)) return;

    const std::string name = section.getStringValue(key);
    spawn.type = particleIdFromName(name);
    if (spawn.type < 0) {
        Logger::error(COMP_GAMERULES, "ParticleInfosLoader::load", "Unknown particle [{}] in {}, nothing is spawned", name, key);
    }
    spawn.offsetX = section.getInt(key + "OffsetX", spawn.offsetX);
    spawn.offsetY = section.getInt(key + "OffsetY", spawn.offsetY);
    spawn.inheritHouse = section.getBoolean(key + "InheritHouse", spawn.inheritHouse);
}

} // namespace

ParticleInfosLoader::ParticleInfosLoader(TextureLookup textureLookup)
    : m_textureLookup(std::move(textureLookup))
{
}

bool ParticleInfosLoader::load(const std::string& filename, cParticleInfos& particleInfos) const
{
    for (auto& info : particleInfos) {
        setDefaults(info);
    }

    Logger::info(COMP_GAMERULES, "ParticleInfosLoader::load", "Opening particle rules from : {}", filename);
    cIniFile file;
    if (!file.load(filename)) {
        Logger::error(COMP_GAMERULES, "ParticleInfosLoader::load", "Could not open file [{}], no particles are available", filename);
        return false;
    }

    for (const auto& sectionName : file.getSectionNames()) {
        const int id = particleIdFromName(sectionName);
        if (id < 0 || !particleInfos.valid(static_cast<std::size_t>(id))) {
            Logger::error(COMP_GAMERULES, "ParticleInfosLoader::load", "[{}] Unknown particle section [{}], skipping it (expected e.g. [EXPLOSION_TRIKE])", filename, sectionName);
            continue;
        }

        const cSection section = file.getSection(sectionName);
        s_ParticleInfo& info = particleInfos[static_cast<std::size_t>(id)];
        if (section.hasValue("Bitmap")) {
            info.bmpIndex = m_textureLookup(section.getStringValue("Bitmap"));
        }
        info.startAlpha = section.getInt("StartAlpha", info.startAlpha);
        info.usesAdditiveBlending = section.getBoolean("AdditiveBlending", info.usesAdditiveBlending);
        if (section.hasValue("Layer")) {
            info.layer = layerFromName(section.getStringValue("Layer"), info.layer);
        }
        info.frameWidth = section.getInt("FrameWidth", info.frameWidth);
        info.frameHeight = section.getInt("FrameHeight", info.frameHeight);
        info.spawnFrameTimer = section.getInt("FrameTimer", info.spawnFrameTimer);
        info.spawnFrameTimerRandom = section.getInt("FrameTimerRandom", info.spawnFrameTimerRandom);
        info.spawnDeadTimer = section.getInt("DeadTimer", info.spawnDeadTimer);
        info.spawnDeadTimerRandom = section.getInt("DeadTimerRandom", info.spawnDeadTimerRandom);
        info.startAlphaRandom = section.getInt("StartAlphaRandom", info.startAlphaRandom);
        info.anchorBottom = section.getBoolean("AnchorBottom", info.anchorBottom);
        for (int i = 0; i < MAX_PARTICLE_SPAWNS; i++) {
            readSpawn(section, i + 1, info.spawns[i]);
        }
    }
    Logger::info(COMP_GAMERULES, "ParticleInfosLoader::load", "[{}] Done", filename);
    return true;
}
