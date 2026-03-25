#include "utils/cIniGameRessouces.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "data/gfxaudio.h"
#include "d2tmc.h"
#include "game/cGame.h"
#include "sidebar/cSideBar.h"
#include "utils/Graphics.hpp"
#include "utils/common.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/sTerrainInfo.h"


/********************************
 House Rules
 ********************************/

/*****************************
 Unit Rules
 *****************************/
void IniGameRessources::install_units()
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
        s_UnitInfo &unitInfo = game.unitInfos[i];
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
    game.unitInfos[CARRYALL].bmp = gfxdata->getSurface(UNIT_CARRYALL);      // pointer to the original 8bit bitmap
    game.unitInfos[CARRYALL].shadow = gfxdata->getTexture(UNIT_CARRYALL_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[CARRYALL].bmp_width = 24 * 2;
    game.unitInfos[CARRYALL].bmp_height = 24 * 2;
    game.unitInfos[CARRYALL].bmp_startpixel = 0;
    game.unitInfos[CARRYALL].bmp_frames = 2; // we have at max 1 extra frame
    game.unitInfos[CARRYALL].icon = ICON_UNIT_CARRYALL;
    game.unitInfos[CARRYALL].airborn = true;   // is airborn
    game.unitInfos[CARRYALL].free_roam = true; // may freely roam the air
    game.unitInfos[CARRYALL].listType = eListType::LIST_UNITS;
    game.unitInfos[CARRYALL].subListId = SUBLIST_HIGHTECH;
    strcpy(game.unitInfos[CARRYALL].name, "Carry-All");

    // Unit        : Ornithopter
    // Description : Pesty little aircraft shooting bastard
    game.unitInfos[ORNITHOPTER].bmp = gfxdata->getSurface(UNIT_ORNITHOPTER);      // pointer to the original 8bit bitmap
    game.unitInfos[ORNITHOPTER].shadow = gfxdata->getTexture(UNIT_ORNITHOPTER_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[ORNITHOPTER].bmp_width = 24 * 2;
    game.unitInfos[ORNITHOPTER].bmp_height = 24 * 2;
    game.unitInfos[ORNITHOPTER].bmp_startpixel = 0;
    game.unitInfos[ORNITHOPTER].bmp_frames = 4; // we have at max 3 extra frames
    game.unitInfos[ORNITHOPTER].icon = ICON_UNIT_ORNITHOPTER;
    game.unitInfos[ORNITHOPTER].bulletType = ROCKET_SMALL_ORNI;
    game.unitInfos[ORNITHOPTER].fireTwice = true;
    game.unitInfos[ORNITHOPTER].airborn = true;   // is airborn
    game.unitInfos[ORNITHOPTER].squish = false;   // can't squish infantry
    game.unitInfos[ORNITHOPTER].free_roam = true; // may freely roam the air
    game.unitInfos[ORNITHOPTER].listType = eListType::LIST_UNITS;
    game.unitInfos[ORNITHOPTER].subListId = SUBLIST_HIGHTECH;
//    game.unitInfos[ORNITHOPTER].canAttackAirUnits = true; // orni's can attack other air units
    strcpy(game.unitInfos[ORNITHOPTER].name, "Ornithopter");

    // Unit        : Devastator
    // Description : Devastator
    game.unitInfos[DEVASTATOR].bmp = gfxdata->getSurface(UNIT_DEVASTATOR);      // pointer to the original 8bit bitmap
    game.unitInfos[DEVASTATOR].shadow = gfxdata->getTexture(UNIT_DEVASTATOR_SHADOW);      // pointer to the original bitmap shadow
    game.unitInfos[DEVASTATOR].bmp_width = 19 * 2;
    game.unitInfos[DEVASTATOR].bmp_startpixel = 0;
    game.unitInfos[DEVASTATOR].bmp_height = 23 * 2;
    game.unitInfos[DEVASTATOR].bmp_frames = 1;
    game.unitInfos[DEVASTATOR].bulletType = BULLET_DEVASTATOR;
    game.unitInfos[DEVASTATOR].fireTwice = true;
    game.unitInfos[DEVASTATOR].icon = ICON_UNIT_DEVASTATOR;
    game.unitInfos[DEVASTATOR].listType = eListType::LIST_UNITS;
    game.unitInfos[DEVASTATOR].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[DEVASTATOR].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[DEVASTATOR].smokeHpFactor = 0.5f;
    game.unitInfos[DEVASTATOR].canGuard = true;
    game.unitInfos[DEVASTATOR].canAttackUnits = true;
    strcpy(game.unitInfos[DEVASTATOR].name, "Devastator");

    // Unit        : Harvester
    // Description : Harvester
    game.unitInfos[HARVESTER].bmp = gfxdata->getSurface(UNIT_HARVESTER);      // pointer to the original 8bit bitmap
    game.unitInfos[HARVESTER].shadow = gfxdata->getTexture(UNIT_HARVESTER_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[HARVESTER].bmp_width = 40 * 2;
    game.unitInfos[HARVESTER].bmp_startpixel = 24;
    game.unitInfos[HARVESTER].bmp_height = 26 * 2;
    game.unitInfos[HARVESTER].bmp_frames = 4;
    game.unitInfos[HARVESTER].icon = ICON_UNIT_HARVESTER;
    game.unitInfos[HARVESTER].credit_capacity = 700;
    game.unitInfos[HARVESTER].harvesting_amount = 5;
    game.unitInfos[HARVESTER].listType = eListType::LIST_UNITS;
    game.unitInfos[HARVESTER].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[HARVESTER].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[HARVESTER].smokeHpFactor = 0.5f;
    strcpy(game.unitInfos[HARVESTER].name, "Harvester");

    // Unit        : Combattank
    // Description : Combattank
    game.unitInfos[TANK].bmp = gfxdata->getSurface(UNIT_TANKBASE);      // pointer to the original 8bit bitmap
    game.unitInfos[TANK].shadow = gfxdata->getTexture(UNIT_TANKBASE_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[TANK].top = gfxdata->getSurface(UNIT_TANKTOP);      // pointer to the original 8bit bitmap
    game.unitInfos[TANK].bmp_width = 16 * 2;
    game.unitInfos[TANK].bmp_startpixel = 0;
    game.unitInfos[TANK].bmp_height = 16 * 2;
    game.unitInfos[TANK].bmp_frames = 0;
    game.unitInfos[TANK].bulletType = BULLET_TANK;
    game.unitInfos[TANK].icon = ICON_UNIT_TANK;
    game.unitInfos[TANK].listType = eListType::LIST_UNITS;
    game.unitInfos[TANK].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[TANK].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[TANK].smokeHpFactor = 0.5f;
    game.unitInfos[TANK].canGuard = true;
    game.unitInfos[TANK].canAttackUnits = true;
    strcpy(game.unitInfos[TANK].name, "Tank");


    // Unit        : Siege Tank
    // Description : Siege tank
    game.unitInfos[SIEGETANK].bmp = gfxdata->getSurface(UNIT_SIEGEBASE);      // pointer to the original 8bit bitmap
    game.unitInfos[SIEGETANK].shadow = gfxdata->getTexture(UNIT_SIEGEBASE_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[SIEGETANK].top = gfxdata->getSurface(UNIT_SIEGETOP);      // pointer to the original 8bit bitmap
    game.unitInfos[SIEGETANK].bmp_width = 18 * 2;
    game.unitInfos[SIEGETANK].bmp_startpixel = 1;
    game.unitInfos[SIEGETANK].bmp_height = 18 * 2;
    game.unitInfos[SIEGETANK].bmp_frames = 0;
    game.unitInfos[SIEGETANK].bulletType = BULLET_SIEGE;
    game.unitInfos[SIEGETANK].fireTwice = true;
    game.unitInfos[SIEGETANK].icon = ICON_UNIT_SIEGETANK;
    game.unitInfos[SIEGETANK].listType = eListType::LIST_UNITS;
    game.unitInfos[SIEGETANK].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[SIEGETANK].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[SIEGETANK].smokeHpFactor = 0.5f;
    game.unitInfos[SIEGETANK].canGuard = true;
    game.unitInfos[SIEGETANK].canAttackUnits = true;
    strcpy(game.unitInfos[SIEGETANK].name, "Siege Tank");

    // Unit        : MCV
    // Description : Movable Construction Vehicle
    game.unitInfos[MCV].bmp = gfxdata->getSurface(UNIT_MCV);      // pointer to the original 8bit bitmap
    game.unitInfos[MCV].shadow = gfxdata->getTexture(UNIT_MCV_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[MCV].bmp_width = 24 * 2;
    game.unitInfos[MCV].bmp_startpixel = 0;
    game.unitInfos[MCV].bmp_height = 25 * 2;
    game.unitInfos[MCV].bmp_frames = 1;
    game.unitInfos[MCV].icon = ICON_UNIT_MCV;
    game.unitInfos[MCV].listType = eListType::LIST_UNITS;
    game.unitInfos[MCV].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[MCV].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[MCV].smokeHpFactor = 0.5f;
    strcpy(game.unitInfos[MCV].name, "MCV");


    // Unit        : Deviator
    // Description : Deviator
    game.unitInfos[DEVIATOR].bmp = gfxdata->getSurface(UNIT_DEVIATOR);      // pointer to the original 8bit bitmap
    game.unitInfos[DEVIATOR].bmp_width = 16 * 2;
    game.unitInfos[DEVIATOR].bmp_height = 16 * 2;
    game.unitInfos[DEVIATOR].bmp_startpixel = 0;
    game.unitInfos[DEVIATOR].bmp_frames = 1;
    game.unitInfos[DEVIATOR].icon = ICON_UNIT_DEVIATOR;
    game.unitInfos[DEVIATOR].bulletType = BULLET_GAS; // our gassy rocket
    game.unitInfos[DEVIATOR].listType = eListType::LIST_UNITS;
    game.unitInfos[DEVIATOR].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[DEVIATOR].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[DEVIATOR].smokeHpFactor = 0.5f;
    game.unitInfos[DEVIATOR].canGuard = true;
    game.unitInfos[DEVIATOR].canAttackUnits = true;
    strcpy(game.unitInfos[DEVIATOR].name, "Deviator");

    // Unit        : Launcher
    // Description : Rocket Launcher
    game.unitInfos[LAUNCHER].bmp = gfxdata->getSurface(UNIT_LAUNCHER);      // pointer to the original 8bit bitmap
    game.unitInfos[LAUNCHER].shadow = gfxdata->getTexture(UNIT_LAUNCHER_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[LAUNCHER].bmp_width = 16 * 2;
    game.unitInfos[LAUNCHER].bmp_height = 16 * 2;
    game.unitInfos[LAUNCHER].bmp_startpixel = 0;
    game.unitInfos[LAUNCHER].bmp_frames = 1;
    game.unitInfos[LAUNCHER].icon = ICON_UNIT_LAUNCHER;
    game.unitInfos[LAUNCHER].fireTwice = true;
    //units[LAUNCHER].bullets = ROCKET_NORMAL; // our gassy rocket
    game.unitInfos[LAUNCHER].bulletType = ROCKET_NORMAL; // our gassy rocket
    game.unitInfos[LAUNCHER].listType = eListType::LIST_UNITS;
    game.unitInfos[LAUNCHER].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[LAUNCHER].canAttackAirUnits = true;
    game.unitInfos[LAUNCHER].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[LAUNCHER].smokeHpFactor = 0.5f;
    game.unitInfos[LAUNCHER].canGuard = true;
    game.unitInfos[LAUNCHER].canAttackUnits = true;
    strcpy(game.unitInfos[LAUNCHER].name, "Launcher");

    // Unit        : Quad
    // Description : Quad, 4 wheeled (double gunned)
    game.unitInfos[QUAD].bmp = gfxdata->getSurface(UNIT_QUAD);      // pointer to the original 8bit bitmap
    game.unitInfos[QUAD].shadow = gfxdata->getTexture(UNIT_QUAD_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[QUAD].bmp_width = 16 * 2;
    game.unitInfos[QUAD].bmp_height = 16 * 2;
    game.unitInfos[QUAD].bmp_startpixel = 0;
    game.unitInfos[QUAD].bmp_frames = 1;
    game.unitInfos[QUAD].icon = ICON_UNIT_QUAD;
    game.unitInfos[QUAD].fireTwice = true;
    game.unitInfos[QUAD].bulletType = BULLET_QUAD;
    game.unitInfos[QUAD].squish = false;
    game.unitInfos[QUAD].listType = eListType::LIST_UNITS;
    game.unitInfos[QUAD].subListId = SUBLIST_LIGHTFCTRY;
    game.unitInfos[QUAD].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[QUAD].smokeHpFactor = 0.5f;
    game.unitInfos[QUAD].canGuard = true;
    game.unitInfos[QUAD].canAttackUnits = true;
    strcpy(game.unitInfos[QUAD].name, "Quad");


    // Unit        : Trike (normal trike)
    // Description : Trike, 3 wheeled (single gunned)
    game.unitInfos[TRIKE].bmp = gfxdata->getSurface(UNIT_TRIKE);      // pointer to the original 8bit bitmap
    game.unitInfos[TRIKE].shadow = gfxdata->getTexture(UNIT_TRIKE_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[TRIKE].bmp_width = 28;
    game.unitInfos[TRIKE].bmp_height = 26;
    game.unitInfos[TRIKE].bmp_startpixel = 0;
    game.unitInfos[TRIKE].bmp_frames = 1;
    game.unitInfos[TRIKE].icon = ICON_UNIT_TRIKE;
    game.unitInfos[TRIKE].bulletType = BULLET_TRIKE;
    game.unitInfos[TRIKE].squish = false;
    game.unitInfos[TRIKE].listType = eListType::LIST_UNITS;
    game.unitInfos[TRIKE].subListId = SUBLIST_LIGHTFCTRY;
    game.unitInfos[TRIKE].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[TRIKE].smokeHpFactor = 0.5f;
    game.unitInfos[TRIKE].canGuard = true;
    game.unitInfos[TRIKE].canAttackUnits = true;
    strcpy(game.unitInfos[TRIKE].name, "Trike");

    // Unit        : Raider Trike (Ordos trike)
    // Description : Raider Trike, 3 wheeled (single gunned), weaker, but faster
    game.unitInfos[RAIDER].bmp = gfxdata->getSurface(UNIT_TRIKE);      // pointer to the original 8bit bitmap
    game.unitInfos[RAIDER].shadow = gfxdata->getTexture(UNIT_TRIKE_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[RAIDER].bmp_width = 14 * 2;
    game.unitInfos[RAIDER].bmp_height = 14 * 2;
    game.unitInfos[RAIDER].bmp_startpixel = 0;
    game.unitInfos[RAIDER].bmp_frames = 1;
    strcpy(game.unitInfos[RAIDER].name, "Raider Trike");
    game.unitInfos[RAIDER].icon = ICON_UNIT_RAIDER;
    game.unitInfos[RAIDER].bulletType = BULLET_TRIKE;
    game.unitInfos[RAIDER].squish = false;
    game.unitInfos[RAIDER].listType = eListType::LIST_UNITS;
    game.unitInfos[RAIDER].subListId = SUBLIST_LIGHTFCTRY;
    game.unitInfos[RAIDER].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[RAIDER].smokeHpFactor = 0.5f;
    game.unitInfos[RAIDER].canGuard = true;
    game.unitInfos[RAIDER].canAttackUnits = true;

    // Unit        : Frigate
    // Description : Frigate
    game.unitInfos[FRIGATE].bmp = gfxdata->getSurface(UNIT_FRIGATE);      // pointer to the original 8bit bitmap
    game.unitInfos[FRIGATE].shadow = gfxdata->getTexture(UNIT_FRIGATE_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[FRIGATE].bmp_width = 32 * 2;
    game.unitInfos[FRIGATE].bmp_height = 32 * 2;
    game.unitInfos[FRIGATE].bmp_startpixel = 0;
    game.unitInfos[FRIGATE].bmp_frames = 2; // we have at max 1 extra frame
    game.unitInfos[FRIGATE].speed = 2;
    game.unitInfos[FRIGATE].turnspeed = 4;
    game.unitInfos[FRIGATE].airborn = true;
    game.unitInfos[FRIGATE].squish = false;
    game.unitInfos[FRIGATE].free_roam = true; // Frigate does not roam, yet needed?
    game.unitInfos[FRIGATE].hp = 9999;
    // frigate has no list
    strcpy(game.unitInfos[FRIGATE].name, "Frigate");

    /*
    units[FRIGATE].speed     = 0;
    units[FRIGATE].turnspeed = 1;

    units[FRIGATE].sight      = 3;
    units[FRIGATE].hp         = 9999;
    */

    //units[FRIGATE].icon = ICON_FRIGATE;

    // Unit        : Sonic Tank
    // Description : Sonic tank (using sound waves to destroy)
    game.unitInfos[SONICTANK].bmp = gfxdata->getSurface(UNIT_SONICTANK);      // pointer to the original 8bit bitmap
    game.unitInfos[SONICTANK].shadow = gfxdata->getTexture(UNIT_SONICTANK_SHADOW);      // pointer to the original 8bit bitmap
    game.unitInfos[SONICTANK].bmp_width = 16 * 2;
    game.unitInfos[SONICTANK].bmp_height = 16 * 2;
    game.unitInfos[SONICTANK].bmp_startpixel = 0;
    game.unitInfos[SONICTANK].bmp_frames = 1; // no extra frames
    game.unitInfos[SONICTANK].bulletType = BULLET_SHIMMER;
    game.unitInfos[SONICTANK].icon = ICON_UNIT_SONICTANK;
    game.unitInfos[SONICTANK].listType = eListType::LIST_UNITS;
    game.unitInfos[SONICTANK].subListId = SUBLIST_HEAVYFCTRY;
    game.unitInfos[SONICTANK].renderSmokeOnUnitWhenThresholdMet = true;
    game.unitInfos[SONICTANK].smokeHpFactor = 0.5f;
    game.unitInfos[SONICTANK].canGuard = true;
    game.unitInfos[SONICTANK].canAttackUnits = true;

    strcpy(game.unitInfos[SONICTANK].name, "Sonic Tank");


    // Unit        : Single Soldier
    // Description : 1 soldier
    game.unitInfos[SOLDIER].bmp = gfxdata->getSurface(UNIT_SOLDIER);      // pointer to the original 8bit bitmap
    game.unitInfos[SOLDIER].bmp_width = 16 * 2;
    game.unitInfos[SOLDIER].bmp_height = 16 * 2;
    game.unitInfos[SOLDIER].bmp_startpixel = 0;
    game.unitInfos[SOLDIER].bmp_frames = 3; // 2 extra frames
    game.unitInfos[SOLDIER].infantry = true;
    game.unitInfos[SOLDIER].bulletType = BULLET_SMALL;
    game.unitInfos[SOLDIER].icon = ICON_UNIT_SOLDIER;
    game.unitInfos[SOLDIER].squish = false;
    game.unitInfos[SOLDIER].canBeSquished = true;
    game.unitInfos[SOLDIER].listType = eListType::LIST_FOOT_UNITS;
    game.unitInfos[SOLDIER].subListId = SUBLIST_INFANTRY;
    game.unitInfos[SOLDIER].canEnterAndDamageStructure = true;
    game.unitInfos[SOLDIER].attackIsEnterStructure = false;
    game.unitInfos[SOLDIER].damageOnEnterStructure = 10.0f;
    game.unitInfos[SOLDIER].canGuard = true;
    game.unitInfos[SOLDIER].canAttackUnits = true;
    strcpy(game.unitInfos[SOLDIER].name, "Soldier");


    // Unit        : Infantry
    // Description : 3 soldiers
    game.unitInfos[INFANTRY].bmp = gfxdata->getSurface(UNIT_SOLDIERS);      // pointer to the original 8bit bitmap
    game.unitInfos[INFANTRY].bmp_width = 16 * 2;
    game.unitInfos[INFANTRY].bmp_height = 16 * 2;
    game.unitInfos[INFANTRY].bmp_startpixel = 0;
    game.unitInfos[INFANTRY].bmp_frames = 3; // 2 extra frames
    game.unitInfos[INFANTRY].speed = 8;
    game.unitInfos[INFANTRY].infantry = true;
    game.unitInfos[INFANTRY].bulletType = BULLET_SMALL;
    game.unitInfos[INFANTRY].fireTwice = true;
    game.unitInfos[INFANTRY].icon = ICON_UNIT_INFANTRY;
    game.unitInfos[INFANTRY].squish = false;
    game.unitInfos[INFANTRY].canBeSquished = true;
    game.unitInfos[INFANTRY].listType = eListType::LIST_FOOT_UNITS;
    game.unitInfos[INFANTRY].subListId = SUBLIST_INFANTRY;
    game.unitInfos[INFANTRY].canEnterAndDamageStructure = true;
    game.unitInfos[INFANTRY].attackIsEnterStructure = false;
    game.unitInfos[INFANTRY].damageOnEnterStructure = 25.0f;
    game.unitInfos[INFANTRY].canGuard = true;
    game.unitInfos[INFANTRY].canAttackUnits = true;
    strcpy(game.unitInfos[INFANTRY].name, "Light Infantry");

    // Unit        : Single Trooper
    // Description : 1 trooper
    game.unitInfos[TROOPER].bmp = gfxdata->getSurface(UNIT_TROOPER);      // pointer to the original 8bit bitmap
    game.unitInfos[TROOPER].bmp_width = 16 * 2;
    game.unitInfos[TROOPER].bmp_height = 16 * 2;
    game.unitInfos[TROOPER].bmp_startpixel = 0;
    game.unitInfos[TROOPER].bmp_frames = 3; // 2 extra frames
    strcpy(game.unitInfos[TROOPER].name, "Trooper");
    game.unitInfos[TROOPER].infantry = true;
    game.unitInfos[TROOPER].bulletType = ROCKET_SMALL;
    game.unitInfos[TROOPER].icon = ICON_UNIT_TROOPER;
    game.unitInfos[TROOPER].listType = eListType::LIST_FOOT_UNITS;
    game.unitInfos[TROOPER].subListId = SUBLIST_TROOPERS;
    game.unitInfos[TROOPER].squish = false;
    game.unitInfos[TROOPER].canBeSquished = true;
    game.unitInfos[TROOPER].canAttackAirUnits = true;
    game.unitInfos[TROOPER].canEnterAndDamageStructure = true;
    game.unitInfos[TROOPER].attackIsEnterStructure = false;
    game.unitInfos[TROOPER].damageOnEnterStructure = 12.0f;
    game.unitInfos[TROOPER].bulletTypeSecondary = BULLET_SMALL;
    game.unitInfos[TROOPER].fireSecondaryWithinRange = 2;
    game.unitInfos[TROOPER].canGuard = true;
    game.unitInfos[TROOPER].canAttackUnits = true;

    // Unit        : Group Trooper
    // Description : 3 troopers
    game.unitInfos[TROOPERS].bmp = gfxdata->getSurface(UNIT_TROOPERS);      // pointer to the original 8bit bitmap
    game.unitInfos[TROOPERS].bmp_width = 16 * 2;
    game.unitInfos[TROOPERS].bmp_height = 16 * 2;
    game.unitInfos[TROOPERS].bmp_startpixel = 0;
    game.unitInfos[TROOPERS].bmp_frames = 3; // 2 extra frames
    strcpy(game.unitInfos[TROOPERS].name, "Troopers");
    game.unitInfos[TROOPERS].icon = ICON_UNIT_TROOPERS;
    game.unitInfos[TROOPERS].bulletType = ROCKET_SMALL;
    game.unitInfos[TROOPERS].fireTwice = true;
    game.unitInfos[TROOPERS].infantry = true;
    game.unitInfos[TROOPERS].listType = eListType::LIST_FOOT_UNITS;
    game.unitInfos[TROOPERS].subListId = SUBLIST_TROOPERS;
    game.unitInfos[TROOPERS].squish = false;
    game.unitInfos[TROOPERS].canBeSquished = true;
    game.unitInfos[TROOPERS].canAttackAirUnits = true;
    game.unitInfos[TROOPERS].canEnterAndDamageStructure = true;
    game.unitInfos[TROOPERS].attackIsEnterStructure = false;
    game.unitInfos[TROOPERS].damageOnEnterStructure = 35.0f;
    game.unitInfos[TROOPERS].bulletTypeSecondary = BULLET_SMALL;
    game.unitInfos[TROOPERS].fireSecondaryWithinRange = 2;
    game.unitInfos[TROOPERS].canGuard = true;
    game.unitInfos[TROOPERS].canAttackUnits = true;

    // Unit        : Fremen
    // Description : A single fremen
    game.unitInfos[UNIT_FREMEN_ONE].bmp = gfxdata->getSurface(UNIT_SINGLEFREMEN);      // pointer to the original 8bit bitmap
    game.unitInfos[UNIT_FREMEN_ONE].bmp_width = 16 * 2;
    game.unitInfos[UNIT_FREMEN_ONE].bmp_height = 16 * 2;
    game.unitInfos[UNIT_FREMEN_ONE].bmp_startpixel = 0;
    game.unitInfos[UNIT_FREMEN_ONE].bmp_frames = 3; // 2 extra frames
    strcpy(game.unitInfos[UNIT_FREMEN_ONE].name, "Fremen (1)");
    game.unitInfos[UNIT_FREMEN_ONE].icon = ICON_SPECIAL_FREMEN;
    game.unitInfos[UNIT_FREMEN_ONE].bulletType = ROCKET_SMALL_FREMEN;
    game.unitInfos[UNIT_FREMEN_ONE].fireTwice = false;
    game.unitInfos[UNIT_FREMEN_ONE].infantry = true;
    game.unitInfos[UNIT_FREMEN_ONE].squish = false;
    game.unitInfos[UNIT_FREMEN_ONE].canBeSquished = true;
    game.unitInfos[UNIT_FREMEN_ONE].canAttackAirUnits = true;
    game.unitInfos[UNIT_FREMEN_ONE].canGuard = true;
    game.unitInfos[UNIT_FREMEN_ONE].canAttackUnits = true;

//  units[UNIT_FREMEN_ONE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_ONE].subListId=0;

    // Unit        : Fremen
    // Description : A group of Fremen
    game.unitInfos[UNIT_FREMEN_THREE].bmp = gfxdata->getSurface(UNIT_TRIPLEFREMEN);      // pointer to the original 8bit bitmap
    game.unitInfos[UNIT_FREMEN_THREE].bmp_width = 16 * 2;
    game.unitInfos[UNIT_FREMEN_THREE].bmp_height = 16 * 2;
    game.unitInfos[UNIT_FREMEN_THREE].bmp_startpixel = 0;
    game.unitInfos[UNIT_FREMEN_THREE].bmp_frames = 3; // 2 extra frames
    strcpy(game.unitInfos[UNIT_FREMEN_THREE].name, "Fremen (3)");
    game.unitInfos[UNIT_FREMEN_THREE].icon = ICON_SPECIAL_FREMEN;
    game.unitInfos[UNIT_FREMEN_THREE].bulletType = ROCKET_SMALL_FREMEN;
    game.unitInfos[UNIT_FREMEN_THREE].fireTwice = true;
    game.unitInfos[UNIT_FREMEN_THREE].infantry = true;
    game.unitInfos[UNIT_FREMEN_THREE].squish = false;
    game.unitInfos[UNIT_FREMEN_THREE].canBeSquished = true;
    game.unitInfos[UNIT_FREMEN_THREE].canAttackAirUnits = true;
    game.unitInfos[UNIT_FREMEN_THREE].canGuard = true;
    game.unitInfos[UNIT_FREMEN_THREE].canAttackUnits = true;
//  units[UNIT_FREMEN_THREE].listType=LIST_PALACE;
//  units[UNIT_FREMEN_THREE].subListId=0;

    // Unit        : Saboteur
    // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
    game.unitInfos[SABOTEUR].bmp = gfxdata->getSurface(UNIT_SABOTEUR);
    game.unitInfos[SABOTEUR].buildTime = 1000;
    game.unitInfos[SABOTEUR].bmp_width = 16 * 2;
    game.unitInfos[SABOTEUR].bmp_height = 16 * 2;
    game.unitInfos[SABOTEUR].bmp_startpixel = 0;
    game.unitInfos[SABOTEUR].bmp_frames = 3; // 2 extra frames
    game.unitInfos[SABOTEUR].speed = 0; // very fast
    game.unitInfos[SABOTEUR].hp = 60;   // quite some health
    game.unitInfos[SABOTEUR].cost = 0;
    game.unitInfos[SABOTEUR].sight = 4; // immense sight! (sorta scouting guys)
    game.unitInfos[SABOTEUR].range = 2;
    game.unitInfos[SABOTEUR].attack_frequency = 0;
    game.unitInfos[SABOTEUR].turnspeed = 0; // very fast
    strcpy(game.unitInfos[SABOTEUR].name, "Saboteur");
    game.unitInfos[SABOTEUR].icon = ICON_SPECIAL_SABOTEUR;
    game.unitInfos[SABOTEUR].squish = false;
    game.unitInfos[SABOTEUR].canBeSquished = true;
    game.unitInfos[SABOTEUR].infantry = true; // infantry unit, so it can be squished
    game.unitInfos[SABOTEUR].listType = eListType::LIST_PALACE;
    game.unitInfos[SABOTEUR].subListId = 0;
    game.unitInfos[SABOTEUR].canEnterAndDamageStructure = true;
    game.unitInfos[SABOTEUR].attackIsEnterStructure = true;
    game.unitInfos[SABOTEUR].damageOnEnterStructure = 9999.99f; // a lot of damage (instant destroy)

    // Unit        : Sandworm
    game.unitInfos[SANDWORM].speed = 3; // very fast
    game.unitInfos[SANDWORM].bmp = gfxdata->getSurface(UNIT_SANDWORM);
    game.unitInfos[SANDWORM].hp = 9999; // set in game.ini to a more sane amount
    game.unitInfos[SANDWORM].dieWhenLowerThanHP = 1000;
    game.unitInfos[SANDWORM].appetite = 10;
    game.unitInfos[SANDWORM].bmp_width = 48;
    game.unitInfos[SANDWORM].bmp_height = 48;
    game.unitInfos[SANDWORM].turnspeed = 0; // very fast
    game.unitInfos[SANDWORM].sight = 16;
    strcpy(game.unitInfos[SANDWORM].name, "Sandworm");
    game.unitInfos[SANDWORM].icon = ICON_UNIT_SANDWORM;
    game.unitInfos[SANDWORM].squish = false;
    game.unitInfos[SANDWORM].canGuard = true;
    game.unitInfos[SANDWORM].canAttackUnits = true;


    // Unit        : <name>
    // Description : <description>

}

void IniGameRessources::install_particles()
{
    for (unsigned int i = 0; i < game.m_particleInfos.size(); i++) {
        s_ParticleInfo &particleInfo = game.m_particleInfos[i];
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
    game.m_particleInfos[D2TM_PARTICLE_MOVE].bmpIndex = MOVE_INDICATOR;
    game.m_particleInfos[D2TM_PARTICLE_MOVE].startAlpha = 128;

    // attack
    game.m_particleInfos[D2TM_PARTICLE_ATTACK].bmpIndex = ATTACK_INDICATOR;
    game.m_particleInfos[D2TM_PARTICLE_ATTACK].startAlpha = 128;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].bmpIndex = EXPLOSION_TRIKE;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TRIKE].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_SMOKE].bmpIndex = OBJECT_SMOKE;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE].startAlpha = 0;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE].frameWidth = 32;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].bmpIndex = OBJECT_SMOKE;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].startAlpha = 0;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameWidth = 32;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE_WITH_SHADOW].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].bmpIndex = OBJECT_SMOKE_SHADOW;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].startAlpha = 0;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].frameWidth = 36;
    game.m_particleInfos[D2TM_PARTICLE_SMOKE_SHADOW].frameHeight = 38;

    game.m_particleInfos[D2TM_PARTICLE_TRACK_DIA].bmpIndex = TRACK_DIA;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_DIA].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_DIA].startAlpha = 128;

    game.m_particleInfos[D2TM_PARTICLE_TRACK_HOR].bmpIndex = TRACK_HOR;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_HOR].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_HOR].startAlpha = 128;

    game.m_particleInfos[D2TM_PARTICLE_TRACK_VER].bmpIndex = TRACK_VER;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_VER].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_VER].startAlpha = 128;

    game.m_particleInfos[D2TM_PARTICLE_TRACK_DIA2].bmpIndex = TRACK_DIA2;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_DIA2].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_TRACK_DIA2].startAlpha = 128;

    game.m_particleInfos[D2TM_PARTICLE_BULLET_PUF].bmpIndex = BULLET_PUF;
    game.m_particleInfos[D2TM_PARTICLE_BULLET_PUF].frameWidth = 18;
    game.m_particleInfos[D2TM_PARTICLE_BULLET_PUF].frameHeight = 18;


    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_FIRE].bmpIndex = EXPLOSION_FIRE;

    game.m_particleInfos[D2TM_PARTICLE_WORMEAT].bmpIndex = OBJECT_WORMEAT;
    game.m_particleInfos[D2TM_PARTICLE_WORMEAT].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_WORMEAT].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_WORMEAT].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].bmpIndex = EXPLOSION_TANK_ONE;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_ONE].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].bmpIndex = EXPLOSION_TANK_TWO;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_TANK_TWO].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].bmpIndex = EXPLOSION_STRUCTURE01;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE01].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].bmpIndex = EXPLOSION_STRUCTURE02;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_STRUCTURE02].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].bmpIndex = EXPLOSION_GAS;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_GAS].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_WORMTRAIL].bmpIndex = OBJECT_WORMTRAIL;
    game.m_particleInfos[D2TM_PARTICLE_WORMTRAIL].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_WORMTRAIL].startAlpha = 96;
    game.m_particleInfos[D2TM_PARTICLE_WORMTRAIL].frameWidth = 48;
    game.m_particleInfos[D2TM_PARTICLE_WORMTRAIL].frameHeight = 48;

    game.m_particleInfos[D2TM_PARTICLE_DEADINF01].bmpIndex = OBJECT_DEADINF01;
    game.m_particleInfos[D2TM_PARTICLE_DEADINF01].layer = RenderLayerParticle::BOTTOM;

    game.m_particleInfos[D2TM_PARTICLE_DEADINF02].bmpIndex = OBJECT_DEADINF02;
    game.m_particleInfos[D2TM_PARTICLE_DEADINF02].layer = RenderLayerParticle::BOTTOM;

    game.m_particleInfos[D2TM_PARTICLE_TANKSHOOT].bmpIndex = OBJECT_TANKSHOOT;
    game.m_particleInfos[D2TM_PARTICLE_TANKSHOOT].startAlpha = 128;
    game.m_particleInfos[D2TM_PARTICLE_TANKSHOOT].frameWidth = 64;
    game.m_particleInfos[D2TM_PARTICLE_TANKSHOOT].frameHeight = 64;

    game.m_particleInfos[D2TM_PARTICLE_SIEGESHOOT].bmpIndex = OBJECT_SIEGESHOOT;
    game.m_particleInfos[D2TM_PARTICLE_SIEGESHOOT].startAlpha = 128;
    game.m_particleInfos[D2TM_PARTICLE_SIEGESHOOT].frameWidth = 64;
    game.m_particleInfos[D2TM_PARTICLE_SIEGESHOOT].frameHeight = 64;

    game.m_particleInfos[D2TM_PARTICLE_SQUISH01].bmpIndex = EXPLOSION_SQUISH01;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH01].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH01].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH01].frameWidth = 32;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH01].frameHeight = 32;

    game.m_particleInfos[D2TM_PARTICLE_SQUISH02].bmpIndex = EXPLOSION_SQUISH02;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH02].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH02].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH02].frameWidth = 32;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH02].frameHeight = 32;

    game.m_particleInfos[D2TM_PARTICLE_SQUISH03].bmpIndex = EXPLOSION_SQUISH03;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH03].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH03].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH03].frameWidth = 32;
    game.m_particleInfos[D2TM_PARTICLE_SQUISH03].frameHeight = 32;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].bmpIndex = EXPLOSION_ORNI;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].startAlpha = 255;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].frameWidth = 32;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_ORNI].frameHeight = 32;

    game.m_particleInfos[D2TM_PARTICLE_SIEGEDIE].bmpIndex = OBJECT_SIEGEDIE;

    game.m_particleInfos[D2TM_PARTICLE_CARRYPUFF].bmpIndex = OBJECT_CARRYPUFF;
    game.m_particleInfos[D2TM_PARTICLE_CARRYPUFF].layer = RenderLayerParticle::BOTTOM;
    game.m_particleInfos[D2TM_PARTICLE_CARRYPUFF].frameWidth = 96;
    game.m_particleInfos[D2TM_PARTICLE_CARRYPUFF].frameHeight = 96;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_ROCKET].bmpIndex = EXPLOSION_ROCKET;
    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL].bmpIndex = EXPLOSION_ROCKET_SMALL;

    game.m_particleInfos[D2TM_PARTICLE_EXPLOSION_BULLET].bmpIndex = EXPLOSION_BULLET;

    // the nice flare/light effects that come with explosions
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].bmpIndex = OBJECT_BOOM01;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].usesAdditiveBlending = true;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].startAlpha = 240;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].frameWidth = 512;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM01].frameHeight = 512;

    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].bmpIndex = OBJECT_BOOM02;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].usesAdditiveBlending = true;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].startAlpha = 230;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].frameWidth = 256;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM02].frameHeight = 256;

    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].bmpIndex = OBJECT_BOOM03;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].usesAdditiveBlending = true;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].startAlpha = 220;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].frameWidth = 128;
    game.m_particleInfos[D2TM_PARTICLE_OBJECT_BOOM03].frameHeight = 128;
}

