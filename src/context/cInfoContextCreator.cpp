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
    initUpgrades(*upgradeInfos, infoContext, settings);
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

/****************
 Install bullets
 ****************/
void cInfoContextCreator::initUpgrades(cUpgradeInfos& upgradeInfos, const cInfoContext* infoContext, const cGameSettings* settings)
{
    Logger::info(COMP_SETUP, "Installing", "UPGRADES");
    for (int i = 0; i < MAX_UPGRADETYPES; i++) {
        upgradeInfos[i].enabled = false;
        upgradeInfos[i].techLevel = -1;
        upgradeInfos[i].house = 0;
        upgradeInfos[i].needsStructureType = -1;
        upgradeInfos[i].icon = m_gfxinter->getTextureID("ICON_STR_PALACE");
        upgradeInfos[i].cost = 100;
        upgradeInfos[i].atUpgradeLevel = -1;
        upgradeInfos[i].structureType = CONSTYARD;
        upgradeInfos[i].providesTypeId = -1;
        upgradeInfos[i].providesType = STRUCTURE;
        upgradeInfos[i].providesTypeList = eListType::LIST_NONE;
        upgradeInfos[i].providesTypeSubList = -1;
        upgradeInfos[i].buildTime = 5;
        upgradeInfos[i].description = "Upgrade";
    }

    // CONSTYARD UPGRADES

    // First upgrade Constyard: 4Slabs
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].enabled = true;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].house = Atreides | Harkonnen | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].techLevel = 4; // start from mission 4
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].icon = m_gfxinter->getTextureID("ICON_STR_4SLAB");
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].cost = 200;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].structureType = CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesType = STRUCTURE;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeId = SLAB4;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeList = eListType::LIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeSubList = SUBLIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].buildTime = 50;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].description = "Build 4 concrete slabs at once";

    // Second upgrade Constyard: Rturret
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].enabled = true;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].house = Atreides | Harkonnen | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].techLevel = 6;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].icon = m_gfxinter->getTextureID("ICON_STR_RTURRET");
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].cost = 200;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].needsStructureType = RADAR;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].structureType = CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].atUpgradeLevel = 1;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesType = STRUCTURE;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeId = RTURRET;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeList = eListType::LIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeSubList = SUBLIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].buildTime = 150;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].description = "Build Rocket Turret";

    // LIGHTFACTORY UPGRADES, only for ATREIDES and ORDOS
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].enabled = true;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].house = Atreides | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].icon = m_gfxinter->getTextureID("ICON_UNIT_QUAD");
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].cost = infoContext->getStructureInfo(LIGHTFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].needsStructureType = LIGHTFACTORY;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].structureType = LIGHTFACTORY;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeId = QUAD;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeSubList =  SUBLIST_LIGHTFCTRY;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].buildTime = 150;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].description = "Build Quad at Light Factory";

    // HEAVYFACTORY UPGRADES:

    // ALL HOUSES GET MVC
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].house = Atreides | Ordos | Harkonnen | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].techLevel = 4;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].icon = m_gfxinter->getTextureID("ICON_UNIT_MCV");
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost = infoContext->getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeId = MCV;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].buildTime = 150;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].description = "Build MCV at Heavy Factory";

    // Harkonnen/Atreides only
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].house = Atreides | Harkonnen | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].techLevel = 5;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].icon = m_gfxinter->getTextureID("ICON_UNIT_LAUNCHER");
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].cost = infoContext->getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].atUpgradeLevel = 1; // requires MCV upgrade first
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeId = LAUNCHER;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].buildTime = 150;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].description = "Build Rocket Launcher at Heavy Factory";

    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].house = Atreides | Harkonnen | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].techLevel = 6;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].icon = m_gfxinter->getTextureID("ICON_UNIT_SIEGETANK");
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].cost = infoContext->getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].atUpgradeLevel = 2; // After upgrade to Rocket Launcher
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeId = SIEGETANK;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].buildTime = 150;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].description = "Build Siege Tank at Heavy Factory";

    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].house = Ordos;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].techLevel = 6;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].icon = m_gfxinter->getTextureID("ICON_UNIT_SIEGETANK");
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].cost = infoContext->getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].atUpgradeLevel = 1;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeId = SIEGETANK;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].buildTime = 150;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].description = "Build Siege Tank at Heavy Factory";

    // HI-TECH UPGRADES (Ordos/Atreides only)
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].house = Atreides | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].techLevel = 8;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].icon = m_gfxinter->getTextureID("ICON_UNIT_ORNITHOPTER");
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].cost = infoContext->getStructureInfo(HIGHTECH).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].structureType = HIGHTECH;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeId = ORNITHOPTER;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeSubList = SUBLIST_HIGHTECH;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].buildTime = 150;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].description = "Build Ornithopter at Hi-Tech";

    // WOR (Harkonnen & Ordos)
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].enabled = true;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].house = Harkonnen | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].icon = m_gfxinter->getTextureID("ICON_UNIT_TROOPERS");
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].cost = infoContext->getStructureInfo(WOR).cost / 2;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].structureType = WOR;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeId = TROOPERS;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeList = eListType::LIST_FOOT_UNITS;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeSubList = SUBLIST_TROOPERS;
    if (!settings->isCheatMode()) {
        upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].buildTime = 150;
    }
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].description = "Build Troopers at WOR";

    // BARRACKS (Atreides & Ordos)
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].enabled = true;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].house = Atreides | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].icon = m_gfxinter->getTextureID("ICON_UNIT_INFANTRY");
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].cost = infoContext->getStructureInfo(BARRACKS).cost / 2;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].structureType = BARRACKS;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeId = INFANTRY;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeList = eListType::LIST_FOOT_UNITS;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeSubList = SUBLIST_INFANTRY;
    if (!settings->isCheatMode()) {
        upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].buildTime = 150;
    }
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].description = "Build Infantry at Barracks";

    // HEAVY FACTORY: Sardaukar - Allow building Sonic Tank
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].house = Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].icon = m_gfxinter->getTextureID("ICON_UNIT_SONICTANK");
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].cost = infoContext->getUnitInfo(SONICTANK).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].structureType = IX;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].dependsOnStructureLevel = false;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].providesTypeId = SONICTANK;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    if (!settings->isCheatMode()) {
        upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].buildTime = 150;
    }
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK].description = "Build SonicTank at Heavy Factory";

    // HEAVY FACTORY: Sardaukar - Allow building Deviator
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].house = Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].icon = m_gfxinter->getTextureID("ICON_UNIT_DEVIATOR");
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].cost = infoContext->getUnitInfo(DEVIATOR).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].structureType = IX;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].dependsOnStructureLevel = false;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].providesTypeId = DEVIATOR;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    if (!settings->isCheatMode()) {
        upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].buildTime = 150;
    }
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR].description = "Build Deviator at Heavy Factory";

    // HEAVY FACTORY: Sardaukar - Allow building Devastator
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].house = Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].icon = m_gfxinter->getTextureID("ICON_UNIT_DEVASTATOR");
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].cost = infoContext->getUnitInfo(DEVASTATOR).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].structureType = IX;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].dependsOnStructureLevel = false;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].providesTypeId = DEVASTATOR;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    if (!settings->isCheatMode()) {
        upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].buildTime = 150;
    }
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR].description = "Build Devastator at Heavy Factory";
}
