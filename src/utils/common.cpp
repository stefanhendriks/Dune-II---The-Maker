/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "common.h"

#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "d2tmc.h"
#include "gameobjects/projectiles/bullet.h"
#include "map/cMapCamera.h"
#include "sidebar/cSideBar.h"
#include "utils/cLog.h"
#include "utils/cSoundPlayer.h"
#include "utils/cIniFile.h"
#include "utils/Graphics.hpp"

#include <cmath>

/**
 * Default printing in logs. Only will be done if game.isDebugMode() is true.
 * @param txt
 */
void logbook(const std::string &txt)
{
    if (game.isDebugMode()) {
        cLogger *logger = cLogger::getInstance();
        logger->log(LOG_WARN, COMP_NONE, "(logbook)", txt);
    }
}

int slowThinkMsToTicks(int desiredMs)
{
    if (desiredMs < 100) {
        return 1; // fastest thinking is 1 tick (100 ms)
    }
    // "slow" thinking, is 1 tick == 100ms
    return desiredMs / 100;
}

int fastThinkTicksToMs(int ticks)
{
    // "fast" thinking, is 1 tick == 5ms
    return ticks * 5;
}

int fastThinkMsToTicks(int desiredMs)
{
    if (desiredMs < 5) {
        return 1; // fastest thinking is 1 tick (5 ms)
    }
    // "fast" thinking, is 1 tick == 5ms
    return desiredMs / 5;
}




/********************************
 House Rules
 ********************************/

/*****************************
 Unit Rules
 *****************************/