void IniGameRessources::install_specials()
{

    for (int i = 0; i < MAX_SPECIALTYPES; i++) {
        game.specialInfos[i].icon = -1;
        game.specialInfos[i].providesType = eBuildType::UNIT;
        game.specialInfos[i].buildTime = 0;
        game.specialInfos[i].deployFrom = eDeployFromType::AT_RANDOM_CELL;
        game.specialInfos[i].deployAtStructure = -1;
        game.specialInfos[i].units = 0;
        game.specialInfos[i].house = eHouseBitFlag::Unknown;
        game.specialInfos[i].autoBuild = false;
        game.specialInfos[i].deployTargetType = eDeployTargetType::TARGET_NONE;
        game.specialInfos[i].deployTargetPrecision = 0;
        game.specialInfos[i].listType = eListType::LIST_NONE;
        game.specialInfos[i].subListId = -1;
        strcpy(game.specialInfos[i].description, "\0");
    }

    // Deploy Saboteur
    game.specialInfos[SPECIAL_SABOTEUR].icon = ICON_SPECIAL_SABOTEUR;
    game.specialInfos[SPECIAL_SABOTEUR].house=eHouseBitFlag::Ordos;
    game.specialInfos[SPECIAL_SABOTEUR].autoBuild=true;
    game.specialInfos[SPECIAL_SABOTEUR].providesType = eBuildType::UNIT;
    game.specialInfos[SPECIAL_SABOTEUR].providesTypeId = SABOTEUR;
    game.specialInfos[SPECIAL_SABOTEUR].deployFrom = eDeployFromType::AT_STRUCTURE;
    game.specialInfos[SPECIAL_SABOTEUR].deployAtStructure = PALACE;
    game.specialInfos[SPECIAL_SABOTEUR].units = 1;
    game.specialInfos[SPECIAL_SABOTEUR].buildTime = 2468; // ~ 6 minutes (but times 1.2 to compensate for faster Ordos building = 2468 to get real 6 minutes)
    game.specialInfos[SPECIAL_SABOTEUR].listType=eListType::LIST_PALACE;
    game.specialInfos[SPECIAL_SABOTEUR].subListId=0;
    strcpy(game.specialInfos[SPECIAL_SABOTEUR].description, "Saboteur");

    // Deploy Fremen
    game.specialInfos[SPECIAL_FREMEN].icon = ICON_SPECIAL_FREMEN;
    game.specialInfos[SPECIAL_FREMEN].house=eHouseBitFlag::Atreides;
    game.specialInfos[SPECIAL_FREMEN].autoBuild=true;
    game.specialInfos[SPECIAL_FREMEN].providesType = eBuildType::UNIT;
    game.specialInfos[SPECIAL_FREMEN].providesTypeId = UNIT_FREMEN_THREE;
    game.specialInfos[SPECIAL_FREMEN].deployFrom = eDeployFromType::AT_RANDOM_CELL;
    game.specialInfos[SPECIAL_FREMEN].deployAtStructure = PALACE; // This is not used with AT_RANDOM_CELL ...
    game.specialInfos[SPECIAL_FREMEN].units = 6; // ... but this is
    game.specialInfos[SPECIAL_FREMEN].buildTime = 1371; // ~ 4 minutes (atreides has baseline build times, ie = real time)
    game.specialInfos[SPECIAL_FREMEN].listType=eListType::LIST_PALACE;
    game.specialInfos[SPECIAL_FREMEN].subListId=0;
    strcpy(game.specialInfos[SPECIAL_FREMEN].description, "Fremen");

    // Launch Death Hand
    game.specialInfos[SPECIAL_DEATHHAND].icon = ICON_SPECIAL_MISSILE;
    game.specialInfos[SPECIAL_DEATHHAND].house = Harkonnen | Sardaukar;
    game.specialInfos[SPECIAL_DEATHHAND].autoBuild=true;
    game.specialInfos[SPECIAL_DEATHHAND].providesType = eBuildType::BULLET;
    game.specialInfos[SPECIAL_DEATHHAND].providesTypeId = ROCKET_BIG;
    game.specialInfos[SPECIAL_DEATHHAND].deployFrom = eDeployFromType::AT_STRUCTURE; // the rocket is fired FROM ...
    game.specialInfos[SPECIAL_DEATHHAND].deployAtStructure = PALACE; // ... the palace
    game.specialInfos[SPECIAL_DEATHHAND].deployTargetType = eDeployTargetType::TARGET_INACCURATE_CELL;
    game.specialInfos[SPECIAL_DEATHHAND].units = 1;
    game.specialInfos[SPECIAL_DEATHHAND].buildTime = 3428; // ~ 10 minutes with base line (Atreides difficulty)
    // (342.8 = ~ 1 minute) -> harkonnen is done * 1.2 so it becomes 12 minutes real-time which is ok)
    // considering the Dune 2 Insider guide mentions 11 to 12 minutes for Harkonnen.

    game.specialInfos[SPECIAL_DEATHHAND].deployTargetPrecision = 6;
    game.specialInfos[SPECIAL_DEATHHAND].listType=eListType::LIST_PALACE;
    game.specialInfos[SPECIAL_DEATHHAND].subListId=0;
    strcpy(game.specialInfos[SPECIAL_DEATHHAND].description, "Death Hand");

}


