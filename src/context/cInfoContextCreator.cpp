#include "context/cInfoContextCreator.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/cSpecialInfos.h"
#include "gameobjects/cUpgradeInfo.h"
#include "gameobjects/units/cUnitInfos.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "data/gfxaudio.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "sidebar/cSideBar.h"
#include "utils/Graphics.hpp"
#include "utils/common.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/sTerrainInfo.h"

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
    initStructures(*structureInfos);
    return structureInfos;
}

std::unique_ptr<cBulletInfos> cInfoContextCreator::createBulletInfos() {
    auto bulletInfos = std::make_unique<cBulletInfos>();
    initBullets(*bulletInfos);
    return bulletInfos;
}

std::unique_ptr<cSpecialInfos> cInfoContextCreator::createSpecialInfos() {
    auto specialInfos = std::make_unique<cSpecialInfos>();
    initSpecials(*specialInfos);
    return specialInfos;
}

std::unique_ptr<cUpgradeInfos> cInfoContextCreator::createUpgradeInfos() {
    auto upgradeInfos = std::make_unique<cUpgradeInfos>();
    initUpgrades(*upgradeInfos);
    return upgradeInfos;
}

std::unique_ptr<s_TerrainInfo> cInfoContextCreator::createTerrainInfos() {
    auto terrainInfo = std::make_unique<s_TerrainInfo>();
    installTerrain(terrainInfo.get());
    return terrainInfo;
}

/********************************
 House Rules
 ********************************/

/*****************************
 Unit Rules
 *****************************/
