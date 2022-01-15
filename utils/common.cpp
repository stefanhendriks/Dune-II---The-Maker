/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */

#include "../include/d2tmh.h"

#include "utils/cLog.h"

#include <math.h>

namespace
{
    constexpr int PAN_CENTER = 128;
    constexpr int VOLUME_MAX = 255;
    constexpr int BUFFER_SIZE = 32768;
}

/**
 * Default printing in logs. Only will be done if DEBUGGING is true.
 * @param txt
 */
void logbook(const std::string& txt) {
  if (DEBUGGING) {
    cLogger *logger = cLogger::getInstance();
    logger->log(LOG_WARN, COMP_NONE, "(logbook)", txt);
  }
}

int slowThinkMsToTicks(int desiredMs) {
    if (desiredMs < 100) {
        return 1; // fastest thinking is 1 tick (100 ms)
    }
    // "slow" thinking, is 1 tick == 100ms
    return desiredMs / 100;
}

int fastThinkMsToTicks(int desiredMs) {
    if (desiredMs < 5) {
        return 1; // fastest thinking is 1 tick (5 ms)
    }
    // "fast" thinking, is 1 tick == 5ms
    return desiredMs / 5;
}

/**
 * Returns true if x,y is within the playable map boundaries
 * @param x
 * @param y
 * @return
 */

/**
 * Returns value within MIN/MAX provided
 * @param val
 * @param min
 * @param max
 * @return
 */
int BETWEEN(int val, int min, int max) {
  if (val < min) return min;
  if (val > max) return max;
  return val;
}

// fixes the positions according to the PLAYABLE size of the map (for unit
// dumping, etc)
void FIX_BORDER_POS(int &x, int &y) {
    if (x < 1) {
        x = 1;
    }
    if (x > (map.getWidth() - 2)) {
        x = (map.getWidth() - 2);
    }

    if (y < 1) {
        y = 1;
    }
    if (y > (map.getHeight() - 2)) {
        y = (map.getHeight() - 2);
    }
}

// Will make sure the X and Y don't get out of their boundaries
void FIX_POS(int &x, int &y) {
    // filled in
    if (x) {
        if (x < 0) x = 0;
        if (x >= map.getWidth()) x = (map.getWidth()-1);
    }

    // filled in
    if (y) {
        if (y < 0) y = 0;
        if (y >= map.getHeight()) y = (map.getHeight() -1);
    }
}


/********************************
 House Rules
 ********************************/
void INSTALL_HOUSES() {
    // General / Default / No House
    sHouseInfo[GENERALHOUSE].swap_color = 128;
    sHouseInfo[GENERALHOUSE].minimap_color = makecol(128, 128, 128);

    // Harkonnen
    sHouseInfo[HARKONNEN].swap_color = -1;  // 144
    sHouseInfo[HARKONNEN].minimap_color = makecol(255, 0, 0);

    // Atreides
    sHouseInfo[ATREIDES].swap_color = 160;
    sHouseInfo[ATREIDES].minimap_color = makecol(0, 0, 255);

    // Ordos
    sHouseInfo[ORDOS].swap_color = 176;
    sHouseInfo[ORDOS].minimap_color = makecol(0, 255, 0);

    // Mercenary
    sHouseInfo[MERCENARY].swap_color = 192;
    sHouseInfo[MERCENARY].minimap_color = makecol(214, 121, 16);

    // Sardaukar
    sHouseInfo[SARDAUKAR].swap_color = 208;
    sHouseInfo[SARDAUKAR].minimap_color = makecol(255, 0, 255);

    // Fremen
    sHouseInfo[FREMEN].swap_color = 224;
    sHouseInfo[FREMEN].minimap_color = makecol(194, 125, 60); // Fremen is colored as "sand" on the minimap

    // GREY

    // ???
    sHouseInfo[CORRINO].swap_color = 136;
    sHouseInfo[CORRINO].minimap_color = makecol(192, 192, 192); // grey
}

/*****************************
 Unit Rules
 *****************************/
