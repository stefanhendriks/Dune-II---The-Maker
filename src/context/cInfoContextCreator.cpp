/**
 * @file cInfoContextCreator.cpp
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

#include "context/cInfoContextCreator.h"
#include "context/BulletInfosLoader.hpp"
#include "context/StructureInfosLoader.hpp"
#include "context/UnitInfosLoader.hpp"
#include "context/cInfoContext.h"
#include "game/cGameSettings.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/cSpecialInfos.h"
#include "gameobjects/cUpgradeInfo.h"
#include "gameobjects/units/cUnitInfos.h"
#include "context/GraphicsContext.hpp"
#include "context/UpgradeInfosLoader.hpp"
#include "context/AudioContext.hpp"
#include "sidebar/cSideBar.h"
#include "utils/Graphics.hpp"
#include "utils/Log.h"
#include "gameobjects/map/cMap.h"
#include "context/SpecialInfosLoader.hpp"
#include "context/TerrainInfoLoader.hpp"
#include "gameobjects/sTerrainInfo.h"
#include "include/enums/HouseFlags.h"

#include <filesystem>
#include <stdexcept>
#include <string_view>

cInfoContextCreator::cInfoContextCreator(GraphicsContext* graphicsContext, const AudioContext* audioContext, std::string rulesDirectory)
    : m_rulesDirectory(std::move(rulesDirectory)) {
    d2tm_assert(graphicsContext != nullptr);
    d2tm_assert(audioContext != nullptr);
    m_audio = audioContext;
    m_gfxdata = graphicsContext->gfxdata.get();
    m_gfxinter = graphicsContext->gfxinter.get();
    d2tm_assert(m_gfxdata != nullptr);
    d2tm_assert(m_gfxinter != nullptr);
}


std::unique_ptr<cUnitInfos> cInfoContextCreator::createUnitInfos() {
    auto unitInfos = std::make_unique<cUnitInfos>();
    const UnitInfosLoader loader(
        [this](std::string_view name) { return m_gfxdata->getSurface(m_gfxdata->getSurfaceID(name)); },
        [this](std::string_view name) { return m_gfxdata->getTexture(m_gfxdata->getTextureID(name)); },
        [this](std::string_view name) { return m_gfxinter->getTextureID(name); },
        [this](std::string_view name) { return m_audio->indexOf(name); });
    loader.load((std::filesystem::path(m_rulesDirectory) / "units.ini").string(), *unitInfos);
    return unitInfos;
}

std::unique_ptr<cParticleInfos> cInfoContextCreator::createParticleInfos() {
    auto particleInfos = std::make_unique<cParticleInfos>();
    initParticles(*particleInfos);
    return particleInfos;
}

std::unique_ptr<cStructureInfos> cInfoContextCreator::createStructureInfos() {
    auto structureInfos = std::make_unique<cStructureInfos>();
    const StructureInfosLoader loader(
        [this](std::string_view name) { return m_gfxdata->getSurface(m_gfxdata->getSurfaceID(name)); },
        [this](std::string_view name) { return m_gfxdata->getTexture(m_gfxdata->getTextureID(name)); },
        [this](std::string_view name) { return m_gfxinter->getTextureID(name); });
    loader.load((std::filesystem::path(m_rulesDirectory) / "structures.ini").string(), *structureInfos);
    return structureInfos;
}

std::unique_ptr<cBulletInfos> cInfoContextCreator::createBulletInfos() {
    auto bulletInfos = std::make_unique<cBulletInfos>();
    const BulletInfosLoader loader(
        [this](std::string_view name) { return m_gfxdata->getTexture(m_gfxdata->getTextureID(name)); },
        [this](std::string_view name) { return m_audio->indexOf(name); });
    loader.load((std::filesystem::path(m_rulesDirectory) / "bullets.ini").string(), *bulletInfos);
    return bulletInfos;
}

std::unique_ptr<cSpecialInfos> cInfoContextCreator::createSpecialInfos() {
    auto specialInfos = std::make_unique<cSpecialInfos>();
    const SpecialInfosLoader loader([this](std::string_view name) { return m_gfxinter->getTextureID(name); });
    loader.load((std::filesystem::path(m_rulesDirectory) / "specials.ini").string(), *specialInfos);
    return specialInfos;
}

std::unique_ptr<cUpgradeInfos> cInfoContextCreator::createUpgradeInfos(const cInfoContext* infoContext, const cGameSettings* settings) {
    auto upgradeInfos = std::make_unique<cUpgradeInfos>();
    const UpgradeInfosLoader loader(
        [this](std::string_view name) { return m_gfxinter->getTextureID(name); },
        [infoContext](int structureType) { return infoContext->getStructureInfo(structureType).cost; },
        [infoContext](int unitType) { return infoContext->getUnitInfo(unitType).cost; },
        settings->isCheatMode());
    loader.load((std::filesystem::path(m_rulesDirectory) / "upgrades.ini").string(), *upgradeInfos);
    return upgradeInfos;
}

std::unique_ptr<s_TerrainInfo> cInfoContextCreator::createTerrainInfos() {
    auto terrainInfo = std::make_unique<s_TerrainInfo>();
    const TerrainInfoLoader loader;
    loader.load((std::filesystem::path(m_rulesDirectory) / "terrain.ini").string(), *terrainInfo);
    return terrainInfo;
}

void cInfoContextCreator::installInfos(cInfoContext& infoContext) {
    Logger::info(COMP_SETUP, "Setup", "STRUCTURES");
    infoContext.setStructureInfos(createStructureInfos());
    Logger::info(COMP_SETUP, "Setup", "PROJECTILES");
    infoContext.setBulletInfos(createBulletInfos());
    Logger::info(COMP_SETUP, "Setup", "UNITS");
    infoContext.setUnitInfos(createUnitInfos());
    Logger::info(COMP_SETUP, "Setup", "SPECIALS");
    infoContext.setSpecialInfos(createSpecialInfos());
    Logger::info(COMP_SETUP, "Setup", "PARTICLES");
    infoContext.setParticleInfos(createParticleInfos());
    Logger::info(COMP_SETUP, "Setup", "TERRAINS");
    infoContext.setTerrainInfo(createTerrainInfos());
}

/********************************
 House Rules
 ********************************/