/****************
 Install bullets
 ****************/
void IniGameRessources::install_bullets()
{
    logbook("Installing:  BULLET TYPES");

    for (int i = 0; i < game.bulletInfos.size(); i++) {
        game.bulletInfos[i].bmp = nullptr; // in case an invalid bitmap; default is a small rocket
        game.bulletInfos[i].moveSpeed = 2;
        game.bulletInfos[i].deathParticle = -1; // this points to a bitmap (in data file, using index)
        game.bulletInfos[i].damage_vehicles = 0;      // damage to vehicles
        game.bulletInfos[i].damage_infantry = 0;  // damage to infantry
        game.bulletInfos[i].max_frames = 1;  // 1 frame animation
        game.bulletInfos[i].max_deadframes = 4; // 4 frame animation
        game.bulletInfos[i].smokeParticle = -1; // by default no smoke particle is spawned
        game.bulletInfos[i].bmp_width = 8 * 2;
        game.bulletInfos[i].sound = -1;    // no sound
        game.bulletInfos[i].explosionSize = 1; // 1 tile sized explosion
        game.bulletInfos[i].deviateProbability = 0; // no probability of deviating a unit
        game.bulletInfos[i].groundBullet = false; // if true, then it gets blocked by walls, mountains or structures. False == flying bullets, ie, rockets
        game.bulletInfos[i].canDamageAirUnits = false; // if true, then upon impact the bullet can also damage air units
        game.bulletInfos[i].canDamageGround = false; // if true, then upon impact the bullet can also damage ground (walls, slabs, etc)
        strcpy(game.bulletInfos[i].description, "Unknown");
    }

    // huge rocket/missile
    game.bulletInfos[ROCKET_BIG].bmp = gfxdata->getTexture(BULLET_ROCKET_LARGE);
    game.bulletInfos[ROCKET_BIG].deathParticle = D2TM_PARTICLE_EXPLOSION_STRUCTURE01;
    game.bulletInfos[ROCKET_BIG].bmp_width = 48;
    game.bulletInfos[ROCKET_BIG].damage_vehicles = 999;
    game.bulletInfos[ROCKET_BIG].damage_infantry = 999;
    game.bulletInfos[ROCKET_BIG].max_frames = 1;
    game.bulletInfos[ROCKET_BIG].sound = SOUND_ROCKET;
    game.bulletInfos[ROCKET_BIG].explosionSize = 7;
    game.bulletInfos[ROCKET_BIG].canDamageAirUnits = true;
    game.bulletInfos[ROCKET_BIG].canDamageGround = true;
    game.bulletInfos[ROCKET_BIG].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(game.bulletInfos[ROCKET_BIG].description, "ROCKET_BIG");

    // small rocket (for ornithopter)
    game.bulletInfos[ROCKET_SMALL_ORNI].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    game.bulletInfos[ROCKET_SMALL_ORNI].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    game.bulletInfos[ROCKET_SMALL_ORNI].bmp_width = 16;
    game.bulletInfos[ROCKET_SMALL_ORNI].damage_vehicles = 48; // they can do pretty damage
    game.bulletInfos[ROCKET_SMALL_ORNI].damage_infantry = 24;
    game.bulletInfos[ROCKET_SMALL_ORNI].max_frames = 1;
    game.bulletInfos[ROCKET_SMALL_ORNI].sound = SOUND_ROCKET_SMALL;
    game.bulletInfos[ROCKET_SMALL_ORNI].canDamageAirUnits = true;
    game.bulletInfos[ROCKET_SMALL_ORNI].max_deadframes = 1;
    game.bulletInfos[ROCKET_SMALL_ORNI].canDamageGround = true;
    game.bulletInfos[ROCKET_SMALL_ORNI].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(game.bulletInfos[ROCKET_SMALL_ORNI].description, "ROCKET_SMALL_ORNI");

    // small rocket
    game.bulletInfos[ROCKET_SMALL].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    game.bulletInfos[ROCKET_SMALL].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    game.bulletInfos[ROCKET_SMALL].bmp_width = 16;
    game.bulletInfos[ROCKET_SMALL].damage_vehicles = 10; // was 8
    game.bulletInfos[ROCKET_SMALL].damage_infantry = 8; // was 4
    game.bulletInfos[ROCKET_SMALL].max_frames = 1;
    game.bulletInfos[ROCKET_SMALL].moveSpeed = 3;
    game.bulletInfos[ROCKET_SMALL].sound = SOUND_ROCKET_SMALL;
    game.bulletInfos[ROCKET_SMALL].canDamageAirUnits = true;
    game.bulletInfos[ROCKET_SMALL].max_deadframes = 1;
    game.bulletInfos[ROCKET_SMALL].canDamageGround = true;
//    bullets[ROCKET_SMALL].smokeParticle = BULLET_PUF; // small rockets have no smoke trail yet
    strcpy(game.bulletInfos[ROCKET_SMALL].description, "ROCKET_SMALL");

    // small rocket - fremen rocket
    game.bulletInfos[ROCKET_SMALL_FREMEN].bmp = gfxdata->getTexture(BULLET_ROCKET_SMALL);
    game.bulletInfos[ROCKET_SMALL_FREMEN].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    game.bulletInfos[ROCKET_SMALL_FREMEN].bmp_width = 16;
    game.bulletInfos[ROCKET_SMALL_FREMEN].damage_vehicles = 22;
    game.bulletInfos[ROCKET_SMALL_FREMEN].damage_infantry = 20;
    game.bulletInfos[ROCKET_SMALL_FREMEN].max_frames = 1;
    game.bulletInfos[ROCKET_SMALL_FREMEN].moveSpeed = 4;
    game.bulletInfos[ROCKET_SMALL_FREMEN].sound = SOUND_ROCKET_SMALL;
    game.bulletInfos[ROCKET_SMALL_FREMEN].max_deadframes = 1;
    game.bulletInfos[ROCKET_SMALL_FREMEN].canDamageAirUnits = true;
    game.bulletInfos[ROCKET_SMALL_FREMEN].canDamageGround = true;
//    bullets[ROCKET_SMALL_FREMEN].smokeParticle = true; // not yet
    strcpy(game.bulletInfos[ROCKET_SMALL_FREMEN].description, "ROCKET_SMALL_FREMEN");

    // normal rocket
    game.bulletInfos[ROCKET_NORMAL].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    game.bulletInfos[ROCKET_NORMAL].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET;
    game.bulletInfos[ROCKET_NORMAL].bmp_width = 32;
    game.bulletInfos[ROCKET_NORMAL].damage_vehicles = 76;
    game.bulletInfos[ROCKET_NORMAL].damage_infantry = 36;  // less damage on infantry
    game.bulletInfos[ROCKET_NORMAL].max_frames = 1;
    game.bulletInfos[ROCKET_NORMAL].sound = SOUND_ROCKET;
    game.bulletInfos[ROCKET_NORMAL].moveSpeed = 3;
    game.bulletInfos[ROCKET_NORMAL].max_deadframes = 4;
    game.bulletInfos[ROCKET_NORMAL].canDamageAirUnits = true;
    game.bulletInfos[ROCKET_NORMAL].canDamageGround = true;
    game.bulletInfos[ROCKET_NORMAL].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(game.bulletInfos[ROCKET_NORMAL].description, "ROCKET_NORMAL");

    // soldier shot
    game.bulletInfos[BULLET_SMALL].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    game.bulletInfos[BULLET_SMALL].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    game.bulletInfos[BULLET_SMALL].bmp_width = 6;
    game.bulletInfos[BULLET_SMALL].damage_vehicles = 4; // vehicles are no match
    game.bulletInfos[BULLET_SMALL].damage_infantry = 10; // infantry vs infantry means big time damage
    game.bulletInfos[BULLET_SMALL].max_frames = 0;
    game.bulletInfos[BULLET_SMALL].moveSpeed = 4;
    game.bulletInfos[BULLET_SMALL].sound = SOUND_GUN;
    game.bulletInfos[BULLET_SMALL].max_deadframes = 0;
    game.bulletInfos[BULLET_SMALL].groundBullet = true;
    strcpy(game.bulletInfos[BULLET_SMALL].description, "BULLET_SMALL");

    // trike shot
    game.bulletInfos[BULLET_TRIKE].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    game.bulletInfos[BULLET_TRIKE].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    game.bulletInfos[BULLET_TRIKE].bmp_width = 6;
    game.bulletInfos[BULLET_TRIKE].damage_vehicles = 3; // trikes do not do much damage to vehicles
    game.bulletInfos[BULLET_TRIKE].damage_infantry = 6; // but more to infantry
    game.bulletInfos[BULLET_TRIKE].moveSpeed = 4;
    game.bulletInfos[BULLET_TRIKE].max_frames = 0;
    game.bulletInfos[BULLET_TRIKE].sound = SOUND_MACHINEGUN;
    game.bulletInfos[BULLET_TRIKE].max_deadframes = 0;
    game.bulletInfos[BULLET_TRIKE].groundBullet = true;
    strcpy(game.bulletInfos[BULLET_TRIKE].description, "BULLET_TRIKE");

    // quad shot
    game.bulletInfos[BULLET_QUAD].bmp = gfxdata->getTexture(BULLET_DOT_SMALL);
    game.bulletInfos[BULLET_QUAD].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    game.bulletInfos[BULLET_QUAD].bmp_width = 6;
    game.bulletInfos[BULLET_QUAD].damage_vehicles = 6;
    game.bulletInfos[BULLET_QUAD].damage_infantry = 8; // bigger impact on infantry
    game.bulletInfos[BULLET_QUAD].max_frames = 0;
    game.bulletInfos[BULLET_QUAD].sound = SOUND_MACHINEGUN;
    game.bulletInfos[BULLET_QUAD].max_deadframes = 0;
    game.bulletInfos[BULLET_QUAD].groundBullet = true;
    strcpy(game.bulletInfos[BULLET_QUAD].description, "BULLET_QUAD");

    // normal tank shot
    game.bulletInfos[BULLET_TANK].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    game.bulletInfos[BULLET_TANK].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    game.bulletInfos[BULLET_TANK].bmp_width = 8;
    game.bulletInfos[BULLET_TANK].damage_vehicles = 12;
    game.bulletInfos[BULLET_TANK].damage_infantry = 4;  // infantry is not much damaged
    game.bulletInfos[BULLET_TANK].max_frames = 0;
    game.bulletInfos[BULLET_TANK].sound = SOUND_EXPL_ROCKET;
    game.bulletInfos[BULLET_TANK].max_deadframes = 1;
    game.bulletInfos[BULLET_TANK].groundBullet = true;
    game.bulletInfos[BULLET_TANK].canDamageGround = true;
    strcpy(game.bulletInfos[BULLET_TANK].description, "BULLET_TANK");

    // siege tank shot
    game.bulletInfos[BULLET_SIEGE].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    game.bulletInfos[BULLET_SIEGE].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    game.bulletInfos[BULLET_SIEGE].bmp_width = 8;
    game.bulletInfos[BULLET_SIEGE].damage_vehicles = 24;
    game.bulletInfos[BULLET_SIEGE].damage_infantry = 6; // infantry is not as much damaged
    game.bulletInfos[BULLET_SIEGE].max_frames = 0;
    game.bulletInfos[BULLET_SIEGE].sound = SOUND_EXPL_ROCKET;
    game.bulletInfos[BULLET_SIEGE].max_deadframes = 2;
    game.bulletInfos[BULLET_SIEGE].groundBullet = true;
    game.bulletInfos[BULLET_SIEGE].canDamageGround = true;
    strcpy(game.bulletInfos[BULLET_SIEGE].description, "BULLET_SIEGE");

    // devastator shot
    game.bulletInfos[BULLET_DEVASTATOR].bmp = gfxdata->getTexture(BULLET_DOT_LARGE);
    game.bulletInfos[BULLET_DEVASTATOR].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL; // not used anyway
    game.bulletInfos[BULLET_DEVASTATOR].bmp_width = 8;
    game.bulletInfos[BULLET_DEVASTATOR].damage_vehicles = 30;
    game.bulletInfos[BULLET_DEVASTATOR].damage_infantry = 12; // infantry again not much damaged
    game.bulletInfos[BULLET_DEVASTATOR].max_frames = 0;
    game.bulletInfos[BULLET_DEVASTATOR].sound = SOUND_EXPL_ROCKET;
    game.bulletInfos[BULLET_DEVASTATOR].max_deadframes = 1;
    game.bulletInfos[BULLET_DEVASTATOR].groundBullet = true;
    game.bulletInfos[BULLET_DEVASTATOR].canDamageGround = true;
    strcpy(game.bulletInfos[BULLET_DEVASTATOR].description, "BULLET_DEVASTATOR");

    // Gas rocket of a deviator
    game.bulletInfos[BULLET_GAS].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    game.bulletInfos[BULLET_GAS].deathParticle = D2TM_PARTICLE_EXPLOSION_GAS;
    game.bulletInfos[BULLET_GAS].bmp_width = 32;
    game.bulletInfos[BULLET_GAS].damage_vehicles = 1;
    game.bulletInfos[BULLET_GAS].damage_infantry = 1;
    game.bulletInfos[BULLET_GAS].max_frames = 1;
    game.bulletInfos[BULLET_GAS].max_deadframes = 4;
    game.bulletInfos[BULLET_GAS].sound = SOUND_ROCKET;
    game.bulletInfos[BULLET_GAS].deviateProbability = 34; // 1 out of 3(ish) should be effective
    game.bulletInfos[BULLET_GAS].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(game.bulletInfos[BULLET_GAS].description, "BULLET_GAS");

    // normal turret shot
    game.bulletInfos[BULLET_TURRET].bmp = gfxdata->getTexture(BULLET_DOT_MEDIUM);
    game.bulletInfos[BULLET_TURRET].deathParticle = D2TM_PARTICLE_EXPLOSION_BULLET; // not used anyway
    game.bulletInfos[BULLET_TURRET].bmp_width = 8;
    game.bulletInfos[BULLET_TURRET].damage_vehicles = 12;
    game.bulletInfos[BULLET_TURRET].damage_infantry = 12; // infantry is a hard target
    game.bulletInfos[BULLET_TURRET].max_frames = 0;
    game.bulletInfos[BULLET_TURRET].max_deadframes = 1;
    game.bulletInfos[BULLET_TURRET].sound = SOUND_GUNTURRET;
    game.bulletInfos[BULLET_TURRET].groundBullet = false; // this can fly over structures, walls, mountains, yes!
    game.bulletInfos[BULLET_TURRET].canDamageGround = true;
    strcpy(game.bulletInfos[BULLET_TURRET].description, "BULLET_TURRET");

    // EXEPTION: Shimmer/ Sonic tank
    game.bulletInfos[BULLET_SHIMMER].bmp = nullptr;
    game.bulletInfos[BULLET_SHIMMER].deathParticle = -1;
    game.bulletInfos[BULLET_SHIMMER].bmp_width = 0;
    game.bulletInfos[BULLET_SHIMMER].damage_vehicles = 55;
    game.bulletInfos[BULLET_SHIMMER].damage_infantry = 70; // infantry cant stand the sound, die very fast
    game.bulletInfos[BULLET_SHIMMER].max_frames = 0;
    game.bulletInfos[BULLET_SHIMMER].max_deadframes = 0;
    game.bulletInfos[BULLET_SHIMMER].sound = SOUND_SHIMMER;
    strcpy(game.bulletInfos[BULLET_SHIMMER].description, "BULLET_SHIMMER");

    // rocket of rocket turret
    game.bulletInfos[ROCKET_RTURRET].bmp = gfxdata->getTexture(BULLET_ROCKET_NORMAL);
    game.bulletInfos[ROCKET_RTURRET].deathParticle = D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL;
    game.bulletInfos[ROCKET_RTURRET].bmp_width = 16 * 2;
    game.bulletInfos[ROCKET_RTURRET].damage_vehicles = 25;
    game.bulletInfos[ROCKET_RTURRET].damage_infantry = 10; // infantry is a bit tougher
    game.bulletInfos[ROCKET_RTURRET].max_frames = 1;
    game.bulletInfos[ROCKET_RTURRET].sound = SOUND_ROCKET;
    game.bulletInfos[ROCKET_RTURRET].max_deadframes = 4;
    game.bulletInfos[ROCKET_RTURRET].canDamageAirUnits = true;
    game.bulletInfos[ROCKET_RTURRET].canDamageGround = true;
    game.bulletInfos[ROCKET_RTURRET].smokeParticle = D2TM_PARTICLE_BULLET_PUF;
    strcpy(game.bulletInfos[ROCKET_RTURRET].description, "ROCKET_RTURRET");
}