void install_units() {
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
        unitInfo.bmp = (BITMAP *) gfxdata[UNIT_QUAD].dat; // default bitmap is a quad!
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
        unitInfo.buildTime = -1;
        unitInfo.airborn = false;
        unitInfo.infantry = false;
        unitInfo.free_roam = false;
        unitInfo.fireTwice = false;
        unitInfo.squish = true;             // most units can squish (infantry)
        unitInfo.canBeSquished = false;     // most units cannot be squished
        unitInfo.range = -1;
        unitInfo.sight = -1;
        unitInfo.queuable = true;
        unitInfo.dieWhenLowerThanHP = -1;
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
    sUnitInfo[CARRYALL].bmp = (BITMAP *) gfxdata[UNIT_CARRYALL].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[CARRYALL].shadow = (BITMAP *) gfxdata[UNIT_CARRYALL_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    sUnitInfo[ORNITHOPTER].bmp = (BITMAP *) gfxdata[UNIT_ORNITHOPTER].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[ORNITHOPTER].shadow = (BITMAP *) gfxdata[UNIT_ORNITHOPTER_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    sUnitInfo[DEVASTATOR].bmp = (BITMAP *) gfxdata[UNIT_DEVASTATOR].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[DEVASTATOR].shadow = (BITMAP *) gfxdata[UNIT_DEVASTATOR_SHADOW].dat;      // pointer to the original bitmap shadow
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
    strcpy(sUnitInfo[DEVASTATOR].name, "Devastator");

    // Unit        : Harvester
    // Description : Harvester
    sUnitInfo[HARVESTER].bmp = (BITMAP *) gfxdata[UNIT_HARVESTER].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[HARVESTER].shadow = (BITMAP *) gfxdata[UNIT_HARVESTER_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    sUnitInfo[TANK].bmp = (BITMAP *) gfxdata[UNIT_TANKBASE].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[TANK].shadow = (BITMAP *) gfxdata[UNIT_TANKBASE_SHADOW].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[TANK].top = (BITMAP *) gfxdata[UNIT_TANKTOP].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[TANK].name, "Tank");


    // Unit        : Siege Tank
    // Description : Siege tank
    sUnitInfo[SIEGETANK].bmp = (BITMAP *) gfxdata[UNIT_SIEGEBASE].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[SIEGETANK].shadow = (BITMAP *) gfxdata[UNIT_SIEGEBASE_SHADOW].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[SIEGETANK].top = (BITMAP *) gfxdata[UNIT_SIEGETOP].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[SIEGETANK].name, "Siege Tank");

    // Unit        : MCV
    // Description : Movable Construction Vehicle
    sUnitInfo[MCV].bmp = (BITMAP *) gfxdata[UNIT_MCV].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[MCV].shadow = (BITMAP *) gfxdata[UNIT_MCV_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    sUnitInfo[DEVIATOR].bmp = (BITMAP *) gfxdata[UNIT_DEVIATOR].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[DEVIATOR].name, "Deviator");

    // Unit        : Launcher
    // Description : Rocket Launcher
    sUnitInfo[LAUNCHER].bmp = (BITMAP *) gfxdata[UNIT_LAUNCHER].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[LAUNCHER].shadow = (BITMAP *) gfxdata[UNIT_LAUNCHER_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[LAUNCHER].name, "Launcher");

    // Unit        : Quad
    // Description : Quad, 4 wheeled (double gunned)
    sUnitInfo[QUAD].bmp = (BITMAP *) gfxdata[UNIT_QUAD].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[QUAD].shadow = (BITMAP *) gfxdata[UNIT_QUAD_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[QUAD].name, "Quad");


    // Unit        : Trike (normal trike)
    // Description : Trike, 3 wheeled (single gunned)
    sUnitInfo[TRIKE].bmp = (BITMAP *) gfxdata[UNIT_TRIKE].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[TRIKE].shadow = (BITMAP *) gfxdata[UNIT_TRIKE_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[TRIKE].name, "Trike");

    // Unit        : Raider Trike (Ordos trike)
    // Description : Raider Trike, 3 wheeled (single gunned), weaker, but faster
    sUnitInfo[RAIDER].bmp = (BITMAP *) gfxdata[UNIT_TRIKE].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[RAIDER].shadow = (BITMAP *) gfxdata[UNIT_TRIKE_SHADOW].dat;      // pointer to the original 8bit bitmap
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

    // Unit        : Frigate
    // Description : Frigate
    sUnitInfo[FRIGATE].bmp = (BITMAP *) gfxdata[UNIT_FRIGATE].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[FRIGATE].shadow = (BITMAP *) gfxdata[UNIT_FRIGATE_SHADOW].dat;      // pointer to the original 8bit bitmap
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
    sUnitInfo[SONICTANK].bmp = (BITMAP *) gfxdata[UNIT_SONICTANK].dat;      // pointer to the original 8bit bitmap
    sUnitInfo[SONICTANK].shadow = (BITMAP *) gfxdata[UNIT_SONICTANK_SHADOW].dat;      // pointer to the original 8bit bitmap
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

    strcpy(sUnitInfo[SONICTANK].name, "Sonic Tank");


    // Unit        : Single Soldier
    // Description : 1 soldier
    sUnitInfo[SOLDIER].bmp = (BITMAP *) gfxdata[UNIT_SOLDIER].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[SOLDIER].name, "Soldier");


    // Unit        : Infantry
    // Description : 3 soldiers
    sUnitInfo[INFANTRY].bmp = (BITMAP *) gfxdata[UNIT_SOLDIERS].dat;      // pointer to the original 8bit bitmap
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
    strcpy(sUnitInfo[INFANTRY].name, "Light Infantry");

    // Unit        : Single Trooper
    // Description : 1 trooper
    sUnitInfo[TROOPER].bmp = (BITMAP *) gfxdata[UNIT_TROOPER].dat;      // pointer to the original 8bit bitmap
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

    // Unit        : Group Trooper
    // Description : 3 troopers
    sUnitInfo[TROOPERS].bmp = (BITMAP *) gfxdata[UNIT_TROOPERS].dat;      // pointer to the original 8bit bitmap
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

    // Unit        : Fremen
    // Description : A single fremen
    sUnitInfo[UNIT_FREMEN_ONE].bmp = (BITMAP *) gfxdata[UNIT_SINGLEFREMEN].dat;      // pointer to the original 8bit bitmap
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

//  units[UNIT_FREMEN_ONE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_ONE].subListId=0;

    // Unit        : Fremen
    // Description : A group of Fremen
    sUnitInfo[UNIT_FREMEN_THREE].bmp = (BITMAP *) gfxdata[UNIT_TRIPLEFREMEN].dat;      // pointer to the original 8bit bitmap
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
//  units[UNIT_FREMEN_THREE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_THREE].subListId=0;

    // Unit        : Saboteur
    // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
    sUnitInfo[SABOTEUR].bmp = (BITMAP *) gfxdata[UNIT_SABOTEUR].dat;
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
    sUnitInfo[SANDWORM].bmp = (BITMAP *) gfxdata[UNIT_SANDWORM].dat;
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


    // Unit        : <name>
    // Description : <description>

}