void cInfoContextCreator::initUnits(cUnitInfos& unitInfos)
{
    logbook("Installing:  UNITS");
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
        unitInfo.bmp = gfxdata->getSurface(UNIT_QUAD); // default bitmap is a quad!
        unitInfo.top = nullptr;  // no top
        unitInfo.shadow = nullptr;  // no shadow (deliverd with picture itself)
        unitInfo.bmp_width = 0;
        unitInfo.bmp_height = 0;
        unitInfo.turnspeed = 0;
        unitInfo.speed = 0;
        unitInfo.icon = -1;
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

        // capturing / damage upon entering structure related
        unitInfo.canEnterAndDamageStructure = false;
        unitInfo.attackIsEnterStructure = false;
        unitInfo.damageOnEnterStructure = 0.0f;

        // spawn smoke when damaged?
        unitInfo.renderSmokeOnUnitWhenThresholdMet = false;
        unitInfo.smokeHpFactor = 0.0f;

        unitInfo.canGuard = false;

        strcpy(unitInfo.name, "\0");
    }

    // Unit        : CarryAll
    // Description : CarryAll, the flying pickuptruck
    unitInfos[CARRYALL].bmp = gfxdata->getSurface(UNIT_CARRYALL);      // pointer to the original 8bit bitmap
    unitInfos[CARRYALL].shadow = gfxdata->getTexture(UNIT_CARRYALL_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[CARRYALL].bmp_width = 24 * 2;
    unitInfos[CARRYALL].bmp_height = 24 * 2;
    unitInfos[CARRYALL].bmp_startpixel = 0;
    unitInfos[CARRYALL].bmp_frames = 2; // we have at max 1 extra frame
    unitInfos[CARRYALL].icon = ICON_UNIT_CARRYALL;
    unitInfos[CARRYALL].airborn = true;   // is airborn
    unitInfos[CARRYALL].free_roam = true; // may freely roam the air
    unitInfos[CARRYALL].listType = eListType::LIST_UNITS;
    unitInfos[CARRYALL].subListId = SUBLIST_HIGHTECH;
    strcpy(unitInfos[CARRYALL].name, "Carry-All");

    // Unit        : Ornithopter
    // Description : Pesty little aircraft shooting bastard
    unitInfos[ORNITHOPTER].bmp = gfxdata->getSurface(UNIT_ORNITHOPTER);      // pointer to the original 8bit bitmap
    unitInfos[ORNITHOPTER].shadow = gfxdata->getTexture(UNIT_ORNITHOPTER_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[ORNITHOPTER].bmp_width = 24 * 2;
    unitInfos[ORNITHOPTER].bmp_height = 24 * 2;
    unitInfos[ORNITHOPTER].bmp_startpixel = 0;
    unitInfos[ORNITHOPTER].bmp_frames = 4; // we have at max 3 extra frames
    unitInfos[ORNITHOPTER].icon = ICON_UNIT_ORNITHOPTER;
    unitInfos[ORNITHOPTER].bulletType = ROCKET_SMALL_ORNI;
    unitInfos[ORNITHOPTER].fireTwice = true;
    unitInfos[ORNITHOPTER].airborn = true;   // is airborn
    unitInfos[ORNITHOPTER].squish = false;   // can't squish infantry
    unitInfos[ORNITHOPTER].free_roam = true; // may freely roam the air
    unitInfos[ORNITHOPTER].listType = eListType::LIST_UNITS;
    unitInfos[ORNITHOPTER].subListId = SUBLIST_HIGHTECH;
//    unitInfos[ORNITHOPTER].canAttackAirUnits = true; // orni's can attack other air units
    strcpy(unitInfos[ORNITHOPTER].name, "Ornithopter");

    // Unit        : Devastator
    // Description : Devastator
    unitInfos[DEVASTATOR].bmp = gfxdata->getSurface(UNIT_DEVASTATOR);      // pointer to the original 8bit bitmap
    unitInfos[DEVASTATOR].shadow = gfxdata->getTexture(UNIT_DEVASTATOR_SHADOW);      // pointer to the original bitmap shadow
    unitInfos[DEVASTATOR].bmp_width = 19 * 2;
    unitInfos[DEVASTATOR].bmp_startpixel = 0;
    unitInfos[DEVASTATOR].bmp_height = 23 * 2;
    unitInfos[DEVASTATOR].bmp_frames = 1;
    unitInfos[DEVASTATOR].bulletType = BULLET_DEVASTATOR;
    unitInfos[DEVASTATOR].fireTwice = true;
    unitInfos[DEVASTATOR].icon = ICON_UNIT_DEVASTATOR;
    unitInfos[DEVASTATOR].listType = eListType::LIST_UNITS;
    unitInfos[DEVASTATOR].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[DEVASTATOR].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[DEVASTATOR].smokeHpFactor = 0.5f;
    unitInfos[DEVASTATOR].canGuard = true;
    unitInfos[DEVASTATOR].canAttackUnits = true;
    strcpy(unitInfos[DEVASTATOR].name, "Devastator");

    // Unit        : Harvester
    // Description : Harvester
    unitInfos[HARVESTER].bmp = gfxdata->getSurface(UNIT_HARVESTER);      // pointer to the original 8bit bitmap
    unitInfos[HARVESTER].shadow = gfxdata->getTexture(UNIT_HARVESTER_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[HARVESTER].bmp_width = 40 * 2;
    unitInfos[HARVESTER].bmp_startpixel = 24;
    unitInfos[HARVESTER].bmp_height = 26 * 2;
    unitInfos[HARVESTER].bmp_frames = 4;
    unitInfos[HARVESTER].icon = ICON_UNIT_HARVESTER;
    unitInfos[HARVESTER].credit_capacity = 700;
    unitInfos[HARVESTER].harvesting_amount = 5;
    unitInfos[HARVESTER].listType = eListType::LIST_UNITS;
    unitInfos[HARVESTER].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[HARVESTER].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[HARVESTER].smokeHpFactor = 0.5f;
    strcpy(unitInfos[HARVESTER].name, "Harvester");

    // Unit        : Combattank
    // Description : Combattank
    unitInfos[TANK].bmp = gfxdata->getSurface(UNIT_TANKBASE);      // pointer to the original 8bit bitmap
    unitInfos[TANK].shadow = gfxdata->getTexture(UNIT_TANKBASE_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[TANK].top = gfxdata->getSurface(UNIT_TANKTOP);      // pointer to the original 8bit bitmap
    unitInfos[TANK].bmp_width = 16 * 2;
    unitInfos[TANK].bmp_startpixel = 0;
    unitInfos[TANK].bmp_height = 16 * 2;
    unitInfos[TANK].bmp_frames = 0;
    unitInfos[TANK].bulletType = BULLET_TANK;
    unitInfos[TANK].icon = ICON_UNIT_TANK;
    unitInfos[TANK].listType = eListType::LIST_UNITS;
    unitInfos[TANK].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[TANK].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[TANK].smokeHpFactor = 0.5f;
    unitInfos[TANK].canGuard = true;
    unitInfos[TANK].canAttackUnits = true;
    strcpy(unitInfos[TANK].name, "Tank");


    // Unit        : Siege Tank
    // Description : Siege tank
    unitInfos[SIEGETANK].bmp = gfxdata->getSurface(UNIT_SIEGEBASE);      // pointer to the original 8bit bitmap
    unitInfos[SIEGETANK].shadow = gfxdata->getTexture(UNIT_SIEGEBASE_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[SIEGETANK].top = gfxdata->getSurface(UNIT_SIEGETOP);      // pointer to the original 8bit bitmap
    unitInfos[SIEGETANK].bmp_width = 18 * 2;
    unitInfos[SIEGETANK].bmp_startpixel = 1;
    unitInfos[SIEGETANK].bmp_height = 18 * 2;
    unitInfos[SIEGETANK].bmp_frames = 0;
    unitInfos[SIEGETANK].bulletType = BULLET_SIEGE;
    unitInfos[SIEGETANK].fireTwice = true;
    unitInfos[SIEGETANK].icon = ICON_UNIT_SIEGETANK;
    unitInfos[SIEGETANK].listType = eListType::LIST_UNITS;
    unitInfos[SIEGETANK].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[SIEGETANK].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[SIEGETANK].smokeHpFactor = 0.5f;
    unitInfos[SIEGETANK].canGuard = true;
    unitInfos[SIEGETANK].canAttackUnits = true;
    strcpy(unitInfos[SIEGETANK].name, "Siege Tank");

    // Unit        : MCV
    // Description : Movable Construction Vehicle
    unitInfos[MCV].bmp = gfxdata->getSurface(UNIT_MCV);      // pointer to the original 8bit bitmap
    unitInfos[MCV].shadow = gfxdata->getTexture(UNIT_MCV_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[MCV].bmp_width = 24 * 2;
    unitInfos[MCV].bmp_startpixel = 0;
    unitInfos[MCV].bmp_height = 25 * 2;
    unitInfos[MCV].bmp_frames = 1;
    unitInfos[MCV].icon = ICON_UNIT_MCV;
    unitInfos[MCV].listType = eListType::LIST_UNITS;
    unitInfos[MCV].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[MCV].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[MCV].smokeHpFactor = 0.5f;
    strcpy(unitInfos[MCV].name, "MCV");


    // Unit        : Deviator
    // Description : Deviator
    unitInfos[DEVIATOR].bmp = gfxdata->getSurface(UNIT_DEVIATOR);      // pointer to the original 8bit bitmap
    unitInfos[DEVIATOR].bmp_width = 16 * 2;
    unitInfos[DEVIATOR].bmp_height = 16 * 2;
    unitInfos[DEVIATOR].bmp_startpixel = 0;
    unitInfos[DEVIATOR].bmp_frames = 1;
    unitInfos[DEVIATOR].icon = ICON_UNIT_DEVIATOR;
    unitInfos[DEVIATOR].bulletType = BULLET_GAS; // our gassy rocket
    unitInfos[DEVIATOR].listType = eListType::LIST_UNITS;
    unitInfos[DEVIATOR].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[DEVIATOR].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[DEVIATOR].smokeHpFactor = 0.5f;
    unitInfos[DEVIATOR].canGuard = true;
    unitInfos[DEVIATOR].canAttackUnits = true;
    strcpy(unitInfos[DEVIATOR].name, "Deviator");

    // Unit        : Launcher
    // Description : Rocket Launcher
    unitInfos[LAUNCHER].bmp = gfxdata->getSurface(UNIT_LAUNCHER);      // pointer to the original 8bit bitmap
    unitInfos[LAUNCHER].shadow = gfxdata->getTexture(UNIT_LAUNCHER_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[LAUNCHER].bmp_width = 16 * 2;
    unitInfos[LAUNCHER].bmp_height = 16 * 2;
    unitInfos[LAUNCHER].bmp_startpixel = 0;
    unitInfos[LAUNCHER].bmp_frames = 1;
    unitInfos[LAUNCHER].icon = ICON_UNIT_LAUNCHER;
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
    strcpy(unitInfos[LAUNCHER].name, "Launcher");

    // Unit        : Quad
    // Description : Quad, 4 wheeled (double gunned)
    unitInfos[QUAD].bmp = gfxdata->getSurface(UNIT_QUAD);      // pointer to the original 8bit bitmap
    unitInfos[QUAD].shadow = gfxdata->getTexture(UNIT_QUAD_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[QUAD].bmp_width = 16 * 2;
    unitInfos[QUAD].bmp_height = 16 * 2;
    unitInfos[QUAD].bmp_startpixel = 0;
    unitInfos[QUAD].bmp_frames = 1;
    unitInfos[QUAD].icon = ICON_UNIT_QUAD;
    unitInfos[QUAD].fireTwice = true;
    unitInfos[QUAD].bulletType = BULLET_QUAD;
    unitInfos[QUAD].squish = false;
    unitInfos[QUAD].listType = eListType::LIST_UNITS;
    unitInfos[QUAD].subListId = SUBLIST_LIGHTFCTRY;
    unitInfos[QUAD].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[QUAD].smokeHpFactor = 0.5f;
    unitInfos[QUAD].canGuard = true;
    unitInfos[QUAD].canAttackUnits = true;
    strcpy(unitInfos[QUAD].name, "Quad");


    // Unit        : Trike (normal trike)
    // Description : Trike, 3 wheeled (single gunned)
    unitInfos[TRIKE].bmp = gfxdata->getSurface(UNIT_TRIKE);      // pointer to the original 8bit bitmap
    unitInfos[TRIKE].shadow = gfxdata->getTexture(UNIT_TRIKE_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[TRIKE].bmp_width = 28;
    unitInfos[TRIKE].bmp_height = 26;
    unitInfos[TRIKE].bmp_startpixel = 0;
    unitInfos[TRIKE].bmp_frames = 1;
    unitInfos[TRIKE].icon = ICON_UNIT_TRIKE;
    unitInfos[TRIKE].bulletType = BULLET_TRIKE;
    unitInfos[TRIKE].squish = false;
    unitInfos[TRIKE].listType = eListType::LIST_UNITS;
    unitInfos[TRIKE].subListId = SUBLIST_LIGHTFCTRY;
    unitInfos[TRIKE].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[TRIKE].smokeHpFactor = 0.5f;
    unitInfos[TRIKE].canGuard = true;
    unitInfos[TRIKE].canAttackUnits = true;
    strcpy(unitInfos[TRIKE].name, "Trike");

    // Unit        : Raider Trike (Ordos trike)
    // Description : Raider Trike, 3 wheeled (single gunned), weaker, but faster
    unitInfos[RAIDER].bmp = gfxdata->getSurface(UNIT_TRIKE);      // pointer to the original 8bit bitmap
    unitInfos[RAIDER].shadow = gfxdata->getTexture(UNIT_TRIKE_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[RAIDER].bmp_width = 14 * 2;
    unitInfos[RAIDER].bmp_height = 14 * 2;
    unitInfos[RAIDER].bmp_startpixel = 0;
    unitInfos[RAIDER].bmp_frames = 1;
    strcpy(unitInfos[RAIDER].name, "Raider Trike");
    unitInfos[RAIDER].icon = ICON_UNIT_RAIDER;
    unitInfos[RAIDER].bulletType = BULLET_TRIKE;
    unitInfos[RAIDER].squish = false;
    unitInfos[RAIDER].listType = eListType::LIST_UNITS;
    unitInfos[RAIDER].subListId = SUBLIST_LIGHTFCTRY;
    unitInfos[RAIDER].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[RAIDER].smokeHpFactor = 0.5f;
    unitInfos[RAIDER].canGuard = true;
    unitInfos[RAIDER].canAttackUnits = true;

    // Unit        : Frigate
    // Description : Frigate
    unitInfos[FRIGATE].bmp = gfxdata->getSurface(UNIT_FRIGATE);      // pointer to the original 8bit bitmap
    unitInfos[FRIGATE].shadow = gfxdata->getTexture(UNIT_FRIGATE_SHADOW);      // pointer to the original 8bit bitmap
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
    strcpy(unitInfos[FRIGATE].name, "Frigate");

    /*
    units[FRIGATE].speed     = 0;
    units[FRIGATE].turnspeed = 1;

    units[FRIGATE].sight      = 3;
    units[FRIGATE].hp         = 9999;
    */

    //units[FRIGATE].icon = ICON_FRIGATE;

    // Unit        : Sonic Tank
    // Description : Sonic tank (using sound waves to destroy)
    unitInfos[SONICTANK].bmp = gfxdata->getSurface(UNIT_SONICTANK);      // pointer to the original 8bit bitmap
    unitInfos[SONICTANK].shadow = gfxdata->getTexture(UNIT_SONICTANK_SHADOW);      // pointer to the original 8bit bitmap
    unitInfos[SONICTANK].bmp_width = 16 * 2;
    unitInfos[SONICTANK].bmp_height = 16 * 2;
    unitInfos[SONICTANK].bmp_startpixel = 0;
    unitInfos[SONICTANK].bmp_frames = 1; // no extra frames
    unitInfos[SONICTANK].bulletType = BULLET_SHIMMER;
    unitInfos[SONICTANK].icon = ICON_UNIT_SONICTANK;
    unitInfos[SONICTANK].listType = eListType::LIST_UNITS;
    unitInfos[SONICTANK].subListId = SUBLIST_HEAVYFCTRY;
    unitInfos[SONICTANK].renderSmokeOnUnitWhenThresholdMet = true;
    unitInfos[SONICTANK].smokeHpFactor = 0.5f;
    unitInfos[SONICTANK].canGuard = true;
    unitInfos[SONICTANK].canAttackUnits = true;

    strcpy(unitInfos[SONICTANK].name, "Sonic Tank");


    // Unit        : Single Soldier
    // Description : 1 soldier
    unitInfos[SOLDIER].bmp = gfxdata->getSurface(UNIT_SOLDIER);      // pointer to the original 8bit bitmap
    unitInfos[SOLDIER].bmp_width = 16 * 2;
    unitInfos[SOLDIER].bmp_height = 16 * 2;
    unitInfos[SOLDIER].bmp_startpixel = 0;
    unitInfos[SOLDIER].bmp_frames = 3; // 2 extra frames
    unitInfos[SOLDIER].infantry = true;
    unitInfos[SOLDIER].bulletType = BULLET_SMALL;
    unitInfos[SOLDIER].icon = ICON_UNIT_SOLDIER;
    unitInfos[SOLDIER].squish = false;
    unitInfos[SOLDIER].canBeSquished = true;
    unitInfos[SOLDIER].listType = eListType::LIST_FOOT_UNITS;
    unitInfos[SOLDIER].subListId = SUBLIST_INFANTRY;
    unitInfos[SOLDIER].canEnterAndDamageStructure = true;
    unitInfos[SOLDIER].attackIsEnterStructure = false;
    unitInfos[SOLDIER].damageOnEnterStructure = 10.0f;
    unitInfos[SOLDIER].canGuard = true;
    unitInfos[SOLDIER].canAttackUnits = true;
    strcpy(unitInfos[SOLDIER].name, "Soldier");


    // Unit        : Infantry
    // Description : 3 soldiers
    unitInfos[INFANTRY].bmp = gfxdata->getSurface(UNIT_SOLDIERS);      // pointer to the original 8bit bitmap
    unitInfos[INFANTRY].bmp_width = 16 * 2;
    unitInfos[INFANTRY].bmp_height = 16 * 2;
    unitInfos[INFANTRY].bmp_startpixel = 0;
    unitInfos[INFANTRY].bmp_frames = 3; // 2 extra frames
    unitInfos[INFANTRY].speed = 8;
    unitInfos[INFANTRY].infantry = true;
    unitInfos[INFANTRY].bulletType = BULLET_SMALL;
    unitInfos[INFANTRY].fireTwice = true;
    unitInfos[INFANTRY].icon = ICON_UNIT_INFANTRY;
    unitInfos[INFANTRY].squish = false;
    unitInfos[INFANTRY].canBeSquished = true;
    unitInfos[INFANTRY].listType = eListType::LIST_FOOT_UNITS;
    unitInfos[INFANTRY].subListId = SUBLIST_INFANTRY;
    unitInfos[INFANTRY].canEnterAndDamageStructure = true;
    unitInfos[INFANTRY].attackIsEnterStructure = false;
    unitInfos[INFANTRY].damageOnEnterStructure = 25.0f;
    unitInfos[INFANTRY].canGuard = true;
    unitInfos[INFANTRY].canAttackUnits = true;
    strcpy(unitInfos[INFANTRY].name, "Light Infantry");

    // Unit        : Single Trooper
    // Description : 1 trooper
    unitInfos[TROOPER].bmp = gfxdata->getSurface(UNIT_TROOPER);      // pointer to the original 8bit bitmap
    unitInfos[TROOPER].bmp_width = 16 * 2;
    unitInfos[TROOPER].bmp_height = 16 * 2;
    unitInfos[TROOPER].bmp_startpixel = 0;
    unitInfos[TROOPER].bmp_frames = 3; // 2 extra frames
    strcpy(unitInfos[TROOPER].name, "Trooper");
    unitInfos[TROOPER].infantry = true;
    unitInfos[TROOPER].bulletType = ROCKET_SMALL;
    unitInfos[TROOPER].icon = ICON_UNIT_TROOPER;
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

    // Unit        : Group Trooper
    // Description : 3 troopers
    unitInfos[TROOPERS].bmp = gfxdata->getSurface(UNIT_TROOPERS);      // pointer to the original 8bit bitmap
    unitInfos[TROOPERS].bmp_width = 16 * 2;
    unitInfos[TROOPERS].bmp_height = 16 * 2;
    unitInfos[TROOPERS].bmp_startpixel = 0;
    unitInfos[TROOPERS].bmp_frames = 3; // 2 extra frames
    strcpy(unitInfos[TROOPERS].name, "Troopers");
    unitInfos[TROOPERS].icon = ICON_UNIT_TROOPERS;
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

    // Unit        : Fremen
    // Description : A single fremen
    unitInfos[UNIT_FREMEN_ONE].bmp = gfxdata->getSurface(UNIT_SINGLEFREMEN);      // pointer to the original 8bit bitmap
    unitInfos[UNIT_FREMEN_ONE].bmp_width = 16 * 2;
    unitInfos[UNIT_FREMEN_ONE].bmp_height = 16 * 2;
    unitInfos[UNIT_FREMEN_ONE].bmp_startpixel = 0;
    unitInfos[UNIT_FREMEN_ONE].bmp_frames = 3; // 2 extra frames
    strcpy(unitInfos[UNIT_FREMEN_ONE].name, "Fremen (1)");
    unitInfos[UNIT_FREMEN_ONE].icon = ICON_SPECIAL_FREMEN;
    unitInfos[UNIT_FREMEN_ONE].bulletType = ROCKET_SMALL_FREMEN;
    unitInfos[UNIT_FREMEN_ONE].fireTwice = false;
    unitInfos[UNIT_FREMEN_ONE].infantry = true;
    unitInfos[UNIT_FREMEN_ONE].squish = false;
    unitInfos[UNIT_FREMEN_ONE].canBeSquished = true;
    unitInfos[UNIT_FREMEN_ONE].canAttackAirUnits = true;
    unitInfos[UNIT_FREMEN_ONE].canGuard = true;
    unitInfos[UNIT_FREMEN_ONE].canAttackUnits = true;

//  units[UNIT_FREMEN_ONE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_ONE].subListId=0;

    // Unit        : Fremen
    // Description : A group of Fremen
    unitInfos[UNIT_FREMEN_THREE].bmp = gfxdata->getSurface(UNIT_TRIPLEFREMEN);      // pointer to the original 8bit bitmap
    unitInfos[UNIT_FREMEN_THREE].bmp_width = 16 * 2;
    unitInfos[UNIT_FREMEN_THREE].bmp_height = 16 * 2;
    unitInfos[UNIT_FREMEN_THREE].bmp_startpixel = 0;
    unitInfos[UNIT_FREMEN_THREE].bmp_frames = 3; // 2 extra frames
    strcpy(unitInfos[UNIT_FREMEN_THREE].name, "Fremen (3)");
    unitInfos[UNIT_FREMEN_THREE].icon = ICON_SPECIAL_FREMEN;
    unitInfos[UNIT_FREMEN_THREE].bulletType = ROCKET_SMALL_FREMEN;
    unitInfos[UNIT_FREMEN_THREE].fireTwice = true;
    unitInfos[UNIT_FREMEN_THREE].infantry = true;
    unitInfos[UNIT_FREMEN_THREE].squish = false;
    unitInfos[UNIT_FREMEN_THREE].canBeSquished = true;
    unitInfos[UNIT_FREMEN_THREE].canAttackAirUnits = true;
    unitInfos[UNIT_FREMEN_THREE].canGuard = true;
    unitInfos[UNIT_FREMEN_THREE].canAttackUnits = true;
//  units[UNIT_FREMEN_THREE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_THREE].subListId=0;

    // Unit        : Saboteur
    // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
    unitInfos[SABOTEUR].bmp = gfxdata->getSurface(UNIT_SABOTEUR);
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
    strcpy(unitInfos[SABOTEUR].name, "Saboteur");
    unitInfos[SABOTEUR].icon = ICON_SPECIAL_SABOTEUR;
    unitInfos[SABOTEUR].squish = false;
    unitInfos[SABOTEUR].canBeSquished = true;
    unitInfos[SABOTEUR].infantry = true; // infantry unit, so it can be squished
    unitInfos[SABOTEUR].listType = eListType::LIST_PALACE;
    unitInfos[SABOTEUR].subListId = 0;
    unitInfos[SABOTEUR].canEnterAndDamageStructure = true;
    unitInfos[SABOTEUR].attackIsEnterStructure = true;
    unitInfos[SABOTEUR].damageOnEnterStructure = 9999.99f; // a lot of damage (instant destroy)

    // Unit        : Sandworm
    unitInfos[SANDWORM].speed = 3; // very fast
    unitInfos[SANDWORM].bmp = gfxdata->getSurface(UNIT_SANDWORM);
    unitInfos[SANDWORM].hp = 9999; // set in game.ini to a more sane amount
    unitInfos[SANDWORM].dieWhenLowerThanHP = 1000;
    unitInfos[SANDWORM].appetite = 10;
    unitInfos[SANDWORM].bmp_width = 48;
    unitInfos[SANDWORM].bmp_height = 48;
    unitInfos[SANDWORM].turnspeed = 0; // very fast
    unitInfos[SANDWORM].sight = 16;
    strcpy(unitInfos[SANDWORM].name, "Sandworm");
    unitInfos[SANDWORM].icon = ICON_UNIT_SANDWORM;
    unitInfos[SANDWORM].squish = false;
    unitInfos[SANDWORM].canGuard = true;
    unitInfos[SANDWORM].canAttackUnits = true;
}

void cInfoContextCreator::initParticles(cParticleInfos& particleInfos)
{
    for (unsigned int i = 0; i < particleInfos.size(); i++) {
        s_ParticleInfo &particleInfo = particleInfos[i];
        particleInfo.bmpIndex = -1;
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
    particleInfos[D2TM_PARTICLE_MOVE].bmpIndex = MOVE_INDICATOR;
    particleInfos[D2TM_PARTICLE_MOVE].startAlpha = 128;

    // attack
    particleInfos[D2TM_PARTICLE_ATTACK].bmpIndex = ATTACK_INDICATOR;
    particleInfos[D2TM_PARTICLE_ATTACK].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].bmpIndex = EXPLOSION_TRIKE;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_SMOKE].bmpIndex = OBJECT_SMOKE;
    particleInfos[D2TM_PARTICLE_SMOKE].startAlpha = 0;
    particleInfos[D2TM_PARTICLE_SMOKE].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SMOKE].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].bmpIndex = OBJECT_SMOKE;
    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].startAlpha = 0;
    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].bmpIndex = OBJECT_SMOKE_SHADOW;
    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].startAlpha = 0;
    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].frameWidth = 36;
    particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].frameHeight = 38;

    particleInfos[D2TM_PARTICLE_TRACK_DIA].bmpIndex = TRACK_DIA;
    particleInfos[D2TM_PARTICLE_TRACK_DIA].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_DIA].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_TRACK_HOR].bmpIndex = TRACK_HOR;
    particleInfos[D2TM_PARTICLE_TRACK_HOR].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_HOR].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_TRACK_VER].bmpIndex = TRACK_VER;
    particleInfos[D2TM_PARTICLE_TRACK_VER].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_VER].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_TRACK_DIA2].bmpIndex = TRACK_DIA2;
    particleInfos[D2TM_PARTICLE_TRACK_DIA2].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_TRACK_DIA2].startAlpha = 128;

    particleInfos[D2TM_PARTICLE_BULLET_PUF].bmpIndex = BULLET_PUF;
    particleInfos[D2TM_PARTICLE_BULLET_PUF].frameWidth = 18;
    particleInfos[D2TM_PARTICLE_BULLET_PUF].frameHeight = 18;


    particleInfos[D2TM_PARTICLE_EXPLOSION_FIRE].bmpIndex = EXPLOSION_FIRE;

    particleInfos[D2TM_PARTICLE_WORMEAT].bmpIndex = OBJECT_WORMEAT;
    particleInfos[D2TM_PARTICLE_WORMEAT].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_WORMEAT].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_WORMEAT].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].bmpIndex = EXPLOSION_TANK_ONE;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].bmpIndex = EXPLOSION_TANK_TWO;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].bmpIndex = EXPLOSION_STRUCTURE01;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].bmpIndex = EXPLOSION_STRUCTURE02;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].bmpIndex = EXPLOSION_GAS;
    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_WORMTRAIL].bmpIndex = OBJECT_WORMTRAIL;
    particleInfos[D2TM_PARTICLE_WORMTRAIL].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_WORMTRAIL].startAlpha = 96;
    particleInfos[D2TM_PARTICLE_WORMTRAIL].frameWidth = 48;
    particleInfos[D2TM_PARTICLE_WORMTRAIL].frameHeight = 48;

    particleInfos[D2TM_PARTICLE_DEADINF01].bmpIndex = OBJECT_DEADINF01;
    particleInfos[D2TM_PARTICLE_DEADINF01].layer = RenderLayerParticle::BOTTOM;

    particleInfos[D2TM_PARTICLE_DEADINF02].bmpIndex = OBJECT_DEADINF02;
    particleInfos[D2TM_PARTICLE_DEADINF02].layer = RenderLayerParticle::BOTTOM;

    particleInfos[D2TM_PARTICLE_TANKSHOOT].bmpIndex = OBJECT_TANKSHOOT;
    particleInfos[D2TM_PARTICLE_TANKSHOOT].startAlpha = 128;
    particleInfos[D2TM_PARTICLE_TANKSHOOT].frameWidth = 64;
    particleInfos[D2TM_PARTICLE_TANKSHOOT].frameHeight = 64;

    particleInfos[D2TM_PARTICLE_SIEGESHOOT].bmpIndex = OBJECT_SIEGESHOOT;
    particleInfos[D2TM_PARTICLE_SIEGESHOOT].startAlpha = 128;
    particleInfos[D2TM_PARTICLE_SIEGESHOOT].frameWidth = 64;
    particleInfos[D2TM_PARTICLE_SIEGESHOOT].frameHeight = 64;

    particleInfos[D2TM_PARTICLE_SQUISH01].bmpIndex = EXPLOSION_SQUISH01;
    particleInfos[D2TM_PARTICLE_SQUISH01].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_SQUISH01].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_SQUISH01].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SQUISH01].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_SQUISH02].bmpIndex = EXPLOSION_SQUISH02;
    particleInfos[D2TM_PARTICLE_SQUISH02].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_SQUISH02].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_SQUISH02].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SQUISH02].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_SQUISH03].bmpIndex = EXPLOSION_SQUISH03;
    particleInfos[D2TM_PARTICLE_SQUISH03].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_SQUISH03].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_SQUISH03].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_SQUISH03].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].bmpIndex = EXPLOSION_ORNI;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].startAlpha = 255;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].frameWidth = 32;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].frameHeight = 32;

    particleInfos[D2TM_PARTICLE_SIEGEDIE].bmpIndex = OBJECT_SIEGEDIE;

    particleInfos[D2TM_PARTICLE_CARRYPUFF].bmpIndex = OBJECT_CARRYPUFF;
    particleInfos[D2TM_PARTICLE_CARRYPUFF].layer = RenderLayerParticle::BOTTOM;
    particleInfos[D2TM_PARTICLE_CARRYPUFF].frameWidth = 96;
    particleInfos[D2TM_PARTICLE_CARRYPUFF].frameHeight = 96;

    particleInfos[D2TM_PARTICLE_EXPLOSION_ROCKET].bmpIndex = EXPLOSION_ROCKET;
    particleInfos[D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL].bmpIndex = EXPLOSION_ROCKET_SMALL;

    particleInfos[D2TM_PARTICLE_EXPLOSION_BULLET].bmpIndex = EXPLOSION_BULLET;

    // the nice flare/light effects that come with explosions
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].bmpIndex = OBJECT_BOOM01;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].usesAdditiveBlending = true;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].startAlpha = 240;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].frameWidth = 512;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].frameHeight = 512;

    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].bmpIndex = OBJECT_BOOM02;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].usesAdditiveBlending = true;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].startAlpha = 230;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].frameWidth = 256;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].frameHeight = 256;

    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].bmpIndex = OBJECT_BOOM03;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].usesAdditiveBlending = true;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].startAlpha = 220;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].frameWidth = 128;
    particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].frameHeight = 128;
}