void install_units()
{
    logbook("Installing:  UNITS");
    // Every unit thinks at 0.1 second. When the unit thinks, it is thinking about the path it
    // is taking, the enemies around him, etc. The speed of how a unit should move is depended on
    // time aswell. Every 0.01 second a unit 'can' move. The movespeed is like this:
    // 0    - slowest (1 second per pixel)
    // 1000 - fastest (1 pixel per 0.01 second)
    // So, the higher the number, the faster it is.


    // some things for ALL unit types; initialization
    for (int i = 0; i < MAX_UNITTYPES; i++) {
        s_UnitInfo &unitInfo = sUnitInfo[i];
        unitInfo.bmp = gfxdata->getSurface(UNIT_QUAD); // default bitmap is a quad!
        unitInfo.top = nullptr;  // no top
        // unitInfo.shadow = nullptr;  // no shadow (deliverd with picture itself)
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
    sUnitInfo[CARRYALL].bmp = gfxdata->getSurface(UNIT_CARRYALL);      // pointer to the original 8bit bitmap
    // sUnitInfo[CARRYALL].shadow = gfxdata->getSurface(UNIT_CARRYALL_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[CARRYALL].bmp_width = 24 * 2;
    sUnitInfo[CARRYALL].bmp_height = 24 * 2;
    sUnitInfo[CARRYALL].bmp_startpixel = 0;
    sUnitInfo[CARRYALL].bmp_frames = 2; // we have at max 1 extra frame
    sUnitInfo[CARRYALL].icon = ICON_UNIT_CARRYALL;
    sUnitInfo[CARRYALL].airborn = true;   // is airborn
    sUnitInfo[CARRYALL].free_roam = true; // may freely roam the air
    sUnitInfo[CARRYALL].listType = eListType::LIST_UNITS;
    sUnitInfo[CARRYALL].subListId = SUBLIST_HIGHTECH;
    strcpy(sUnitInfo[CARRYALL].name, "Carry-All");

    // Unit        : Ornithopter
    // Description : Pesty little aircraft shooting bastard
    sUnitInfo[ORNITHOPTER].bmp = gfxdata->getSurface(UNIT_ORNITHOPTER);      // pointer to the original 8bit bitmap
    // sUnitInfo[ORNITHOPTER].shadow = gfxdata->getSurface(UNIT_ORNITHOPTER_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[ORNITHOPTER].bmp_width = 24 * 2;
    sUnitInfo[ORNITHOPTER].bmp_height = 24 * 2;
    sUnitInfo[ORNITHOPTER].bmp_startpixel = 0;
    sUnitInfo[ORNITHOPTER].bmp_frames = 4; // we have at max 3 extra frames
    sUnitInfo[ORNITHOPTER].icon = ICON_UNIT_ORNITHOPTER;
    sUnitInfo[ORNITHOPTER].bulletType = ROCKET_SMALL_ORNI;
    sUnitInfo[ORNITHOPTER].fireTwice = true;
    sUnitInfo[ORNITHOPTER].airborn = true;   // is airborn
    sUnitInfo[ORNITHOPTER].squish = false;   // can't squish infantry
    sUnitInfo[ORNITHOPTER].free_roam = true; // may freely roam the air
    sUnitInfo[ORNITHOPTER].listType = eListType::LIST_UNITS;
    sUnitInfo[ORNITHOPTER].subListId = SUBLIST_HIGHTECH;
//    sUnitInfo[ORNITHOPTER].canAttackAirUnits = true; // orni's can attack other air units
    strcpy(sUnitInfo[ORNITHOPTER].name, "Ornithopter");

    // Unit        : Devastator
    // Description : Devastator
    sUnitInfo[DEVASTATOR].bmp = gfxdata->getSurface(UNIT_DEVASTATOR);      // pointer to the original 8bit bitmap
    // sUnitInfo[DEVASTATOR].shadow = gfxdata->getSurface(UNIT_DEVASTATOR_SHADOW);      // pointer to the original bitmap shadow
    sUnitInfo[DEVASTATOR].bmp_width = 19 * 2;
    sUnitInfo[DEVASTATOR].bmp_startpixel = 0;
    sUnitInfo[DEVASTATOR].bmp_height = 23 * 2;
    sUnitInfo[DEVASTATOR].bmp_frames = 1;
    sUnitInfo[DEVASTATOR].bulletType = BULLET_DEVASTATOR;
    sUnitInfo[DEVASTATOR].fireTwice = true;
    sUnitInfo[DEVASTATOR].icon = ICON_UNIT_DEVASTATOR;
    sUnitInfo[DEVASTATOR].listType = eListType::LIST_UNITS;
    sUnitInfo[DEVASTATOR].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[DEVASTATOR].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[DEVASTATOR].smokeHpFactor = 0.5f;
    sUnitInfo[DEVASTATOR].canGuard = true;
    sUnitInfo[DEVASTATOR].canAttackUnits = true;
    strcpy(sUnitInfo[DEVASTATOR].name, "Devastator");

    // Unit        : Harvester
    // Description : Harvester
    sUnitInfo[HARVESTER].bmp = gfxdata->getSurface(UNIT_HARVESTER);      // pointer to the original 8bit bitmap
    // sUnitInfo[HARVESTER].shadow = gfxdata->getSurface(UNIT_HARVESTER_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[HARVESTER].bmp_width = 40 * 2;
    sUnitInfo[HARVESTER].bmp_startpixel = 24;
    sUnitInfo[HARVESTER].bmp_height = 26 * 2;
    sUnitInfo[HARVESTER].bmp_frames = 4;
    sUnitInfo[HARVESTER].icon = ICON_UNIT_HARVESTER;
    sUnitInfo[HARVESTER].credit_capacity = 700;
    sUnitInfo[HARVESTER].harvesting_amount = 5;
    sUnitInfo[HARVESTER].listType = eListType::LIST_UNITS;
    sUnitInfo[HARVESTER].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[HARVESTER].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[HARVESTER].smokeHpFactor = 0.5f;
    strcpy(sUnitInfo[HARVESTER].name, "Harvester");

    // Unit        : Combattank
    // Description : Combattank
    sUnitInfo[TANK].bmp = gfxdata->getSurface(UNIT_TANKBASE);      // pointer to the original 8bit bitmap
    // sUnitInfo[TANK].shadow = gfxdata->getSurface(UNIT_TANKBASE_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[TANK].top = gfxdata->getSurface(UNIT_TANKTOP);      // pointer to the original 8bit bitmap
    sUnitInfo[TANK].bmp_width = 16 * 2;
    sUnitInfo[TANK].bmp_startpixel = 0;
    sUnitInfo[TANK].bmp_height = 16 * 2;
    sUnitInfo[TANK].bmp_frames = 0;
    sUnitInfo[TANK].bulletType = BULLET_TANK;
    sUnitInfo[TANK].icon = ICON_UNIT_TANK;
    sUnitInfo[TANK].listType = eListType::LIST_UNITS;
    sUnitInfo[TANK].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[TANK].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[TANK].smokeHpFactor = 0.5f;
    sUnitInfo[TANK].canGuard = true;
    sUnitInfo[TANK].canAttackUnits = true;
    strcpy(sUnitInfo[TANK].name, "Tank");


    // Unit        : Siege Tank
    // Description : Siege tank
    sUnitInfo[SIEGETANK].bmp = gfxdata->getSurface(UNIT_SIEGEBASE);      // pointer to the original 8bit bitmap
    // sUnitInfo[SIEGETANK].shadow = gfxdata->getSurface(UNIT_SIEGEBASE_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[SIEGETANK].top = gfxdata->getSurface(UNIT_SIEGETOP);      // pointer to the original 8bit bitmap
    sUnitInfo[SIEGETANK].bmp_width = 18 * 2;
    sUnitInfo[SIEGETANK].bmp_startpixel = 1;
    sUnitInfo[SIEGETANK].bmp_height = 18 * 2;
    sUnitInfo[SIEGETANK].bmp_frames = 0;
    sUnitInfo[SIEGETANK].bulletType = BULLET_SIEGE;
    sUnitInfo[SIEGETANK].fireTwice = true;
    sUnitInfo[SIEGETANK].icon = ICON_UNIT_SIEGETANK;
    sUnitInfo[SIEGETANK].listType = eListType::LIST_UNITS;
    sUnitInfo[SIEGETANK].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[SIEGETANK].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[SIEGETANK].smokeHpFactor = 0.5f;
    sUnitInfo[SIEGETANK].canGuard = true;
    sUnitInfo[SIEGETANK].canAttackUnits = true;
    strcpy(sUnitInfo[SIEGETANK].name, "Siege Tank");

    // Unit        : MCV
    // Description : Movable Construction Vehicle
    sUnitInfo[MCV].bmp = gfxdata->getSurface(UNIT_MCV);      // pointer to the original 8bit bitmap
    // sUnitInfo[MCV].shadow = gfxdata->getSurface(UNIT_MCV_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[MCV].bmp_width = 24 * 2;
    sUnitInfo[MCV].bmp_startpixel = 0;
    sUnitInfo[MCV].bmp_height = 25 * 2;
    sUnitInfo[MCV].bmp_frames = 1;
    sUnitInfo[MCV].icon = ICON_UNIT_MCV;
    sUnitInfo[MCV].listType = eListType::LIST_UNITS;
    sUnitInfo[MCV].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[MCV].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[MCV].smokeHpFactor = 0.5f;
    strcpy(sUnitInfo[MCV].name, "MCV");


    // Unit        : Deviator
    // Description : Deviator
    sUnitInfo[DEVIATOR].bmp = gfxdata->getSurface(UNIT_DEVIATOR);      // pointer to the original 8bit bitmap
    sUnitInfo[DEVIATOR].bmp_width = 16 * 2;
    sUnitInfo[DEVIATOR].bmp_height = 16 * 2;
    sUnitInfo[DEVIATOR].bmp_startpixel = 0;
    sUnitInfo[DEVIATOR].bmp_frames = 1;
    sUnitInfo[DEVIATOR].icon = ICON_UNIT_DEVIATOR;
    sUnitInfo[DEVIATOR].bulletType = BULLET_GAS; // our gassy rocket
    sUnitInfo[DEVIATOR].listType = eListType::LIST_UNITS;
    sUnitInfo[DEVIATOR].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[DEVIATOR].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[DEVIATOR].smokeHpFactor = 0.5f;
    sUnitInfo[DEVIATOR].canGuard = true;
    sUnitInfo[DEVIATOR].canAttackUnits = true;
    strcpy(sUnitInfo[DEVIATOR].name, "Deviator");

    // Unit        : Launcher
    // Description : Rocket Launcher
    sUnitInfo[LAUNCHER].bmp = gfxdata->getSurface(UNIT_LAUNCHER);      // pointer to the original 8bit bitmap
    // sUnitInfo[LAUNCHER].shadow = gfxdata->getSurface(UNIT_LAUNCHER_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[LAUNCHER].bmp_width = 16 * 2;
    sUnitInfo[LAUNCHER].bmp_height = 16 * 2;
    sUnitInfo[LAUNCHER].bmp_startpixel = 0;
    sUnitInfo[LAUNCHER].bmp_frames = 1;
    sUnitInfo[LAUNCHER].icon = ICON_UNIT_LAUNCHER;
    sUnitInfo[LAUNCHER].fireTwice = true;
    //units[LAUNCHER].bullets = ROCKET_NORMAL; // our gassy rocket
    sUnitInfo[LAUNCHER].bulletType = ROCKET_NORMAL; // our gassy rocket
    sUnitInfo[LAUNCHER].listType = eListType::LIST_UNITS;
    sUnitInfo[LAUNCHER].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[LAUNCHER].canAttackAirUnits = true;
    sUnitInfo[LAUNCHER].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[LAUNCHER].smokeHpFactor = 0.5f;
    sUnitInfo[LAUNCHER].canGuard = true;
    sUnitInfo[LAUNCHER].canAttackUnits = true;
    strcpy(sUnitInfo[LAUNCHER].name, "Launcher");

    // Unit        : Quad
    // Description : Quad, 4 wheeled (double gunned)
    sUnitInfo[QUAD].bmp = gfxdata->getSurface(UNIT_QUAD);      // pointer to the original 8bit bitmap
    // sUnitInfo[QUAD].shadow = gfxdata->getSurface(UNIT_QUAD_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[QUAD].bmp_width = 16 * 2;
    sUnitInfo[QUAD].bmp_height = 16 * 2;
    sUnitInfo[QUAD].bmp_startpixel = 0;
    sUnitInfo[QUAD].bmp_frames = 1;
    sUnitInfo[QUAD].icon = ICON_UNIT_QUAD;
    sUnitInfo[QUAD].fireTwice = true;
    sUnitInfo[QUAD].bulletType = BULLET_QUAD;
    sUnitInfo[QUAD].squish = false;
    sUnitInfo[QUAD].listType = eListType::LIST_UNITS;
    sUnitInfo[QUAD].subListId = SUBLIST_LIGHTFCTRY;
    sUnitInfo[QUAD].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[QUAD].smokeHpFactor = 0.5f;
    sUnitInfo[QUAD].canGuard = true;
    sUnitInfo[QUAD].canAttackUnits = true;
    strcpy(sUnitInfo[QUAD].name, "Quad");


    // Unit        : Trike (normal trike)
    // Description : Trike, 3 wheeled (single gunned)
    sUnitInfo[TRIKE].bmp = gfxdata->getSurface(UNIT_TRIKE);      // pointer to the original 8bit bitmap
    // sUnitInfo[TRIKE].shadow = gfxdata->getSurface(UNIT_TRIKE_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[TRIKE].bmp_width = 28;
    sUnitInfo[TRIKE].bmp_height = 26;
    sUnitInfo[TRIKE].bmp_startpixel = 0;
    sUnitInfo[TRIKE].bmp_frames = 1;
    sUnitInfo[TRIKE].icon = ICON_UNIT_TRIKE;
    sUnitInfo[TRIKE].bulletType = BULLET_TRIKE;
    sUnitInfo[TRIKE].squish = false;
    sUnitInfo[TRIKE].listType = eListType::LIST_UNITS;
    sUnitInfo[TRIKE].subListId = SUBLIST_LIGHTFCTRY;
    sUnitInfo[TRIKE].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[TRIKE].smokeHpFactor = 0.5f;
    sUnitInfo[TRIKE].canGuard = true;
    sUnitInfo[TRIKE].canAttackUnits = true;
    strcpy(sUnitInfo[TRIKE].name, "Trike");

    // Unit        : Raider Trike (Ordos trike)
    // Description : Raider Trike, 3 wheeled (single gunned), weaker, but faster
    sUnitInfo[RAIDER].bmp = gfxdata->getSurface(UNIT_TRIKE);      // pointer to the original 8bit bitmap
    // sUnitInfo[RAIDER].shadow = gfxdata->getSurface(UNIT_TRIKE_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[RAIDER].bmp_width = 14 * 2;
    sUnitInfo[RAIDER].bmp_height = 14 * 2;
    sUnitInfo[RAIDER].bmp_startpixel = 0;
    sUnitInfo[RAIDER].bmp_frames = 1;
    strcpy(sUnitInfo[RAIDER].name, "Raider Trike");
    sUnitInfo[RAIDER].icon = ICON_UNIT_RAIDER;
    sUnitInfo[RAIDER].bulletType = BULLET_TRIKE;
    sUnitInfo[RAIDER].squish = false;
    sUnitInfo[RAIDER].listType = eListType::LIST_UNITS;
    sUnitInfo[RAIDER].subListId = SUBLIST_LIGHTFCTRY;
    sUnitInfo[RAIDER].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[RAIDER].smokeHpFactor = 0.5f;
    sUnitInfo[RAIDER].canGuard = true;
    sUnitInfo[RAIDER].canAttackUnits = true;

    // Unit        : Frigate
    // Description : Frigate
    sUnitInfo[FRIGATE].bmp = gfxdata->getSurface(UNIT_FRIGATE);      // pointer to the original 8bit bitmap
    // sUnitInfo[FRIGATE].shadow = gfxdata->getSurface(UNIT_FRIGATE_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[FRIGATE].bmp_width = 32 * 2;
    sUnitInfo[FRIGATE].bmp_height = 32 * 2;
    sUnitInfo[FRIGATE].bmp_startpixel = 0;
    sUnitInfo[FRIGATE].bmp_frames = 2; // we have at max 1 extra frame
    sUnitInfo[FRIGATE].speed = 2;
    sUnitInfo[FRIGATE].turnspeed = 4;
    sUnitInfo[FRIGATE].airborn = true;
    sUnitInfo[FRIGATE].squish = false;
    sUnitInfo[FRIGATE].free_roam = true; // Frigate does not roam, yet needed?
    sUnitInfo[FRIGATE].hp = 9999;
    // frigate has no list
    strcpy(sUnitInfo[FRIGATE].name, "Frigate");

    /*
    units[FRIGATE].speed     = 0;
    units[FRIGATE].turnspeed = 1;

    units[FRIGATE].sight      = 3;
    units[FRIGATE].hp         = 9999;
    */

    //units[FRIGATE].icon = ICON_FRIGATE;

    // Unit        : Sonic Tank
    // Description : Sonic tank (using sound waves to destroy)
    sUnitInfo[SONICTANK].bmp = gfxdata->getSurface(UNIT_SONICTANK);      // pointer to the original 8bit bitmap
    // sUnitInfo[SONICTANK].shadow = gfxdata->getSurface(UNIT_SONICTANK_SHADOW);      // pointer to the original 8bit bitmap
    sUnitInfo[SONICTANK].bmp_width = 16 * 2;
    sUnitInfo[SONICTANK].bmp_height = 16 * 2;
    sUnitInfo[SONICTANK].bmp_startpixel = 0;
    sUnitInfo[SONICTANK].bmp_frames = 1; // no extra frames
    sUnitInfo[SONICTANK].bulletType = BULLET_SHIMMER;
    sUnitInfo[SONICTANK].icon = ICON_UNIT_SONICTANK;
    sUnitInfo[SONICTANK].listType = eListType::LIST_UNITS;
    sUnitInfo[SONICTANK].subListId = SUBLIST_HEAVYFCTRY;
    sUnitInfo[SONICTANK].renderSmokeOnUnitWhenThresholdMet = true;
    sUnitInfo[SONICTANK].smokeHpFactor = 0.5f;
    sUnitInfo[SONICTANK].canGuard = true;
    sUnitInfo[SONICTANK].canAttackUnits = true;

    strcpy(sUnitInfo[SONICTANK].name, "Sonic Tank");


    // Unit        : Single Soldier
    // Description : 1 soldier
    sUnitInfo[SOLDIER].bmp = gfxdata->getSurface(UNIT_SOLDIER);      // pointer to the original 8bit bitmap
    sUnitInfo[SOLDIER].bmp_width = 16 * 2;
    sUnitInfo[SOLDIER].bmp_height = 16 * 2;
    sUnitInfo[SOLDIER].bmp_startpixel = 0;
    sUnitInfo[SOLDIER].bmp_frames = 3; // 2 extra frames
    sUnitInfo[SOLDIER].infantry = true;
    sUnitInfo[SOLDIER].bulletType = BULLET_SMALL;
    sUnitInfo[SOLDIER].icon = ICON_UNIT_SOLDIER;
    sUnitInfo[SOLDIER].squish = false;
    sUnitInfo[SOLDIER].canBeSquished = true;
    sUnitInfo[SOLDIER].listType = eListType::LIST_FOOT_UNITS;
    sUnitInfo[SOLDIER].subListId = SUBLIST_INFANTRY;
    sUnitInfo[SOLDIER].canEnterAndDamageStructure = true;
    sUnitInfo[SOLDIER].attackIsEnterStructure = false;
    sUnitInfo[SOLDIER].damageOnEnterStructure = 10.0f;
    sUnitInfo[SOLDIER].canGuard = true;
    sUnitInfo[SOLDIER].canAttackUnits = true;
    strcpy(sUnitInfo[SOLDIER].name, "Soldier");


    // Unit        : Infantry
    // Description : 3 soldiers
    sUnitInfo[INFANTRY].bmp = gfxdata->getSurface(UNIT_SOLDIERS);      // pointer to the original 8bit bitmap
    sUnitInfo[INFANTRY].bmp_width = 16 * 2;
    sUnitInfo[INFANTRY].bmp_height = 16 * 2;
    sUnitInfo[INFANTRY].bmp_startpixel = 0;
    sUnitInfo[INFANTRY].bmp_frames = 3; // 2 extra frames
    sUnitInfo[INFANTRY].speed = 8;
    sUnitInfo[INFANTRY].infantry = true;
    sUnitInfo[INFANTRY].bulletType = BULLET_SMALL;
    sUnitInfo[INFANTRY].fireTwice = true;
    sUnitInfo[INFANTRY].icon = ICON_UNIT_INFANTRY;
    sUnitInfo[INFANTRY].squish = false;
    sUnitInfo[INFANTRY].canBeSquished = true;
    sUnitInfo[INFANTRY].listType = eListType::LIST_FOOT_UNITS;
    sUnitInfo[INFANTRY].subListId = SUBLIST_INFANTRY;
    sUnitInfo[INFANTRY].canEnterAndDamageStructure = true;
    sUnitInfo[INFANTRY].attackIsEnterStructure = false;
    sUnitInfo[INFANTRY].damageOnEnterStructure = 25.0f;
    sUnitInfo[INFANTRY].canGuard = true;
    sUnitInfo[INFANTRY].canAttackUnits = true;
    strcpy(sUnitInfo[INFANTRY].name, "Light Infantry");

    // Unit        : Single Trooper
    // Description : 1 trooper
    sUnitInfo[TROOPER].bmp = gfxdata->getSurface(UNIT_TROOPER);      // pointer to the original 8bit bitmap
    sUnitInfo[TROOPER].bmp_width = 16 * 2;
    sUnitInfo[TROOPER].bmp_height = 16 * 2;
    sUnitInfo[TROOPER].bmp_startpixel = 0;
    sUnitInfo[TROOPER].bmp_frames = 3; // 2 extra frames
    strcpy(sUnitInfo[TROOPER].name, "Trooper");
    sUnitInfo[TROOPER].infantry = true;
    sUnitInfo[TROOPER].bulletType = ROCKET_SMALL;
    sUnitInfo[TROOPER].icon = ICON_UNIT_TROOPER;
    sUnitInfo[TROOPER].listType = eListType::LIST_FOOT_UNITS;
    sUnitInfo[TROOPER].subListId = SUBLIST_TROOPERS;
    sUnitInfo[TROOPER].squish = false;
    sUnitInfo[TROOPER].canBeSquished = true;
    sUnitInfo[TROOPER].canAttackAirUnits = true;
    sUnitInfo[TROOPER].canEnterAndDamageStructure = true;
    sUnitInfo[TROOPER].attackIsEnterStructure = false;
    sUnitInfo[TROOPER].damageOnEnterStructure = 12.0f;
    sUnitInfo[TROOPER].bulletTypeSecondary = BULLET_SMALL;
    sUnitInfo[TROOPER].fireSecondaryWithinRange = 2;
    sUnitInfo[TROOPER].canGuard = true;
    sUnitInfo[TROOPER].canAttackUnits = true;

    // Unit        : Group Trooper
    // Description : 3 troopers
    sUnitInfo[TROOPERS].bmp = gfxdata->getSurface(UNIT_TROOPERS);      // pointer to the original 8bit bitmap
    sUnitInfo[TROOPERS].bmp_width = 16 * 2;
    sUnitInfo[TROOPERS].bmp_height = 16 * 2;
    sUnitInfo[TROOPERS].bmp_startpixel = 0;
    sUnitInfo[TROOPERS].bmp_frames = 3; // 2 extra frames
    strcpy(sUnitInfo[TROOPERS].name, "Troopers");
    sUnitInfo[TROOPERS].icon = ICON_UNIT_TROOPERS;
    sUnitInfo[TROOPERS].bulletType = ROCKET_SMALL;
    sUnitInfo[TROOPERS].fireTwice = true;
    sUnitInfo[TROOPERS].infantry = true;
    sUnitInfo[TROOPERS].listType = eListType::LIST_FOOT_UNITS;
    sUnitInfo[TROOPERS].subListId = SUBLIST_TROOPERS;
    sUnitInfo[TROOPERS].squish = false;
    sUnitInfo[TROOPERS].canBeSquished = true;
    sUnitInfo[TROOPERS].canAttackAirUnits = true;
    sUnitInfo[TROOPERS].canEnterAndDamageStructure = true;
    sUnitInfo[TROOPERS].attackIsEnterStructure = false;
    sUnitInfo[TROOPERS].damageOnEnterStructure = 35.0f;
    sUnitInfo[TROOPERS].bulletTypeSecondary = BULLET_SMALL;
    sUnitInfo[TROOPERS].fireSecondaryWithinRange = 2;
    sUnitInfo[TROOPERS].canGuard = true;
    sUnitInfo[TROOPERS].canAttackUnits = true;

    // Unit        : Fremen
    // Description : A single fremen
    sUnitInfo[UNIT_FREMEN_ONE].bmp = gfxdata->getSurface(UNIT_SINGLEFREMEN);      // pointer to the original 8bit bitmap
    sUnitInfo[UNIT_FREMEN_ONE].bmp_width = 16 * 2;
    sUnitInfo[UNIT_FREMEN_ONE].bmp_height = 16 * 2;
    sUnitInfo[UNIT_FREMEN_ONE].bmp_startpixel = 0;
    sUnitInfo[UNIT_FREMEN_ONE].bmp_frames = 3; // 2 extra frames
    strcpy(sUnitInfo[UNIT_FREMEN_ONE].name, "Fremen (1)");
    sUnitInfo[UNIT_FREMEN_ONE].icon = ICON_SPECIAL_FREMEN;
    sUnitInfo[UNIT_FREMEN_ONE].bulletType = ROCKET_SMALL_FREMEN;
    sUnitInfo[UNIT_FREMEN_ONE].fireTwice = false;
    sUnitInfo[UNIT_FREMEN_ONE].infantry = true;
    sUnitInfo[UNIT_FREMEN_ONE].squish = false;
    sUnitInfo[UNIT_FREMEN_ONE].canBeSquished = true;
    sUnitInfo[UNIT_FREMEN_ONE].canAttackAirUnits = true;
    sUnitInfo[UNIT_FREMEN_ONE].canGuard = true;
    sUnitInfo[UNIT_FREMEN_ONE].canAttackUnits = true;

//  units[UNIT_FREMEN_ONE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_ONE].subListId=0;

    // Unit        : Fremen
    // Description : A group of Fremen
    sUnitInfo[UNIT_FREMEN_THREE].bmp = gfxdata->getSurface(UNIT_TRIPLEFREMEN);      // pointer to the original 8bit bitmap
    sUnitInfo[UNIT_FREMEN_THREE].bmp_width = 16 * 2;
    sUnitInfo[UNIT_FREMEN_THREE].bmp_height = 16 * 2;
    sUnitInfo[UNIT_FREMEN_THREE].bmp_startpixel = 0;
    sUnitInfo[UNIT_FREMEN_THREE].bmp_frames = 3; // 2 extra frames
    strcpy(sUnitInfo[UNIT_FREMEN_THREE].name, "Fremen (3)");
    sUnitInfo[UNIT_FREMEN_THREE].icon = ICON_SPECIAL_FREMEN;
    sUnitInfo[UNIT_FREMEN_THREE].bulletType = ROCKET_SMALL_FREMEN;
    sUnitInfo[UNIT_FREMEN_THREE].fireTwice = true;
    sUnitInfo[UNIT_FREMEN_THREE].infantry = true;
    sUnitInfo[UNIT_FREMEN_THREE].squish = false;
    sUnitInfo[UNIT_FREMEN_THREE].canBeSquished = true;
    sUnitInfo[UNIT_FREMEN_THREE].canAttackAirUnits = true;
    sUnitInfo[UNIT_FREMEN_THREE].canGuard = true;
    sUnitInfo[UNIT_FREMEN_THREE].canAttackUnits = true;
//  units[UNIT_FREMEN_THREE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_THREE].subListId=0;

    // Unit        : Saboteur
    // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
    sUnitInfo[SABOTEUR].bmp = gfxdata->getSurface(UNIT_SABOTEUR);
    sUnitInfo[SABOTEUR].buildTime = 1000;
    sUnitInfo[SABOTEUR].bmp_width = 16 * 2;
    sUnitInfo[SABOTEUR].bmp_height = 16 * 2;
    sUnitInfo[SABOTEUR].bmp_startpixel = 0;
    sUnitInfo[SABOTEUR].bmp_frames = 3; // 2 extra frames
    sUnitInfo[SABOTEUR].speed = 0; // very fast
    sUnitInfo[SABOTEUR].hp = 60;   // quite some health
    sUnitInfo[SABOTEUR].cost = 0;
    sUnitInfo[SABOTEUR].sight = 4; // immense sight! (sorta scouting guys)
    sUnitInfo[SABOTEUR].range = 2;
    sUnitInfo[SABOTEUR].attack_frequency = 0;
    sUnitInfo[SABOTEUR].turnspeed = 0; // very fast
    strcpy(sUnitInfo[SABOTEUR].name, "Saboteur");
    sUnitInfo[SABOTEUR].icon = ICON_SPECIAL_SABOTEUR;
    sUnitInfo[SABOTEUR].squish = false;
    sUnitInfo[SABOTEUR].canBeSquished = true;
    sUnitInfo[SABOTEUR].infantry = true; // infantry unit, so it can be squished
    sUnitInfo[SABOTEUR].listType = eListType::LIST_PALACE;
    sUnitInfo[SABOTEUR].subListId = 0;
    sUnitInfo[SABOTEUR].canEnterAndDamageStructure = true;
    sUnitInfo[SABOTEUR].attackIsEnterStructure = true;
    sUnitInfo[SABOTEUR].damageOnEnterStructure = 9999.99f; // a lot of damage (instant destroy)

    // Unit        : Sandworm
    sUnitInfo[SANDWORM].speed = 3; // very fast
    sUnitInfo[SANDWORM].bmp = gfxdata->getSurface(UNIT_SANDWORM);
    sUnitInfo[SANDWORM].hp = 9999; // set in game.ini to a more sane amount
    sUnitInfo[SANDWORM].dieWhenLowerThanHP = 1000;
    sUnitInfo[SANDWORM].appetite = 10;
    sUnitInfo[SANDWORM].bmp_width = 48;
    sUnitInfo[SANDWORM].bmp_height = 48;
    sUnitInfo[SANDWORM].turnspeed = 0; // very fast
    sUnitInfo[SANDWORM].sight = 16;
    strcpy(sUnitInfo[SANDWORM].name, "Sandworm");
    sUnitInfo[SANDWORM].icon = ICON_UNIT_SANDWORM;
    sUnitInfo[SANDWORM].squish = false;
    sUnitInfo[SANDWORM].canGuard = true;
    sUnitInfo[SANDWORM].canAttackUnits = true;


    // Unit        : <name>
    // Description : <description>

}

void install_particles()
{
    for (int i = 0; i < MAX_PARTICLE_TYPES; i++) {
        s_ParticleInfo &particleInfo = sParticleInfo[i];
        particleInfo.bmpIndex = -1;
        particleInfo.startAlpha = -1;
        particleInfo.usesAdditiveBlending = false;

        particleInfo.layer = D2TM_RENDER_LAYER_PARTICLE_TOP;

        // default to 32x32 (for now)
        particleInfo.frameWidth = 32;
        particleInfo.frameHeight = 32;

        // mark particles as old for now
        particleInfo.oldParticle = true;
    }

    // move
    sParticleInfo[D2TM_PARTICLE_MOVE].bmpIndex = MOVE_INDICATOR;
    sParticleInfo[D2TM_PARTICLE_MOVE].startAlpha = 128;

    // attack
    sParticleInfo[D2TM_PARTICLE_ATTACK].bmpIndex = ATTACK_INDICATOR;
    sParticleInfo[D2TM_PARTICLE_ATTACK].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].bmpIndex = EXPLOSION_TRIKE;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_SMOKE].bmpIndex = OBJECT_SMOKE;
    sParticleInfo[D2TM_PARTICLE_SMOKE].startAlpha = 0;
    sParticleInfo[D2TM_PARTICLE_SMOKE].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SMOKE].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_SMOKE_WITH_SHADOW].bmpIndex = OBJECT_SMOKE;
    sParticleInfo[D2TM_PARTICLE_SMOKE_WITH_SHADOW].startAlpha = 0;
    sParticleInfo[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].bmpIndex = OBJECT_SMOKE_SHADOW;
    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].startAlpha = 0;
    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].frameWidth = 36;
    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].frameHeight = 38;

    sParticleInfo[D2TM_PARTICLE_TRACK_DIA].bmpIndex = TRACK_DIA;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_TRACK_HOR].bmpIndex = TRACK_HOR;
    sParticleInfo[D2TM_PARTICLE_TRACK_HOR].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_HOR].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_TRACK_VER].bmpIndex = TRACK_VER;
    sParticleInfo[D2TM_PARTICLE_TRACK_VER].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_VER].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_TRACK_DIA2].bmpIndex = TRACK_DIA2;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA2].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA2].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_BULLET_PUF].bmpIndex = BULLET_PUF;
    sParticleInfo[D2TM_PARTICLE_BULLET_PUF].frameWidth = 18;
    sParticleInfo[D2TM_PARTICLE_BULLET_PUF].frameHeight = 18;


    sParticleInfo[D2TM_PARTICLE_EXPLOSION_FIRE].bmpIndex = EXPLOSION_FIRE;

    sParticleInfo[D2TM_PARTICLE_WORMEAT].bmpIndex = OBJECT_WORMEAT;
    sParticleInfo[D2TM_PARTICLE_WORMEAT].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_WORMEAT].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_WORMEAT].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].bmpIndex = EXPLOSION_TANK_ONE;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].bmpIndex = EXPLOSION_TANK_TWO;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].bmpIndex = EXPLOSION_STRUCTURE01;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].bmpIndex = EXPLOSION_STRUCTURE02;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].bmpIndex = EXPLOSION_GAS;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].bmpIndex = OBJECT_WORMTRAIL;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].startAlpha = 96;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_DEADINF01].bmpIndex = OBJECT_DEADINF01;
    sParticleInfo[D2TM_PARTICLE_DEADINF01].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;

    sParticleInfo[D2TM_PARTICLE_DEADINF02].bmpIndex = OBJECT_DEADINF02;
    sParticleInfo[D2TM_PARTICLE_DEADINF02].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;

    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].bmpIndex = OBJECT_TANKSHOOT;
    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].startAlpha = 128;
    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].frameWidth = 64;
    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].frameHeight = 64;

    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].bmpIndex = OBJECT_SIEGESHOOT;
    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].startAlpha = 128;
    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].frameWidth = 64;
    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].frameHeight = 64;

    sParticleInfo[D2TM_PARTICLE_SQUISH01].bmpIndex = EXPLOSION_SQUISH01;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_SQUISH02].bmpIndex = EXPLOSION_SQUISH02;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_SQUISH03].bmpIndex = EXPLOSION_SQUISH03;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].bmpIndex = EXPLOSION_ORNI;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_SIEGEDIE].bmpIndex = OBJECT_SIEGEDIE;

    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].bmpIndex = OBJECT_CARRYPUFF;
    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].frameWidth = 96;
    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].frameHeight = 96;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ROCKET].bmpIndex = EXPLOSION_ROCKET;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL].bmpIndex = EXPLOSION_ROCKET_SMALL;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_BULLET].bmpIndex = EXPLOSION_BULLET;

    // the nice flare/light effects that come with explosions
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].bmpIndex = OBJECT_BOOM01;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].usesAdditiveBlending = true;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].startAlpha = 240;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].frameWidth = 512;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].frameHeight = 512;
    ;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].bmpIndex = OBJECT_BOOM02;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].usesAdditiveBlending = true;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].startAlpha = 230;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].frameWidth = 256;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].frameHeight = 256;

    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].bmpIndex = OBJECT_BOOM03;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].usesAdditiveBlending = true;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].startAlpha = 220;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].frameWidth = 128;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].frameHeight = 128;

}

