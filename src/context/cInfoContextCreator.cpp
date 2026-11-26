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
#include "context/AiParametersLoader.hpp"
#include "context/BulletInfosLoader.hpp"
#include "context/ParticleInfosLoader.hpp"
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
    const ParticleInfosLoader loader([this](std::string_view name) { return m_gfxdata->getTextureID(name); });
    loader.load((std::filesystem::path(m_rulesDirectory) / "particles.ini").string(), *particleInfos);
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
    Logger::info(COMP_SETUP, "Setup", "AI PARAMETERS");
    infoContext.setAiParameters(createAiParameters());
}

brains::s_AiParameters cInfoContextCreator::createAiParameters()
{
    brains::s_AiParameters aiParameters;
    AiParametersLoader().load("AI.ini", aiParameters);
    return aiParameters;
}

/********************************
 House Rules
 ********************************/