void cInfoContextCreator::initSpecials(cSpecialInfos& specialInfos)
{

    for (int i = 0; i < MAX_SPECIALTYPES; i++) {
        specialInfos[i].icon = -1;
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
        strcpy(specialInfos[i].description, "\0");
    }

    // Deploy Saboteur
    specialInfos[SPECIAL_SABOTEUR].icon = ICON_SPECIAL_SABOTEUR;
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
    strcpy(specialInfos[SPECIAL_SABOTEUR].description, "Saboteur");

    // Deploy Fremen
    specialInfos[SPECIAL_FREMEN].icon = ICON_SPECIAL_FREMEN;
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
    strcpy(specialInfos[SPECIAL_FREMEN].description, "Fremen");

    // Launch Death Hand
    specialInfos[SPECIAL_DEATHHAND].icon = ICON_SPECIAL_MISSILE;
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
    strcpy(specialInfos[SPECIAL_DEATHHAND].description, "Death Hand");

}


/****************
 Install bullets
 ****************/
void cInfoContextCreator::initBullets(cBulletInfos& bulletInfos)
{
    logbook("Installing:  BULLET TYPES");

    for (int i = 0; i < bulletInfos.size(); i++) {
        bulletInfos[i].bmp = nullptr; // in case an invalid bitmap; default is a small rocket
        bulletInfos[i].moveSpeed = 2;
        bulletInfos[i].deathParticle = -1; // this points to a bitmap (in data file, using index)
        bulletInfos[i].damage_vehicles = 0;      // damage to vehicles
        bulletInfos[i].damage_infantry = 0;  // damage to infantry
        bulletInfos[i].max_frames = 1;  // 1 frame animation
        bulletInfos[i].max_deadframes = 4; // 4 frame animation
        bulletInfos[i].smokeParticle = -1; // by default no smoke particle is spawned
        bulletInfos[i].bmp_width = 8 * 2;
        bulletInfos[i].sound = -1;    // no sound
        bulletInfos[i].explosionSize = 1; // 1 tile sized explosion
        bulletInfos[i].deviateProbability = 0; // no probability of deviating a unit
        bulletInfos[i].groundBullet = false; // if true, then it gets blocked by walls, mountains or structures. False == flying bullets, ie, rockets
        bulletInfos[i].canDamageAirUnits = false; // if true, then upon impact the bullet can also damage air units
        bulletInfos[i].canDamageGround = false; // if true, then upon impact the bullet can also damage ground (walls, slabs, etc)
        strcpy(bulletInfos[i].description, "Unknown");
    }

    // huge rocket/missile
    bulletInfos[ROCKET_BIG].bmp = gfxdata->getTexture(BULLET_ROCKET_LARGE);
    bulletInfos[ROCKET_BIG].deathParticle = D2TM_PARTICLE_EXPLOSION_STRUCTURE01;
    bulletInfos[ROCKET_BIG].bmp_width = 48;
    bulletInfos[ROCKET_BIG].damage_vehicles = 999;
    bulletInfos[ROCKET_BIG].damage_infantry = 999;
    bulletInfos[ROCKET_BIG].max_frames = 1;
    bulletInfos[ROCKET_BIG].sound = SOUND_ROCKET;
    bulletInfos[ROCKET_BIG].explosionSize = 7;
    bulletInfos[ROCKET_BIG].canDamageAirUnits = true;
    bulletInfos[ROCKET_BIG].canDamageGround = true;
    bulletInfos[ROCKET_BIG].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(bulletInfos[ROCKET_BIG].description, "ROCKET_BIG");

    // small rocket (for ornithopter)
    bulletInfos[ROCKET_SMALL_ORNI].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    bulletInfos[ROCKET_SMALL_ORNI].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    bulletInfos[ROCKET_SMALL_ORNI].bmp_width = 16;
    bulletInfos[ROCKET_SMALL_ORNI].damage_vehicles = 48; // they can do pretty damage
    bulletInfos[ROCKET_SMALL_ORNI].damage_infantry = 24;
    bulletInfos[ROCKET_SMALL_ORNI].max_frames = 1;
    bulletInfos[ROCKET_SMALL_ORNI].sound = SOUND_ROCKET_SMALL;
    bulletInfos[ROCKET_SMALL_ORNI].canDamageAirUnits = true;
    bulletInfos[ROCKET_SMALL_ORNI].max_deadframes = 1;
    bulletInfos[ROCKET_SMALL_ORNI].canDamageGround = true;
    bulletInfos[ROCKET_SMALL_ORNI].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(bulletInfos[ROCKET_SMALL_ORNI].description, "ROCKET_SMALL_ORNI");

    // small rocket
    bulletInfos[ROCKET_SMALL].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    bulletInfos[ROCKET_SMALL].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    bulletInfos[ROCKET_SMALL].bmp_width = 16;
    bulletInfos[ROCKET_SMALL].damage_vehicles = 10; // was 8
    bulletInfos[ROCKET_SMALL].damage_infantry = 8; // was 4
    bulletInfos[ROCKET_SMALL].max_frames = 1;
    bulletInfos[ROCKET_SMALL].moveSpeed = 3;
    bulletInfos[ROCKET_SMALL].sound = SOUND_ROCKET_SMALL;
    bulletInfos[ROCKET_SMALL].canDamageAirUnits = true;
    bulletInfos[ROCKET_SMALL].max_deadframes = 1;
    bulletInfos[ROCKET_SMALL].canDamageGround = true;
//    bullets[ROCKET_SMALL].smokeParticle = BULLET_PUF; // small rockets have no smoke trail yet
    strcpy(bulletInfos[ROCKET_SMALL].description, "ROCKET_SMALL");

    // small rocket - fremen rocket
    bulletInfos[ROCKET_SMALL_FREMEN].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    bulletInfos[ROCKET_SMALL_FREMEN].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    bulletInfos[ROCKET_SMALL_FREMEN].bmp_width = 16;
    bulletInfos[ROCKET_SMALL_FREMEN].damage_vehicles = 22;
    bulletInfos[ROCKET_SMALL_FREMEN].damage_infantry = 20;
    bulletInfos[ROCKET_SMALL_FREMEN].max_frames = 1;
    bulletInfos[ROCKET_SMALL_FREMEN].moveSpeed = 4;
    bulletInfos[ROCKET_SMALL_FREMEN].sound = SOUND_ROCKET_SMALL;
    bulletInfos[ROCKET_SMALL_FREMEN].max_deadframes = 1;
    bulletInfos[ROCKET_SMALL_FREMEN].canDamageAirUnits = true;
    bulletInfos[ROCKET_SMALL_FREMEN].canDamageGround = true;
//    bullets[ROCKET_SMALL_FREMEN].smokeParticle = true; // not yet
    strcpy(bulletInfos[ROCKET_SMALL_FREMEN].description, "ROCKET_SMALL_FREMEN");

    // normal rocket
    bulletInfos[ROCKET_NORMAL].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    bulletInfos[ROCKET_NORMAL].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET;
    bulletInfos[ROCKET_NORMAL].bmp_width = 32;
    bulletInfos[ROCKET_NORMAL].damage_vehicles = 76;
    bulletInfos[ROCKET_NORMAL].damage_infantry = 36;  // less damage on infantry
    bulletInfos[ROCKET_NORMAL].max_frames = 1;
    bulletInfos[ROCKET_NORMAL].sound = SOUND_ROCKET;
    bulletInfos[ROCKET_NORMAL].moveSpeed = 3;
    bulletInfos[ROCKET_NORMAL].max_deadframes = 4;
    bulletInfos[ROCKET_NORMAL].canDamageAirUnits = true;
    bulletInfos[ROCKET_NORMAL].canDamageGround = true;
    bulletInfos[ROCKET_NORMAL].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(bulletInfos[ROCKET_NORMAL].description, "ROCKET_NORMAL");

    // soldier shot
    bulletInfos[BULLET_SMALL].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    bulletInfos[BULLET_SMALL].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    bulletInfos[BULLET_SMALL].bmp_width = 6;
    bulletInfos[BULLET_SMALL].damage_vehicles = 4; // vehicles are no match
    bulletInfos[BULLET_SMALL].damage_infantry = 10; // infantry vs infantry means big time damage
    bulletInfos[BULLET_SMALL].max_frames = 0;
    bulletInfos[BULLET_SMALL].moveSpeed = 4;
    bulletInfos[BULLET_SMALL].sound = SOUND_GUN;
    bulletInfos[BULLET_SMALL].max_deadframes = 0;
    bulletInfos[BULLET_SMALL].groundBullet = true;
    strcpy(bulletInfos[BULLET_SMALL].description, "BULLET_SMALL");

    // trike shot
    bulletInfos[BULLET_TRIKE].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    bulletInfos[BULLET_TRIKE].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    bulletInfos[BULLET_TRIKE].bmp_width = 6;
    bulletInfos[BULLET_TRIKE].damage_vehicles = 3; // trikes do not do much damage to vehicles
    bulletInfos[BULLET_TRIKE].damage_infantry = 6; // but more to infantry
    bulletInfos[BULLET_TRIKE].moveSpeed = 4;
    bulletInfos[BULLET_TRIKE].max_frames = 0;
    bulletInfos[BULLET_TRIKE].sound = SOUND_MACHINEGUN;
    bulletInfos[BULLET_TRIKE].max_deadframes = 0;
    bulletInfos[BULLET_TRIKE].groundBullet = true;
    strcpy(bulletInfos[BULLET_TRIKE].description, "BULLET_TRIKE");

    // quad shot
    bulletInfos[BULLET_QUAD].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    bulletInfos[BULLET_QUAD].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    bulletInfos[BULLET_QUAD].bmp_width = 6;
    bulletInfos[BULLET_QUAD].damage_vehicles = 6;
    bulletInfos[BULLET_QUAD].damage_infantry = 8; // bigger impact on infantry
    bulletInfos[BULLET_QUAD].max_frames = 0;
    bulletInfos[BULLET_QUAD].sound = SOUND_MACHINEGUN;
    bulletInfos[BULLET_QUAD].max_deadframes = 0;
    bulletInfos[BULLET_QUAD].groundBullet = true;
    strcpy(bulletInfos[BULLET_QUAD].description, "BULLET_QUAD");

    // normal tank shot
    bulletInfos[BULLET_TANK].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    bulletInfos[BULLET_TANK].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    bulletInfos[BULLET_TANK].bmp_width = 8;
    bulletInfos[BULLET_TANK].damage_vehicles = 12;
    bulletInfos[BULLET_TANK].damage_infantry = 4;  // infantry is not much damaged
    bulletInfos[BULLET_TANK].max_frames = 0;
    bulletInfos[BULLET_TANK].sound = SOUND_EXPL_ROCKET;
    bulletInfos[BULLET_TANK].max_deadframes = 1;
    bulletInfos[BULLET_TANK].groundBullet = true;
    bulletInfos[BULLET_TANK].canDamageGround = true;
    strcpy(bulletInfos[BULLET_TANK].description, "BULLET_TANK");

    // siege tank shot
    bulletInfos[BULLET_SIEGE].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    bulletInfos[BULLET_SIEGE].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    bulletInfos[BULLET_SIEGE].bmp_width = 8;
    bulletInfos[BULLET_SIEGE].damage_vehicles = 24;
    bulletInfos[BULLET_SIEGE].damage_infantry = 6; // infantry is not as much damaged
    bulletInfos[BULLET_SIEGE].max_frames = 0;
    bulletInfos[BULLET_SIEGE].sound = SOUND_EXPL_ROCKET;
    bulletInfos[BULLET_SIEGE].max_deadframes = 2;
    bulletInfos[BULLET_SIEGE].groundBullet = true;
    bulletInfos[BULLET_SIEGE].canDamageGround = true;
    strcpy(bulletInfos[BULLET_SIEGE].description, "BULLET_SIEGE");

    // devastator shot
    bulletInfos[BULLET_DEVASTATOR].bmp = gfxdata->getTexture(BULLET_DOT_LARGE);
    bulletInfos[BULLET_DEVASTATOR].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL; // not used anyway
    bulletInfos[BULLET_DEVASTATOR].bmp_width = 8;
    bulletInfos[BULLET_DEVASTATOR].damage_vehicles = 30;
    bulletInfos[BULLET_DEVASTATOR].damage_infantry = 12; // infantry again not much damaged
    bulletInfos[BULLET_DEVASTATOR].max_frames = 0;
    bulletInfos[BULLET_DEVASTATOR].sound = SOUND_EXPL_ROCKET;
    bulletInfos[BULLET_DEVASTATOR].max_deadframes = 1;
    bulletInfos[BULLET_DEVASTATOR].groundBullet = true;
    bulletInfos[BULLET_DEVASTATOR].canDamageGround = true;
    strcpy(bulletInfos[BULLET_DEVASTATOR].description, "BULLET_DEVASTATOR");

    // Gas rocket of a deviator
    bulletInfos[BULLET_GAS].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    bulletInfos[BULLET_GAS].deathParticle = D2TM_PARTICLE_EXPLOSION_GAS;
    bulletInfos[BULLET_GAS].bmp_width = 32;
    bulletInfos[BULLET_GAS].damage_vehicles = 1;
    bulletInfos[BULLET_GAS].damage_infantry = 1;
    bulletInfos[BULLET_GAS].max_frames = 1;
    bulletInfos[BULLET_GAS].max_deadframes = 4;
    bulletInfos[BULLET_GAS].sound = SOUND_ROCKET;
    bulletInfos[BULLET_GAS].deviateProbability = 34; // 1 out of 3(ish) should be effective
    bulletInfos[BULLET_GAS].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(bulletInfos[BULLET_GAS].description, "BULLET_GAS");

    // normal turret shot
    bulletInfos[BULLET_TURRET].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    bulletInfos[BULLET_TURRET].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    bulletInfos[BULLET_TURRET].bmp_width = 8;
    bulletInfos[BULLET_TURRET].damage_vehicles = 12;
    bulletInfos[BULLET_TURRET].damage_infantry = 12; // infantry is a hard target
    bulletInfos[BULLET_TURRET].max_frames = 0;
    bulletInfos[BULLET_TURRET].max_deadframes = 1;
    bulletInfos[BULLET_TURRET].sound = SOUND_GUNTURRET;
    bulletInfos[BULLET_TURRET].groundBullet = false; // this can fly over structures, walls, mountains, yes!
    bulletInfos[BULLET_TURRET].canDamageGround = true;
    strcpy(bulletInfos[BULLET_TURRET].description, "BULLET_TURRET");

    // EXEPTION: Shimmer/ Sonic tank
    bulletInfos[BULLET_SHIMMER].bmp = nullptr;
    bulletInfos[BULLET_SHIMMER].deathParticle = -1;
    bulletInfos[BULLET_SHIMMER].bmp_width = 0;
    bulletInfos[BULLET_SHIMMER].damage_vehicles = 55;
    bulletInfos[BULLET_SHIMMER].damage_infantry = 70; // infantry cant stand the sound, die very fast
    bulletInfos[BULLET_SHIMMER].max_frames = 0;
    bulletInfos[BULLET_SHIMMER].max_deadframes = 0;
    bulletInfos[BULLET_SHIMMER].sound = SOUND_SHIMMER;
    strcpy(bulletInfos[BULLET_SHIMMER].description, "BULLET_SHIMMER");

    // rocket of rocket turret
    bulletInfos[ROCKET_RTURRET].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    bulletInfos[ROCKET_RTURRET].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    bulletInfos[ROCKET_RTURRET].bmp_width = 16 * 2;
    bulletInfos[ROCKET_RTURRET].damage_vehicles = 25;
    bulletInfos[ROCKET_RTURRET].damage_infantry = 10; // infantry is a bit tougher
    bulletInfos[ROCKET_RTURRET].max_frames = 1;
    bulletInfos[ROCKET_RTURRET].sound = SOUND_ROCKET;
    bulletInfos[ROCKET_RTURRET].max_deadframes = 4;
    bulletInfos[ROCKET_RTURRET].canDamageAirUnits = true;
    bulletInfos[ROCKET_RTURRET].canDamageGround = true;
    bulletInfos[ROCKET_RTURRET].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(bulletInfos[ROCKET_RTURRET].description, "ROCKET_RTURRET");
}