void install_specials()
{

    for (int i = 0; i < MAX_SPECIALTYPES; i++) {
        sSpecialInfo[i].icon = -1;
        sSpecialInfo[i].providesType = eBuildType::UNIT;
        sSpecialInfo[i].buildTime = 0;
        sSpecialInfo[i].deployFrom = eDeployFromType::AT_RANDOM_CELL;
        sSpecialInfo[i].deployAtStructure = -1;
        sSpecialInfo[i].units = 0;
        sSpecialInfo[i].house = eHouseBitFlag::Unknown;
        sSpecialInfo[i].autoBuild = false;
        sSpecialInfo[i].deployTargetType = eDeployTargetType::TARGET_NONE;
        sSpecialInfo[i].deployTargetPrecision = 0;
        sSpecialInfo[i].listType = eListType::LIST_NONE;
        sSpecialInfo[i].subListId = -1;
        strcpy(sSpecialInfo[i].description, "\0");
    }

    // Deploy Saboteur
    sSpecialInfo[SPECIAL_SABOTEUR].icon = ICON_SPECIAL_SABOTEUR;
    sSpecialInfo[SPECIAL_SABOTEUR].house=eHouseBitFlag::Ordos;
    sSpecialInfo[SPECIAL_SABOTEUR].autoBuild=true;
    sSpecialInfo[SPECIAL_SABOTEUR].providesType = eBuildType::UNIT;
    sSpecialInfo[SPECIAL_SABOTEUR].providesTypeId = SABOTEUR;
    sSpecialInfo[SPECIAL_SABOTEUR].deployFrom = eDeployFromType::AT_STRUCTURE;
    sSpecialInfo[SPECIAL_SABOTEUR].deployAtStructure = PALACE;
    sSpecialInfo[SPECIAL_SABOTEUR].units = 1;
    sSpecialInfo[SPECIAL_SABOTEUR].buildTime = 2468; // ~ 6 minutes (but times 1.2 to compensate for faster Ordos building = 2468 to get real 6 minutes)
    sSpecialInfo[SPECIAL_SABOTEUR].listType=eListType::LIST_PALACE;
    sSpecialInfo[SPECIAL_SABOTEUR].subListId=0;
    strcpy(sSpecialInfo[SPECIAL_SABOTEUR].description, "Saboteur");

    // Deploy Fremen
    sSpecialInfo[SPECIAL_FREMEN].icon = ICON_SPECIAL_FREMEN;
    sSpecialInfo[SPECIAL_FREMEN].house=eHouseBitFlag::Atreides;
    sSpecialInfo[SPECIAL_FREMEN].autoBuild=true;
    sSpecialInfo[SPECIAL_FREMEN].providesType = eBuildType::UNIT;
    sSpecialInfo[SPECIAL_FREMEN].providesTypeId = UNIT_FREMEN_THREE;
    sSpecialInfo[SPECIAL_FREMEN].deployFrom = eDeployFromType::AT_RANDOM_CELL;
    sSpecialInfo[SPECIAL_FREMEN].deployAtStructure = PALACE; // This is not used with AT_RANDOM_CELL ...
    sSpecialInfo[SPECIAL_FREMEN].units = 6; // ... but this is
    sSpecialInfo[SPECIAL_FREMEN].buildTime = 1371; // ~ 4 minutes (atreides has baseline build times, ie = real time)
    sSpecialInfo[SPECIAL_FREMEN].listType=eListType::LIST_PALACE;
    sSpecialInfo[SPECIAL_FREMEN].subListId=0;
    strcpy(sSpecialInfo[SPECIAL_FREMEN].description, "Fremen");

    // Launch Death Hand
    sSpecialInfo[SPECIAL_DEATHHAND].icon = ICON_SPECIAL_MISSILE;
    sSpecialInfo[SPECIAL_DEATHHAND].house = Harkonnen | Sardaukar;
    sSpecialInfo[SPECIAL_DEATHHAND].autoBuild=true;
    sSpecialInfo[SPECIAL_DEATHHAND].providesType = eBuildType::BULLET;
    sSpecialInfo[SPECIAL_DEATHHAND].providesTypeId = ROCKET_BIG;
    sSpecialInfo[SPECIAL_DEATHHAND].deployFrom = eDeployFromType::AT_STRUCTURE; // the rocket is fired FROM ...
    sSpecialInfo[SPECIAL_DEATHHAND].deployAtStructure = PALACE; // ... the palace
    sSpecialInfo[SPECIAL_DEATHHAND].deployTargetType = eDeployTargetType::TARGET_INACCURATE_CELL;
    sSpecialInfo[SPECIAL_DEATHHAND].units = 1;
    sSpecialInfo[SPECIAL_DEATHHAND].buildTime = 3428; // ~ 10 minutes with base line (Atreides difficulty)
    // (342.8 = ~ 1 minute) -> harkonnen is done * 1.2 so it becomes 12 minutes real-time which is ok)
    // considering the Dune 2 Insider guide mentions 11 to 12 minutes for Harkonnen.

    sSpecialInfo[SPECIAL_DEATHHAND].deployTargetPrecision = 6;
    sSpecialInfo[SPECIAL_DEATHHAND].listType=eListType::LIST_PALACE;
    sSpecialInfo[SPECIAL_DEATHHAND].subListId=0;
    strcpy(sSpecialInfo[SPECIAL_DEATHHAND].description, "Death Hand");

}


