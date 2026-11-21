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
    initUnits(*unitInfos);
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
    initSpecials(*specialInfos);
    return specialInfos;
}

std::unique_ptr<cUpgradeInfos> cInfoContextCreator::createUpgradeInfos(const cInfoContext* infoContext, const cGameSettings* settings) {
    auto upgradeInfos = std::make_unique<cUpgradeInfos>();
    initUpgrades(*upgradeInfos, infoContext, settings);
    return upgradeInfos;
}

std::unique_ptr<s_TerrainInfo> cInfoContextCreator::createTerrainInfos() {
    auto terrainInfo = std::make_unique<s_TerrainInfo>();
    installTerrain(terrainInfo.get());
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

/*****************************
 Unit Rules
 *****************************/
void cInfoContextCreator::initUnits(cUnitInfos& unitInfos)
{
    Logger::info(COMP_SETUP, "Installing", "UNITS");
    // Every unit thinks at 0.1 second. When the unit thinks, it is thinking about the path it
    // is taking, the enemies around him, etc. The speed of how a unit should move is depended on
    // time aswell. Every 0.01 second a unit 'can' move. The movespeed is like this:
    // 0    - slowest (1 second per pixel)
    // 1000 - fastest (1 pixel per 0.01 second)
    // So, the higher the number, the faster it is.



    // Unit        : <name>
    // Description : <description>
    // some things for ALL unit types; initialization
    for (int i = 0; i < MAX_UNITTYPES; i++) {
        s_UnitInfo &unitInfo = unitInfos[i];
        unitInfo.bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_QUAD")); // default bitmap is a quad!
        unitInfo.top = nullptr;  // no top
        unitInfo.shadow = nullptr;  // no shadow (deliverd with picture itself)
        unitInfo.bmp_width = 0;
        unitInfo.bmp_height = 0;
        unitInfo.turnspeed = 0;
        unitInfo.speed = 0;
        unitInfo.icon = TextureID::invalid;
        unitInfo.hp = -1;
        unitInfo.bulletType = -1;
        unitInfo.bulletTypeSecondary = -1;
        unitInfo.fireSecondaryWithinRange = -1;
        unitInfo.attack_frequency = -1;
        unitInfo.next_attack_frequency = -1;
        unitInfo.buildTime = -1;
        unitInfo.airborn = false;
        unitInfo.infantry = false;
        unitInfo.free_roam = false;
        unitInfo.fireTwice = false;
        unitInfo.fireTwiceHpThresholdFactor = 0.5f; // default 50% hp
        unitInfo.squish = true;             // most units can squish (infantry)
        unitInfo.canBeSquished = false;     // most units cannot be squished
        unitInfo.range = -1;
        unitInfo.sight = -1;
        unitInfo.queuable = true;
        unitInfo.dieWhenLowerThanHP = 1; // lower than 1 means dead
        unitInfo.appetite = -1;
        unitInfo.deathSound = audioID::invalid;
        unitInfo.eatSound = audioID::invalid;

        // harvester properties
        unitInfo.harvesting_amount = 0;
        unitInfo.harvesting_speed = 0;
        unitInfo.credit_capacity = 0;

        // list properties
        unitInfo.listType = eListType::LIST_NONE;
        unitInfo.subListId = 0;

        // attack related
        unitInfo.canAttackAirUnits = false;
        unitInfo.canAttackUnits = false;
        unitInfo.canDamageStructures = true;

        // capturing / damage upon entering structure related
        unitInfo.canEnterAndDamageStructure = false;
        unitInfo.attackIsEnterStructure = false;
        unitInfo.damageOnEnterStructure = 0.0f;

        // spawn smoke when damaged?
        unitInfo.renderSmokeOnUnitWhenThresholdMet = false;
        unitInfo.smokeHpFactor = 0.0f;

        unitInfo.canGuard = false;

        unitInfo.configured = false;

        unitInfo.name = "";
    }

    // Unit        : CarryAll
    // Description : CarryAll, the flying pickuptruck
    unitInfos[CARRYALL].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_CARRYALL"));      // pointer to the original 8bit bitmap
    unitInfos[CARRYALL].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_CARRYALL_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[CARRYALL].bmp_width = 24 * 2;
    unitInfos[CARRYALL].bmp_height = 24 * 2;
    unitInfos[CARRYALL].bmp_startpixel = 0;
    unitInfos[CARRYALL].bmp_frames = 2; // we have at max 1 extra frame
    unitInfos[CARRYALL].icon = m_gfxinter->getTextureID("ICON_UNIT_CARRYALL");
    unitInfos[CARRYALL].airborn = true;   // is airborn
    unitInfos[CARRYALL].free_roam = true; // may freely roam the air
    unitInfos[CARRYALL].listType = eListType::LIST_UNITS;
    unitInfos[CARRYALL].subListId = SUBLIST_HIGHTECH;
    unitInfos[CARRYALL].name = "Carry-All";
    unitInfos[CARRYALL].configured = true;

    // Unit        : Ornithopter
    // Description : Pesty little aircraft shooting bastard
    unitInfos[ORNITHOPTER].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_ORNITHOPTER"));      // pointer to the original 8bit bitmap
    unitInfos[ORNITHOPTER].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_ORNITHOPTER_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[ORNITHOPTER].bmp_width = 24 * 2;
    unitInfos[ORNITHOPTER].bmp_height = 24 * 2;
    unitInfos[ORNITHOPTER].bmp_startpixel = 0;
    unitInfos[ORNITHOPTER].bmp_frames = 4; // we have at max 3 extra frames
    unitInfos[ORNITHOPTER].icon = m_gfxinter->getTextureID("ICON_UNIT_ORNITHOPTER");
    unitInfos[ORNITHOPTER].bulletType = ROCKET_SMALL_ORNI;
    unitInfos[ORNITHOPTER].fireTwice = true;
    unitInfos[ORNITHOPTER].airborn = true;   // is airborn
    unitInfos[ORNITHOPTER].squish = false;   // can't squish infantry
    unitInfos[ORNITHOPTER].free_roam = true; // may freely roam the air
    unitInfos[ORNITHOPTER].listType = eListType::LIST_UNITS;
    unitInfos[ORNITHOPTER].subListId = SUBLIST_HIGHTECH;
//    unitInfos[ORNITHOPTER].canAttackAirUnits = true; // orni's can attack other air units
    unitInfos[ORNITHOPTER].name = "Ornithopter";
    unitInfos[ORNITHOPTER].configured = true;

    // Unit        : Devastator
    // Description : Devastator
    unitInfos[DEVASTATOR].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_DEVASTATOR"));      // pointer to the original 8bit bitmap
    unitInfos[DEVASTATOR].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_DEVASTATOR_SHADOW"));      // pointer to the original bitmap shadow
    unitInfos[DEVASTATOR].bmp_width = 19 * 2;
    unitInfos[DEVASTATOR].bmp_startpixel = 0;
    unitInfos[DEVASTATOR].bmp_height = 23 * 2;
    unitInfos[DEVASTATOR].bmp_frames = 1;
    unitInfos[DEVASTATOR].bulletType = BULLET_DEVASTATOR;
    unitInfos[DEVASTATOR].fireTwice = true;
    unitInfos[DEVASTATOR].icon = m_gfxinter->getTextureID("ICON_UNIT_DEVASTATOR");
    unitInfos[DEVASTATOR].listType = eListType::LIST_UNITS;
    unitInfos[DEVASTATOR].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[DEVASTATOR].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[DEVASTATOR].smokeHpFactor = 0.5f;
    unitInfos[DEVASTATOR].canGuard = true;
    unitInfos[DEVASTATOR].canAttackUnits = true;
    unitInfos[DEVASTATOR].name = "Devastator";
    unitInfos[DEVASTATOR].configured = true;

    // Unit        : Harvester
    // Description : Harvester
    unitInfos[HARVESTER].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_HARVESTER"));      // pointer to the original 8bit bitmap
    unitInfos[HARVESTER].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_HARVESTER_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[HARVESTER].bmp_width = 40 * 2;
    unitInfos[HARVESTER].bmp_startpixel = 24;
    unitInfos[HARVESTER].bmp_height = 26 * 2;
    unitInfos[HARVESTER].bmp_frames = 4;
    unitInfos[HARVESTER].icon = m_gfxinter->getTextureID("ICON_UNIT_HARVESTER");
    unitInfos[HARVESTER].credit_capacity = 700;
    unitInfos[HARVESTER].harvesting_amount = 5;
    unitInfos[HARVESTER].listType = eListType::LIST_UNITS;
    unitInfos[HARVESTER].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[HARVESTER].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[HARVESTER].smokeHpFactor = 0.5f;
    unitInfos[HARVESTER].name = "Harvester";
    unitInfos[HARVESTER].configured = true;

    // Unit        : Combattank
    // Description : Combattank
    unitInfos[TANK].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_TANKBASE"));      // pointer to the original 8bit bitmap
    unitInfos[TANK].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_TANKBASE_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[TANK].top = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_TANKTOP"));      // pointer to the original 8bit bitmap
    unitInfos[TANK].bmp_width = 16 * 2;
    unitInfos[TANK].bmp_startpixel = 0;
    unitInfos[TANK].bmp_height = 16 * 2;
    unitInfos[TANK].bmp_frames = 0;
    unitInfos[TANK].bulletType = BULLET_TANK;
    unitInfos[TANK].icon = m_gfxinter->getTextureID("ICON_UNIT_TANK");
    unitInfos[TANK].listType = eListType::LIST_UNITS;
    unitInfos[TANK].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[TANK].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[TANK].smokeHpFactor = 0.5f;
    unitInfos[TANK].canGuard = true;
    unitInfos[TANK].canAttackUnits = true;
    unitInfos[TANK].name = "Tank";
    unitInfos[TANK].configured = true;


    // Unit        : Siege Tank
    // Description : Siege tank
    unitInfos[SIEGETANK].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SIEGEBASE"));      // pointer to the original 8bit bitmap
    unitInfos[SIEGETANK].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_SIEGEBASE_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[SIEGETANK].top = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SIEGETOP"));      // pointer to the original 8bit bitmap
    unitInfos[SIEGETANK].bmp_width = 18 * 2;
    unitInfos[SIEGETANK].bmp_startpixel = 1;
    unitInfos[SIEGETANK].bmp_height = 18 * 2;
    unitInfos[SIEGETANK].bmp_frames = 0;
    unitInfos[SIEGETANK].bulletType = BULLET_SIEGE;
    unitInfos[SIEGETANK].fireTwice = true;
    unitInfos[SIEGETANK].icon = m_gfxinter->getTextureID("ICON_UNIT_SIEGETANK");
    unitInfos[SIEGETANK].listType = eListType::LIST_UNITS;
    unitInfos[SIEGETANK].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[SIEGETANK].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[SIEGETANK].smokeHpFactor = 0.5f;
    unitInfos[SIEGETANK].canGuard = true;
    unitInfos[SIEGETANK].canAttackUnits = true;
    unitInfos[SIEGETANK].name = "Siege Tank";
    unitInfos[SIEGETANK].configured = true;

    // Unit        : MCV
    // Description : Movable Construction Vehicle
    unitInfos[MCV].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_MCV"));      // pointer to the original 8bit bitmap
    unitInfos[MCV].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_MCV_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[MCV].bmp_width = 24 * 2;
    unitInfos[MCV].bmp_startpixel = 0;
    unitInfos[MCV].bmp_height = 25 * 2;
    unitInfos[MCV].bmp_frames = 1;
    unitInfos[MCV].icon = m_gfxinter->getTextureID("ICON_UNIT_MCV");
    unitInfos[MCV].listType = eListType::LIST_UNITS;
    unitInfos[MCV].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[MCV].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[MCV].smokeHpFactor = 0.5f;
    unitInfos[MCV].name = "MCV";
    unitInfos[MCV].configured = true;


    // Unit        : Deviator
    // Description : Deviator
    unitInfos[DEVIATOR].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_DEVIATOR"));      // pointer to the original 8bit bitmap
    unitInfos[DEVIATOR].bmp_width = 16 * 2;
    unitInfos[DEVIATOR].bmp_height = 16 * 2;
    unitInfos[DEVIATOR].bmp_startpixel = 0;
    unitInfos[DEVIATOR].bmp_frames = 1;
    unitInfos[DEVIATOR].icon = m_gfxinter->getTextureID("ICON_UNIT_DEVIATOR");
    unitInfos[DEVIATOR].bulletType = BULLET_GAS; // our gassy rocket
    unitInfos[DEVIATOR].listType = eListType::LIST_UNITS;
    unitInfos[DEVIATOR].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[DEVIATOR].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[DEVIATOR].smokeHpFactor = 0.5f;
    unitInfos[DEVIATOR].canGuard = true;
    unitInfos[DEVIATOR].canAttackUnits = true;
    unitInfos[DEVIATOR].canDamageStructures = false; // gas warhead only deviates units, does no structural damage
    unitInfos[DEVIATOR].name = "Deviator";
    unitInfos[DEVIATOR].configured = true;

    // Unit        : Launcher
    // Description : Rocket Launcher
    unitInfos[LAUNCHER].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_LAUNCHER"));      // pointer to the original 8bit bitmap
    unitInfos[LAUNCHER].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_LAUNCHER_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[LAUNCHER].bmp_width = 16 * 2;
    unitInfos[LAUNCHER].bmp_height = 16 * 2;
    unitInfos[LAUNCHER].bmp_startpixel = 0;
    unitInfos[LAUNCHER].bmp_frames = 1;
    unitInfos[LAUNCHER].icon = m_gfxinter->getTextureID("ICON_UNIT_LAUNCHER");
    unitInfos[LAUNCHER].fireTwice = true;
    //units[LAUNCHER].bullets = ROCKET_NORMAL; // our gassy rocket
    unitInfos[LAUNCHER].bulletType = ROCKET_NORMAL; // our gassy rocket
    unitInfos[LAUNCHER].listType = eListType::LIST_UNITS;
    unitInfos[LAUNCHER].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[LAUNCHER].canAttackAirUnits = true;
    unitInfos[LAUNCHER].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[LAUNCHER].smokeHpFactor = 0.5f;
    unitInfos[LAUNCHER].canGuard = true;
    unitInfos[LAUNCHER].canAttackUnits = true;
    unitInfos[LAUNCHER].name = "Launcher";
    unitInfos[LAUNCHER].configured = true;

    // Unit        : Quad
    // Description : Quad, 4 wheeled (double gunned)
    unitInfos[QUAD].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_QUAD"));      // pointer to the original 8bit bitmap
    unitInfos[QUAD].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_QUAD_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[QUAD].bmp_width = 16 * 2;
    unitInfos[QUAD].bmp_height = 16 * 2;
    unitInfos[QUAD].bmp_startpixel = 0;
    unitInfos[QUAD].bmp_frames = 1;
    unitInfos[QUAD].icon = m_gfxinter->getTextureID("ICON_UNIT_QUAD");
    unitInfos[QUAD].fireTwice = true;
    unitInfos[QUAD].bulletType = BULLET_QUAD;
    unitInfos[QUAD].squish = false;
    unitInfos[QUAD].listType = eListType::LIST_UNITS;
    unitInfos[QUAD].subListId = SUBLIST_LIGHTFCTRY;
    unitInfos[QUAD].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[QUAD].smokeHpFactor = 0.5f;
    unitInfos[QUAD].canGuard = true;
    unitInfos[QUAD].canAttackUnits = true;
    unitInfos[QUAD].name = "Quad";
    unitInfos[QUAD].configured = true;


    // Unit        : Trike (normal trike)
    // Description : Trike, 3 wheeled (single gunned)
    unitInfos[TRIKE].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_TRIKE"));      // pointer to the original 8bit bitmap
    unitInfos[TRIKE].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_TRIKE_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[TRIKE].bmp_width = 28;
    unitInfos[TRIKE].bmp_height = 26;
    unitInfos[TRIKE].bmp_startpixel = 0;
    unitInfos[TRIKE].bmp_frames = 1;
    unitInfos[TRIKE].icon = m_gfxinter->getTextureID("ICON_UNIT_TRIKE");
    unitInfos[TRIKE].bulletType = BULLET_TRIKE;
    unitInfos[TRIKE].squish = false;
    unitInfos[TRIKE].listType = eListType::LIST_UNITS;
    unitInfos[TRIKE].subListId = SUBLIST_LIGHTFCTRY;
    unitInfos[TRIKE].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[TRIKE].smokeHpFactor = 0.5f;
    unitInfos[TRIKE].canGuard = true;
    unitInfos[TRIKE].canAttackUnits = true;
    unitInfos[TRIKE].name = "Trike";
    unitInfos[TRIKE].configured = true;

    // Unit        : Raider Trike (Ordos trike)
    // Description : Raider Trike, 3 wheeled (single gunned), weaker, but faster
    unitInfos[RAIDER].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_TRIKE"));      // pointer to the original 8bit bitmap
    unitInfos[RAIDER].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_TRIKE_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[RAIDER].bmp_width = 14 * 2;
    unitInfos[RAIDER].bmp_height = 14 * 2;
    unitInfos[RAIDER].bmp_startpixel = 0;
    unitInfos[RAIDER].bmp_frames = 1;
    unitInfos[RAIDER].name = "Raider Trike";
    unitInfos[RAIDER].icon = m_gfxinter->getTextureID("ICON_UNIT_RAIDER");
    unitInfos[RAIDER].bulletType = BULLET_TRIKE;
    unitInfos[RAIDER].squish = false;
    unitInfos[RAIDER].listType = eListType::LIST_UNITS;
    unitInfos[RAIDER].subListId = SUBLIST_LIGHTFCTRY;
    unitInfos[RAIDER].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[RAIDER].smokeHpFactor = 0.5f;
    unitInfos[RAIDER].canGuard = true;
    unitInfos[RAIDER].canAttackUnits = true;
    unitInfos[RAIDER].configured = true;

    // Unit        : Frigate
    // Description : Frigate
    unitInfos[FRIGATE].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_FRIGATE"));      // pointer to the original 8bit bitmap
    unitInfos[FRIGATE].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_FRIGATE_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[FRIGATE].bmp_width = 32 * 2;
    unitInfos[FRIGATE].bmp_height = 32 * 2;
    unitInfos[FRIGATE].bmp_startpixel = 0;
    unitInfos[FRIGATE].bmp_frames = 2; // we have at max 1 extra frame
    unitInfos[FRIGATE].speed = 2;
    unitInfos[FRIGATE].turnspeed = 4;
    unitInfos[FRIGATE].airborn = true;
    unitInfos[FRIGATE].squish = false;
    unitInfos[FRIGATE].free_roam = true; // Frigate does not roam, yet needed?
    unitInfos[FRIGATE].hp = 9999;
    // frigate has no list
    unitInfos[FRIGATE].name = "Frigate";

    /*
    units[FRIGATE].speed     = 0;
    units[FRIGATE].turnspeed = 1;

    units[FRIGATE].sight      = 3;
    units[FRIGATE].hp         = 9999;
    */

    //units[FRIGATE].icon = ICON_FRIGATE;
    unitInfos[FRIGATE].configured = true;

    // Unit        : Sonic Tank
    // Description : Sonic tank (using sound waves to destroy)
    unitInfos[SONICTANK].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SONICTANK"));      // pointer to the original 8bit bitmap
    unitInfos[SONICTANK].shadow = m_gfxdata->getTexture(m_gfxdata->getTextureID("UNIT_SONICTANK_SHADOW"));      // pointer to the original 8bit bitmap
    unitInfos[SONICTANK].bmp_width = 16 * 2;
    unitInfos[SONICTANK].bmp_height = 16 * 2;
    unitInfos[SONICTANK].bmp_startpixel = 0;
    unitInfos[SONICTANK].bmp_frames = 1; // no extra frames
    unitInfos[SONICTANK].bulletType = BULLET_SHIMMER;
    unitInfos[SONICTANK].icon = m_gfxinter->getTextureID("ICON_UNIT_SONICTANK");
    unitInfos[SONICTANK].listType = eListType::LIST_UNITS;
    unitInfos[SONICTANK].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[SONICTANK].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[SONICTANK].smokeHpFactor = 0.5f;
    unitInfos[SONICTANK].canGuard = true;
    unitInfos[SONICTANK].canAttackUnits = true;

    unitInfos[SONICTANK].name = "Sonic Tank";
    unitInfos[SONICTANK].configured = true;


    // Unit        : Single Soldier
    // Description : 1 soldier
    unitInfos[SOLDIER].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SOLDIER"));      // pointer to the original 8bit bitmap
    unitInfos[SOLDIER].bmp_width = 16 * 2;
    unitInfos[SOLDIER].bmp_height = 16 * 2;
    unitInfos[SOLDIER].bmp_startpixel = 0;
    unitInfos[SOLDIER].bmp_frames = 3; // 2 extra frames
    unitInfos[SOLDIER].infantry = true;
    unitInfos[SOLDIER].bulletType = BULLET_SMALL;
    unitInfos[SOLDIER].icon = m_gfxinter->getTextureID("ICON_UNIT_SOLDIER");
    unitInfos[SOLDIER].squish = false;
    unitInfos[SOLDIER].canBeSquished = true;
    unitInfos[SOLDIER].listType = eListType::LIST_FOOT_UNITS;
    unitInfos[SOLDIER].subListId = SUBLIST_INFANTRY;
    unitInfos[SOLDIER].canEnterAndDamageStructure = true;
    unitInfos[SOLDIER].attackIsEnterStructure = false;
    unitInfos[SOLDIER].damageOnEnterStructure = 10.0f;
    unitInfos[SOLDIER].canGuard = true;
    unitInfos[SOLDIER].canAttackUnits = true;
    unitInfos[SOLDIER].name = "Soldier";
    unitInfos[SOLDIER].configured = true;


    // Unit        : Infantry
    // Description : 3 soldiers
    unitInfos[INFANTRY].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SOLDIERS"));      // pointer to the original 8bit bitmap
    unitInfos[INFANTRY].bmp_width = 16 * 2;
    unitInfos[INFANTRY].bmp_height = 16 * 2;
    unitInfos[INFANTRY].bmp_startpixel = 0;
    unitInfos[INFANTRY].bmp_frames = 3; // 2 extra frames
    unitInfos[INFANTRY].speed = 8;
    unitInfos[INFANTRY].infantry = true;
    unitInfos[INFANTRY].bulletType = BULLET_SMALL;
    unitInfos[INFANTRY].fireTwice = true;
    unitInfos[INFANTRY].icon = m_gfxinter->getTextureID("ICON_UNIT_INFANTRY");
    unitInfos[INFANTRY].squish = false;
    unitInfos[INFANTRY].canBeSquished = true;
    unitInfos[INFANTRY].listType = eListType::LIST_FOOT_UNITS;
    unitInfos[INFANTRY].subListId = SUBLIST_INFANTRY;
    unitInfos[INFANTRY].canEnterAndDamageStructure = true;
    unitInfos[INFANTRY].attackIsEnterStructure = false;
    unitInfos[INFANTRY].damageOnEnterStructure = 25.0f;
    unitInfos[INFANTRY].canGuard = true;
    unitInfos[INFANTRY].canAttackUnits = true;
    unitInfos[INFANTRY].name = "Light Infantry";
    unitInfos[INFANTRY].configured = true;

    // Unit        : Single Trooper
    // Description : 1 trooper
    unitInfos[TROOPER].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_TROOPER"));      // pointer to the original 8bit bitmap
    unitInfos[TROOPER].bmp_width = 16 * 2;
    unitInfos[TROOPER].bmp_height = 16 * 2;
    unitInfos[TROOPER].bmp_startpixel = 0;
    unitInfos[TROOPER].bmp_frames = 3; // 2 extra frames
    unitInfos[TROOPER].name = "Trooper";
    unitInfos[TROOPER].infantry = true;
    unitInfos[TROOPER].bulletType = ROCKET_SMALL;
    unitInfos[TROOPER].icon = m_gfxinter->getTextureID("ICON_UNIT_TROOPER");
    unitInfos[TROOPER].listType = eListType::LIST_FOOT_UNITS;
    unitInfos[TROOPER].subListId = SUBLIST_TROOPERS;
    unitInfos[TROOPER].squish = false;
    unitInfos[TROOPER].canBeSquished = true;
    unitInfos[TROOPER].canAttackAirUnits = true;
    unitInfos[TROOPER].canEnterAndDamageStructure = true;
    unitInfos[TROOPER].attackIsEnterStructure = false;
    unitInfos[TROOPER].damageOnEnterStructure = 12.0f;
    unitInfos[TROOPER].bulletTypeSecondary = BULLET_SMALL;
    unitInfos[TROOPER].fireSecondaryWithinRange = 2;
    unitInfos[TROOPER].canGuard = true;
    unitInfos[TROOPER].canAttackUnits = true;
    unitInfos[TROOPER].configured = true;

    // Unit        : Group Trooper
    // Description : 3 troopers
    unitInfos[TROOPERS].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_TROOPERS"));      // pointer to the original 8bit bitmap
    unitInfos[TROOPERS].bmp_width = 16 * 2;
    unitInfos[TROOPERS].bmp_height = 16 * 2;
    unitInfos[TROOPERS].bmp_startpixel = 0;
    unitInfos[TROOPERS].bmp_frames = 3; // 2 extra frames
    unitInfos[TROOPERS].name = "Troopers";
    unitInfos[TROOPERS].icon = m_gfxinter->getTextureID("ICON_UNIT_TROOPERS");
    unitInfos[TROOPERS].bulletType = ROCKET_SMALL;
    unitInfos[TROOPERS].fireTwice = true;
    unitInfos[TROOPERS].infantry = true;
    unitInfos[TROOPERS].listType = eListType::LIST_FOOT_UNITS;
    unitInfos[TROOPERS].subListId = SUBLIST_TROOPERS;
    unitInfos[TROOPERS].squish = false;
    unitInfos[TROOPERS].canBeSquished = true;
    unitInfos[TROOPERS].canAttackAirUnits = true;
    unitInfos[TROOPERS].canEnterAndDamageStructure = true;
    unitInfos[TROOPERS].attackIsEnterStructure = false;
    unitInfos[TROOPERS].damageOnEnterStructure = 35.0f;
    unitInfos[TROOPERS].bulletTypeSecondary = BULLET_SMALL;
    unitInfos[TROOPERS].fireSecondaryWithinRange = 2;
    unitInfos[TROOPERS].canGuard = true;
    unitInfos[TROOPERS].canAttackUnits = true;
    unitInfos[TROOPERS].configured = true;

    // Unit        : Fremen
    // Description : A single fremen
    unitInfos[UNIT_FREMEN_ONE].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SINGLEFREMEN"));      // pointer to the original 8bit bitmap
    unitInfos[UNIT_FREMEN_ONE].bmp_width = 16 * 2;
    unitInfos[UNIT_FREMEN_ONE].bmp_height = 16 * 2;
    unitInfos[UNIT_FREMEN_ONE].bmp_startpixel = 0;
    unitInfos[UNIT_FREMEN_ONE].bmp_frames = 3; // 2 extra frames
    unitInfos[UNIT_FREMEN_ONE].name = "Fremen (1)";
    unitInfos[UNIT_FREMEN_ONE].icon = m_gfxinter->getTextureID("ICON_SPECIAL_FREMEN");
    unitInfos[UNIT_FREMEN_ONE].bulletType = ROCKET_SMALL_FREMEN;
    unitInfos[UNIT_FREMEN_ONE].fireTwice = false;
    unitInfos[UNIT_FREMEN_ONE].infantry = true;
    unitInfos[UNIT_FREMEN_ONE].squish = false;
    unitInfos[UNIT_FREMEN_ONE].canBeSquished = true;
    unitInfos[UNIT_FREMEN_ONE].canAttackAirUnits = true;
    unitInfos[UNIT_FREMEN_ONE].canGuard = true;
    unitInfos[UNIT_FREMEN_ONE].canAttackUnits = true;
    unitInfos[UNIT_FREMEN_ONE].configured = true;

//  units[UNIT_FREMEN_ONE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_ONE].subListId=0;

    // Unit        : Fremen
    // Description : A group of Fremen
    unitInfos[UNIT_FREMEN_THREE].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_TRIPLEFREMEN"));      // pointer to the original 8bit bitmap
    unitInfos[UNIT_FREMEN_THREE].bmp_width = 16 * 2;
    unitInfos[UNIT_FREMEN_THREE].bmp_height = 16 * 2;
    unitInfos[UNIT_FREMEN_THREE].bmp_startpixel = 0;
    unitInfos[UNIT_FREMEN_THREE].bmp_frames = 3; // 2 extra frames
    unitInfos[UNIT_FREMEN_THREE].name = "Fremen (3)";
    unitInfos[UNIT_FREMEN_THREE].icon = m_gfxinter->getTextureID("ICON_SPECIAL_FREMEN");
    unitInfos[UNIT_FREMEN_THREE].bulletType = ROCKET_SMALL_FREMEN;
    unitInfos[UNIT_FREMEN_THREE].fireTwice = true;
    unitInfos[UNIT_FREMEN_THREE].infantry = true;
    unitInfos[UNIT_FREMEN_THREE].squish = false;
    unitInfos[UNIT_FREMEN_THREE].canBeSquished = true;
    unitInfos[UNIT_FREMEN_THREE].canAttackAirUnits = true;
    unitInfos[UNIT_FREMEN_THREE].canGuard = true;
    unitInfos[UNIT_FREMEN_THREE].canAttackUnits = true;
    unitInfos[UNIT_FREMEN_THREE].configured = true;
//  units[UNIT_FREMEN_THREE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_THREE].subListId=0;

    // Unit        : Saboteur
    // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
    unitInfos[SABOTEUR].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SABOTEUR"));
    unitInfos[SABOTEUR].buildTime = 1000;
    unitInfos[SABOTEUR].bmp_width = 16 * 2;
    unitInfos[SABOTEUR].bmp_height = 16 * 2;
    unitInfos[SABOTEUR].bmp_startpixel = 0;
    unitInfos[SABOTEUR].bmp_frames = 3; // 2 extra frames
    unitInfos[SABOTEUR].speed = 0; // very fast
    unitInfos[SABOTEUR].hp = 60;   // quite some health
    unitInfos[SABOTEUR].cost = 0;
    unitInfos[SABOTEUR].sight = 4; // immense sight! (sorta scouting guys)
    unitInfos[SABOTEUR].range = 2;
    unitInfos[SABOTEUR].attack_frequency = 0;
    unitInfos[SABOTEUR].turnspeed = 0; // very fast
    unitInfos[SABOTEUR].name = "Saboteur";
    unitInfos[SABOTEUR].icon = m_gfxinter->getTextureID("ICON_SPECIAL_SABOTEUR");
    unitInfos[SABOTEUR].squish = false;
    unitInfos[SABOTEUR].canBeSquished = true;
    unitInfos[SABOTEUR].infantry = true; // infantry unit, so it can be squished
    unitInfos[SABOTEUR].listType = eListType::LIST_PALACE;
    unitInfos[SABOTEUR].subListId = 0;
    unitInfos[SABOTEUR].canEnterAndDamageStructure = true;
    unitInfos[SABOTEUR].attackIsEnterStructure = true;
    unitInfos[SABOTEUR].damageOnEnterStructure = 9999.99f; // a lot of damage (instant destroy)
    unitInfos[SABOTEUR].configured = true;

    // Unit        : Sandworm
    unitInfos[SANDWORM].speed = 3; // very fast
    unitInfos[SANDWORM].bmp = m_gfxdata->getSurface(m_gfxdata->getSurfaceID("UNIT_SANDWORM"));
    unitInfos[SANDWORM].hp = 9999; // set in game.ini to a more sane amount
    unitInfos[SANDWORM].dieWhenLowerThanHP = 1000;
    unitInfos[SANDWORM].appetite = 10;
    unitInfos[SANDWORM].eatSound = m_audio->indexOf("SOUND_WORM");
    unitInfos[SANDWORM].bmp_width = 48;
    unitInfos[SANDWORM].bmp_height = 48;
    unitInfos[SANDWORM].turnspeed = 0; // very fast
    unitInfos[SANDWORM].sight = 16;
    unitInfos[SANDWORM].name = "Sandworm";
    unitInfos[SANDWORM].icon = m_gfxinter->getTextureID("ICON_UNIT_SANDWORM");
    unitInfos[SANDWORM].squish = false;
    unitInfos[SANDWORM].canGuard = true;
    unitInfos[SANDWORM].canAttackUnits = true;
    unitInfos[SANDWORM].configured = true;

    // Infantry is squished, light vehicles go "boom" (heavier vehicles pick from soundpools::tankDie)
    const audioID squishSound = m_audio->indexOf("SOUND_SQUISH");
    for (const int type : {SOLDIER, TROOPER, UNIT_FREMEN_ONE, TROOPERS, INFANTRY, UNIT_FREMEN_THREE}) {
        unitInfos[type].deathSound = squishSound;
    }
    const audioID trikeDieSound = m_audio->indexOf("SOUND_TRIKEDIE");
    for (const int type : {TRIKE, RAIDER, QUAD}) {
        unitInfos[type].deathSound = trikeDieSound;
    }
}

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

void cInfoContextCreator::initSpecials(cSpecialInfos& specialInfos)
{

    for (int i = 0; i < MAX_SPECIALTYPES; i++) {
        specialInfos[i].icon = TextureID::invalid;
        specialInfos[i].providesType = eBuildType::UNIT;
        specialInfos[i].buildTime = 0;
        specialInfos[i].deployFrom = eDeployFromType::AT_RANDOM_CELL;
        specialInfos[i].deployAtStructure = -1;
        specialInfos[i].units = 0;
        specialInfos[i].house = eHouseBitFlag::Unknown;
        specialInfos[i].autoBuild = false;
        specialInfos[i].deployTargetType = eDeployTargetType::TARGET_NONE;
        specialInfos[i].deployTargetPrecision = 0;
        specialInfos[i].listType = eListType::LIST_NONE;
        specialInfos[i].subListId = -1;
        specialInfos[i].description = "\0";
    }

    // Deploy Saboteur
    specialInfos[SPECIAL_SABOTEUR].icon = m_gfxinter->getTextureID("ICON_SPECIAL_SABOTEUR");
    specialInfos[SPECIAL_SABOTEUR].house=eHouseBitFlag::Ordos;
    specialInfos[SPECIAL_SABOTEUR].autoBuild=true;
    specialInfos[SPECIAL_SABOTEUR].providesType = eBuildType::UNIT;
    specialInfos[SPECIAL_SABOTEUR].providesTypeId = SABOTEUR;
    specialInfos[SPECIAL_SABOTEUR].deployFrom = eDeployFromType::AT_STRUCTURE;
    specialInfos[SPECIAL_SABOTEUR].deployAtStructure = PALACE;
    specialInfos[SPECIAL_SABOTEUR].units = 1;
    specialInfos[SPECIAL_SABOTEUR].buildTime = 2468; // ~ 6 minutes (but times 1.2 to compensate for faster Ordos building = 2468 to get real 6 minutes)
    specialInfos[SPECIAL_SABOTEUR].listType=eListType::LIST_PALACE;
    specialInfos[SPECIAL_SABOTEUR].subListId=0;
    specialInfos[SPECIAL_SABOTEUR].description = "Saboteur";

    // Deploy Fremen
    specialInfos[SPECIAL_FREMEN].icon = m_gfxinter->getTextureID("ICON_SPECIAL_FREMEN");
    specialInfos[SPECIAL_FREMEN].house=eHouseBitFlag::Atreides;
    specialInfos[SPECIAL_FREMEN].autoBuild=true;
    specialInfos[SPECIAL_FREMEN].providesType = eBuildType::UNIT;
    specialInfos[SPECIAL_FREMEN].providesTypeId = UNIT_FREMEN_THREE;
    specialInfos[SPECIAL_FREMEN].deployFrom = eDeployFromType::AT_RANDOM_CELL;
    specialInfos[SPECIAL_FREMEN].deployAtStructure = PALACE; // This is not used with AT_RANDOM_CELL ...
    specialInfos[SPECIAL_FREMEN].units = 6; // ... but this is
    specialInfos[SPECIAL_FREMEN].buildTime = 1371; // ~ 4 minutes (atreides has baseline build times, ie = real time)
    specialInfos[SPECIAL_FREMEN].listType=eListType::LIST_PALACE;
    specialInfos[SPECIAL_FREMEN].subListId=0;
    specialInfos[SPECIAL_FREMEN].description = "Fremen";

    // Launch Death Hand
    specialInfos[SPECIAL_DEATHHAND].icon = m_gfxinter->getTextureID("ICON_SPECIAL_MISSILE");
    specialInfos[SPECIAL_DEATHHAND].house = Harkonnen | Sardaukar;
    specialInfos[SPECIAL_DEATHHAND].autoBuild=true;
    specialInfos[SPECIAL_DEATHHAND].providesType = eBuildType::BULLET;
    specialInfos[SPECIAL_DEATHHAND].providesTypeId = ROCKET_BIG;
    specialInfos[SPECIAL_DEATHHAND].deployFrom = eDeployFromType::AT_STRUCTURE; // the rocket is fired FROM ...
    specialInfos[SPECIAL_DEATHHAND].deployAtStructure = PALACE; // ... the palace
    specialInfos[SPECIAL_DEATHHAND].deployTargetType = eDeployTargetType::TARGET_INACCURATE_CELL;
    specialInfos[SPECIAL_DEATHHAND].units = 1;
    specialInfos[SPECIAL_DEATHHAND].buildTime = 3428; // ~ 10 minutes with base line (Atreides difficulty)
    // (342.8 = ~ 1 minute) -> harkonnen is done * 1.2 so it becomes 12 minutes real-time which is ok)
    // considering the Dune 2 Insider guide mentions 11 to 12 minutes for Harkonnen.

    specialInfos[SPECIAL_DEATHHAND].deployTargetPrecision = 6;
    specialInfos[SPECIAL_DEATHHAND].listType=eListType::LIST_PALACE;
    specialInfos[SPECIAL_DEATHHAND].subListId=0;
    specialInfos[SPECIAL_DEATHHAND].description = "Death Hand";

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

void cInfoContextCreator::installTerrain(s_TerrainInfo* terrainInfo)
{
    terrainInfo->bloomTimerDuration = 200;
    terrainInfo->terrainSpiceMinSpice = 50;
    terrainInfo->terrainSpiceMaxSpice = 125;
    terrainInfo->terrainSpiceHillMinSpice = 75;
    terrainInfo->terrainSpiceHillMaxSpice = 150;
    terrainInfo->terrainWallHp = 100;
}