void IniGameRessources::install_upgrades()
{
    logbook("Installing:  UPGRADES");
    for (int i = 0; i < MAX_UPGRADETYPES; i++) {
        game.upgradeInfos[i].enabled = false;
        game.upgradeInfos[i].techLevel = -1;
        game.upgradeInfos[i].house = 0;
        game.upgradeInfos[i].needsStructureType = -1;
        game.upgradeInfos[i].icon = ICON_STR_PALACE;
        game.upgradeInfos[i].cost = 100;
        game.upgradeInfos[i].atUpgradeLevel = -1;
        game.upgradeInfos[i].structureType = CONSTYARD;
        game.upgradeInfos[i].providesTypeId = -1;
        game.upgradeInfos[i].providesType = STRUCTURE;
        game.upgradeInfos[i].providesTypeList = eListType::LIST_NONE;
        game.upgradeInfos[i].providesTypeSubList = -1;
        game.upgradeInfos[i].buildTime = 5;
        strcpy(game.upgradeInfos[i].description, "Upgrade");
    }

    // CONSTYARD UPGRADES

    // First upgrade Constyard: 4Slabs
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].house = Atreides | Harkonnen | Ordos | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].techLevel = 4; // start from mission 4
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].icon = ICON_STR_4SLAB;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].cost = 200;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].structureType = CONSTYARD;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].atUpgradeLevel = 0;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesType = STRUCTURE;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeId = SLAB4;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeList = eListType::LIST_CONSTYARD;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeSubList = SUBLIST_CONSTYARD;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].buildTime = 50;
    strcpy(game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_SLAB4].description, "Build 4 concrete slabs at once");

    // Second upgrade Constyard: Rturret
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].house = Atreides | Harkonnen | Ordos | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].techLevel = 6;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].icon = ICON_STR_RTURRET;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].cost = 200;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].needsStructureType = RADAR;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].structureType = CONSTYARD;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].atUpgradeLevel = 1;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesType = STRUCTURE;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeId = RTURRET;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeList = eListType::LIST_CONSTYARD;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeSubList = SUBLIST_CONSTYARD;
    game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].buildTime = 150;

    strcpy(game.upgradeInfos[UPGRADE_TYPE_CONSTYARD_RTURRET].description, "Build Rocket Turret");

    // LIGHTFACTORY UPGRADES, only for ATREIDES and ORDOS
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].house = Atreides | Ordos | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].techLevel = 3;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].icon = ICON_UNIT_QUAD;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].cost = game.structureInfos[LIGHTFACTORY].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].needsStructureType = LIGHTFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].structureType = LIGHTFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].atUpgradeLevel = 0;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeId = QUAD;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeList = eListType::LIST_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeSubList =  SUBLIST_LIGHTFCTRY;
    game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].buildTime = 150;

    strcpy(game.upgradeInfos[UPGRADE_TYPE_LIGHTFCTRY_QUAD].description, "Build Quad at Light Factory");

    // HEAVYFACTORY UPGRADES:

    // ALL HOUSES GET MVC
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].house = Atreides | Ordos | Harkonnen | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].techLevel = 4;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].icon = ICON_UNIT_MCV;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost = game.structureInfos[HEAVYFACTORY].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].needsStructureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].structureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].atUpgradeLevel = 0;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeId = MCV;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeList = eListType::LIST_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].buildTime = 150;

    strcpy(game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_MVC].description, "Build MCV at Heavy Factory");

    // Harkonnen/Atreides only
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].house = Atreides | Harkonnen | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].techLevel = 5;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].icon = ICON_UNIT_LAUNCHER;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].cost = game.structureInfos[HEAVYFACTORY].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].needsStructureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].structureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].atUpgradeLevel = 1; // requires MCV upgrade first
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeId = LAUNCHER;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeList = eListType::LIST_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].buildTime = 150;

    strcpy(game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].description, "Build Rocket Launcher at Heavy Factory");

    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].house = Atreides | Harkonnen | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].techLevel = 6;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].icon = ICON_UNIT_SIEGETANK;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].cost = game.structureInfos[HEAVYFACTORY].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].needsStructureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].structureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].atUpgradeLevel = 2; // After upgrade to Rocket Launcher
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeId = SIEGETANK;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeList = eListType::LIST_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].buildTime = 150;

    strcpy(game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].description, "Build Siege Tank at Heavy Factory");

    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].house = Ordos;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].techLevel = 6;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].icon = ICON_UNIT_SIEGETANK;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].cost = game.structureInfos[HEAVYFACTORY].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].needsStructureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].structureType = HEAVYFACTORY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].atUpgradeLevel = 1;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeId = SIEGETANK;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeList = eListType::LIST_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].buildTime = 150;

    strcpy(game.upgradeInfos[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].description, "Build Siege Tank at Heavy Factory");

    // HI-TECH UPGRADES (Ordos/Atreides only)
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].house = Atreides | Ordos | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].techLevel = 8;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].icon = ICON_UNIT_ORNITHOPTER;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].cost = game.structureInfos[HIGHTECH].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].structureType = HIGHTECH;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].atUpgradeLevel = 0;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeId = ORNITHOPTER;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeList = eListType::LIST_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].providesTypeSubList = SUBLIST_HIGHTECH;
    game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].buildTime = 150;
    strcpy(game.upgradeInfos[UPGRADE_TYPE_HITECH_ORNI].description, "Build Ornithopter at Hi-Tech");

    // WOR (Harkonnen & Ordos)
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].house = Harkonnen | Ordos | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].techLevel = 3;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].icon = ICON_UNIT_TROOPERS;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].cost = game.structureInfos[WOR].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].structureType = WOR;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].atUpgradeLevel = 0;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeId = TROOPERS;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeList = eListType::LIST_FOOT_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].providesTypeSubList = SUBLIST_TROOPERS;
    if (!game.isDebugMode()) {
        game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].buildTime = 150;
    }
    strcpy(game.upgradeInfos[UPGRADE_TYPE_WOR_TROOPERS].description, "Build Troopers at WOR");

    // BARRACKS (Atreides & Ordos)
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].enabled = true;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].house = Atreides | Ordos | Sardaukar;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].techLevel = 3;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].icon = ICON_UNIT_INFANTRY;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].cost = game.structureInfos[BARRACKS].cost / 2;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].structureType = BARRACKS;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].atUpgradeLevel = 0;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesType = UNIT;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeId = INFANTRY;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeList = eListType::LIST_FOOT_UNITS;
    game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeSubList = SUBLIST_INFANTRY;
    if (!game.isDebugMode()) {
        game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].buildTime = 150;
    }
    strcpy(game.upgradeInfos[UPGRADE_TYPE_BARRACKS_INFANTRY].description, "Build Infantry at Barracks");

}