/****************
 Install bullets
 ****************/
void install_bullets()
{
    logbook("Installing:  BULLET TYPES");

    for (int i = 0; i < MAX_BULLET_TYPES; i++) {
        sBulletInfo[i].bmp = nullptr; // in case an invalid bitmap; default is a small rocket
        sBulletInfo[i].deathParticle = -1; // this points to a bitmap (in data file, using index)
        sBulletInfo[i].damage = 0;      // damage to vehicles
        sBulletInfo[i].damage_inf = 0;  // damage to infantry
        sBulletInfo[i].max_frames = 1;  // 1 frame animation
        sBulletInfo[i].max_deadframes = 4; // 4 frame animation
        sBulletInfo[i].smokeParticle = -1; // by default no smoke particle is spawned
        sBulletInfo[i].bmp_width = 8 * 2;
        sBulletInfo[i].sound = -1;    // no sound
        sBulletInfo[i].explosionSize = 1; // 1 tile sized explosion
        sBulletInfo[i].deviateProbability = 0; // no probability of deviating a unit
        sBulletInfo[i].groundBullet = false; // if true, then it gets blocked by walls, mountains or structures. False == flying bullets, ie, rockets
        sBulletInfo[i].canDamageAirUnits = false; // if true, then upon impact the bullet can also damage air units
        sBulletInfo[i].canDamageGround = false; // if true, then upon impact the bullet can also damage ground (walls, slabs, etc)
        strcpy(sBulletInfo[i].description, "Unknown");
    }

    // huge rocket/missile
    sBulletInfo[ROCKET_BIG].bmp = gfxdata->getTexture(BULLET_ROCKET_LARGE);
    sBulletInfo[ROCKET_BIG].deathParticle = D2TM_PARTICLE_EXPLOSION_STRUCTURE01;
    sBulletInfo[ROCKET_BIG].bmp_width = 48;
    sBulletInfo[ROCKET_BIG].damage = 999;
    sBulletInfo[ROCKET_BIG].damage_inf = 999;
    sBulletInfo[ROCKET_BIG].max_frames = 1;
    sBulletInfo[ROCKET_BIG].sound = SOUND_ROCKET;
    sBulletInfo[ROCKET_BIG].explosionSize = 7;
    sBulletInfo[ROCKET_BIG].canDamageAirUnits = true;
    sBulletInfo[ROCKET_BIG].canDamageGround = true;
    sBulletInfo[ROCKET_BIG].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(sBulletInfo[ROCKET_BIG].description, "ROCKET_BIG");

    // small rocket (for ornithopter)
    sBulletInfo[ROCKET_SMALL_ORNI].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    sBulletInfo[ROCKET_SMALL_ORNI].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    sBulletInfo[ROCKET_SMALL_ORNI].bmp_width = 16;
    sBulletInfo[ROCKET_SMALL_ORNI].damage = 12; // they can do pretty damage
    sBulletInfo[ROCKET_SMALL_ORNI].damage_inf = 9;
    sBulletInfo[ROCKET_SMALL_ORNI].max_frames = 1;
    sBulletInfo[ROCKET_SMALL_ORNI].sound = SOUND_ROCKET_SMALL;
    sBulletInfo[ROCKET_SMALL_ORNI].canDamageAirUnits = true;
    sBulletInfo[ROCKET_SMALL_ORNI].max_deadframes = 1;
    sBulletInfo[ROCKET_SMALL_ORNI].canDamageGround = true;
    sBulletInfo[ROCKET_SMALL_ORNI].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(sBulletInfo[ROCKET_SMALL_ORNI].description, "ROCKET_SMALL_ORNI");

    // small rocket
    sBulletInfo[ROCKET_SMALL].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    sBulletInfo[ROCKET_SMALL].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    sBulletInfo[ROCKET_SMALL].bmp_width = 16;
    sBulletInfo[ROCKET_SMALL].damage = 10; // was 8
    sBulletInfo[ROCKET_SMALL].damage_inf = 8; // was 4
    sBulletInfo[ROCKET_SMALL].max_frames = 1;
    sBulletInfo[ROCKET_SMALL].sound = SOUND_ROCKET_SMALL;
    sBulletInfo[ROCKET_SMALL].canDamageAirUnits = true;
    sBulletInfo[ROCKET_SMALL].max_deadframes = 1;
    sBulletInfo[ROCKET_SMALL].canDamageGround = true;
//    bullets[ROCKET_SMALL].smokeParticle = BULLET_PUF; // small rockets have no smoke trail yet
    strcpy(sBulletInfo[ROCKET_SMALL].description, "ROCKET_SMALL");

    // small rocket - fremen rocket
    sBulletInfo[ROCKET_SMALL_FREMEN].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    sBulletInfo[ROCKET_SMALL_FREMEN].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    sBulletInfo[ROCKET_SMALL_FREMEN].bmp_width = 16;
    sBulletInfo[ROCKET_SMALL_FREMEN].damage = 22;
    sBulletInfo[ROCKET_SMALL_FREMEN].damage_inf = 20;
    sBulletInfo[ROCKET_SMALL_FREMEN].max_frames = 1;
    sBulletInfo[ROCKET_SMALL_FREMEN].sound = SOUND_ROCKET_SMALL;
    sBulletInfo[ROCKET_SMALL_FREMEN].max_deadframes = 1;
    sBulletInfo[ROCKET_SMALL_FREMEN].canDamageAirUnits = true;
    sBulletInfo[ROCKET_SMALL_FREMEN].canDamageGround = true;
//    bullets[ROCKET_SMALL_FREMEN].smokeParticle = true; // not yet
    strcpy(sBulletInfo[ROCKET_SMALL_FREMEN].description, "ROCKET_SMALL_FREMEN");

    // normal rocket
    sBulletInfo[ROCKET_NORMAL].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    sBulletInfo[ROCKET_NORMAL].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET;
    sBulletInfo[ROCKET_NORMAL].bmp_width = 32;
    sBulletInfo[ROCKET_NORMAL].damage = 76;
    sBulletInfo[ROCKET_NORMAL].damage_inf = 36;  // less damage on infantry
    sBulletInfo[ROCKET_NORMAL].max_frames = 1;
    sBulletInfo[ROCKET_NORMAL].sound = SOUND_ROCKET;
    sBulletInfo[ROCKET_NORMAL].max_deadframes = 4;
    sBulletInfo[ROCKET_NORMAL].canDamageAirUnits = true;
    sBulletInfo[ROCKET_NORMAL].canDamageGround = true;
    sBulletInfo[ROCKET_NORMAL].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(sBulletInfo[ROCKET_NORMAL].description, "ROCKET_NORMAL");

    // soldier shot
    sBulletInfo[BULLET_SMALL].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    sBulletInfo[BULLET_SMALL].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    sBulletInfo[BULLET_SMALL].bmp_width = 6;
    sBulletInfo[BULLET_SMALL].damage = 4; // vehicles are no match
    sBulletInfo[BULLET_SMALL].damage_inf = 10; // infantry vs infantry means big time damage
    sBulletInfo[BULLET_SMALL].max_frames = 0;
    sBulletInfo[BULLET_SMALL].sound = SOUND_GUN;
    sBulletInfo[BULLET_SMALL].max_deadframes = 0;
    sBulletInfo[BULLET_SMALL].groundBullet = true;
    strcpy(sBulletInfo[BULLET_SMALL].description, "BULLET_SMALL");

    // trike shot
    sBulletInfo[BULLET_TRIKE].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    sBulletInfo[BULLET_TRIKE].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    sBulletInfo[BULLET_TRIKE].bmp_width = 6;
    sBulletInfo[BULLET_TRIKE].damage = 3; // trikes do not do much damage to vehicles
    sBulletInfo[BULLET_TRIKE].damage_inf = 6; // but more to infantry
    sBulletInfo[BULLET_TRIKE].max_frames = 0;
    sBulletInfo[BULLET_TRIKE].sound = SOUND_MACHINEGUN;
    sBulletInfo[BULLET_TRIKE].max_deadframes = 0;
    sBulletInfo[BULLET_TRIKE].groundBullet = true;
    strcpy(sBulletInfo[BULLET_TRIKE].description, "BULLET_TRIKE");

    // quad shot
    sBulletInfo[BULLET_QUAD].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    sBulletInfo[BULLET_QUAD].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    sBulletInfo[BULLET_QUAD].bmp_width = 6;
    sBulletInfo[BULLET_QUAD].damage = 6;
    sBulletInfo[BULLET_QUAD].damage_inf = 8; // bigger impact on infantry
    sBulletInfo[BULLET_QUAD].max_frames = 0;
    sBulletInfo[BULLET_QUAD].sound = SOUND_MACHINEGUN;
    sBulletInfo[BULLET_QUAD].max_deadframes = 0;
    sBulletInfo[BULLET_QUAD].groundBullet = true;
    strcpy(sBulletInfo[BULLET_QUAD].description, "BULLET_QUAD");

    // normal tank shot
    sBulletInfo[BULLET_TANK].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    sBulletInfo[BULLET_TANK].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    sBulletInfo[BULLET_TANK].bmp_width = 8;
    sBulletInfo[BULLET_TANK].damage = 12;
    sBulletInfo[BULLET_TANK].damage_inf = 4;  // infantry is not much damaged
    sBulletInfo[BULLET_TANK].max_frames = 0;
    sBulletInfo[BULLET_TANK].sound = SOUND_EXPL_ROCKET;
    sBulletInfo[BULLET_TANK].max_deadframes = 1;
    sBulletInfo[BULLET_TANK].groundBullet = true;
    sBulletInfo[BULLET_TANK].canDamageGround = true;
    strcpy(sBulletInfo[BULLET_TANK].description, "BULLET_TANK");

    // siege tank shot
    sBulletInfo[BULLET_SIEGE].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    sBulletInfo[BULLET_SIEGE].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    sBulletInfo[BULLET_SIEGE].bmp_width = 8;
    sBulletInfo[BULLET_SIEGE].damage = 24;
    sBulletInfo[BULLET_SIEGE].damage_inf = 6; // infantry is not as much damaged
    sBulletInfo[BULLET_SIEGE].max_frames = 0;
    sBulletInfo[BULLET_SIEGE].sound = SOUND_EXPL_ROCKET;
    sBulletInfo[BULLET_SIEGE].max_deadframes = 2;
    sBulletInfo[BULLET_SIEGE].groundBullet = true;
    sBulletInfo[BULLET_SIEGE].canDamageGround = true;
    strcpy(sBulletInfo[BULLET_SIEGE].description, "BULLET_SIEGE");

    // devastator shot
    sBulletInfo[BULLET_DEVASTATOR].bmp = gfxdata->getTexture(BULLET_DOT_LARGE);
    sBulletInfo[BULLET_DEVASTATOR].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL; // not used anyway
    sBulletInfo[BULLET_DEVASTATOR].bmp_width = 8;
    sBulletInfo[BULLET_DEVASTATOR].damage = 30;
    sBulletInfo[BULLET_DEVASTATOR].damage_inf = 12; // infantry again not much damaged
    sBulletInfo[BULLET_DEVASTATOR].max_frames = 0;
    sBulletInfo[BULLET_DEVASTATOR].sound = SOUND_EXPL_ROCKET;
    sBulletInfo[BULLET_DEVASTATOR].max_deadframes = 1;
    sBulletInfo[BULLET_DEVASTATOR].groundBullet = true;
    sBulletInfo[BULLET_DEVASTATOR].canDamageGround = true;
    strcpy(sBulletInfo[BULLET_DEVASTATOR].description, "BULLET_DEVASTATOR");

    // Gas rocket of a deviator
    sBulletInfo[BULLET_GAS].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    sBulletInfo[BULLET_GAS].deathParticle = D2TM_PARTICLE_EXPLOSION_GAS;
    sBulletInfo[BULLET_GAS].bmp_width = 32;
    sBulletInfo[BULLET_GAS].damage = 1;
    sBulletInfo[BULLET_GAS].damage_inf = 1;
    sBulletInfo[BULLET_GAS].max_frames = 1;
    sBulletInfo[BULLET_GAS].max_deadframes = 4;
    sBulletInfo[BULLET_GAS].sound = SOUND_ROCKET;
    sBulletInfo[BULLET_GAS].deviateProbability = 34; // 1 out of 3(ish) should be effective
    sBulletInfo[BULLET_GAS].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(sBulletInfo[BULLET_GAS].description, "BULLET_GAS");

    // normal turret shot
    sBulletInfo[BULLET_TURRET].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    sBulletInfo[BULLET_TURRET].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    sBulletInfo[BULLET_TURRET].bmp_width = 8;
    sBulletInfo[BULLET_TURRET].damage = 12;
    sBulletInfo[BULLET_TURRET].damage_inf = 12; // infantry is a hard target
    sBulletInfo[BULLET_TURRET].max_frames = 0;
    sBulletInfo[BULLET_TURRET].max_deadframes = 1;
    sBulletInfo[BULLET_TURRET].sound = SOUND_GUNTURRET;
    sBulletInfo[BULLET_TURRET].groundBullet = false; // this can fly over structures, walls, mountains, yes!
    sBulletInfo[BULLET_TURRET].canDamageGround = true;
    strcpy(sBulletInfo[BULLET_TURRET].description, "BULLET_TURRET");

    // EXEPTION: Shimmer/ Sonic tank
    sBulletInfo[BULLET_SHIMMER].bmp = nullptr;
    sBulletInfo[BULLET_SHIMMER].deathParticle = -1;
    sBulletInfo[BULLET_SHIMMER].bmp_width = 0;
    sBulletInfo[BULLET_SHIMMER].damage = 55;
    sBulletInfo[BULLET_SHIMMER].damage_inf = 70; // infantry cant stand the sound, die very fast
    sBulletInfo[BULLET_SHIMMER].max_frames = 0;
    sBulletInfo[BULLET_SHIMMER].max_deadframes = 0;
    sBulletInfo[BULLET_SHIMMER].sound = SOUND_SHIMMER;
    strcpy(sBulletInfo[BULLET_SHIMMER].description, "BULLET_SHIMMER");

    // rocket of rocket turret
    sBulletInfo[ROCKET_RTURRET].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    sBulletInfo[ROCKET_RTURRET].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    sBulletInfo[ROCKET_RTURRET].bmp_width = 16 * 2;
    sBulletInfo[ROCKET_RTURRET].damage = 25;
    sBulletInfo[ROCKET_RTURRET].damage_inf = 10; // infantry is a bit tougher
    sBulletInfo[ROCKET_RTURRET].max_frames = 1;
    sBulletInfo[ROCKET_RTURRET].sound = SOUND_ROCKET;
    sBulletInfo[ROCKET_RTURRET].max_deadframes = 4;
    sBulletInfo[ROCKET_RTURRET].canDamageAirUnits = true;
    sBulletInfo[ROCKET_RTURRET].canDamageGround = true;
    sBulletInfo[ROCKET_RTURRET].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(sBulletInfo[ROCKET_RTURRET].description, "ROCKET_RTURRET");
}