void cInfoContextCreator::initUpgrades(cUpgradeInfos& upgradeInfos)
{
    logbook("Installing:  UPGRADES");
    for (int i = 0; i < MAX_UPGRADETYPES; i++) {
        upgradeInfos[i].enabled = false;
        upgradeInfos[i].techLevel = -1;
        upgradeInfos[i].house = 0;
        upgradeInfos[i].needsStructureType = -1;
        upgradeInfos[i].icon = ICON_STR_PALACE;
        upgradeInfos[i].cost = 100;
        upgradeInfos[i].atUpgradeLevel = -1;
        upgradeInfos[i].structureType = CONSTYARD;
        upgradeInfos[i].providesTypeId = -1;
        upgradeInfos[i].providesType = STRUCTURE;
        upgradeInfos[i].providesTypeList = eListType::LIST_NONE;
        upgradeInfos[i].providesTypeSubList = -1;
        upgradeInfos[i].buildTime = 5;
        strcpy(upgradeInfos[i].description, "Upgrade");
    }

    // CONSTYARD UPGRADES

    // First upgrade Constyard: 4Slabs
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].enabled = true;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].house = Atreides | Harkonnen | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].techLevel = 4; // start from mission 4
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].icon = ICON_STR_4SLAB;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].cost = 200;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].structureType = CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesType = STRUCTURE;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeId = SLAB4;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeList = eListType::LIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeSubList = SUBLIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].buildTime = 50;
    strcpy(upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].description, "Build 4 concrete slabs at once");

    // Second upgrade Constyard: Rturret
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].enabled = true;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].house = Atreides | Harkonnen | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].techLevel = 6;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].icon = ICON_STR_RTURRET;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].cost = 200;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].needsStructureType = RADAR;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].structureType = CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].atUpgradeLevel = 1;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesType = STRUCTURE;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeId = RTURRET;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeList = eListType::LIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeSubList = SUBLIST_CONSTYARD;
    upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].buildTime = 150;
    strcpy(upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].description, "Build Rocket Turret");

    // LIGHTFACTORY UPGRADES, only for ATREIDES and ORDOS
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].enabled = true;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].house = Atreides | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].icon = ICON_UNIT_QUAD;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].cost = game.m_infoContext.getStructureInfo(LIGHTFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].needsStructureType = LIGHTFACTORY;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].structureType = LIGHTFACTORY;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeId = QUAD;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeSubList =  SUBLIST_LIGHTFCTRY;
    upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].buildTime = 150;
    strcpy(upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].description, "Build Quad at Light Factory");

    // HEAVYFACTORY UPGRADES:

    // ALL HOUSES GET MVC
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].house = Atreides | Ordos | Harkonnen | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].techLevel = 4;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].icon = ICON_UNIT_MCV;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost = game.m_infoContext.getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeId = MCV;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].buildTime = 150;
    strcpy(upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].description, "Build MCV at Heavy Factory");

    // Harkonnen/Atreides only
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].house = Atreides | Harkonnen | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].techLevel = 5;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].icon = ICON_UNIT_LAUNCHER;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].cost = game.m_infoContext.getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].atUpgradeLevel = 1; // requires MCV upgrade first
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeId = LAUNCHER;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].buildTime = 150;
    strcpy(upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].description, "Build Rocket Launcher at Heavy Factory");

    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].house = Atreides | Harkonnen | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].techLevel = 6;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].icon = ICON_UNIT_SIEGETANK;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].cost = game.m_infoContext.getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].atUpgradeLevel = 2; // After upgrade to Rocket Launcher
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeId = SIEGETANK;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].buildTime = 150;
    strcpy(upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].description, "Build Siege Tank at Heavy Factory");

    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].house = Ordos;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].techLevel = 6;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].icon = ICON_UNIT_SIEGETANK;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].cost = game.m_infoContext.getStructureInfo(HEAVYFACTORY).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].needsStructureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].structureType = HEAVYFACTORY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].atUpgradeLevel = 1;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeId = SIEGETANK;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].buildTime = 150;
    strcpy(upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].description, "Build Siege Tank at Heavy Factory");

    // HI-TECH UPGRADES (Ordos/Atreides only)
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].enabled = true;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].house = Atreides | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].techLevel = 8;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].icon = ICON_UNIT_ORNITHOPTER;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].cost = game.m_infoContext.getStructureInfo(HIGHTECH).cost / 2;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].structureType = HIGHTECH;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeId = ORNITHOPTER;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeList = eListType::LIST_UNITS;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeSubList = SUBLIST_HIGHTECH;
    upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].buildTime = 150;
    strcpy(upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].description, "Build Ornithopter at Hi-Tech");

    // WOR (Harkonnen & Ordos)
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].enabled = true;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].house = Harkonnen | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].icon = ICON_UNIT_TROOPERS;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].cost = game.m_infoContext.getStructureInfo(WOR).cost / 2;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].structureType = WOR;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeId = TROOPERS;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeList = eListType::LIST_FOOT_UNITS;
    upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeSubList = SUBLIST_TROOPERS;
    if (!game.isCheatMode()) {
        upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].buildTime = 150;
    }
    strcpy(upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].description, "Build Troopers at WOR");

    // BARRACKS (Atreides & Ordos)
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].enabled = true;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].house = Atreides | Ordos | Sardaukar;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].techLevel = 3;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].icon = ICON_UNIT_INFANTRY;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].cost = game.m_infoContext.getStructureInfo(BARRACKS).cost / 2;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].structureType = BARRACKS;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].atUpgradeLevel = 0;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesType = UNIT;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeId = INFANTRY;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeList = eListType::LIST_FOOT_UNITS;
    upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeSubList = SUBLIST_INFANTRY;
    if (!game.isCheatMode()) {
        upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].buildTime = 150;
    }
    strcpy(upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].description, "Build Infantry at Barracks");
}