void cInfoContextCreator::initParticles(cParticleInfos& particleInfos)
{
    for (unsigned int i = 0; i < particleInfos.size(); i++) {
        s_ParticleInfo &particleInfo = particleInfos[i];
        particleInfo.bmpIndex = TextureID::invalid;
        particleInfo.startAlpha = -1;
        particleInfo.usesAdditiveBlending = false;

        particleInfo.layer = RenderLayerParticle::TOP;

        // default to 32x32 (for now)
        particleInfo.frameWidth = 32;
        particleInfo.frameHeight = 32;

        // mark particles as old for now
        particleInfo.oldParticle = true;
    }

    // move
    particleInfos[D2TM_PARTICLE_MOVE].bmpIndex = m_gfxdata->getTextureID("MOVE_INDICATOR");
    particleInfos[D2TM_PARTICLE_MOVE].startAlpha = 128;

    // attack
    particleInfos[D2TM_PARTICLE_ATTACK].bmpIndex = m_gfxdata->getTextureID("ATTACK_INDICATOR");
    particleInfos[D2TM_PARTICLE_ATTACK].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_TRIKE");
    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_SMOKE].bmpIndex = m_gfxdata->getTextureID("OBJECT_SMOKE");
    particleInfos[D2TM_PARTICLE_SMOKE].startAlpha = 0;
    particleInfos[D2TM_PARTICLE_SMOKE].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SMOKE].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].bmpIndex = m_gfxdata->getTextureID("OBJECT_SMOKE");
    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].startAlpha = 0;
    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].bmpIndex = m_gfxdata->getTextureID("OBJECT_SMOKE_SHADOW");
    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].startAlpha = 0;
    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].frameWidth = 36;
    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].frameHeight = 38;

    particleInfos[D2TM_PARTICLE_TRACK_DIA].bmpIndex = m_gfxdata->getTextureID("TRACK_DIA");
    particleInfos[D2TM_PARTICLE_TRACK_DIA].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_DIA].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_TRACK_HOR].bmpIndex = m_gfxdata->getTextureID("TRACK_HOR");
    particleInfos[D2TM_PARTICLE_TRACK_HOR].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_HOR].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_TRACK_VER].bmpIndex = m_gfxdata->getTextureID("TRACK_VER");
    particleInfos[D2TM_PARTICLE_TRACK_VER].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_VER].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_TRACK_DIA2].bmpIndex = m_gfxdata->getTextureID("TRACK_DIA2");
    particleInfos[D2TM_PARTICLE_TRACK_DIA2].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_DIA2].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_BULLET_PUF].bmpIndex = m_gfxdata->getTextureID("BULLET_PUF");
    particleInfos[D2TM_PARTICLE_BULLET_PUF].frameWidth = 18;
    particleInfos[D2TM_PARTICLE_BULLET_PUF].frameHeight = 18;


    particleInfos[D2TM_PARTICLE_EXPLOSION_FIRE].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_FIRE");

    particleInfos[D2TM_PARTICLE_WORMEAT].bmpIndex = m_gfxdata->getTextureID("OBJECT_WORMEAT");
    particleInfos[D2TM_PARTICLE_WORMEAT].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_WORMEAT].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_WORMEAT].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_TANK_ONE");
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_TANK_TWO");
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_STRUCTURE01");
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_STRUCTURE02");
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_GAS");
    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_WORMTRAIL].bmpIndex = m_gfxdata->getTextureID("OBJECT_WORMTRAIL");
    particleInfos[D2TM_PARTICLE_WORMTRAIL].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_WORMTRAIL].startAlpha = 96;
    particleInfos[D2TM_PARTICLE_WORMTRAIL].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_WORMTRAIL].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_DEADINF01].bmpIndex = m_gfxdata->getTextureID("OBJECT_DEADINF01");
    particleInfos[D2TM_PARTICLE_DEADINF01].layer = RenderLayerParticle::BOTTOM;

    particleInfos[D2TM_PARTICLE_DEADINF02].bmpIndex = m_gfxdata->getTextureID("OBJECT_DEADINF02");
    particleInfos[D2TM_PARTICLE_DEADINF02].layer = RenderLayerParticle::BOTTOM;

    particleInfos[D2TM_PARTICLE_TANKSHOOT].bmpIndex = m_gfxdata->getTextureID("OBJECT_TANKSHOOT");
    particleInfos[D2TM_PARTICLE_TANKSHOOT].startAlpha = 128;
    particleInfos[D2TM_PARTICLE_TANKSHOOT].frameWidth = 64;
    particleInfos[D2TM_PARTICLE_TANKSHOOT].frameHeight = 64;

    particleInfos[D2TM_PARTICLE_SIEGESHOOT].bmpIndex = m_gfxdata->getTextureID("OBJECT_SIEGESHOOT");
    particleInfos[D2TM_PARTICLE_SIEGESHOOT].startAlpha = 128;
    particleInfos[D2TM_PARTICLE_SIEGESHOOT].frameWidth = 64;
    particleInfos[D2TM_PARTICLE_SIEGESHOOT].frameHeight = 64;

    particleInfos[D2TM_PARTICLE_SQUISH01].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_SQUISH01");
    particleInfos[D2TM_PARTICLE_SQUISH01].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_SQUISH01].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_SQUISH01].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SQUISH01].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_SQUISH02].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_SQUISH02");
    particleInfos[D2TM_PARTICLE_SQUISH02].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_SQUISH02].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_SQUISH02].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SQUISH02].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_SQUISH03].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_SQUISH03");
    particleInfos[D2TM_PARTICLE_SQUISH03].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_SQUISH03].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_SQUISH03].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SQUISH03].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_ORNI");
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_EXPLOSION_CARRYALL].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_CARRYALL");
    particleInfos[D2TM_PARTICLE_EXPLOSION_CARRYALL].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_EXPLOSION_CARRYALL].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_CARRYALL].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_EXPLOSION_CARRYALL].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_SIEGEDIE].bmpIndex = m_gfxdata->getTextureID("OBJECT_SIEGEDIE");

    particleInfos[D2TM_PARTICLE_CARRYPUFF].bmpIndex = m_gfxdata->getTextureID("OBJECT_CARRYPUFF");
    particleInfos[D2TM_PARTICLE_CARRYPUFF].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_CARRYPUFF].frameWidth = 96;
    particleInfos[D2TM_PARTICLE_CARRYPUFF].frameHeight = 96;

    particleInfos[D2TM_PARTICLE_EXPLOSION_ROCKET].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_ROCKET");
    particleInfos[D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_ROCKET_SMALL");

    particleInfos[D2TM_PARTICLE_EXPLOSION_BULLET].bmpIndex = m_gfxdata->getTextureID("EXPLOSION_BULLET");

    // the nice flare/light effects that come with explosions
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].bmpIndex = m_gfxdata->getTextureID("OBJECT_BOOM01");
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].usesAdditiveBlending = true;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].startAlpha = 240;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].frameWidth = 512;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].frameHeight = 512;

    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].bmpIndex = m_gfxdata->getTextureID("OBJECT_BOOM02");
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].usesAdditiveBlending = true;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].startAlpha = 230;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].frameWidth = 256;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].frameHeight = 256;

    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].bmpIndex = m_gfxdata->getTextureID("OBJECT_BOOM03");
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].usesAdditiveBlending = true;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].startAlpha = 220;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].frameWidth = 128;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].frameHeight = 128;
}