void install_upgrades()
{
    logbook("Installing:  UPGRADES");
    for (int i = 0; i < MAX_UPGRADETYPES; i++) {
        sUpgradeInfo[i].enabled = false;
        sUpgradeInfo[i].techLevel = -1;
        sUpgradeInfo[i].house = 0;
        sUpgradeInfo[i].needsStructureType = -1;
        sUpgradeInfo[i].icon = ICON_STR_PALACE;
        sUpgradeInfo[i].cost = 100;
        sUpgradeInfo[i].atUpgradeLevel = -1;
        sUpgradeInfo[i].structureType = CONSTYARD;
        sUpgradeInfo[i].providesTypeId = -1;
        sUpgradeInfo[i].providesType = STRUCTURE;
        sUpgradeInfo[i].providesTypeList = eListType::LIST_NONE;
        sUpgradeInfo[i].providesTypeSubList = -1;
        sUpgradeInfo[i].buildTime = 5;
        strcpy(sUpgradeInfo[i].description, "Upgrade");
    }

    // CONSTYARD UPGRADES

    // First upgrade Constyard: 4Slabs
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].house = Atreides | Harkonnen | Ordos | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].techLevel = 4; // start from mission 4
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].icon = ICON_STR_4SLAB;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].cost = 200;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].structureType = CONSTYARD;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].atUpgradeLevel = 0;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].providesType = STRUCTURE;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeId = SLAB4;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeList = eListType::LIST_CONSTYARD;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeSubList = SUBLIST_CONSTYARD;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].buildTime = 50;
    strcpy(sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_SLAB4].description, "Build 4 concrete slabs at once");

    // Second upgrade Constyard: Rturret
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].house = Atreides | Harkonnen | Ordos | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].techLevel = 6;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].icon = ICON_STR_RTURRET;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].cost = 200;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].needsStructureType = RADAR;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].structureType = CONSTYARD;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].atUpgradeLevel = 1;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].providesType = STRUCTURE;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeId = RTURRET;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeList = eListType::LIST_CONSTYARD;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeSubList = SUBLIST_CONSTYARD;
    sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].buildTime = 150;

    strcpy(sUpgradeInfo[UPGRADE_TYPE_CONSTYARD_RTURRET].description, "Build Rocket Turret");

    // LIGHTFACTORY UPGRADES, only for ATREIDES and ORDOS
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].house = Atreides | Ordos | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].techLevel = 3;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].icon = ICON_UNIT_QUAD;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].cost = sStructureInfo[LIGHTFACTORY].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].needsStructureType = LIGHTFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].structureType = LIGHTFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].atUpgradeLevel = 0;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeId = QUAD;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeList = eListType::LIST_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeSubList =  SUBLIST_LIGHTFCTRY;
    sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].buildTime = 150;

    strcpy(sUpgradeInfo[UPGRADE_TYPE_LIGHTFCTRY_QUAD].description, "Build Quad at Light Factory");

    // HEAVYFACTORY UPGRADES:

    // ALL HOUSES GET MVC
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].house = Atreides | Ordos | Harkonnen | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].techLevel = 4;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].icon = ICON_UNIT_MCV;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost = sStructureInfo[HEAVYFACTORY].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].needsStructureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].structureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].atUpgradeLevel = 0;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeId = MCV;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeList = eListType::LIST_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].buildTime = 150;

    strcpy(sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_MVC].description, "Build MCV at Heavy Factory");

    // Harkonnen/Atreides only
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].house = Atreides | Harkonnen | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].techLevel = 5;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].icon = ICON_UNIT_LAUNCHER;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].cost = sStructureInfo[HEAVYFACTORY].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].needsStructureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].structureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].atUpgradeLevel = 1; // requires MCV upgrade first
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeId = LAUNCHER;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeList = eListType::LIST_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].buildTime = 150;

    strcpy(sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].description, "Build Rocket Launcher at Heavy Factory");

    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].house = Atreides | Harkonnen | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].techLevel = 6;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].icon = ICON_UNIT_SIEGETANK;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].cost = sStructureInfo[HEAVYFACTORY].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].needsStructureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].structureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].atUpgradeLevel = 2; // After upgrade to Rocket Launcher
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeId = SIEGETANK;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeList = eListType::LIST_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].buildTime = 150;

    strcpy(sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].description, "Build Siege Tank at Heavy Factory");

    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].house = Ordos;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].techLevel = 6;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].icon = ICON_UNIT_SIEGETANK;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].cost = sStructureInfo[HEAVYFACTORY].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].needsStructureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].structureType = HEAVYFACTORY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].atUpgradeLevel = 1;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeId = SIEGETANK;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeList = eListType::LIST_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].buildTime = 150;

    strcpy(sUpgradeInfo[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].description, "Build Siege Tank at Heavy Factory");

    // HI-TECH UPGRADES (Ordos/Atreides only)
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].house = Atreides | Ordos | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].techLevel = 8;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].icon = ICON_UNIT_ORNITHOPTER;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].cost = sStructureInfo[HIGHTECH].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].structureType = HIGHTECH;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].atUpgradeLevel = 0;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].providesTypeId = ORNITHOPTER;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].providesTypeList = eListType::LIST_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].providesTypeSubList = SUBLIST_HIGHTECH;
    sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].buildTime = 150;
    strcpy(sUpgradeInfo[UPGRADE_TYPE_HITECH_ORNI].description, "Build Ornithopter at Hi-Tech");

    // WOR (Harkonnen & Ordos)
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].house = Harkonnen | Ordos | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].techLevel = 3;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].icon = ICON_UNIT_TROOPERS;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].cost = sStructureInfo[WOR].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].structureType = WOR;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].atUpgradeLevel = 0;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].providesTypeId = TROOPERS;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].providesTypeList = eListType::LIST_FOOT_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].providesTypeSubList = SUBLIST_TROOPERS;
    if (!game.isDebugMode()) {
        sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].buildTime = 150;
    }
    strcpy(sUpgradeInfo[UPGRADE_TYPE_WOR_TROOPERS].description, "Build Troopers at WOR");

    // BARRACKS (Atreides & Ordos)
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].enabled = true;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].house = Atreides | Ordos | Sardaukar;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].techLevel = 3;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].icon = ICON_UNIT_INFANTRY;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].cost = sStructureInfo[BARRACKS].cost / 2;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].structureType = BARRACKS;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].atUpgradeLevel = 0;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].providesType = UNIT;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeId = INFANTRY;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeList = eListType::LIST_FOOT_UNITS;
    sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeSubList = SUBLIST_INFANTRY;
    if (!game.isDebugMode()) {
        sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].buildTime = 150;
    }
    strcpy(sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].description, "Build Infantry at Barracks");

}


