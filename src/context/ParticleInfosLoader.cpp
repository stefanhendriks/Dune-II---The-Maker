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
    info.spawnFrameTimer = 0;
    info.spawnFrameTimerRandom = 0;
    info.spawnDeadTimer = 0;
    info.spawnDeadTimerRandom = 0;
    info.startAlphaRandom = 0;
    info.anchorBottom = false;
    for (auto& spawn : info.spawns) {
        spawn = s_ParticleSpawn{-1, 0, 0, false};
    }
    info.frameDelay = 10;
    info.frameDelayRandom = 0;
    info.fadeDelay = -1;
    info.lastAnimFrame = 0;
    info.holdFrame = 0;
    info.frameCount = 1;
    info.fadeStep = 0;
    info.fadeInStep = 0;
    info.fadeInMaxAlpha = 255;
    info.dieAlpha = 0;
    info.nextFrameBelowAlpha = 0;
    info.wearChance = 0;
    info.wearMinAlpha = 0;
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

// True when spawning from 'current' leads back to 'origin' (a particle that spawns itself, directly or not)
bool spawnsLeadBackTo(const cParticleInfos& infos, int origin, int current, int depth)
{
    if (depth > static_cast<int>(infos.size())) return true;
    for (const auto& spawn : infos[static_cast<std::size_t>(current)].spawns) {
        if (spawn.type < 0) continue;
        if (spawn.type == origin || spawnsLeadBackTo(infos, origin, spawn.type, depth + 1)) return true;
    }
    return false;
}

// A spawn that leads back to its own particle would spawn forever, so it is dropped
void removeCyclicSpawns(cParticleInfos& infos)
{
    for (std::size_t id = 0; id < infos.size(); id++) {
        for (auto& spawn : infos[id].spawns) {
            if (spawn.type < 0) continue;
            if (spawn.type == static_cast<int>(id) || spawnsLeadBackTo(infos, static_cast<int>(id), spawn.type, 0)) {
                Logger::error(COMP_GAMERULES, "ParticleInfosLoader::load", "Particle {} spawns itself (directly or through other particles), skipping that spawn", id);
                spawn.type = -1;
            }
        }
    }
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
        info.frameDelay = section.getInt("FrameDelay", info.frameDelay);
        info.frameDelayRandom = section.getInt("FrameDelayRandom", info.frameDelayRandom);
        info.fadeDelay = section.getInt("FadeDelay", info.fadeDelay);
        info.lastAnimFrame = section.getInt("LastAnimFrame", info.lastAnimFrame);
        info.holdFrame = section.getInt("HoldFrame", info.holdFrame);
        info.frameCount = section.getInt("FrameCount", info.frameCount);
        info.fadeStep = section.getInt("FadeStep", info.fadeStep);
        info.fadeInStep = section.getInt("FadeInStep", info.fadeInStep);
        info.fadeInMaxAlpha = section.getInt("FadeInMaxAlpha", info.fadeInMaxAlpha);
        info.dieAlpha = section.getInt("DieAlpha", info.dieAlpha);
        info.nextFrameBelowAlpha = section.getInt("NextFrameBelowAlpha", info.nextFrameBelowAlpha);
        info.wearChance = section.getInt("WearChance", info.wearChance);
        info.wearMinAlpha = section.getInt("WearMinAlpha", info.wearMinAlpha);
    }
    removeCyclicSpawns(particleInfos);
    Logger::info(COMP_GAMERULES, "ParticleInfosLoader::load", "[{}] Done", filename);
    return true;
}