void install_particles() {
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
    sParticleInfo[D2TM_PARTICLE_MOVE].bmpIndex = D2TM_BITMAP_PARTICLE_MOVE;
    sParticleInfo[D2TM_PARTICLE_MOVE].startAlpha = 128;

    // attack
    sParticleInfo[D2TM_PARTICLE_ATTACK].bmpIndex = D2TM_BITMAP_PARTICLE_ATTACK;
    sParticleInfo[D2TM_PARTICLE_ATTACK].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_TRIKE;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TRIKE].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_SMOKE].bmpIndex = D2TM_BITMAP_PARTICLE_SMOKE;
    sParticleInfo[D2TM_PARTICLE_SMOKE].startAlpha = 0;
    sParticleInfo[D2TM_PARTICLE_SMOKE].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SMOKE].frameHeight = 48;


    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].bmpIndex = D2TM_BITMAP_PARTICLE_SMOKE_SHADOW;
    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].startAlpha = 0;
    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].frameWidth = 36;
    sParticleInfo[D2TM_PARTICLE_SMOKE_SHADOW].frameHeight = 38;

    sParticleInfo[D2TM_PARTICLE_TRACK_DIA].bmpIndex = D2TM_BITMAP_PARTICLE_TRACK_DIA;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_TRACK_HOR].bmpIndex = D2TM_BITMAP_PARTICLE_TRACK_HOR;
    sParticleInfo[D2TM_PARTICLE_TRACK_HOR].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_HOR].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_TRACK_VER].bmpIndex = D2TM_BITMAP_PARTICLE_TRACK_VER;
    sParticleInfo[D2TM_PARTICLE_TRACK_VER].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_VER].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_TRACK_DIA2].bmpIndex = D2TM_BITMAP_PARTICLE_TRACK_DIA2;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA2].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_TRACK_DIA2].startAlpha = 128;

    sParticleInfo[D2TM_PARTICLE_BULLET_PUF].bmpIndex = D2TM_BITMAP_PARTICLE_BULLET_PUF;
    sParticleInfo[D2TM_PARTICLE_BULLET_PUF].frameWidth = 18;
    sParticleInfo[D2TM_PARTICLE_BULLET_PUF].frameHeight = 18;


    sParticleInfo[D2TM_PARTICLE_EXPLOSION_FIRE].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_FIRE;

    sParticleInfo[D2TM_PARTICLE_WORMEAT].bmpIndex = D2TM_BITMAP_PARTICLE_WORMEAT;
    sParticleInfo[D2TM_PARTICLE_WORMEAT].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_WORMEAT].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_WORMEAT].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_TANK_ONE;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_TANK_TWO;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_STRUCTURE01;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_STRUCTURE02;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_GAS;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_GAS].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].bmpIndex = D2TM_BITMAP_PARTICLE_WORMTRAIL;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].startAlpha = 96;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].frameWidth = 48;
    sParticleInfo[D2TM_PARTICLE_WORMTRAIL].frameHeight = 48;

    sParticleInfo[D2TM_PARTICLE_DEADINF01].bmpIndex = D2TM_BITMAP_PARTICLE_DEADINF01;
    sParticleInfo[D2TM_PARTICLE_DEADINF01].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;

    sParticleInfo[D2TM_PARTICLE_DEADINF02].bmpIndex = D2TM_BITMAP_PARTICLE_DEADINF02;
    sParticleInfo[D2TM_PARTICLE_DEADINF02].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;

    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].bmpIndex = D2TM_BITMAP_PARTICLE_TANKSHOOT;
    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].startAlpha = 128;
    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].frameWidth = 64;
    sParticleInfo[D2TM_PARTICLE_TANKSHOOT].frameHeight = 64;

    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].bmpIndex = D2TM_BITMAP_PARTICLE_SIEGESHOOT;
    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].startAlpha = 128;
    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].frameWidth = 64;
    sParticleInfo[D2TM_PARTICLE_SIEGESHOOT].frameHeight = 64;

    sParticleInfo[D2TM_PARTICLE_SQUISH01].bmpIndex = D2TM_BITMAP_PARTICLE_SQUISH01;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SQUISH01].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_SQUISH02].bmpIndex = D2TM_BITMAP_PARTICLE_SQUISH02;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SQUISH02].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_SQUISH03].bmpIndex = D2TM_BITMAP_PARTICLE_SQUISH03;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_SQUISH03].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_ORNI;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].startAlpha = 255;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].frameWidth = 32;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ORNI].frameHeight = 32;

    sParticleInfo[D2TM_PARTICLE_SIEGEDIE].bmpIndex = D2TM_BITMAP_PARTICLE_SIEGEDIE;

    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].bmpIndex = D2TM_BITMAP_PARTICLE_CARRYPUFF;
    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].layer = D2TM_RENDER_LAYER_PARTICLE_BOTTOM;
    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].frameWidth = 96;
    sParticleInfo[D2TM_PARTICLE_CARRYPUFF].frameHeight = 96;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ROCKET].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_ROCKET;
    sParticleInfo[D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_ROCKET_SMALL;

    sParticleInfo[D2TM_PARTICLE_EXPLOSION_BULLET].bmpIndex = D2TM_BITMAP_PARTICLE_EXPLOSION_BULLET;

    // the nice flare/light effects that come with explosions
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].bmpIndex = D2TM_BITMAP_PARTICLE_OBJECT_BOOM01;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].usesAdditiveBlending = true;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].startAlpha = 240;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].frameWidth = 512;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM01].frameHeight = 512;
            ;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].bmpIndex = D2TM_BITMAP_PARTICLE_OBJECT_BOOM02;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].usesAdditiveBlending = true;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].startAlpha = 230;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].frameWidth = 256;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM02].frameHeight = 256;

    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].bmpIndex = D2TM_BITMAP_PARTICLE_OBJECT_BOOM03;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].usesAdditiveBlending = true;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].startAlpha = 220;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].frameWidth = 128;
    sParticleInfo[D2TM_PARTICLE_OBJECT_BOOM03].frameHeight = 128;

}