/*****************************
 Structure Rules
 *****************************/
void install_structures()
{

    logbook("Installing:  STRUCTURES");
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        s_StructureInfo &structureInfo = sStructureInfo[i];
        structureInfo.bmp = gfxdata->getSurface(BUILD_WINDTRAP); // in case an invalid bitmap, we are a windtrap
        // structureInfo.shadow = nullptr;
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
        structureInfo.configured = false;
        structureInfo.canAttackAirUnits = false;
        structureInfo.canAttackGroundUnits = false;
        strcpy(structureInfo.name, "Unknown");
    }

    // Single and 4 slabs
    sStructureInfo[SLAB1].bmp = gfxdata->getSurface(PLACE_SLAB1); // in case an invalid bitmap, we are a windtrap
    sStructureInfo[SLAB1].icon = ICON_STR_1SLAB;
    sStructureInfo[SLAB1].hp = 25;            // Not functional in-game, only for building
    sStructureInfo[SLAB1].bmp_width = 16 * 2;
    sStructureInfo[SLAB1].bmp_height = 16 * 2;
    sStructureInfo[SLAB1].configured = true;
    sStructureInfo[SLAB1].queuable = true;
    strcpy(sStructureInfo[SLAB1].name, "Concrete Slab");

    sStructureInfo[SLAB4].bmp = gfxdata->getSurface(PLACE_SLAB4); // in case an invalid bitmap, we are a windtrap
    sStructureInfo[SLAB4].icon = ICON_STR_4SLAB;
    sStructureInfo[SLAB4].hp = 75;            // Not functional in-game, only for building
    sStructureInfo[SLAB4].bmp_width = 32 * 2;
    sStructureInfo[SLAB4].bmp_height = 32 * 2;
    sStructureInfo[SLAB4].configured = true;
    sStructureInfo[SLAB4].queuable = true;
    strcpy(sStructureInfo[SLAB4].name, "4 Concrete Slabs");


    // Concrete Wall
    sStructureInfo[WALL].bmp = gfxdata->getSurface(PLACE_WALL); // in case an invalid bitmap, we are a windtrap
    sStructureInfo[WALL].icon = ICON_STR_WALL;
    sStructureInfo[WALL].hp = 75;            // Not functional in-game, only for building
    sStructureInfo[WALL].bmp_width = 16 * 2;
    sStructureInfo[WALL].bmp_height = 16 * 2;
    sStructureInfo[WALL].queuable = true;
    sStructureInfo[WALL].configured = true;
    strcpy(sStructureInfo[WALL].name, "Concrete Wall");

    // Structure    : Windtrap
    // Description  : <none>
    sStructureInfo[WINDTRAP].bmp = gfxdata->getSurface(BUILD_WINDTRAP);
    // sStructureInfo[WINDTRAP].shadow = gfxdata->getSurface(BUILD_WINDTRAP_SHADOW); // shadow
    sStructureInfo[WINDTRAP].fadecol = 128;
    sStructureInfo[WINDTRAP].fademax = 134;
    sStructureInfo[WINDTRAP].icon = ICON_STR_WINDTRAP;
    sStructureInfo[WINDTRAP].configured = true;
    sStructureInfo[WINDTRAP].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 53,
        .relY = 37
    });
    strcpy(sStructureInfo[WINDTRAP].name, "Windtrap");

    // Structure    : Heavy Factory
    // Description  : <none>
    sStructureInfo[HEAVYFACTORY].bmp_width = 48 * 2;
    sStructureInfo[HEAVYFACTORY].bmp_height = 32 * 2;
    sStructureInfo[HEAVYFACTORY].bmp = gfxdata->getSurface(BUILD_HEAVYFACTORY);
    // sStructureInfo[HEAVYFACTORY].shadow = gfxdata->getSurface(BUILD_HEAVYFACTORY_SHADOW); // shadow
    sStructureInfo[HEAVYFACTORY].flash = gfxdata->getSurface(BUILD_HEAVYFACTORY_FLASH);
    sStructureInfo[HEAVYFACTORY].fadecol = -1;
    sStructureInfo[HEAVYFACTORY].icon = ICON_STR_HEAVYFACTORY;
    sStructureInfo[HEAVYFACTORY].configured = true;
    sStructureInfo[HEAVYFACTORY].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 25,
        .relY = 3
    });
    strcpy(sStructureInfo[HEAVYFACTORY].name, "Heavy Factory");

    // Structure    : Hight Tech Factory (for aircraft only)
    // Description  : <none>
    sStructureInfo[HIGHTECH].bmp_width = 48 * 2;
    sStructureInfo[HIGHTECH].bmp_height = 32 * 2;
    sStructureInfo[HIGHTECH].bmp = gfxdata->getSurface(BUILD_HIGHTECH);
    // sStructureInfo[HIGHTECH].shadow = gfxdata->getSurface(BUILD_HIGHTECH_SHADOW);
    sStructureInfo[HIGHTECH].fadecol = -1;
    sStructureInfo[HIGHTECH].icon = ICON_STR_HIGHTECH;
    sStructureInfo[HIGHTECH].configured = true;
    sStructureInfo[HIGHTECH].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 19,
        .relY = 36
    });
    strcpy(sStructureInfo[HIGHTECH].name, "Hi-Tech");

    // Structure    : Repair
    // Description  : <none>
    sStructureInfo[REPAIR].bmp_width = 48 * 2;
    sStructureInfo[REPAIR].bmp_height = 32 * 2;
    sStructureInfo[REPAIR].bmp = gfxdata->getSurface(BUILD_REPAIR);
    // sStructureInfo[REPAIR].shadow = gfxdata->getSurface(BUILD_REPAIR_SHADOW);
    sStructureInfo[REPAIR].fadecol = -1;
    sStructureInfo[REPAIR].icon = ICON_STR_REPAIR;
    sStructureInfo[REPAIR].configured = true;
    sStructureInfo[REPAIR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 41,
        .relY = 3
    });
    strcpy(sStructureInfo[REPAIR].name, "Repair Facility");

    // Structure    : Palace
    // Description  : <none>
    sStructureInfo[PALACE].bmp_width = 48 * 2;
    sStructureInfo[PALACE].bmp_height = 48 * 2;
    sStructureInfo[PALACE].bmp = gfxdata->getSurface(BUILD_PALACE);
    // sStructureInfo[PALACE].shadow = gfxdata->getSurface(BUILD_PALACE_SHADOW);
    sStructureInfo[PALACE].icon = ICON_STR_PALACE;
    sStructureInfo[PALACE].configured = true;
    sStructureInfo[PALACE].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 28,
        .relY = 14
    });
    strcpy(sStructureInfo[PALACE].name, "Palace");

    // Structure    : Light Factory
    // Description  : <none>
    sStructureInfo[LIGHTFACTORY].bmp_width = 32 * 2;
    sStructureInfo[LIGHTFACTORY].bmp_height = 32 * 2;
    sStructureInfo[LIGHTFACTORY].bmp = gfxdata->getSurface(BUILD_LIGHTFACTORY);
    // sStructureInfo[LIGHTFACTORY].shadow = gfxdata->getSurface(BUILD_LIGHTFACTORY_SHADOW);
    sStructureInfo[LIGHTFACTORY].flash = gfxdata->getSurface(BUILD_LIGHTFACTORY_FLASH);
    sStructureInfo[LIGHTFACTORY].fadecol = -1;
    sStructureInfo[LIGHTFACTORY].icon = ICON_STR_LIGHTFACTORY;
    sStructureInfo[LIGHTFACTORY].configured = true;
    sStructureInfo[LIGHTFACTORY].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 41,
        .relY = 2
    });
    strcpy(sStructureInfo[LIGHTFACTORY].name, "Light Factory");

    // Structure    : Radar
    // Description  : <none>
    sStructureInfo[RADAR].bmp_width = 32 * 2;
    sStructureInfo[RADAR].bmp_height = 32 * 2;
    sStructureInfo[RADAR].bmp = gfxdata->getSurface(BUILD_RADAR);
    // sStructureInfo[RADAR].shadow = gfxdata->getSurface(BUILD_RADAR_SHADOW); // shadow
    sStructureInfo[RADAR].sight = 12;
    sStructureInfo[RADAR].fadecol = -1;
    sStructureInfo[RADAR].icon = ICON_STR_RADAR;
    sStructureInfo[RADAR].configured = true;
    // outpost has 2 flags
    sStructureInfo[RADAR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 17,
        .relY = 38
    });
    sStructureInfo[RADAR].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 12,
        .relY = 46
    });
    strcpy(sStructureInfo[RADAR].name, "Outpost");

    // Structure    : Barracks
    // Description  : <none>
    sStructureInfo[BARRACKS].bmp_width = 32 * 2;
    sStructureInfo[BARRACKS].bmp_height = 32 * 2;
    sStructureInfo[BARRACKS].bmp = gfxdata->getSurface(BUILD_BARRACKS);
    // sStructureInfo[BARRACKS].shadow = gfxdata->getSurface(BUILD_BARRACKS_SHADOW);
    sStructureInfo[BARRACKS].fadecol = -1;
    sStructureInfo[BARRACKS].icon = ICON_STR_BARRACKS;
    sStructureInfo[BARRACKS].configured = true;
    sStructureInfo[BARRACKS].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 60,
        .relY = 47
    });
    sStructureInfo[BARRACKS].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 51,
        .relY = 50
    });
    strcpy(sStructureInfo[BARRACKS].name, "Barracks");

    // Structure    : WOR
    // Description  : <none>
    sStructureInfo[WOR].bmp_width = 32 * 2;
    sStructureInfo[WOR].bmp_height = 32 * 2;
    sStructureInfo[WOR].bmp = gfxdata->getSurface(BUILD_WOR);
    // sStructureInfo[WOR].shadow = gfxdata->getSurface(BUILD_WOR_SHADOW);
    sStructureInfo[WOR].fadecol = -1;
    sStructureInfo[WOR].icon = ICON_STR_WOR;
    sStructureInfo[WOR].configured = true;
    sStructureInfo[WOR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 21,
        .relY = 31
    });
    strcpy(sStructureInfo[WOR].name, "WOR");


    // Structure    : Silo
    // Description  : <none>
    sStructureInfo[SILO].bmp_width = 32 * 2;
    sStructureInfo[SILO].bmp_height = 32 * 2;
    sStructureInfo[SILO].bmp = gfxdata->getSurface(BUILD_SILO);
    // sStructureInfo[SILO].shadow = gfxdata->getSurface(BUILD_SILO_SHADOW);
    sStructureInfo[SILO].fadecol = -1;
    sStructureInfo[SILO].icon = ICON_STR_SILO;
    sStructureInfo[SILO].configured = true;
    sStructureInfo[SILO].queuable = true;
    sStructureInfo[SILO].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 43,
        .relY = 20
    });
    strcpy(sStructureInfo[SILO].name, "Spice Storage Silo");

    // Structure    : Refinery
    // Description  : <none>
    sStructureInfo[REFINERY].bmp_width = 48 * 2;
    sStructureInfo[REFINERY].bmp_height = 32 * 2;
    sStructureInfo[REFINERY].bmp = gfxdata->getSurface(BUILD_REFINERY);
    // sStructureInfo[REFINERY].shadow = gfxdata->getSurface(BUILD_REFINERY_SHADOW);
    sStructureInfo[REFINERY].fadecol = -1;
    sStructureInfo[REFINERY].icon = ICON_STR_REFINERY;
    sStructureInfo[REFINERY].configured = true;
    sStructureInfo[REFINERY].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 45,
        .relY = 12
    });
    strcpy(sStructureInfo[REFINERY].name, "Spice Refinery");

    // Structure    : Construction Yard
    // Description  : <none>
    sStructureInfo[CONSTYARD].bmp_width = 32 * 2;
    sStructureInfo[CONSTYARD].bmp_height = 32 * 2;
    sStructureInfo[CONSTYARD].sight = 4;
    sStructureInfo[CONSTYARD].bmp = gfxdata->getSurface(BUILD_CONSTYARD);
    // sStructureInfo[CONSTYARD].shadow = gfxdata->getSurface(BUILD_CONSTYARD_SHADOW);
    sStructureInfo[CONSTYARD].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 14,
        .relY = 14
    });
    sStructureInfo[CONSTYARD].fadecol = -1;
    sStructureInfo[CONSTYARD].icon = ICON_STR_CONSTYARD;
    sStructureInfo[CONSTYARD].configured = true;
    strcpy(sStructureInfo[CONSTYARD].name, "Construction Yard");

    // Structure    : Starport
    // Description  : You can order units from this structure
    sStructureInfo[STARPORT].bmp_width = 48 * 2;
    sStructureInfo[STARPORT].bmp_height = 48 * 2;
    sStructureInfo[STARPORT].bmp = gfxdata->getSurface(BUILD_STARPORT);
    // sStructureInfo[STARPORT].shadow = gfxdata->getSurface(BUILD_STARPORT_SHADOW);
    sStructureInfo[STARPORT].fadecol = -1;
    sStructureInfo[STARPORT].icon = ICON_STR_STARPORT;
    sStructureInfo[STARPORT].configured = true;
    sStructureInfo[STARPORT].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 16,
        .relY = 3
    });
    strcpy(sStructureInfo[STARPORT].name, "Starport");

    // Structure    : House of IX
    // Description  : Makes it possible for the player to upgrade its Heavy Factory in order to build their special weapon
    sStructureInfo[IX].bmp_width = 32 * 2;
    sStructureInfo[IX].bmp_height = 32 * 2;
    sStructureInfo[IX].bmp = gfxdata->getSurface(BUILD_IX);
    // sStructureInfo[IX].shadow = gfxdata->getSurface(BUILD_IX_SHADOW);
    sStructureInfo[IX].fadecol = -1;
    sStructureInfo[IX].icon = ICON_STR_IX;
    sStructureInfo[IX].configured = true;
    sStructureInfo[IX].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 60, // 60
        .relY = 40  // 40
    });
    strcpy(sStructureInfo[IX].name, "House of IX");

    // Structure    : Normal Turret
    // Description  : defence
    sStructureInfo[TURRET].bmp_width = 16 * 2;
    sStructureInfo[TURRET].bmp_height = 16 * 2;
    sStructureInfo[TURRET].bmp = gfxdata->getSurface(BUILD_TURRET);
    // sStructureInfo[TURRET].shadow = gfxdata->getSurface(BUILD_TURRET_SHADOW);
    sStructureInfo[TURRET].fadecol = -1;
    sStructureInfo[TURRET].icon = ICON_STR_TURRET;
    sStructureInfo[TURRET].sight = 7;
    sStructureInfo[TURRET].configured = true;
    sStructureInfo[TURRET].canAttackGroundUnits = true;
    sStructureInfo[TURRET].fireRate = 275;
    strcpy(sStructureInfo[TURRET].name, "Gun Turret");

    // Structure    : Rocket Turret
    // Description  : defence
    sStructureInfo[RTURRET].bmp_width = 16 * 2;
    sStructureInfo[RTURRET].bmp_height = 16 * 2;
    sStructureInfo[RTURRET].bmp = gfxdata->getSurface(BUILD_RTURRET);
    // sStructureInfo[RTURRET].shadow = gfxdata->getSurface(BUILD_RTURRET_SHADOW);
    sStructureInfo[RTURRET].fadecol = -1;
    sStructureInfo[RTURRET].icon = ICON_STR_RTURRET;
    sStructureInfo[RTURRET].sight = 10;
    sStructureInfo[RTURRET].configured = true;
    sStructureInfo[RTURRET].canAttackAirUnits = true;
    sStructureInfo[RTURRET].canAttackGroundUnits = true;
    sStructureInfo[RTURRET].fireRate = 350;
    strcpy(sStructureInfo[RTURRET].name, "Rocket Turret");

    // Structure    : Windtrap
    // Description  : <none>

}