/*****************************
 Structure Rules
 *****************************/
void cInfoContextCreator::initStructures(cStructureInfos& structureInfos)
{

    logbook("Installing:  STRUCTURES");
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        s_StructureInfo &structureInfo = structureInfos[i];
        structureInfo.bmp = gfxdata->getSurface(BUILD_WINDTRAP); // in case an invalid bitmap, we are a windtrap
        structureInfo.shadow = nullptr;
        structureInfo.flash = nullptr;
        structureInfo.flags = std::vector<s_FlagInfo>();
        structureInfo.sight = 1;
        structureInfo.bmp_width = 32 * 2;
        structureInfo.bmp_height = 32 * 2;
        structureInfo.hp = 1; // low health
        structureInfo.fixhp = -1; // no fixing hp yet
        structureInfo.fadecol = -1;
        structureInfo.fademax = -1;
        structureInfo.power_drain = 0;       // the power that this building drains...
        structureInfo.power_give = 0;        // the power that this building gives...
        structureInfo.cost = 0;
        structureInfo.icon = -1; // stupid default icon
        structureInfo.buildTime = 0;
        structureInfo.list = -1; // no list attached
        structureInfo.queuable = false;
        structureInfo.hasConcrete = true;
        structureInfo.configured = false;
        structureInfo.canAttackAirUnits = false;
        structureInfo.canAttackGroundUnits = false;
        structureInfo.uponDestructionSpawnUnitAmountMin = 0;
        structureInfo.uponDestructionSpawnUnitAmountMax = 0;
        structureInfo.uponDestructionSpawnUnitType = -1;
        strcpy(structureInfo.name, "Unknown");
    }

    // Single and 4 slabs
    structureInfos[SLAB1].bmp = gfxdata->getSurface(PLACE_SLAB1); // in case an invalid bitmap, we are a windtrap
    structureInfos[SLAB1].icon = ICON_STR_1SLAB;
    structureInfos[SLAB1].hp = 25;            // Not functional in-game, only for building
    structureInfos[SLAB1].bmp_width = 16 * 2;
    structureInfos[SLAB1].bmp_height = 16 * 2;
    structureInfos[SLAB1].configured = true;
    structureInfos[SLAB1].queuable = true;
    strcpy(structureInfos[SLAB1].name, "Concrete Slab");

    structureInfos[SLAB4].bmp = gfxdata->getSurface(PLACE_SLAB4); // in case an invalid bitmap, we are a windtrap
    structureInfos[SLAB4].icon = ICON_STR_4SLAB;
    structureInfos[SLAB4].hp = 75;            // Not functional in-game, only for building
    structureInfos[SLAB4].bmp_width = 32 * 2;
    structureInfos[SLAB4].bmp_height = 32 * 2;
    structureInfos[SLAB4].configured = true;
    structureInfos[SLAB4].queuable = true;
    strcpy(structureInfos[SLAB4].name, "4 Concrete Slabs");


    // Concrete Wall
    structureInfos[WALL].bmp = gfxdata->getSurface(PLACE_WALL); // in case an invalid bitmap, we are a windtrap
    structureInfos[WALL].icon = ICON_STR_WALL;
    structureInfos[WALL].hp = 75;            // Not functional in-game, only for building
    structureInfos[WALL].bmp_width = 16 * 2;
    structureInfos[WALL].bmp_height = 16 * 2;
    structureInfos[WALL].queuable = true;
    structureInfos[WALL].configured = true;
    strcpy(structureInfos[WALL].name, "Concrete Wall");

    // Structure    : Windtrap
    // Description  : <none>
    structureInfos[WINDTRAP].bmp = gfxdata->getSurface(BUILD_WINDTRAP);
    structureInfos[WINDTRAP].shadow = gfxdata->getTexture(BUILD_WINDTRAP_SHADOW); // shadow
    structureInfos[WINDTRAP].fadecol = 128;
    structureInfos[WINDTRAP].fademax = 134;
    structureInfos[WINDTRAP].icon = ICON_STR_WINDTRAP;
    structureInfos[WINDTRAP].configured = true;
    structureInfos[WINDTRAP].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 53,
        .relY = 37
    });
    strcpy(structureInfos[WINDTRAP].name, "Windtrap");

    // Structure    : Heavy Factory
    // Description  : <none>
    structureInfos[HEAVYFACTORY].bmp_width = 48 * 2;
    structureInfos[HEAVYFACTORY].bmp_height = 32 * 2;
    structureInfos[HEAVYFACTORY].bmp = gfxdata->getSurface(BUILD_HEAVYFACTORY);
    structureInfos[HEAVYFACTORY].shadow = gfxdata->getTexture(BUILD_HEAVYFACTORY_SHADOW); // shadow
    structureInfos[HEAVYFACTORY].flash = gfxdata->getSurface(BUILD_HEAVYFACTORY_FLASH);
    structureInfos[HEAVYFACTORY].fadecol = -1;
    structureInfos[HEAVYFACTORY].icon = ICON_STR_HEAVYFACTORY;
    structureInfos[HEAVYFACTORY].configured = true;
    structureInfos[HEAVYFACTORY].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 25,
        .relY = 3
    });
    strcpy(structureInfos[HEAVYFACTORY].name, "Heavy Factory");

    // Structure    : Hight Tech Factory (for aircraft only)
    // Description  : <none>
    structureInfos[HIGHTECH].bmp_width = 48 * 2;
    structureInfos[HIGHTECH].bmp_height = 32 * 2;
    structureInfos[HIGHTECH].bmp = gfxdata->getSurface(BUILD_HIGHTECH);
    structureInfos[HIGHTECH].shadow = gfxdata->getTexture(BUILD_HIGHTECH_SHADOW);
    structureInfos[HIGHTECH].fadecol = -1;
    structureInfos[HIGHTECH].icon = ICON_STR_HIGHTECH;
    structureInfos[HIGHTECH].configured = true;
    structureInfos[HIGHTECH].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 19,
        .relY = 36
    });
    strcpy(structureInfos[HIGHTECH].name, "Hi-Tech");

    // Structure    : Repair
    // Description  : <none>
    structureInfos[REPAIR].bmp_width = 48 * 2;
    structureInfos[REPAIR].bmp_height = 32 * 2;
    structureInfos[REPAIR].bmp = gfxdata->getSurface(BUILD_REPAIR);
    structureInfos[REPAIR].shadow = gfxdata->getTexture(BUILD_REPAIR_SHADOW);
    structureInfos[REPAIR].fadecol = -1;
    structureInfos[REPAIR].icon = ICON_STR_REPAIR;
    structureInfos[REPAIR].configured = true;
    structureInfos[REPAIR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 41,
        .relY = 3
    });
    strcpy(structureInfos[REPAIR].name, "Repair Facility");

    // Structure    : Palace
    // Description  : <none>
    structureInfos[PALACE].bmp_width = 48 * 2;
    structureInfos[PALACE].bmp_height = 48 * 2;
    structureInfos[PALACE].bmp = gfxdata->getSurface(BUILD_PALACE);
    structureInfos[PALACE].shadow = gfxdata->getTexture(BUILD_PALACE_SHADOW);
    structureInfos[PALACE].icon = ICON_STR_PALACE;
    structureInfos[PALACE].configured = true;
    structureInfos[PALACE].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 28,
        .relY = 14
    });
    strcpy(structureInfos[PALACE].name, "Palace");

    // Structure    : Light Factory
    // Description  : <none>
    structureInfos[LIGHTFACTORY].bmp_width = 32 * 2;
    structureInfos[LIGHTFACTORY].bmp_height = 32 * 2;
    structureInfos[LIGHTFACTORY].bmp = gfxdata->getSurface(BUILD_LIGHTFACTORY);
    structureInfos[LIGHTFACTORY].shadow = gfxdata->getTexture(BUILD_LIGHTFACTORY_SHADOW);
    structureInfos[LIGHTFACTORY].flash = gfxdata->getSurface(BUILD_LIGHTFACTORY_FLASH);
    structureInfos[LIGHTFACTORY].fadecol = -1;
    structureInfos[LIGHTFACTORY].icon = ICON_STR_LIGHTFACTORY;
    structureInfos[LIGHTFACTORY].configured = true;
    structureInfos[LIGHTFACTORY].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 41,
        .relY = 2
    });
    strcpy(structureInfos[LIGHTFACTORY].name, "Light Factory");

    // Structure    : Radar
    // Description  : <none>
    structureInfos[RADAR].bmp_width = 32 * 2;
    structureInfos[RADAR].bmp_height = 32 * 2;
    structureInfos[RADAR].bmp = gfxdata->getSurface(BUILD_RADAR);
    structureInfos[RADAR].shadow = gfxdata->getTexture(BUILD_RADAR_SHADOW); // shadow
    structureInfos[RADAR].sight = 12;
    structureInfos[RADAR].fadecol = -1;
    structureInfos[RADAR].icon = ICON_STR_RADAR;
    structureInfos[RADAR].configured = true;
    // outpost has 2 flags
    structureInfos[RADAR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 17,
        .relY = 38
    });
    structureInfos[RADAR].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 12,
        .relY = 46
    });
    strcpy(structureInfos[RADAR].name, "Outpost");

    // Structure    : Barracks
    // Description  : <none>
    structureInfos[BARRACKS].bmp_width = 32 * 2;
    structureInfos[BARRACKS].bmp_height = 32 * 2;
    structureInfos[BARRACKS].bmp = gfxdata->getSurface(BUILD_BARRACKS);
    structureInfos[BARRACKS].shadow = gfxdata->getTexture(BUILD_BARRACKS_SHADOW);
    structureInfos[BARRACKS].fadecol = -1;
    structureInfos[BARRACKS].icon = ICON_STR_BARRACKS;
    structureInfos[BARRACKS].configured = true;
    structureInfos[BARRACKS].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 60,
        .relY = 47
    });
    structureInfos[BARRACKS].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 51,
        .relY = 50
    });
    strcpy(structureInfos[BARRACKS].name, "Barracks");

    // Structure    : WOR
    // Description  : <none>
    structureInfos[WOR].bmp_width = 32 * 2;
    structureInfos[WOR].bmp_height = 32 * 2;
    structureInfos[WOR].bmp = gfxdata->getSurface(BUILD_WOR);
    structureInfos[WOR].shadow = gfxdata->getTexture(BUILD_WOR_SHADOW);
    structureInfos[WOR].fadecol = -1;
    structureInfos[WOR].icon = ICON_STR_WOR;
    structureInfos[WOR].configured = true;
    structureInfos[WOR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 21,
        .relY = 31
    });
    strcpy(structureInfos[WOR].name, "WOR");


    // Structure    : Silo
    // Description  : <none>
    structureInfos[SILO].bmp_width = 32 * 2;
    structureInfos[SILO].bmp_height = 32 * 2;
    structureInfos[SILO].bmp = gfxdata->getSurface(BUILD_SILO);
    structureInfos[SILO].shadow = gfxdata->getTexture(BUILD_SILO_SHADOW);
    structureInfos[SILO].fadecol = -1;
    structureInfos[SILO].icon = ICON_STR_SILO;
    structureInfos[SILO].configured = true;
    structureInfos[SILO].queuable = true;
    structureInfos[SILO].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 43,
        .relY = 20
    });
    strcpy(structureInfos[SILO].name, "Spice Storage Silo");

    // Structure    : Refinery
    // Description  : <none>
    structureInfos[REFINERY].bmp_width = 48 * 2;
    structureInfos[REFINERY].bmp_height = 32 * 2;
    structureInfos[REFINERY].bmp = gfxdata->getSurface(BUILD_REFINERY);
    structureInfos[REFINERY].shadow = gfxdata->getTexture(BUILD_REFINERY_SHADOW);
    structureInfos[REFINERY].fadecol = -1;
    structureInfos[REFINERY].icon = ICON_STR_REFINERY;
    structureInfos[REFINERY].configured = true;
    structureInfos[REFINERY].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 45,
        .relY = 12
    });
    strcpy(structureInfos[REFINERY].name, "Spice Refinery");

    // Structure    : Construction Yard
    // Description  : <none>
    structureInfos[CONSTYARD].bmp_width = 32 * 2;
    structureInfos[CONSTYARD].bmp_height = 32 * 2;
    structureInfos[CONSTYARD].sight = 4;
    structureInfos[CONSTYARD].bmp = gfxdata->getSurface(BUILD_CONSTYARD);
    structureInfos[CONSTYARD].shadow = gfxdata->getTexture(BUILD_CONSTYARD_SHADOW);
    structureInfos[CONSTYARD].hasConcrete = false;
    structureInfos[CONSTYARD].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 14,
        .relY = 14
    });
    structureInfos[CONSTYARD].fadecol = -1;
    structureInfos[CONSTYARD].icon = ICON_STR_CONSTYARD;
    structureInfos[CONSTYARD].configured = true;
    strcpy(structureInfos[CONSTYARD].name, "Construction Yard");

    // Structure    : Starport
    // Description  : You can order units from this structure
    structureInfos[STARPORT].bmp_width = 48 * 2;
    structureInfos[STARPORT].bmp_height = 48 * 2;
    structureInfos[STARPORT].bmp = gfxdata->getSurface(BUILD_STARPORT);
    structureInfos[STARPORT].shadow = gfxdata->getTexture(BUILD_STARPORT_SHADOW);
    structureInfos[STARPORT].fadecol = -1;
    structureInfos[STARPORT].icon = ICON_STR_STARPORT;
    structureInfos[STARPORT].configured = true;
    structureInfos[STARPORT].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 16,
        .relY = 3
    });
    strcpy(structureInfos[STARPORT].name, "Starport");

    // Structure    : House of IX
    // Description  : Makes it possible for the player to upgrade its Heavy Factory in order to build their special weapon
    structureInfos[IX].bmp_width = 32 * 2;
    structureInfos[IX].bmp_height = 32 * 2;
    structureInfos[IX].bmp = gfxdata->getSurface(BUILD_IX);
    structureInfos[IX].shadow = gfxdata->getTexture(BUILD_IX_SHADOW);
    structureInfos[IX].fadecol = -1;
    structureInfos[IX].icon = ICON_STR_IX;
    structureInfos[IX].configured = true;
    structureInfos[IX].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 60, // 60
        .relY = 40  // 40
    });
    strcpy(structureInfos[IX].name, "House of IX");

    // Structure    : Normal Turret
    // Description  : defence
    structureInfos[TURRET].bmp_width = 16 * 2;
    structureInfos[TURRET].bmp_height = 16 * 2;
    structureInfos[TURRET].bmp = gfxdata->getSurface(BUILD_TURRET);
    structureInfos[TURRET].shadow = gfxdata->getTexture(BUILD_TURRET_SHADOW);
    structureInfos[TURRET].fadecol = -1;
    structureInfos[TURRET].icon = ICON_STR_TURRET;
    structureInfos[TURRET].sight = 7;
    structureInfos[TURRET].configured = true;
    structureInfos[TURRET].canAttackGroundUnits = true;
    structureInfos[TURRET].fireRate = 275;
    strcpy(structureInfos[TURRET].name, "Gun Turret");

    // Structure    : Rocket Turret
    // Description  : defence
    structureInfos[RTURRET].bmp_width = 16 * 2;
    structureInfos[RTURRET].bmp_height = 16 * 2;
    structureInfos[RTURRET].bmp = gfxdata->getSurface(BUILD_RTURRET);
    structureInfos[RTURRET].shadow = gfxdata->getTexture(BUILD_RTURRET_SHADOW);
    structureInfos[RTURRET].fadecol = -1;
    structureInfos[RTURRET].icon = ICON_STR_RTURRET;
    structureInfos[RTURRET].sight = 10;
    structureInfos[RTURRET].configured = true;
    structureInfos[RTURRET].canAttackAirUnits = true;
    structureInfos[RTURRET].canAttackGroundUnits = true;
    structureInfos[RTURRET].fireRate = 350;
    strcpy(structureInfos[RTURRET].name, "Rocket Turret");
}

/*****************************
 Terrain Rules
 *****************************/
void cInfoContextCreator::installTerrain(s_TerrainInfo* terrainInfo)
{
    terrainInfo->bloomTimerDuration = 200;
    terrainInfo->terrainSpiceMinSpice = 50;
    terrainInfo->terrainSpiceMaxSpice = 125;
    terrainInfo->terrainSpiceHillMinSpice = 75;
    terrainInfo->terrainSpiceHillMaxSpice = 150;
    terrainInfo->terrainWallHp = 100;
}