void install_specials() {

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
void install_bullets() {
    logbook("Installing:  BULLET TYPES");

    for (int i = 0; i < MAX_BULLET_TYPES; i++) {
        sBulletInfo[i].bmp = NULL; // in case an invalid bitmap; default is a small rocket
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
    sBulletInfo[ROCKET_BIG].bmp = (BITMAP *) gfxdata[BULLET_ROCKET_LARGE].dat;
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
    sBulletInfo[ROCKET_SMALL_ORNI].bmp = (BITMAP *) gfxdata[BULLET_ROCKET_SMALL].dat;
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
    sBulletInfo[ROCKET_SMALL].bmp = (BITMAP *) gfxdata[BULLET_ROCKET_SMALL].dat;
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
    sBulletInfo[ROCKET_SMALL_FREMEN].bmp = (BITMAP *) gfxdata[BULLET_ROCKET_SMALL].dat;
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
    sBulletInfo[ROCKET_NORMAL].bmp = (BITMAP *) gfxdata[BULLET_ROCKET_NORMAL].dat;
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
    sBulletInfo[BULLET_SMALL].bmp = (BITMAP *) gfxdata[BULLET_DOT_SMALL].dat;
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
    sBulletInfo[BULLET_TRIKE].bmp = (BITMAP *) gfxdata[BULLET_DOT_SMALL].dat;
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
    sBulletInfo[BULLET_QUAD].bmp = (BITMAP *) gfxdata[BULLET_DOT_SMALL].dat;
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
    sBulletInfo[BULLET_TANK].bmp = (BITMAP *) gfxdata[BULLET_DOT_MEDIUM].dat;
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
    sBulletInfo[BULLET_SIEGE].bmp = (BITMAP *) gfxdata[BULLET_DOT_MEDIUM].dat;
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
    sBulletInfo[BULLET_DEVASTATOR].bmp = (BITMAP *) gfxdata[BULLET_DOT_LARGE].dat;
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
    sBulletInfo[BULLET_GAS].bmp = (BITMAP *) gfxdata[BULLET_ROCKET_NORMAL].dat;
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
    sBulletInfo[BULLET_TURRET].bmp = (BITMAP *) gfxdata[BULLET_DOT_MEDIUM].dat;
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
    sBulletInfo[BULLET_SHIMMER].bmp = NULL;
    sBulletInfo[BULLET_SHIMMER].deathParticle = -1;
    sBulletInfo[BULLET_SHIMMER].bmp_width = 0;
    sBulletInfo[BULLET_SHIMMER].damage = 55;
    sBulletInfo[BULLET_SHIMMER].damage_inf = 70; // infantry cant stand the sound, die very fast
    sBulletInfo[BULLET_SHIMMER].max_frames = 0;
    sBulletInfo[BULLET_SHIMMER].max_deadframes = 0;
    sBulletInfo[BULLET_SHIMMER].sound = SOUND_SHIMMER;
    strcpy(sBulletInfo[BULLET_SHIMMER].description, "BULLET_SHIMMER");

    // rocket of rocket turret
    sBulletInfo[ROCKET_RTURRET].bmp = (BITMAP *) gfxdata[BULLET_ROCKET_NORMAL].dat;
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


void install_upgrades() {
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
    if (!DEBUGGING) {
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
    if (!DEBUGGING) {
        sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].buildTime = 150;
    }
    strcpy(sUpgradeInfo[UPGRADE_TYPE_BARRACKS_INFANTRY].description, "Build Infantry at Barracks");

}


/*****************************
 Structure Rules
 *****************************/
void install_structures() {

    logbook("Installing:  STRUCTURES");
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        s_StructureInfo &structureInfo = sStructureInfo[i];
        structureInfo.bmp = (BITMAP *) gfxdata[BUILD_WINDTRAP].dat; // in case an invalid bitmap, we are a windtrap
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
        structureInfo.configured = false;
        structureInfo.canAttackAirUnits = false;
        strcpy(structureInfo.name, "Unknown");
    }

    // Single and 4 slabs
    sStructureInfo[SLAB1].bmp = (BITMAP *) gfxdata[PLACE_SLAB1].dat; // in case an invalid bitmap, we are a windtrap
    sStructureInfo[SLAB1].icon = ICON_STR_1SLAB;
    sStructureInfo[SLAB1].hp = 25;            // Not functional in-game, only for building
    sStructureInfo[SLAB1].bmp_width = 16 * 2;
    sStructureInfo[SLAB1].bmp_height = 16 * 2;
    sStructureInfo[SLAB1].configured = true;
    sStructureInfo[SLAB1].queuable = true;
    strcpy(sStructureInfo[SLAB1].name, "Concrete Slab");

    sStructureInfo[SLAB4].bmp = (BITMAP *) gfxdata[PLACE_SLAB4].dat; // in case an invalid bitmap, we are a windtrap
    sStructureInfo[SLAB4].icon = ICON_STR_4SLAB;
    sStructureInfo[SLAB4].hp = 75;            // Not functional in-game, only for building
    sStructureInfo[SLAB4].bmp_width = 32 * 2;
    sStructureInfo[SLAB4].bmp_height = 32 * 2;
    sStructureInfo[SLAB4].configured = true;
    sStructureInfo[SLAB4].queuable = true;
    strcpy(sStructureInfo[SLAB4].name, "4 Concrete Slabs");


    // Concrete Wall
    sStructureInfo[WALL].bmp = (BITMAP *) gfxdata[PLACE_WALL].dat; // in case an invalid bitmap, we are a windtrap
    sStructureInfo[WALL].icon = ICON_STR_WALL;
    sStructureInfo[WALL].hp = 75;            // Not functional in-game, only for building
    sStructureInfo[WALL].bmp_width = 16 * 2;
    sStructureInfo[WALL].bmp_height = 16 * 2;
    sStructureInfo[WALL].queuable = true;
    sStructureInfo[WALL].configured = true;
    strcpy(sStructureInfo[WALL].name, "Concrete Wall");

    // Structure    : Windtrap
    // Description  : <none>
    sStructureInfo[WINDTRAP].bmp = (BITMAP *) gfxdata[BUILD_WINDTRAP].dat;
    sStructureInfo[WINDTRAP].shadow = (BITMAP *) gfxdata[BUILD_WINDTRAP_SHADOW].dat; // shadow
    sStructureInfo[WINDTRAP].fadecol = PAN_CENTER;
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
    sStructureInfo[HEAVYFACTORY].bmp = (BITMAP *) gfxdata[BUILD_HEAVYFACTORY].dat;
    sStructureInfo[HEAVYFACTORY].shadow = (BITMAP *) gfxdata[BUILD_HEAVYFACTORY_SHADOW].dat; // shadow
    sStructureInfo[HEAVYFACTORY].flash = (BITMAP *) gfxdata[BUILD_HEAVYFACTORY_FLASH].dat;
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
    sStructureInfo[HIGHTECH].bmp = (BITMAP *) gfxdata[BUILD_HIGHTECH].dat;
    sStructureInfo[HIGHTECH].shadow = (BITMAP *) gfxdata[BUILD_HIGHTECH_SHADOW].dat;
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
    sStructureInfo[REPAIR].bmp = (BITMAP *) gfxdata[BUILD_REPAIR].dat;
    sStructureInfo[REPAIR].shadow = (BITMAP *) gfxdata[BUILD_REPAIR_SHADOW].dat;
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
    sStructureInfo[PALACE].bmp = (BITMAP *) gfxdata[BUILD_PALACE].dat;
    sStructureInfo[PALACE].shadow = (BITMAP *) gfxdata[BUILD_PALACE_SHADOW].dat;
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
    sStructureInfo[LIGHTFACTORY].bmp = (BITMAP *) gfxdata[BUILD_LIGHTFACTORY].dat;
    sStructureInfo[LIGHTFACTORY].shadow = (BITMAP *) gfxdata[BUILD_LIGHTFACTORY_SHADOW].dat;
    sStructureInfo[LIGHTFACTORY].flash = (BITMAP *) gfxdata[BUILD_LIGHTFACTORY_FLASH].dat;
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
    sStructureInfo[RADAR].bmp = (BITMAP *) gfxdata[BUILD_RADAR].dat;
    sStructureInfo[RADAR].shadow = (BITMAP *) gfxdata[BUILD_RADAR_SHADOW].dat; // shadow
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
    sStructureInfo[BARRACKS].bmp = (BITMAP *) gfxdata[BUILD_BARRACKS].dat;
    sStructureInfo[BARRACKS].shadow = (BITMAP *) gfxdata[BUILD_BARRACKS_SHADOW].dat;
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
    sStructureInfo[WOR].bmp = (BITMAP *) gfxdata[BUILD_WOR].dat;
    sStructureInfo[WOR].shadow = (BITMAP *) gfxdata[BUILD_WOR_SHADOW].dat;
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
    sStructureInfo[SILO].bmp = (BITMAP *) gfxdata[BUILD_SILO].dat;
    sStructureInfo[SILO].shadow = (BITMAP *) gfxdata[BUILD_SILO_SHADOW].dat;
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
    sStructureInfo[REFINERY].bmp = (BITMAP *) gfxdata[BUILD_REFINERY].dat;
    sStructureInfo[REFINERY].shadow = (BITMAP *) gfxdata[BUILD_REFINERY_SHADOW].dat;
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
    sStructureInfo[CONSTYARD].bmp = (BITMAP *) gfxdata[BUILD_CONSTYARD].dat;
    sStructureInfo[CONSTYARD].shadow = (BITMAP *) gfxdata[BUILD_CONSTYARD_SHADOW].dat;
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
    sStructureInfo[STARPORT].bmp = (BITMAP *) gfxdata[BUILD_STARPORT].dat;
    sStructureInfo[STARPORT].shadow = (BITMAP *) gfxdata[BUILD_STARPORT_SHADOW].dat;
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
    sStructureInfo[IX].bmp = (BITMAP *) gfxdata[BUILD_IX].dat;
    sStructureInfo[IX].shadow = (BITMAP *) gfxdata[BUILD_IX_SHADOW].dat;
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
    sStructureInfo[TURRET].bmp = (BITMAP *) gfxdata[BUILD_TURRET].dat;
    sStructureInfo[TURRET].shadow = (BITMAP *) gfxdata[BUILD_TURRET_SHADOW].dat;
    sStructureInfo[TURRET].fadecol = -1;
    sStructureInfo[TURRET].icon = ICON_STR_TURRET;
    sStructureInfo[TURRET].sight = 7;
    sStructureInfo[TURRET].configured = true;
    strcpy(sStructureInfo[TURRET].name, "Gun Turret");

    // Structure    : Rocket Turret
    // Description  : defence
    sStructureInfo[RTURRET].bmp_width = 16 * 2;
    sStructureInfo[RTURRET].bmp_height = 16 * 2;
    sStructureInfo[RTURRET].bmp = (BITMAP *) gfxdata[BUILD_RTURRET].dat;
    sStructureInfo[RTURRET].shadow = (BITMAP *) gfxdata[BUILD_RTURRET_SHADOW].dat;
    sStructureInfo[RTURRET].fadecol = -1;
    sStructureInfo[RTURRET].icon = ICON_STR_RTURRET;
    sStructureInfo[RTURRET].sight = 10;
    sStructureInfo[RTURRET].configured = true;
    sStructureInfo[RTURRET].canAttackAirUnits = true;
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
float health_bar(float max_w, int i, int w) {
  float flHP   = i;
  float flMAX  = w;

  if (flHP > flMAX) {
      return max_w;
  }

  // amount of pixels (max_w = 100%)
  auto health = flHP / flMAX;

  return (health * max_w);
}

/**
 * Given value v, and a MIN and MAX. Make sure v is between MIN and MAX. Ie, if v < MIN, it returns MIN. if v > MAX
 * it returns MAX. Else returns v.
 * @param value
 * @param min
 * @param max
 * @return
 */
int keepBetween(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// return a border cell, close to iCll
int iFindCloseBorderCell(int iCll) {
    return map.findCloseMapBorderCellRelativelyToDestinationCel(iCll);
}


int distanceBetweenCellAndCenterOfScreen(int iCell) {
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

void play_sound_id(int s, int volume) {
    if (!game.bPlaySound) return; // do not play sound when boolean is false.

    int vol = keepBetween(volume, 0, 255);

    if (vol > 0) {
        game.getSoundPlayer()->playSound(s, 127, vol);
    }
}

void play_sound_id(int s) {
    play_sound_id(s, VOLUME_MAX);
}

/**
 * Play sound (s = ID from gfxaudio) with distance into account. iDistance
 * is the distance outside screen. Meaning <= 1 is ON SCREEN > 1 means distance from screen.
 *
 * @param s
 * @param iDistance
 */
void play_sound_id_with_distance(int s, int iDistance) {
	if (!game.bPlaySound) return; // do not play sound when boolean is false.

	if (gfxaudio[s].dat == NULL) return; // no data file at the specified position in index.

	if (iDistance <= 1) { // means "on screen" (meaning fixed volume, and no need for panning)
	    play_sound_id(s, VOLUME_MAX);
		return;
	}

	// zoom factor influences distance we can 'hear'. The closer up, the less max distance. Unzoomed, this is half the map.
	// where when unit is at half map, we can hear it only a bit.
    float maxDistance = mapCamera->divideByZoomLevel(map.getMaxDistanceInPixels()/2);
    float distanceNormalized = 1.0 - ((float)iDistance / maxDistance);

    float volume = game.getMaxVolume() * distanceNormalized;

    // zoom factor influences volume (more zoomed in means louder)
    float volumeFactor = mapCamera->factorZoomLevel(0.7f);
    int iVolFactored = volumeFactor * volume;

    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "iDistance [%d], distanceNormalized [%f] maxDistance [%f], zoomLevel [%f], volumeFactor [%f], volume [%f], iVolFactored [%d]",
                iDistance,
                distanceNormalized,
                maxDistance,
                mapCamera->getZoomLevel(),
                volumeFactor,
                volume,
                iVolFactored);
        logbook(msg);
    }

    play_sound_id(s, iVolFactored);
}

void play_voice(int iType) {
	if (players[0].getHouse() == HARKONNEN) {
		iType++;
	}

	if (players[0].getHouse() == ORDOS) {
		iType+=2;
	}

    play_sound_id(iType); // pass -1 as 'onscreen' since its a normal sound
}


bool MIDI_music_playing() {
	if (midi_pos > -1) {
		return true;
	} else {
		return false;
	}
}

void setMusicVolume(int i) {
    if (game.bMp3) {
        if (mp3_music != NULL) {
        	almp3_adjust_mp3(mp3_music, i, 127, 1000, false);
        }
    } else {
       set_volume(VOLUME_MAX, i);
    }
}


void mp3_play_file(char filename[VOLUME_MAX]) {
	auto len = file_size(filename);
  char *data = new char[len];  // mp3 file in memory
	FILE *f = fopen(filename, "r");

	if (f != NULL) {
		fread(data, 1, len, f);
		fclose(f);
	} else {
		logbook("MP3: Could not find mp3 file for add-on, switching to MIDI mode");
		allegro_message("Could not find MP3 file, add-on incomplete. Switching to MIDI mode");
		game.bMp3=false;

		if (mp3_music != NULL) {
		   almp3_destroy_mp3(mp3_music);
		}

		mp3_music = NULL;
		return;
	}

	almp3_destroy_mp3(mp3_music); // stop music

	mp3_music = almp3_create_mp3(data, len);

	// play music, use big buffer
	int result = almp3_play_mp3(mp3_music, BUFFER_SIZE, VOLUME_MAX, PAN_CENTER);
	assert(result == ALMP3_OK);

	setMusicVolume(game.iMusicVolume);
}

// play type of music
void playMusicByType(int iType) {
    game.iMusicType = iType;

    if (!game.bPlayMusic) return;

    int iNumber=0;

    if (iType == MUSIC_WIN || iType == MUSIC_LOSE) {
        iNumber=rnd(3)+1;
    } else if (iType == MUSIC_ATTACK) {
        iNumber=rnd(6)+1;
    } else if (iType == MUSIC_PEACE) {
        iNumber=rnd(9)+1;
    } else if (iType == MUSIC_MENU) {
        iNumber=MIDI_MENU;
    } else if (iType == MUSIC_CONQUEST) {
    	iNumber=MIDI_SCENARIO;
    } else if (iType == MUSIC_BRIEFING) {
      int houseIndex = players[HUMAN].getHouse();
        if (houseIndex == ATREIDES)         iNumber=MIDI_MENTAT_ATR;
        if (houseIndex == HARKONNEN)     	iNumber=MIDI_MENTAT_HAR;
        if (houseIndex == ORDOS)     		iNumber=MIDI_MENTAT_ORD;
    }

    // In the end, when mp3, play it:
    if (game.bMp3) {
        char filename[50];
        memset(filename, 0, sizeof(filename));

        if (iType == MUSIC_WIN) {
            sprintf(filename, "mp3/win%d.mp3", iNumber);
        } else if (iType == MUSIC_LOSE) {
            sprintf(filename, "mp3/lose%d.mp3", iNumber);
        } else if (iType == MUSIC_ATTACK) {
        	sprintf(filename, "mp3/attack%d.mp3", iNumber);
        } else if (iType == MUSIC_PEACE) {
            sprintf(filename, "mp3/peace%d.mp3", iNumber);
        } else if (iType == MUSIC_MENU) {
            sprintf(filename, "mp3/menu.mp3");
        } else if (iType == MUSIC_CONQUEST) {
            sprintf(filename, "mp3/nextconq.mp3");
        } else if (iType == MUSIC_BRIEFING) {
          int houseIndex = players[HUMAN].getHouse();
            if (houseIndex == ATREIDES)	sprintf(filename, "mp3/mentata.mp3");
            if (houseIndex == HARKONNEN)	sprintf(filename, "mp3/mentath.mp3");
            if (houseIndex == ORDOS)		sprintf(filename, "mp3/mentato.mp3");
        }

        mp3_play_file(filename);
    } else {
        iNumber--; // make 0 based

        if (iType == MUSIC_WIN) {
            iNumber = MIDI_WIN01+(iNumber);
        } else if (iType == MUSIC_LOSE) {
            iNumber = MIDI_LOSE01+(iNumber);
        } else if (iType == MUSIC_ATTACK) {
            iNumber = MIDI_ATTACK01+(iNumber);
        } else if (iType == MUSIC_PEACE) {
            iNumber = MIDI_BUILDING01+(iNumber);
        } else if (iType == MUSIC_MENU) {
        // single ones are 'corrected back'...
            iNumber = iNumber+1;
        } else if (iType == MUSIC_CONQUEST) {
            iNumber = iNumber+1;
        } else if (iType == MUSIC_BRIEFING) {
            iNumber = iNumber+1;
        }
        // play midi file
        play_midi((MIDI *)gfxaudio[iNumber].dat, 0);
    }
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
int create_bullet(int type, int fromCell, int targetCell, int unitWhichShoots, int structureWhichShoots) {
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
        play_sound_id_with_distance(sBullet.sound, distanceBetweenCellAndCenterOfScreen(fromCell));
    }

    return new_id;
}


/**
 * Scramble the pixels. In a radius "r" around position X,Y at the screen! (the x,y are screencoordinates!)
 * @param r
 * @param x
 * @param y
 */
void Shimmer(int r, int x, int y) {
    int x1, y1;
    int nx, ny;
    int gp = 0;     // Get Pixel Result
    int tc = 0;

    // go around 360 fDegrees (twice as fast now)
    float step = std::fmax(1.0f, mapCamera->divideByZoomLevel(4));

    for (float dr = 0; dr < r; dr += step) {
        for (double d = 0; d < 360; d++) {
            x1 = (x + (cos(d) * (dr)));
            y1 = (y + (sin(d) * (dr)));

            if (x1 < 0) x1 = 0;
            if (y1 < 0) y1 = 0;
            if (x1 >= game.screen_x) x1 = game.screen_x - 1;
            if (y1 >= game.screen_y) y1 = game.screen_y - 1;

            gp = getpixel(bmp_screen, x1, y1); // use this inline function to speed up things.
            // Now choose random spot to 'switch' with.
            nx = (x1 - 1) + rnd(2);
            ny = (y1 - 1) + rnd(2);

            if (nx < 0) nx = 0;
            if (ny < 0) ny = 0;
            if (nx >= game.screen_x) nx = game.screen_x - 1;
            if (ny >= game.screen_y) ny = game.screen_y - 1;

            tc = getpixel(bmp_screen, nx, ny);

            if (gp > -1 && tc > -1) {
                // Now switch colors
                putpixel(bmp_screen, nx, ny, gp);
                putpixel(bmp_screen, x1, y1, tc);
            }
        }
    }

}

// Skirmish map initialization
void INIT_PREVIEWS() {
    for (int i = 0; i < MAX_SKIRMISHMAPS; i++) {
        s_PreviewMap &previewMap = PreviewMap[i];
        previewMap.terrain = NULL;

        // clear out name
        memset(previewMap.name, 0, sizeof(previewMap.name));

        // clear out map data
        if (i == 0) { // first entry/random map
            int maxCells = 128*128;
            previewMap.mapdata = std::vector<int>(maxCells, -1);
        } else {
//            int maxCells = 4096; // 64x64 map
            previewMap.mapdata = std::vector<int>();
        }

        previewMap.iPlayers = 0;

        previewMap.width = 0;
        previewMap.height = 0;

        previewMap.iStartCell[0] = -1;
        previewMap.iStartCell[1] = -1;
        previewMap.iStartCell[2] = -1;
        previewMap.iStartCell[3] = -1;
        previewMap.iStartCell[4] = -1;
    }

    sprintf(PreviewMap[0].name, "RANDOM MAP");
    //PreviewMap[0].terrain = (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat;
    PreviewMap[0].terrain = create_bitmap(128, 128);
}

int getAmountReservedVoicesAndInstallSound() {
	int voices = 256;
	while (1) {
		if (voices < 4) {
			// failed!
			return -1;
		}
		reserve_voices(voices, 0);
		char msg[VOLUME_MAX];
		if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == 0)
		{
			sprintf(msg, "Success reserving %d voices.", voices);
			cLogger::getInstance()->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
			break;
		}
		else {
			sprintf(msg, "Failed reserving %d voices. Will try %d.", voices, (voices / 2));
			cLogger::getInstance()->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_FAILED);
			voices /= 2;
		}
	}

	return voices;
}

const char* toStringBuildTypeSpecificType(const eBuildType &buildType, const int &specificTypeId) {
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
        default:
            assert(false && "Unknown buildType?");
            break;
    }
    return "";
}

char scanCodeToAscii(int scanCode) {
    return (char)scancode_to_ascii(scanCode);
}