/******************************
 Calculation for SPICE/POWER bars, returns amount of pixels
 ******************************/
// MAX_W = pixels maxed
// I = How much we have (CURRENT STATE)
// W = MAX it can have
float health_bar(float max_w, int i, int w)
{
    float flHP   = i;
    float flMAX  = w;

    if (flHP > flMAX) {
        return max_w;
    }

    // amount of pixels (max_w = 100%)
    auto health = flHP / flMAX;

    return (health * max_w);
}

// return a border cell, close to iCll
int iFindCloseBorderCell(int iCll)
{
    return map.findCloseMapBorderCellRelativelyToDestinationCel(iCll);
}


int distanceBetweenCellAndCenterOfScreen(int iCell)
{
    if (map.isValidCell(iCell)) {
        int centerX = mapCamera->getViewportCenterX();
        int centerY = mapCamera->getViewportCenterY();

        int cellX = map.getAbsoluteXPositionFromCell(iCell);
        int cellY = map.getAbsoluteYPositionFromCell(iCell);

        return ABS_length(centerX, centerY, cellX, cellY);
    }
    // return as if very close?
    return 1;
}

/**
 * Creates a bullet, of type, starting at *fromCell* and moving towards *targetCell*. The 'unitWhichShoots' or
 * 'structureWhichShoots' is the owner of the bullet.
 * @param type
 * @param fromCell
 * @param targetCell
 * @param unitWhichShoots
 * @param structureWhichShoots
 * @return
 */