/*****************************
 Structure Rules
 *****************************/
void IniGameRessources::install_structures()
{

    logbook("Installing:  STRUCTURES");
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        s_StructureInfo &structureInfo = game.structureInfos[i];
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
        strcpy(structureInfo.name, "Unknown");
    }

    // Single and 4 slabs
    game.structureInfos[SLAB1].bmp = gfxdata->getSurface(PLACE_SLAB1); // in case an invalid bitmap, we are a windtrap
    game.structureInfos[SLAB1].icon = ICON_STR_1SLAB;
    game.structureInfos[SLAB1].hp = 25;            // Not functional in-game, only for building
    game.structureInfos[SLAB1].bmp_width = 16 * 2;
    game.structureInfos[SLAB1].bmp_height = 16 * 2;
    game.structureInfos[SLAB1].configured = true;
    game.structureInfos[SLAB1].queuable = true;
    strcpy(game.structureInfos[SLAB1].name, "Concrete Slab");

    game.structureInfos[SLAB4].bmp = gfxdata->getSurface(PLACE_SLAB4); // in case an invalid bitmap, we are a windtrap
    game.structureInfos[SLAB4].icon = ICON_STR_4SLAB;
    game.structureInfos[SLAB4].hp = 75;            // Not functional in-game, only for building
    game.structureInfos[SLAB4].bmp_width = 32 * 2;
    game.structureInfos[SLAB4].bmp_height = 32 * 2;
    game.structureInfos[SLAB4].configured = true;
    game.structureInfos[SLAB4].queuable = true;
    strcpy(game.structureInfos[SLAB4].name, "4 Concrete Slabs");


    // Concrete Wall
    game.structureInfos[WALL].bmp = gfxdata->getSurface(PLACE_WALL); // in case an invalid bitmap, we are a windtrap
    game.structureInfos[WALL].icon = ICON_STR_WALL;
    game.structureInfos[WALL].hp = 75;            // Not functional in-game, only for building
    game.structureInfos[WALL].bmp_width = 16 * 2;
    game.structureInfos[WALL].bmp_height = 16 * 2;
    game.structureInfos[WALL].queuable = true;
    game.structureInfos[WALL].configured = true;
    strcpy(game.structureInfos[WALL].name, "Concrete Wall");

    // Structure    : Windtrap
    // Description  : <none>
    game.structureInfos[WINDTRAP].bmp = gfxdata->getSurface(BUILD_WINDTRAP);
    game.structureInfos[WINDTRAP].shadow = gfxdata->getTexture(BUILD_WINDTRAP_SHADOW); // shadow
    game.structureInfos[WINDTRAP].fadecol = 128;
    game.structureInfos[WINDTRAP].fademax = 134;
    game.structureInfos[WINDTRAP].icon = ICON_STR_WINDTRAP;
    game.structureInfos[WINDTRAP].configured = true;
    game.structureInfos[WINDTRAP].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 53,
        .relY = 37
    });
    strcpy(game.structureInfos[WINDTRAP].name, "Windtrap");

    // Structure    : Heavy Factory
    // Description  : <none>
    game.structureInfos[HEAVYFACTORY].bmp_width = 48 * 2;
    game.structureInfos[HEAVYFACTORY].bmp_height = 32 * 2;
    game.structureInfos[HEAVYFACTORY].bmp = gfxdata->getSurface(BUILD_HEAVYFACTORY);
    game.structureInfos[HEAVYFACTORY].shadow = gfxdata->getTexture(BUILD_HEAVYFACTORY_SHADOW); // shadow
    game.structureInfos[HEAVYFACTORY].flash = gfxdata->getSurface(BUILD_HEAVYFACTORY_FLASH);
    game.structureInfos[HEAVYFACTORY].fadecol = -1;
    game.structureInfos[HEAVYFACTORY].icon = ICON_STR_HEAVYFACTORY;
    game.structureInfos[HEAVYFACTORY].configured = true;
    game.structureInfos[HEAVYFACTORY].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 25,
        .relY = 3
    });
    strcpy(game.structureInfos[HEAVYFACTORY].name, "Heavy Factory");

    // Structure    : Hight Tech Factory (for aircraft only)
    // Description  : <none>
    game.structureInfos[HIGHTECH].bmp_width = 48 * 2;
    game.structureInfos[HIGHTECH].bmp_height = 32 * 2;
    game.structureInfos[HIGHTECH].bmp = gfxdata->getSurface(BUILD_HIGHTECH);
    game.structureInfos[HIGHTECH].shadow = gfxdata->getTexture(BUILD_HIGHTECH_SHADOW);
    game.structureInfos[HIGHTECH].fadecol = -1;
    game.structureInfos[HIGHTECH].icon = ICON_STR_HIGHTECH;
    game.structureInfos[HIGHTECH].configured = true;
    game.structureInfos[HIGHTECH].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 19,
        .relY = 36
    });
    strcpy(game.structureInfos[HIGHTECH].name, "Hi-Tech");

    // Structure    : Repair
    // Description  : <none>
    game.structureInfos[REPAIR].bmp_width = 48 * 2;
    game.structureInfos[REPAIR].bmp_height = 32 * 2;
    game.structureInfos[REPAIR].bmp = gfxdata->getSurface(BUILD_REPAIR);
    game.structureInfos[REPAIR].shadow = gfxdata->getTexture(BUILD_REPAIR_SHADOW);
    game.structureInfos[REPAIR].fadecol = -1;
    game.structureInfos[REPAIR].icon = ICON_STR_REPAIR;
    game.structureInfos[REPAIR].configured = true;
    game.structureInfos[REPAIR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 41,
        .relY = 3
    });
    strcpy(game.structureInfos[REPAIR].name, "Repair Facility");

    // Structure    : Palace
    // Description  : <none>
    game.structureInfos[PALACE].bmp_width = 48 * 2;
    game.structureInfos[PALACE].bmp_height = 48 * 2;
    game.structureInfos[PALACE].bmp = gfxdata->getSurface(BUILD_PALACE);
    game.structureInfos[PALACE].shadow = gfxdata->getTexture(BUILD_PALACE_SHADOW);
    game.structureInfos[PALACE].icon = ICON_STR_PALACE;
    game.structureInfos[PALACE].configured = true;
    game.structureInfos[PALACE].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 28,
        .relY = 14
    });
    strcpy(game.structureInfos[PALACE].name, "Palace");

    // Structure    : Light Factory
    // Description  : <none>
    game.structureInfos[LIGHTFACTORY].bmp_width = 32 * 2;
    game.structureInfos[LIGHTFACTORY].bmp_height = 32 * 2;
    game.structureInfos[LIGHTFACTORY].bmp = gfxdata->getSurface(BUILD_LIGHTFACTORY);
    game.structureInfos[LIGHTFACTORY].shadow = gfxdata->getTexture(BUILD_LIGHTFACTORY_SHADOW);
    game.structureInfos[LIGHTFACTORY].flash = gfxdata->getSurface(BUILD_LIGHTFACTORY_FLASH);
    game.structureInfos[LIGHTFACTORY].fadecol = -1;
    game.structureInfos[LIGHTFACTORY].icon = ICON_STR_LIGHTFACTORY;
    game.structureInfos[LIGHTFACTORY].configured = true;
    game.structureInfos[LIGHTFACTORY].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 41,
        .relY = 2
    });
    strcpy(game.structureInfos[LIGHTFACTORY].name, "Light Factory");

    // Structure    : Radar
    // Description  : <none>
    game.structureInfos[RADAR].bmp_width = 32 * 2;
    game.structureInfos[RADAR].bmp_height = 32 * 2;
    game.structureInfos[RADAR].bmp = gfxdata->getSurface(BUILD_RADAR);
    game.structureInfos[RADAR].shadow = gfxdata->getTexture(BUILD_RADAR_SHADOW); // shadow
    game.structureInfos[RADAR].sight = 12;
    game.structureInfos[RADAR].fadecol = -1;
    game.structureInfos[RADAR].icon = ICON_STR_RADAR;
    game.structureInfos[RADAR].configured = true;
    // outpost has 2 flags
    game.structureInfos[RADAR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 17,
        .relY = 38
    });
    game.structureInfos[RADAR].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 12,
        .relY = 46
    });
    strcpy(game.structureInfos[RADAR].name, "Outpost");

    // Structure    : Barracks
    // Description  : <none>
    game.structureInfos[BARRACKS].bmp_width = 32 * 2;
    game.structureInfos[BARRACKS].bmp_height = 32 * 2;
    game.structureInfos[BARRACKS].bmp = gfxdata->getSurface(BUILD_BARRACKS);
    game.structureInfos[BARRACKS].shadow = gfxdata->getTexture(BUILD_BARRACKS_SHADOW);
    game.structureInfos[BARRACKS].fadecol = -1;
    game.structureInfos[BARRACKS].icon = ICON_STR_BARRACKS;
    game.structureInfos[BARRACKS].configured = true;
    game.structureInfos[BARRACKS].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 60,
        .relY = 47
    });
    game.structureInfos[BARRACKS].flags.push_back(s_FlagInfo{
        .big = false,
        .relX = 51,
        .relY = 50
    });
    strcpy(game.structureInfos[BARRACKS].name, "Barracks");

    // Structure    : WOR
    // Description  : <none>
    game.structureInfos[WOR].bmp_width = 32 * 2;
    game.structureInfos[WOR].bmp_height = 32 * 2;
    game.structureInfos[WOR].bmp = gfxdata->getSurface(BUILD_WOR);
    game.structureInfos[WOR].shadow = gfxdata->getTexture(BUILD_WOR_SHADOW);
    game.structureInfos[WOR].fadecol = -1;
    game.structureInfos[WOR].icon = ICON_STR_WOR;
    game.structureInfos[WOR].configured = true;
    game.structureInfos[WOR].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 21,
        .relY = 31
    });
    strcpy(game.structureInfos[WOR].name, "WOR");


    // Structure    : Silo
    // Description  : <none>
    game.structureInfos[SILO].bmp_width = 32 * 2;
    game.structureInfos[SILO].bmp_height = 32 * 2;
    game.structureInfos[SILO].bmp = gfxdata->getSurface(BUILD_SILO);
    game.structureInfos[SILO].shadow = gfxdata->getTexture(BUILD_SILO_SHADOW);
    game.structureInfos[SILO].fadecol = -1;
    game.structureInfos[SILO].icon = ICON_STR_SILO;
    game.structureInfos[SILO].configured = true;
    game.structureInfos[SILO].queuable = true;
    game.structureInfos[SILO].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 43,
        .relY = 20
    });
    strcpy(game.structureInfos[SILO].name, "Spice Storage Silo");

    // Structure    : Refinery
    // Description  : <none>
    game.structureInfos[REFINERY].bmp_width = 48 * 2;
    game.structureInfos[REFINERY].bmp_height = 32 * 2;
    game.structureInfos[REFINERY].bmp = gfxdata->getSurface(BUILD_REFINERY);
    game.structureInfos[REFINERY].shadow = gfxdata->getTexture(BUILD_REFINERY_SHADOW);
    game.structureInfos[REFINERY].fadecol = -1;
    game.structureInfos[REFINERY].icon = ICON_STR_REFINERY;
    game.structureInfos[REFINERY].configured = true;
    game.structureInfos[REFINERY].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 45,
        .relY = 12
    });
    strcpy(game.structureInfos[REFINERY].name, "Spice Refinery");

    // Structure    : Construction Yard
    // Description  : <none>
    game.structureInfos[CONSTYARD].bmp_width = 32 * 2;
    game.structureInfos[CONSTYARD].bmp_height = 32 * 2;
    game.structureInfos[CONSTYARD].sight = 4;
    game.structureInfos[CONSTYARD].bmp = gfxdata->getSurface(BUILD_CONSTYARD);
    game.structureInfos[CONSTYARD].shadow = gfxdata->getTexture(BUILD_CONSTYARD_SHADOW);
    game.structureInfos[CONSTYARD].hasConcrete = false;
    game.structureInfos[CONSTYARD].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 14,
        .relY = 14
    });
    game.structureInfos[CONSTYARD].fadecol = -1;
    game.structureInfos[CONSTYARD].icon = ICON_STR_CONSTYARD;
    game.structureInfos[CONSTYARD].configured = true;
    strcpy(game.structureInfos[CONSTYARD].name, "Construction Yard");

    // Structure    : Starport
    // Description  : You can order units from this structure
    game.structureInfos[STARPORT].bmp_width = 48 * 2;
    game.structureInfos[STARPORT].bmp_height = 48 * 2;
    game.structureInfos[STARPORT].bmp = gfxdata->getSurface(BUILD_STARPORT);
    game.structureInfos[STARPORT].shadow = gfxdata->getTexture(BUILD_STARPORT_SHADOW);
    game.structureInfos[STARPORT].fadecol = -1;
    game.structureInfos[STARPORT].icon = ICON_STR_STARPORT;
    game.structureInfos[STARPORT].configured = true;
    game.structureInfos[STARPORT].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 16,
        .relY = 3
    });
    strcpy(game.structureInfos[STARPORT].name, "Starport");

    // Structure    : House of IX
    // Description  : Makes it possible for the player to upgrade its Heavy Factory in order to build their special weapon
    game.structureInfos[IX].bmp_width = 32 * 2;
    game.structureInfos[IX].bmp_height = 32 * 2;
    game.structureInfos[IX].bmp = gfxdata->getSurface(BUILD_IX);
    game.structureInfos[IX].shadow = gfxdata->getTexture(BUILD_IX_SHADOW);
    game.structureInfos[IX].fadecol = -1;
    game.structureInfos[IX].icon = ICON_STR_IX;
    game.structureInfos[IX].configured = true;
    game.structureInfos[IX].flags.push_back(s_FlagInfo{
        .big = true,
        .relX = 60, // 60
        .relY = 40  // 40
    });
    strcpy(game.structureInfos[IX].name, "House of IX");

    // Structure    : Normal Turret
    // Description  : defence
    game.structureInfos[TURRET].bmp_width = 16 * 2;
    game.structureInfos[TURRET].bmp_height = 16 * 2;
    game.structureInfos[TURRET].bmp = gfxdata->getSurface(BUILD_TURRET);
    game.structureInfos[TURRET].shadow = gfxdata->getTexture(BUILD_TURRET_SHADOW);
    game.structureInfos[TURRET].fadecol = -1;
    game.structureInfos[TURRET].icon = ICON_STR_TURRET;
    game.structureInfos[TURRET].sight = 7;
    game.structureInfos[TURRET].configured = true;
    game.structureInfos[TURRET].canAttackGroundUnits = true;
    game.structureInfos[TURRET].fireRate = 275;
    strcpy(game.structureInfos[TURRET].name, "Gun Turret");

    // Structure    : Rocket Turret
    // Description  : defence
    game.structureInfos[RTURRET].bmp_width = 16 * 2;
    game.structureInfos[RTURRET].bmp_height = 16 * 2;
    game.structureInfos[RTURRET].bmp = gfxdata->getSurface(BUILD_RTURRET);
    game.structureInfos[RTURRET].shadow = gfxdata->getTexture(BUILD_RTURRET_SHADOW);
    game.structureInfos[RTURRET].fadecol = -1;
    game.structureInfos[RTURRET].icon = ICON_STR_RTURRET;
    game.structureInfos[RTURRET].sight = 10;
    game.structureInfos[RTURRET].configured = true;
    game.structureInfos[RTURRET].canAttackAirUnits = true;
    game.structureInfos[RTURRET].canAttackGroundUnits = true;
    game.structureInfos[RTURRET].fireRate = 350;
    strcpy(game.structureInfos[RTURRET].name, "Rocket Turret");

    // Structure    : Windtrap
    // Description  : <none>

}


void IniGameRessources::install_terrain(std::shared_ptr<s_TerrainInfo>& terrainInfo)
{
    terrainInfo->bloomTimerDuration = 200;
    terrainInfo->terrainSpiceMinSpice = 50;
    terrainInfo->terrainSpiceMaxSpice = 125;
    terrainInfo->terrainSpiceHillMinSpice = 75;
    terrainInfo->terrainSpiceHillMaxSpice = 150;
    terrainInfo->terrainWallHp = 100;
}