int create_bullet(int type, int fromCell, int targetCell, int unitWhichShoots, int structureWhichShoots)
{
    int new_id = -1;

    for (int i = 0; i < MAX_BULLETS; i++)
        if (bullet[i].bAlive == false) {
            new_id = i;
            break;
        }

    if (new_id < 0)
        return -1;  // failed

    if (type < 0)
        return -1; // failed


    cBullet &newBullet = bullet[new_id];
    newBullet.init();

    newBullet.iType = type;
    newBullet.posX = map.getAbsoluteXPositionFromCellCentered(fromCell);
    newBullet.posY = map.getAbsoluteYPositionFromCellCentered(fromCell);
    newBullet.iOwnerStructure = structureWhichShoots;
    newBullet.iOwnerUnit = unitWhichShoots;

    newBullet.targetX = map.getAbsoluteXPositionFromCellCentered(targetCell);
    newBullet.targetY = map.getAbsoluteYPositionFromCellCentered(targetCell);

    // if we start firing from a mountain, flag it so the bullet won't be blocked by mountains along
    // the way
    newBullet.bStartedFromMountain = map.getCell(fromCell)->type == TERRAIN_MOUNTAIN;

    int structureIdAtTargetCell = map.getCellIdStructuresLayer(targetCell);
    if (structureIdAtTargetCell > -1) {
        cAbstractStructure *pStructure = structure[structureIdAtTargetCell];
        if (pStructure && pStructure->isValid()) {
            newBullet.targetX = pStructure->getRandomPosX();
            newBullet.targetY = pStructure->getRandomPosY();
        }
    }

    newBullet.bAlive = true;
    newBullet.iFrame = 0;

    newBullet.iPlayer = -1;

    if (unitWhichShoots > -1 ) {
        cUnit &cUnit = unit[unitWhichShoots];
        newBullet.iPlayer = cUnit.iPlayer;
        // if an airborn unit shoots (ie Ornithopter), reveal on map for everyone
        if (cUnit.isAirbornUnit()) {
            map.clearShroudForAllPlayers(fromCell, 2);
        }
    }

    if (structureWhichShoots > -1) {
        cAbstractStructure *pStructure = structure[structureWhichShoots];
        newBullet.iPlayer = pStructure->getOwner();

        int unitIdAtTargetCell = map.getCellIdUnitLayer(targetCell);
        if (unitIdAtTargetCell > -1) {
            cUnit &unitTarget = unit[unitIdAtTargetCell];
            // reveal for player which is being attacked
            map.clearShroud(fromCell, 2, unitTarget.iPlayer);
        }

        unitIdAtTargetCell = map.getCellIdAirUnitLayer(targetCell);
        if (unitIdAtTargetCell > -1) {
            cUnit &unitTarget = unit[unitIdAtTargetCell];
            // reveal for player which is being attacked
            map.clearShroud(fromCell, 2, unitTarget.iPlayer);
        }
    }

    if (newBullet.iPlayer < 0) {
        logbook("New bullet produced without any player!?");
    }

    // play sound (when we have one)
    s_BulletInfo &sBullet = sBulletInfo[type];
    if (sBullet.sound > -1) {
        game.playSoundWithDistance(sBullet.sound, distanceBetweenCellAndCenterOfScreen(fromCell));
    }

    return new_id;
}

const char *toStringBuildTypeSpecificType(const eBuildType &buildType, const int &specificTypeId)
{
    switch (buildType) {
        case eBuildType::SPECIAL:
            return sSpecialInfo[specificTypeId].description;
        case eBuildType::UNIT:
            return sUnitInfo[specificTypeId].name;
        case eBuildType::STRUCTURE:
            return sStructureInfo[specificTypeId].name;
        case eBuildType::BULLET:
            return sBulletInfo[specificTypeId].description;
        case eBuildType::UPGRADE:
            return sUpgradeInfo[specificTypeId].description;
        case eBuildType::UNKNOWN:
            return "Unknown";
        default:
            assert(false && "Undefined buildType?");
            break;
    }
    return "";
}
