/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include "../include/d2tmh.h"

#include <math.h>

namespace
{
    const int PAN_CENTER = 128;
}

namespace
{
    const int VOLUME_MAX = 255;
}

namespace
{
    const int BUFFER_SIZE = 32768;
}

/**
 * Default printing in logs. Only will be done if DEBUGGING is true.
 * @param txt
 */
void logbook(const char *txt) {
  if (DEBUGGING) {
    cLogger *logger = cLogger::getInstance();
    logger->log(LOG_WARN, COMP_NONE, "(logbook)", txt);
  }
}

/**
 * Returns true if x,y is within the playable map boundaries
 * @param x
 * @param y
 * @return
 */
// determine if this cell is not out of boundries
bool BORDER_POS(int x, int y) {
    if (x < 1) return false;
    if (x > (game.map_width - 2)) return false;

    if (y < 1) return false;
    if (y > (game.map_height - 2)) return false;

    return true;
}

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
    // filled in
    if (x) {
        if (x < 1) {
            x = 1;
        }
        if (x > (game.map_width - 1)) {
            x = (game.map_width - 1);
        }
    }

    // filled in
    if (y) {
        if (y < 1) {
            y = 1;
        }
        if (y > (game.map_height - 1)) {
            y = (game.map_height - 1);
        }
    }
}

// Will make sure the X and Y don't get out of their boundaries
void FIX_POS(int &x, int &y) {
    // filled in
    if (x) {
        if (x < 0) x = 0;
        if (x > game.map_width) x = game.map_width;
    }

    // filled in
    if (y) {
        if (y < 0) y = 0;
        if (y > game.map_height) y = game.map_height;
    }
}


void INIT_ALL_PLAYERS() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        player[i].init(i);
    }
}


/********************************
 House Rules
 ********************************/
void INSTALL_HOUSES() {
    // General / Default / No House
    houses[GENERALHOUSE].swap_color = 128;
    houses[GENERALHOUSE].minimap_color = makecol(128, 128, 128);

    // Harkonnen
    houses[HARKONNEN].swap_color = -1;  // 144
    houses[HARKONNEN].minimap_color = makecol(255, 0, 0);

    // Atreides
    houses[ATREIDES].swap_color = 160;
    houses[ATREIDES].minimap_color = makecol(0, 0, 255);

    // Ordos
    houses[ORDOS].swap_color = 176;
    houses[ORDOS].minimap_color = makecol(0, 255, 0);

    // Mercenary
    houses[MERCENARY].swap_color = 192;
    houses[MERCENARY].minimap_color = makecol(214, 121, 16);

    // Sardaukar
    houses[SARDAUKAR].swap_color = 208;
    houses[SARDAUKAR].minimap_color = makecol(255, 0, 255);

    // Fremen
    houses[FREMEN].swap_color = 224;
    houses[FREMEN].minimap_color = makecol(194, 125, 60); // Fremen is colored as "sand" on the minimap

    // GREY

    // ???
    houses[CORRINO].swap_color = 136;
    houses[CORRINO].minimap_color = makecol(192, 192, 192); // grey
}


/**
 * Returns true if left mouse button is pressed.
 * @return
 */
bool MOUSE_BTN_LEFT() {
	return cMouse::isLeftButtonPressed();
}

/**
 * Returns true if right mouse button is pressed
 * @return
 */
bool MOUSE_BTN_RIGHT() {
	return cMouse::isRightButtonPressed();
}


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
    units[i].bmp              = (BITMAP *)gfxdata[UNIT_QUAD].dat; // default bitmap is a quad!
    units[i].top              = NULL;  // no top
    units[i].shadow           = NULL;  // no shadow (deliverd with picture itself)
    units[i].bmp_width        = 0;
    units[i].bmp_height       = 0;
    units[i].turnspeed        = 0;
    units[i].speed            = 0;
    units[i].icon             = -1;
    units[i].hp               = -1;
    units[i].bullets          = -1;
    units[i].attack_frequency = -1;
    units[i].build_time       = -1;
    units[i].airborn          = false;
    units[i].infantry         = false;
    units[i].free_roam        = false;
    units[i].second_shot      = false;
    units[i].squish           = true;     // most units can squish
    units[i].range            = -1;
    units[i].sight            = -1;
    units[i].queuable         = true;

    // harvester properties
    units[i].harvesting_amount= 0;
    units[i].harvesting_speed = 0;
    units[i].credit_capacity  = 0;

    // list properties
    units[i].listId           = 0;
    units[i].subListId        = 0;

    // attack related
    units[i].canAttackAirUnits = false;

    strcpy(units[i].name, "\0");
  }

  // Unit        : CarryAll
  // Description : CarryAll, the flying pickuptruck
  units[CARRYALL].bmp = (BITMAP *)gfxdata[UNIT_CARRYALL].dat;      // pointer to the original 8bit bitmap
  units[CARRYALL].shadow = (BITMAP *)gfxdata[UNIT_CARRYALL_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[CARRYALL].bmp_width  = 24*2;
  units[CARRYALL].bmp_height = 24*2;
  units[CARRYALL].bmp_startpixel = 0;
  units[CARRYALL].bmp_frames = 2; // we have at max 1 extra frame
  units[CARRYALL].icon = ICON_UNIT_CARRYALL;
  units[CARRYALL].airborn=true;   // is airborn
  units[CARRYALL].free_roam=true; // may freely roam the air
  units[CARRYALL].listId=LIST_UNITS;
  units[CARRYALL].subListId=SUBLIST_HIGHTECH;
  strcpy(units[CARRYALL].name, "Carry-All");

      // Unit        : Ornithopter
  // Description : Pesty little aircraft shooting bastard
  units[ORNITHOPTER].bmp = (BITMAP *)gfxdata[UNIT_ORNITHOPTER].dat;      // pointer to the original 8bit bitmap
  units[ORNITHOPTER].shadow = (BITMAP *)gfxdata[UNIT_ORNITHOPTER_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[ORNITHOPTER].bmp_width  = 24*2;
  units[ORNITHOPTER].bmp_height = 24*2;
  units[ORNITHOPTER].bmp_startpixel = 0;
  units[ORNITHOPTER].bmp_frames = 4; // we have at max 3 extra frames
  units[ORNITHOPTER].icon = ICON_UNIT_ORNITHOPTER;
  units[ORNITHOPTER].bullets = ROCKET_SMALL_ORNI;
  units[ORNITHOPTER].second_shot = true;
  units[ORNITHOPTER].airborn = true;   // is airborn
  units[ORNITHOPTER].free_roam=true; // may freely roam the air
  units[ORNITHOPTER].listId=LIST_UNITS;
  units[ORNITHOPTER].subListId=SUBLIST_HIGHTECH;
  strcpy(units[ORNITHOPTER].name, "Ornithopter");

    // Unit        : Devastator
  // Description : Devastator
  units[DEVASTATOR].bmp = (BITMAP *)gfxdata[UNIT_DEVASTATOR].dat;      // pointer to the original 8bit bitmap
  units[DEVASTATOR].shadow = (BITMAP *)gfxdata[UNIT_DEVASTATOR_SHADOW].dat;      // pointer to the original bitmap shadow
  units[DEVASTATOR].bmp_width  = 19*2;
  units[DEVASTATOR].bmp_startpixel = 0;
  units[DEVASTATOR].bmp_height = 23*2;
  units[DEVASTATOR].bmp_frames = 1;
  units[DEVASTATOR].bullets = BULLET_DEVASTATOR;
  units[DEVASTATOR].second_shot = true;
  units[DEVASTATOR].icon    = ICON_UNIT_DEVASTATOR;
  units[DEVASTATOR].listId=LIST_UNITS;
  units[DEVASTATOR].subListId=SUBLIST_HEAVYFCTRY;
  strcpy(units[DEVASTATOR].name, "Devastator");

  // Unit        : Harvester
  // Description : Harvester
  units[HARVESTER].bmp = (BITMAP *)gfxdata[UNIT_HARVESTER].dat;      // pointer to the original 8bit bitmap
  units[HARVESTER].shadow = (BITMAP *)gfxdata[UNIT_HARVESTER_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[HARVESTER].bmp_width  = 40*2;
  units[HARVESTER].bmp_startpixel = 24;
  units[HARVESTER].bmp_height = 26*2;
  units[HARVESTER].bmp_frames = 4;
  units[HARVESTER].icon = ICON_UNIT_HARVESTER;
  units[HARVESTER].credit_capacity = 700;
  units[HARVESTER].harvesting_amount = 5;
  units[HARVESTER].listId=LIST_UNITS;
  units[HARVESTER].subListId=SUBLIST_HEAVYFCTRY;
  strcpy(units[HARVESTER].name, "Harvester");

  // Unit        : Combattank
  // Description : Combattank
  units[TANK].bmp = (BITMAP *)gfxdata[UNIT_TANKBASE].dat;      // pointer to the original 8bit bitmap
  units[TANK].shadow = (BITMAP *)gfxdata[UNIT_TANKBASE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[TANK].top = (BITMAP *)gfxdata[UNIT_TANKTOP].dat;      // pointer to the original 8bit bitmap
  units[TANK].bmp_width  = 16*2;
  units[TANK].bmp_startpixel = 0;
  units[TANK].bmp_height = 16*2;
  units[TANK].bmp_frames = 0;
  units[TANK].bullets = BULLET_TANK;
  units[TANK].icon    = ICON_UNIT_TANK;
  units[TANK].listId=LIST_UNITS;
  units[TANK].subListId=SUBLIST_HEAVYFCTRY;
  strcpy(units[TANK].name, "Tank");


  // Unit        : Siege Tank
  // Description : Siege tank
  units[SIEGETANK].bmp = (BITMAP *)gfxdata[UNIT_SIEGEBASE].dat;      // pointer to the original 8bit bitmap
  units[SIEGETANK].shadow = (BITMAP *)gfxdata[UNIT_SIEGEBASE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[SIEGETANK].top = (BITMAP *)gfxdata[UNIT_SIEGETOP].dat;      // pointer to the original 8bit bitmap
  units[SIEGETANK].bmp_width  = 18*2;
  units[SIEGETANK].bmp_startpixel = 1;
  units[SIEGETANK].bmp_height = 18*2;
  units[SIEGETANK].bmp_frames = 0;
  units[SIEGETANK].bullets = BULLET_SIEGE;
  units[SIEGETANK].second_shot = true;
  units[SIEGETANK].icon    = ICON_UNIT_SIEGETANK;
  units[SIEGETANK].listId=LIST_UNITS;
  units[SIEGETANK].subListId=SUBLIST_HEAVYFCTRY;
  strcpy(units[SIEGETANK].name, "Siege Tank");

  // Unit        : MCV
  // Description : Movable Construction Vehicle
  units[MCV].bmp = (BITMAP *)gfxdata[UNIT_MCV].dat;      // pointer to the original 8bit bitmap
  units[MCV].shadow = (BITMAP *)gfxdata[UNIT_MCV_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[MCV].bmp_width  = 24*2;
  units[MCV].bmp_startpixel = 0;
  units[MCV].bmp_height = 25*2;
  units[MCV].bmp_frames = 1;
  units[MCV].icon = ICON_UNIT_MCV;
  units[MCV].listId=LIST_UNITS;
  units[MCV].subListId=SUBLIST_HEAVYFCTRY;
  strcpy(units[MCV].name, "MCV");


  // Unit        : Deviator
  // Description : Deviator
  units[DEVIATOR].bmp = (BITMAP *)gfxdata[UNIT_DEVIATOR].dat;      // pointer to the original 8bit bitmap
  units[DEVIATOR].bmp_width  = 16*2;
  units[DEVIATOR].bmp_height = 16*2;
  units[DEVIATOR].bmp_startpixel = 0;
  units[DEVIATOR].bmp_frames = 1;
  units[DEVIATOR].icon = ICON_UNIT_DEVIATOR;
  units[DEVIATOR].bullets = BULLET_GAS; // our gassy rocket
  units[DEVIATOR].listId=LIST_UNITS;
  units[DEVIATOR].subListId=SUBLIST_HEAVYFCTRY;
  strcpy(units[DEVIATOR].name, "Deviator");

  // Unit        : Launcher
  // Description : Rocket Launcher
  units[LAUNCHER].bmp = (BITMAP *)gfxdata[UNIT_LAUNCHER].dat;      // pointer to the original 8bit bitmap
  units[LAUNCHER].shadow = (BITMAP *)gfxdata[UNIT_LAUNCHER_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[LAUNCHER].bmp_width  = 16*2;
  units[LAUNCHER].bmp_height = 16*2;
  units[LAUNCHER].bmp_startpixel = 0;
  units[LAUNCHER].bmp_frames = 1;
  units[LAUNCHER].icon = ICON_UNIT_LAUNCHER;
  units[LAUNCHER].second_shot = true;
  //units[LAUNCHER].bullets = ROCKET_NORMAL; // our gassy rocket
  units[LAUNCHER].bullets = ROCKET_NORMAL; // our gassy rocket
  units[LAUNCHER].listId=LIST_UNITS;
  units[LAUNCHER].subListId=SUBLIST_HEAVYFCTRY;
  units[LAUNCHER].canAttackAirUnits=true;
  strcpy(units[LAUNCHER].name, "Launcher");

  // Unit        : Quad
  // Description : Quad, 4 wheeled (double gunned)
  units[QUAD].bmp = (BITMAP *)gfxdata[UNIT_QUAD].dat;      // pointer to the original 8bit bitmap
  units[QUAD].shadow = (BITMAP *)gfxdata[UNIT_QUAD_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[QUAD].bmp_width  = 16*2;
  units[QUAD].bmp_height = 16*2;
  units[QUAD].bmp_startpixel = 0;
  units[QUAD].bmp_frames = 1;
  units[QUAD].icon = ICON_UNIT_QUAD;
  units[QUAD].second_shot = true;
  units[QUAD].bullets = BULLET_QUAD;
  units[QUAD].squish=false;
  units[QUAD].listId=LIST_UNITS;
  units[QUAD].subListId=SUBLIST_LIGHTFCTRY;
  strcpy(units[QUAD].name, "Quad");


  // Unit        : Trike (normal trike)
  // Description : Trike, 3 wheeled (single gunned)
  units[TRIKE].bmp = (BITMAP *)gfxdata[UNIT_TRIKE].dat;      // pointer to the original 8bit bitmap
  units[TRIKE].shadow = (BITMAP *)gfxdata[UNIT_TRIKE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[TRIKE].bmp_width  = 28;
  units[TRIKE].bmp_height = 26;
  units[TRIKE].bmp_startpixel = 0;
  units[TRIKE].bmp_frames = 1;
  units[TRIKE].icon = ICON_UNIT_TRIKE;
  units[TRIKE].bullets = BULLET_TRIKE;
  units[TRIKE].squish=false;
  units[TRIKE].listId=LIST_UNITS;
  units[TRIKE].subListId=SUBLIST_LIGHTFCTRY;
  strcpy(units[TRIKE].name, "Trike");

  // Unit        : Raider Trike (Ordos trike)
  // Description : Raider Trike, 3 wheeled (single gunned), weaker, but faster
  units[RAIDER].bmp = (BITMAP *)gfxdata[UNIT_TRIKE].dat;      // pointer to the original 8bit bitmap
  units[RAIDER].shadow = (BITMAP *)gfxdata[UNIT_TRIKE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[RAIDER].bmp_width  = 14*2;
  units[RAIDER].bmp_height = 14*2;
  units[RAIDER].bmp_startpixel = 0;
  units[RAIDER].bmp_frames = 1;
  strcpy(units[RAIDER].name, "Raider Trike");
  units[RAIDER].icon = ICON_UNIT_RAIDER;
  units[RAIDER].bullets = BULLET_TRIKE;
  units[RAIDER].squish=false;
  units[RAIDER].listId=LIST_UNITS;
  units[RAIDER].subListId=SUBLIST_LIGHTFCTRY;


  // Unit        : Frigate
  // Description : Frigate
  units[FRIGATE].bmp = (BITMAP *)gfxdata[UNIT_FRIGATE].dat;      // pointer to the original 8bit bitmap
  units[FRIGATE].shadow = (BITMAP *)gfxdata[UNIT_FRIGATE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[FRIGATE].bmp_width  = 32*2;
  units[FRIGATE].bmp_height = 32*2;
  units[FRIGATE].bmp_startpixel = 0;
  units[FRIGATE].bmp_frames = 2; // we have at max 1 extra frame
  units[FRIGATE].speed = 2;
  units[FRIGATE].turnspeed = 40;
  units[FRIGATE].airborn = true;
  units[FRIGATE].squish = false;
  units[FRIGATE].free_roam = true; // Frigate does not roam, yet needed?
  units[FRIGATE].hp = 9999;
  // frigate has no list
  strcpy(units[FRIGATE].name, "Frigate");

  /*
  units[FRIGATE].speed     = 0;
  units[FRIGATE].turnspeed = 1;

  units[FRIGATE].sight      = 3;
  units[FRIGATE].hp         = 9999;
  */

  //units[FRIGATE].icon = ICON_FRIGATE;

  // Unit        : Sonic Tank
  // Description : Sonic tank (using sound waves to destroy)
  units[SONICTANK].bmp = (BITMAP *)gfxdata[UNIT_SONICTANK].dat;      // pointer to the original 8bit bitmap
  units[SONICTANK].shadow = (BITMAP *)gfxdata[UNIT_SONICTANK_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[SONICTANK].bmp_width  = 16*2;
  units[SONICTANK].bmp_height = 16*2;
  units[SONICTANK].bmp_startpixel = 0;
  units[SONICTANK].bmp_frames = 1; // no extra frames
  units[SONICTANK].bullets = BULLET_SHIMMER;
  units[SONICTANK].icon = ICON_UNIT_SONICTANK;
  units[SONICTANK].listId=LIST_UNITS;
  units[SONICTANK].subListId=SUBLIST_HEAVYFCTRY;
  strcpy(units[SONICTANK].name, "Sonic Tank");


  // Unit        : Single Soldier
  // Description : 1 soldier
  units[SOLDIER].bmp = (BITMAP *)gfxdata[UNIT_SOLDIER].dat;      // pointer to the original 8bit bitmap
  units[SOLDIER].bmp_width  = 16*2;
  units[SOLDIER].bmp_height = 16*2;
  units[SOLDIER].bmp_startpixel = 0;
  units[SOLDIER].bmp_frames = 3; // 2 extra frames
  units[SOLDIER].infantry=true;
  units[SOLDIER].bullets  = BULLET_SMALL;
  units[SOLDIER].icon       = ICON_UNIT_SOLDIER;
  units[SOLDIER].squish=false;
  units[SOLDIER].listId=LIST_FOOT_UNITS;
  units[SOLDIER].subListId=SUBLIST_INFANTRY;
  strcpy(units[SOLDIER].name, "Soldier");


  // Unit        : Infantry
  // Description : 3 soldiers
  units[INFANTRY].bmp = (BITMAP *)gfxdata[UNIT_SOLDIERS].dat;      // pointer to the original 8bit bitmap
  units[INFANTRY].bmp_width  = 16*2;
  units[INFANTRY].bmp_height = 16*2;
  units[INFANTRY].bmp_startpixel = 0;
  units[INFANTRY].bmp_frames = 3; // 2 extra frames
  units[INFANTRY].speed = 8;
  units[INFANTRY].infantry = true;
  units[INFANTRY].bullets  = BULLET_SMALL;
  units[INFANTRY].second_shot = true;
  units[INFANTRY].icon = ICON_UNIT_INFANTRY;
  units[INFANTRY].squish=false;
  units[INFANTRY].listId=LIST_FOOT_UNITS;
  units[INFANTRY].subListId=SUBLIST_INFANTRY;
  strcpy(units[INFANTRY].name, "Light Infantry");

  // Unit        : Single Trooper
  // Description : 1 trooper
  units[TROOPER].bmp = (BITMAP *)gfxdata[UNIT_TROOPER].dat;      // pointer to the original 8bit bitmap
  units[TROOPER].bmp_width  = 16*2;
  units[TROOPER].bmp_height = 16*2;
  units[TROOPER].bmp_startpixel = 0;
  units[TROOPER].bmp_frames = 3; // 2 extra frames
  strcpy(units[TROOPER].name, "Trooper");
  units[TROOPER].infantry = true;
  units[TROOPER].bullets = ROCKET_SMALL;
  units[TROOPER].icon      = ICON_UNIT_TROOPER;
  units[TROOPER].listId=LIST_FOOT_UNITS;
  units[TROOPER].subListId=SUBLIST_TROOPERS;
  units[TROOPER].squish=false;
  units[TROOPER].canAttackAirUnits=true;

  // Unit        : Group Trooper
  // Description : 3 troopers
  units[TROOPERS].bmp = (BITMAP *)gfxdata[UNIT_TROOPERS].dat;      // pointer to the original 8bit bitmap
  units[TROOPERS].bmp_width  = 16*2;
  units[TROOPERS].bmp_height = 16*2;
  units[TROOPERS].bmp_startpixel = 0;
  units[TROOPERS].bmp_frames = 3; // 2 extra frames
  strcpy(units[TROOPERS].name, "Troopers");
  units[TROOPERS].icon      = ICON_UNIT_TROOPERS;
  units[TROOPERS].bullets = ROCKET_SMALL;
  units[TROOPERS].second_shot = true;
  units[TROOPERS].infantry = true;
  units[TROOPERS].listId=LIST_FOOT_UNITS;
  units[TROOPERS].subListId=SUBLIST_TROOPERS;
  units[TROOPERS].squish=false;
  units[TROOPERS].canAttackAirUnits=true;

  // Unit        : Fremen
  // Description : A single fremen
  units[UNIT_FREMEN_ONE].bmp = (BITMAP *)gfxdata[UNIT_SINGLEFREMEN].dat;      // pointer to the original 8bit bitmap
  units[UNIT_FREMEN_ONE].bmp_width  = 16*2;
  units[UNIT_FREMEN_ONE].bmp_height = 16*2;
  units[UNIT_FREMEN_ONE].bmp_startpixel = 0;
  units[UNIT_FREMEN_ONE].bmp_frames = 3; // 2 extra frames
  strcpy(units[UNIT_FREMEN_ONE].name, "Fremen (1)");
  units[UNIT_FREMEN_ONE].icon      = ICON_SPECIAL_FREMEN;
  units[UNIT_FREMEN_ONE].bullets = ROCKET_SMALL_FREMEN;
  units[UNIT_FREMEN_ONE].second_shot = false;
  units[UNIT_FREMEN_ONE].infantry = true;
  units[UNIT_FREMEN_ONE].squish=false;
  units[UNIT_FREMEN_ONE].canAttackAirUnits=true;
//  units[UNIT_FREMEN_ONE].listId=LIST_PALACE;
//  units[UNIT_FREMEN_ONE].subListId=0;

  // Unit        : Fremen
  // Description : A group of Fremen
  units[UNIT_FREMEN_THREE].bmp = (BITMAP *)gfxdata[UNIT_TRIPLEFREMEN].dat;      // pointer to the original 8bit bitmap
  units[UNIT_FREMEN_THREE].bmp_width  = 16*2;
  units[UNIT_FREMEN_THREE].bmp_height = 16*2;
  units[UNIT_FREMEN_THREE].bmp_startpixel = 0;
  units[UNIT_FREMEN_THREE].bmp_frames = 3; // 2 extra frames
  strcpy(units[UNIT_FREMEN_THREE].name, "Fremen (3)");
  units[UNIT_FREMEN_THREE].icon      = ICON_SPECIAL_FREMEN;
  units[UNIT_FREMEN_THREE].bullets = ROCKET_SMALL_FREMEN;
  units[UNIT_FREMEN_THREE].second_shot = true;
  units[UNIT_FREMEN_THREE].infantry = true;
  units[UNIT_FREMEN_THREE].squish=false;
  units[UNIT_FREMEN_THREE].canAttackAirUnits=true;
//  units[UNIT_FREMEN_THREE].listId=LIST_PALACE;
//  units[UNIT_FREMEN_THREE].subListId=0;

  // Unit        : Saboteur
  // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
  units[SABOTEUR].bmp = (BITMAP *)gfxdata[UNIT_SABOTEUR].dat;
  units[SABOTEUR].build_time = 1000;
  units[SABOTEUR].bmp_width  = 16*2;
  units[SABOTEUR].bmp_height = 16*2;
  units[SABOTEUR].bmp_startpixel = 0;
  units[SABOTEUR].bmp_frames = 3; // 2 extra frames
  units[SABOTEUR].speed = 0; // very fast
  units[SABOTEUR].hp = 60;   // quite some health
  units[SABOTEUR].cost = 0;
  units[SABOTEUR].sight = 3; // immense sight! (sorta scouting guys)
  units[SABOTEUR].range = 2;
  units[SABOTEUR].attack_frequency = 0;
  units[SABOTEUR].turnspeed = 0; // very fast
  strcpy(units[SABOTEUR].name, "Saboteur");
  units[SABOTEUR].infantry = true;
  units[SABOTEUR].icon      = ICON_SPECIAL_SABOTEUR;
  units[SABOTEUR].squish=false;
  units[SABOTEUR].listId=LIST_PALACE;
  units[SABOTEUR].subListId=0;

  // Unit        : Sandworm
  units[SANDWORM].speed = 3; // very fast
  units[SANDWORM].bmp = (BITMAP *)gfxdata[UNIT_SANDWORM].dat;
  units[SANDWORM].hp = 9999;
  units[SANDWORM].bmp_width = 24*2;
  units[SANDWORM].bmp_height = 24*2;
  units[SANDWORM].turnspeed = 0; // very fast
  units[SANDWORM].sight = 16;
  strcpy(units[SANDWORM].name, "Sandworm");
  units[SANDWORM].icon      = ICON_UNIT_SANDWORM;
  units[SANDWORM].squish=false;


  // Unit        : <name>
  // Description : <description>

}

void install_specials() {

    for (int i = 0; i < MAX_SPECIALTYPES; i++) {
        specials[i].icon = -1;
        specials[i].providesType = eBuildType::UNIT;
        specials[i].buildTime = 0;
        specials[i].deployAt = eDeployType::AT_RANDOM_CELL;
        specials[i].deployAtStructureType = -1;
        specials[i].units = 0;
        specials[i].house = eHouseBitFlag::Unknown;
        specials[i].autoBuild = false;
        strcpy(specials[i].description, "\0");
    }

    // Deploy Saboteur
    specials[SPECIAL_SABOTEUR].icon = ICON_SPECIAL_SABOTEUR;
    specials[SPECIAL_SABOTEUR].house=eHouseBitFlag::Ordos;
    specials[SPECIAL_SABOTEUR].autoBuild=true;
    specials[SPECIAL_SABOTEUR].providesType = eBuildType::UNIT;
    specials[SPECIAL_SABOTEUR].providesTypeId = UNIT_SABOTEUR;
    specials[SPECIAL_SABOTEUR].deployAt = eDeployType::AT_STRUCTURE;
    specials[SPECIAL_SABOTEUR].deployAtStructureType = PALACE;
    specials[SPECIAL_SABOTEUR].units = 1;
    specials[SPECIAL_SABOTEUR].buildTime = 10;
    specials[SPECIAL_SABOTEUR].listId=LIST_PALACE;
    specials[SPECIAL_SABOTEUR].subListId=0;
    strcpy(specials[SPECIAL_SABOTEUR].description, "Saboteur");

    // Deploy Fremen
    specials[SPECIAL_FREMEN].icon = ICON_SPECIAL_FREMEN;
    specials[SPECIAL_FREMEN].house=eHouseBitFlag::Atreides;
    specials[SPECIAL_FREMEN].autoBuild=true;
    specials[SPECIAL_FREMEN].providesType = eBuildType::UNIT;
    specials[SPECIAL_FREMEN].providesTypeId = UNIT_FREMEN_THREE;
    specials[SPECIAL_FREMEN].deployAt = eDeployType::AT_RANDOM_CELL;
    specials[SPECIAL_FREMEN].deployAtStructureType = PALACE; // This is not used with AT_RANDOM_CELL ...
    specials[SPECIAL_FREMEN].units = 6; // ... but this is
    specials[SPECIAL_FREMEN].buildTime = 10;
    specials[SPECIAL_FREMEN].listId=LIST_PALACE;
    specials[SPECIAL_FREMEN].subListId=0;
    strcpy(specials[SPECIAL_FREMEN].description, "Fremen");

    // Launch Death Hand
    specials[SPECIAL_DEATHHAND].icon = ICON_SPECIAL_MISSILE;
    specials[SPECIAL_DEATHHAND].house = Harkonnen | Sardaukar;
    specials[SPECIAL_DEATHHAND].autoBuild=true;
    specials[SPECIAL_DEATHHAND].providesType = eBuildType::BULLET;
    specials[SPECIAL_DEATHHAND].providesTypeId = ROCKET_BIG;
    specials[SPECIAL_DEATHHAND].deployAt = eDeployType::AT_STRUCTURE; // the rocket is fired FROM ...
    specials[SPECIAL_DEATHHAND].deployAtStructureType = PALACE; // ... the palace
    specials[SPECIAL_DEATHHAND].units = 1;
    specials[SPECIAL_DEATHHAND].buildTime = 10;
    specials[SPECIAL_DEATHHAND].listId=LIST_PALACE;
    specials[SPECIAL_DEATHHAND].subListId=0;
    strcpy(specials[SPECIAL_DEATHHAND].description, "Death Hand");

}


/****************
 Install bullets
 ****************/
void install_bullets()
{
    logbook("Installing:  BULLET TYPES");

    for (int i=0; i < MAX_BULLET_TYPES; i++)
  {
    bullets[i].bmp = NULL; // in case an invalid bitmap; default is a small rocket
    bullets[i].deadbmp = -1; // this points to a bitmap (in data file, using index)
    bullets[i].damage = 0;      // damage to vehicles
    bullets[i].damage_inf = 0;  // damage to infantry
    bullets[i].max_frames = 1;  // 1 frame animation
    bullets[i].max_deadframes = 4; // 4 frame animation
    bullets[i].bmp_width = 8*2;
    bullets[i].sound = -1;    // no sound
  }

  // huge rocket/missile
  bullets[ROCKET_BIG].bmp = (BITMAP *)gfxdata[BULLET_ROCKET_LARGE].dat;
  bullets[ROCKET_BIG].deadbmp = EXPLOSION_STRUCTURE01;
  bullets[ROCKET_BIG].bmp_width = 48;
  bullets[ROCKET_BIG].damage = 999;
  bullets[ROCKET_BIG].damage_inf = 999;
  bullets[ROCKET_BIG].max_frames = 0;
  bullets[ROCKET_BIG].sound = SOUND_ROCKET;


    // small rocket (for ornithopter)
  bullets[ROCKET_SMALL_ORNI].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_SMALL].dat;
  bullets[ROCKET_SMALL_ORNI].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_SMALL_ORNI].bmp_width = 16;
  bullets[ROCKET_SMALL_ORNI].damage = 12; // they can do pretty damage
  bullets[ROCKET_SMALL_ORNI].damage_inf = 9;
  bullets[ROCKET_SMALL_ORNI].max_frames = 1;
  bullets[ROCKET_SMALL_ORNI].sound = SOUND_ROCKET_SMALL;
  bullets[ROCKET_SMALL_ORNI].max_deadframes = 1;

  // small rocket
  bullets[ROCKET_SMALL].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_SMALL].dat;
  bullets[ROCKET_SMALL].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_SMALL].bmp_width = 16;
  bullets[ROCKET_SMALL].damage = 10; // was 8
  bullets[ROCKET_SMALL].damage_inf = 8; // was 4
  bullets[ROCKET_SMALL].max_frames = 1;
  bullets[ROCKET_SMALL].sound = SOUND_ROCKET_SMALL;
  bullets[ROCKET_SMALL].max_deadframes = 1;

  // small rocket - fremen rocket
  bullets[ROCKET_SMALL_FREMEN].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_SMALL].dat;
  bullets[ROCKET_SMALL_FREMEN].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_SMALL_FREMEN].bmp_width = 16;
  bullets[ROCKET_SMALL_FREMEN].damage = 22;
  bullets[ROCKET_SMALL_FREMEN].damage_inf = 20;
  bullets[ROCKET_SMALL_FREMEN].max_frames = 1;
  bullets[ROCKET_SMALL_FREMEN].sound = SOUND_ROCKET_SMALL;
  bullets[ROCKET_SMALL_FREMEN].max_deadframes = 1;

  // normal rocket
  bullets[ROCKET_NORMAL].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_NORMAL].dat;
  bullets[ROCKET_NORMAL].deadbmp = EXPLOSION_ROCKET;
  bullets[ROCKET_NORMAL].bmp_width = 32;
  bullets[ROCKET_NORMAL].damage = 76;
  bullets[ROCKET_NORMAL].damage_inf = 36;  // less damage on infantry
  bullets[ROCKET_NORMAL].max_frames = 1;
  bullets[ROCKET_NORMAL].sound = SOUND_ROCKET;
  bullets[ROCKET_NORMAL].max_deadframes = 4;

  // soldier shot
  bullets[BULLET_SMALL].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[BULLET_SMALL].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_SMALL].bmp_width = 6;
  bullets[BULLET_SMALL].damage = 4; // vehicles are no match
  bullets[BULLET_SMALL].damage_inf = 10; // infantry vs infantry means big time damage
  bullets[BULLET_SMALL].max_frames = 0;
  bullets[BULLET_SMALL].sound = SOUND_GUN;
  bullets[BULLET_SMALL].max_deadframes = 0;

  // trike shot
  bullets[BULLET_TRIKE].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[BULLET_TRIKE].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_TRIKE].bmp_width = 6;
  bullets[BULLET_TRIKE].damage = 3; // trikes do not do much damage to vehicles
  bullets[BULLET_TRIKE].damage_inf = 6; // but more to infantry
  bullets[BULLET_TRIKE].max_frames = 0;
  bullets[BULLET_TRIKE].sound = SOUND_MACHINEGUN;
  bullets[BULLET_TRIKE].max_deadframes = 0;

  // quad shot
  bullets[BULLET_QUAD].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[BULLET_QUAD].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_QUAD].bmp_width = 6;
  bullets[BULLET_QUAD].damage = 6;
  bullets[BULLET_QUAD].damage_inf = 8; // bigger impact on infantry
  bullets[BULLET_QUAD].max_frames = 0;
  bullets[BULLET_QUAD].sound = SOUND_MACHINEGUN;
  bullets[BULLET_QUAD].max_deadframes = 0;

  // normal tank shot
  bullets[BULLET_TANK].bmp     = (BITMAP *)gfxdata[BULLET_DOT_MEDIUM].dat;
  bullets[BULLET_TANK].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_TANK].bmp_width = 8;
  bullets[BULLET_TANK].damage = 12;
  bullets[BULLET_TANK].damage_inf = 4;  // infantry is not much damaged
  bullets[BULLET_TANK].max_frames = 0;
  bullets[BULLET_TANK].sound = SOUND_EXPL_ROCKET;
  bullets[BULLET_TANK].max_deadframes = 1;

  // siege tank shot
  bullets[BULLET_SIEGE].bmp     = (BITMAP *)gfxdata[BULLET_DOT_MEDIUM].dat;
  bullets[BULLET_SIEGE].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_SIEGE].bmp_width = 8;
  bullets[BULLET_SIEGE].damage = 24;
  bullets[BULLET_SIEGE].damage_inf = 6; // infantry is not as much damaged
  bullets[BULLET_SIEGE].max_frames = 0;
  bullets[BULLET_SIEGE].sound = SOUND_EXPL_ROCKET;
  bullets[BULLET_SIEGE].max_deadframes = 2;

  // devastator shot
  bullets[BULLET_DEVASTATOR].bmp     = (BITMAP *)gfxdata[BULLET_DOT_LARGE].dat;
  bullets[BULLET_DEVASTATOR].deadbmp = EXPLOSION_ROCKET_SMALL; // not used anyway
  bullets[BULLET_DEVASTATOR].bmp_width = 8;
  bullets[BULLET_DEVASTATOR].damage = 30;
  bullets[BULLET_DEVASTATOR].damage_inf = 12; // infantry again not much damaged
  bullets[BULLET_DEVASTATOR].max_frames = 0;
  bullets[BULLET_DEVASTATOR].sound = SOUND_EXPL_ROCKET;
  bullets[BULLET_DEVASTATOR].max_deadframes = 1;

  // Gas rocket of a deviator
  bullets[BULLET_GAS].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_NORMAL].dat;
  bullets[BULLET_GAS].deadbmp = EXPLOSION_GAS;
  bullets[BULLET_GAS].bmp_width = 32;
  bullets[BULLET_GAS].damage = 1;
  bullets[BULLET_GAS].damage_inf = 1;
  bullets[BULLET_GAS].max_frames = 1;
  bullets[BULLET_GAS].max_deadframes = 4;
  bullets[BULLET_GAS].sound = SOUND_ROCKET;

  // normal turret shot
  bullets[BULLET_TURRET].bmp     = (BITMAP *)gfxdata[BULLET_DOT_MEDIUM].dat;
  bullets[BULLET_TURRET].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_TURRET].bmp_width = 8;
  bullets[BULLET_TURRET].damage = 12;
  bullets[BULLET_TURRET].damage_inf = 12; // infantry is a hard target
  bullets[BULLET_TURRET].max_frames = 0;
  bullets[BULLET_TURRET].max_deadframes = 1;
  bullets[BULLET_TURRET].sound = SOUND_GUNTURRET;

  // EXEPTION: Shimmer/ Sonic tank
  bullets[BULLET_SHIMMER].bmp     = NULL;
  bullets[BULLET_SHIMMER].deadbmp = -1;
  bullets[BULLET_SHIMMER].bmp_width = 0;
  bullets[BULLET_SHIMMER].damage = 55;
  bullets[BULLET_SHIMMER].damage_inf = 70; // infantry cant stand the sound, die very fast
  bullets[BULLET_SHIMMER].max_frames = 0;
  bullets[BULLET_SHIMMER].max_deadframes = 0;
  bullets[BULLET_SHIMMER].sound = SOUND_SHIMMER;

  // rocket of rocket turret
  bullets[ROCKET_RTURRET].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_NORMAL].dat;
  bullets[ROCKET_RTURRET].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_RTURRET].bmp_width = 16*2;
  bullets[ROCKET_RTURRET].damage = 25;
  bullets[ROCKET_RTURRET].damage_inf = 10; // infantry is a bit tougher
  bullets[ROCKET_RTURRET].max_frames = 1;
  bullets[ROCKET_RTURRET].sound = SOUND_ROCKET;
  bullets[ROCKET_RTURRET].max_deadframes = 4;

  // SABOTEUR BULLET - FAKE BULLET
  bullets[BULLET_SAB].bmp     = NULL;
  bullets[BULLET_SAB].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[BULLET_SAB].bmp_width = 16*2;
  bullets[BULLET_SAB].damage = 9999;
  bullets[BULLET_SAB].damage_inf = 9999; // infantry is a bit tougher
  bullets[BULLET_SAB].max_frames = 1;
  bullets[BULLET_SAB].sound = -1;
  bullets[BULLET_SAB].max_deadframes = 4;


  /****************** EXPLOSIONS *****************/

  bullets[EXPL_ONE].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_ONE].deadbmp = -1;
  bullets[EXPL_ONE].bmp_width = 16*2;
  bullets[EXPL_ONE].damage = 0;
  bullets[EXPL_ONE].max_frames = 0;
  bullets[EXPL_ONE].max_deadframes = 4;

  // 2 of structure explosions
  bullets[EXPL_TWO].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_TWO].deadbmp = -1;
  bullets[EXPL_TWO].bmp_width = 16*2;
  bullets[EXPL_TWO].damage = 0;
  bullets[EXPL_TWO].max_frames = 0;
  bullets[EXPL_TWO].max_deadframes = 4;

  // Tank explosion #1
  bullets[EXPL_TANK].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_TANK].deadbmp = -1;
  bullets[EXPL_TANK].bmp_width = 24*2;
  bullets[EXPL_TANK].damage = 0;
  bullets[EXPL_TANK].max_frames = 0;
  bullets[EXPL_TANK].max_deadframes = 4;

  // Tank explosion #2
  bullets[EXPL_TANK_TWO].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_TANK_TWO].deadbmp = -1;
  bullets[EXPL_TANK_TWO].bmp_width = 24*2;
  bullets[EXPL_TANK_TWO].damage = 0;
  bullets[EXPL_TANK_TWO].max_frames = 0;
  bullets[EXPL_TANK_TWO].max_deadframes = 4;

}


void install_upgrades() {
    logbook("Installing:  UPGRADES");
    for (int i = 0; i < MAX_UPGRADETYPES; i++) {
        upgrades[i].enabled = false;
        upgrades[i].techLevel = -1;
        upgrades[i].house = 0;
        upgrades[i].needsStructureType = -1;
        upgrades[i].icon = ICON_STR_PALACE;
        upgrades[i].cost = 100;
        upgrades[i].atUpgradeLevel = -1;
        upgrades[i].structureType = CONSTYARD;
        upgrades[i].providesTypeId = -1;
        upgrades[i].providesType = STRUCTURE;
        upgrades[i].providesTypeList = -1;
        upgrades[i].providesTypeSubList = -1;
        upgrades[i].buildTime = 5;
        strcpy(upgrades[i].description, "Upgrade");
    }

    // CONSTYARD UPGRADES

    // First upgrade Constyard: 4Slabs
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].enabled = true;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].house = Atreides | Harkonnen | Ordos;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].techLevel = 4; // start from mission 4
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].icon = ICON_STR_4SLAB;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].cost = structures[CONSTYARD].cost / 2;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].structureType = CONSTYARD;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].atUpgradeLevel = 0;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].providesType = STRUCTURE;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeId = SLAB4;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeList = LIST_CONSTYARD;
    upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].providesTypeSubList = SUBLIST_CONSTYARD;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].buildTime = 50;
    }
    strcpy(upgrades[UPGRADE_TYPE_CONSTYARD_SLAB4].description, "Build 4 concrete slabs at once");

    // Second upgrade Constyard: Rturret
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].enabled = true;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].house = Atreides | Harkonnen | Ordos;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].techLevel = 6;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].icon = ICON_STR_RTURRET;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].cost = structures[CONSTYARD].cost / 2;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].needsStructureType = RADAR;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].structureType = CONSTYARD;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].atUpgradeLevel = 1;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].providesType = STRUCTURE;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeId = RTURRET;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeList = LIST_CONSTYARD;
    upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].providesTypeSubList = SUBLIST_CONSTYARD;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_CONSTYARD_RTURRET].description, "Build Rocket Turret");

    // LIGHTFACTORY UPGRADES, only for ATREIDES and ORDOS
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].enabled = true;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].house = Atreides | Ordos;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].techLevel = 3;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].icon = ICON_UNIT_QUAD;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].cost = structures[LIGHTFACTORY].cost / 2;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].needsStructureType = LIGHTFACTORY;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].structureType = LIGHTFACTORY;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].atUpgradeLevel = 0;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesType = UNIT;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeId = QUAD;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeList = LIST_UNITS;
    upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].providesTypeSubList =  SUBLIST_LIGHTFCTRY;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_LIGHTFCTRY_QUAD].description, "Build Quad at Light Factory");

    // HEAVYFACTORY UPGRADES:

    // ALL HOUSES GET MVC
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].enabled = true;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].house = Atreides | Ordos | Harkonnen;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].techLevel = 4;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].icon = ICON_UNIT_MCV;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].cost = structures[HEAVYFACTORY].cost / 2;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].needsStructureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].structureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].atUpgradeLevel = 0;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesType = UNIT;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeId = MCV;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeList = LIST_UNITS;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_HEAVYFCTRY_MVC].description, "Build MCV at Heavy Factory");

    // Harkonnen/Atreides only
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].enabled = true;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].house = Atreides | Harkonnen;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].techLevel = 5;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].icon = ICON_UNIT_LAUNCHER;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].cost = structures[HEAVYFACTORY].cost / 2;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].needsStructureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].structureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].atUpgradeLevel = 1; // requires MCV upgrade first
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesType = UNIT;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeId = LAUNCHER;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeList = LIST_UNITS;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER].description, "Build Rocket Launcher at Heavy Factory");

    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].enabled = true;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].house = Atreides | Harkonnen;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].techLevel = 6;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].icon = ICON_UNIT_SIEGETANK;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].cost = structures[HEAVYFACTORY].cost / 2;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].needsStructureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].structureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].atUpgradeLevel = 2; // After upgrade to Rocket Launcher
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesType = UNIT;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeId = SIEGETANK;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeList = LIST_UNITS;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK].description, "Build Siege Tank at Heavy Factory");

    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].enabled = true;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].house = Ordos;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].techLevel = 6;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].icon = ICON_UNIT_SIEGETANK;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].cost = structures[HEAVYFACTORY].cost / 2;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].needsStructureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].structureType = HEAVYFACTORY;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].atUpgradeLevel = 1; // After upgrade to MCV
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesType = UNIT;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeId = SIEGETANK;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeList = LIST_UNITS;
    upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].providesTypeSubList = SUBLIST_HEAVYFCTRY;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD].description, "Build Siege Tank at Heavy Factory");

    // HI-TECH UPGRADES (Ordos/Atreides only)
    upgrades[UPGRADE_TYPE_HITECH_ORNI].enabled = true;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].house = Atreides | Ordos;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].techLevel = 8;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].icon = ICON_UNIT_ORNITHOPTER;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].cost = structures[HIGHTECH].cost / 2;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].structureType = HIGHTECH;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].atUpgradeLevel = 0; // After upgrade to MCV
    upgrades[UPGRADE_TYPE_HITECH_ORNI].providesType = UNIT;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].providesTypeId = ORNITHOPTER;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].providesTypeList = LIST_UNITS;
    upgrades[UPGRADE_TYPE_HITECH_ORNI].providesTypeSubList = SUBLIST_HIGHTECH;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_HITECH_ORNI].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_HITECH_ORNI].description, "Build Ornithopter at Hi-Tech");

    // WOR (Harkonnen & Ordos)
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].enabled = true;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].house = Harkonnen | Ordos;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].techLevel = 3;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].icon = ICON_UNIT_TROOPERS;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].cost = structures[WOR].cost / 2;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].structureType = WOR;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].atUpgradeLevel = 0; // After upgrade to MCV
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].providesType = UNIT;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].providesTypeId = TROOPERS;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].providesTypeList = LIST_FOOT_UNITS;
    upgrades[UPGRADE_TYPE_WOR_TROOPERS].providesTypeSubList = SUBLIST_TROOPERS;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_WOR_TROOPERS].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_WOR_TROOPERS].description, "Build Troopers at WOR");

    // BARRACKS (Atreides & Ordos)
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].enabled = true;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].house = Atreides | Ordos;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].techLevel = 3;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].icon = ICON_UNIT_INFANTRY;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].cost = structures[BARRACKS].cost / 2;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].structureType = BARRACKS;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].atUpgradeLevel = 0; // After upgrade to MCV
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].providesType = UNIT;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeId = INFANTRY;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeList = LIST_FOOT_UNITS;
    upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].providesTypeSubList = SUBLIST_INFANTRY;
    if (!DEBUGGING) {
        upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].buildTime = 150;
    }
    strcpy(upgrades[UPGRADE_TYPE_BARRACKS_INFANTRY].description, "Build Infantry at Barracks");

}


/*****************************
 Structure Rules
 *****************************/
void install_structures() {

    logbook("Installing:  STRUCTURES");
  for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
    structures[i].bmp = (BITMAP *)gfxdata[BUILD_WINDTRAP].dat; // in case an invalid bitmap, we are a windtrap
	structures[i].shadow = NULL; // in case an invalid bitmap, we are a windtrap
    structures[i].sight = 1;
    structures[i].bmp_width = 32*2;
    structures[i].bmp_height = 32*2;
    structures[i].sight = 5;
    structures[i].hp = 1; // low health
	structures[i].fixhp = -1; // no fixing hp yet
    structures[i].fadecol = -1;
    structures[i].fademax = -1;
    structures[i].power_drain = 0;       // the power that this building drains...
    structures[i].power_give = 0;        // the power that this building gives...
    structures[i].cost = 0;
    structures[i].icon = -1; // stupid default icon
    structures[i].build_time = 0;
    structures[i].list = -1; // no list attached
    structures[i].queuable = false;
    structures[i].configured = false;
    structures[i].canAttackAirUnits = false;
    strcpy(structures[i].name,   "Unknown");
  }

  // Single and 4 slabs
  structures[SLAB1].bmp = (BITMAP *)gfxdata[PLACE_SLAB1].dat; // in case an invalid bitmap, we are a windtrap
  structures[SLAB1].icon = ICON_STR_1SLAB;
  structures[SLAB1].hp   = 25;            // Not functional in-game, only for building
  structures[SLAB1].bmp_width = 16*2;
  structures[SLAB1].bmp_height = 16*2;
  structures[SLAB1].configured = true;
  structures[SLAB1].queuable = true;
  strcpy(structures[SLAB1].name, "Concrete Slab");

  structures[SLAB4].bmp = (BITMAP *)gfxdata[PLACE_SLAB4].dat; // in case an invalid bitmap, we are a windtrap
  structures[SLAB4].icon = ICON_STR_4SLAB;
  structures[SLAB4].hp   = 75;            // Not functional in-game, only for building
  structures[SLAB4].bmp_width = 32*2;
  structures[SLAB4].bmp_height = 32*2;
  structures[SLAB4].configured = true;
  structures[SLAB4].queuable = true;
  strcpy(structures[SLAB4].name, "4 Concrete Slabs");


  // Concrete Wall
  structures[WALL].bmp = (BITMAP *)gfxdata[PLACE_WALL].dat; // in case an invalid bitmap, we are a windtrap
  structures[WALL].icon = ICON_STR_WALL;
  structures[WALL].hp   = 75;            // Not functional in-game, only for building
  structures[WALL].bmp_width = 16*2;
  structures[WALL].bmp_height = 16*2;
  structures[WALL].queuable = true;
  structures[WALL].configured = true;
  strcpy(structures[WALL].name, "Concrete Wall");

  // Structure    : Windtrap
  // Description  : <none>
  structures[WINDTRAP].bmp = (BITMAP *)gfxdata[BUILD_WINDTRAP].dat;
  structures[WINDTRAP].shadow = (BITMAP *)gfxdata[BUILD_WINDTRAP_SHADOW].dat; // shadow
  structures[WINDTRAP].fadecol = PAN_CENTER;
  structures[WINDTRAP].fademax = 134;
  structures[WINDTRAP].icon  = ICON_STR_WINDTRAP;
  structures[WINDTRAP].configured = true;
  strcpy(structures[WINDTRAP].name, "Windtrap");

  // Structure    : Heavy Factory
  // Description  : <none>
  structures[HEAVYFACTORY].bmp_width = 48*2;
  structures[HEAVYFACTORY].bmp_height = 32*2;
  structures[HEAVYFACTORY].bmp = (BITMAP *)gfxdata[BUILD_HEAVYFACTORY].dat;
  structures[HEAVYFACTORY].shadow = (BITMAP *)gfxdata[BUILD_HEAVYFACTORY_SHADOW].dat; // shadow
  structures[HEAVYFACTORY].fadecol = -1;
  structures[HEAVYFACTORY].icon = ICON_STR_HEAVYFACTORY;
  structures[HEAVYFACTORY].configured = true;
  strcpy(structures[HEAVYFACTORY].name, "Heavy Factory");

  // Structure    : Hight Tech Factory (for aircraft only)
  // Description  : <none>
  structures[HIGHTECH].bmp_width = 48*2;
  structures[HIGHTECH].bmp_height = 32*2;
  structures[HIGHTECH].bmp = (BITMAP *)gfxdata[BUILD_HIGHTECH].dat;
  structures[HIGHTECH].shadow = (BITMAP *)gfxdata[BUILD_HIGHTECH_SHADOW].dat;
  structures[HIGHTECH].fadecol = -1;
  structures[HIGHTECH].icon = ICON_STR_HIGHTECH;
  structures[HIGHTECH].configured = true;
  strcpy(structures[HIGHTECH].name, "Hi-Tech");

  // Structure    : Repair
  // Description  : <none>
  structures[REPAIR].bmp_width = 48*2;
  structures[REPAIR].bmp_height = 32*2;
  structures[REPAIR].bmp = (BITMAP *)gfxdata[BUILD_REPAIR].dat;
  structures[REPAIR].shadow = (BITMAP *)gfxdata[BUILD_REPAIR_SHADOW].dat;
  structures[REPAIR].fadecol = -1;
  structures[REPAIR].icon = ICON_STR_REPAIR;
  structures[REPAIR].configured = true;
  strcpy(structures[REPAIR].name, "Repair Facility");

  // Structure    : Palace
  // Description  : <none>
  structures[PALACE].bmp_width = 48*2;
  structures[PALACE].bmp_height = 48*2;
  structures[PALACE].bmp = (BITMAP *)gfxdata[BUILD_PALACE].dat;
  structures[PALACE].shadow = (BITMAP *)gfxdata[BUILD_PALACE_SHADOW].dat;
  structures[PALACE].icon = ICON_STR_PALACE;
  structures[PALACE].configured = true;
  strcpy(structures[PALACE].name, "Palace");

  // Structure    : Light Factory
  // Description  : <none>
  structures[LIGHTFACTORY].bmp_width = 32*2;
  structures[LIGHTFACTORY].bmp_height = 32*2;
  structures[LIGHTFACTORY].bmp = (BITMAP *)gfxdata[BUILD_LIGHTFACTORY].dat;
  structures[LIGHTFACTORY].shadow = (BITMAP *)gfxdata[BUILD_LIGHTFACTORY_SHADOW].dat;
  structures[LIGHTFACTORY].fadecol = -1;
  structures[LIGHTFACTORY].icon = ICON_STR_LIGHTFACTORY;
  structures[LIGHTFACTORY].configured = true;
  strcpy(structures[LIGHTFACTORY].name, "Light Factory");

  // Structure    : Radar
  // Description  : <none>
  structures[RADAR].bmp_width = 32*2;
  structures[RADAR].bmp_height = 32*2;
  structures[RADAR].bmp = (BITMAP *)gfxdata[BUILD_RADAR].dat;
  structures[RADAR].shadow = (BITMAP *)gfxdata[BUILD_RADAR_SHADOW].dat; // shadow
  structures[RADAR].sight = 12;
  structures[RADAR].fadecol = -1;
  structures[RADAR].icon = ICON_STR_RADAR;
  structures[RADAR].configured = true;
  strcpy(structures[RADAR].name, "Outpost");

  // Structure    : Barracks
  // Description  : <none>
  structures[BARRACKS].bmp_width = 32*2;
  structures[BARRACKS].bmp_height = 32*2;
  structures[BARRACKS].bmp = (BITMAP *)gfxdata[BUILD_BARRACKS].dat;
  structures[BARRACKS].shadow = (BITMAP *)gfxdata[BUILD_BARRACKS_SHADOW].dat;
  structures[BARRACKS].fadecol = -1;
  structures[BARRACKS].icon = ICON_STR_BARRACKS;
  structures[BARRACKS].configured = true;
  strcpy(structures[BARRACKS].name, "Barracks");

  // Structure    : WOR
  // Description  : <none>
  structures[WOR].bmp_width = 32*2;
  structures[WOR].bmp_height = 32*2;
  structures[WOR].bmp = (BITMAP *)gfxdata[BUILD_WOR].dat;
  structures[WOR].shadow = (BITMAP *)gfxdata[BUILD_WOR_SHADOW].dat;
  structures[WOR].fadecol = -1;
  structures[WOR].icon = ICON_STR_WOR;
  structures[WOR].configured = true;
  strcpy(structures[WOR].name, "WOR");


  // Structure    : Silo
  // Description  : <none>
  structures[SILO].bmp_width = 32*2;
  structures[SILO].bmp_height = 32*2;
  structures[SILO].bmp = (BITMAP *)gfxdata[BUILD_SILO].dat;
  structures[SILO].shadow = (BITMAP *)gfxdata[BUILD_SILO_SHADOW].dat;
  structures[SILO].fadecol = -1;
  structures[SILO].icon = ICON_STR_SILO;
  structures[SILO].configured = true;
  structures[SILO].queuable = true;
  strcpy(structures[SILO].name, "Spice Storage Silo");

  // Structure    : Refinery
  // Description  : <none>
  structures[REFINERY].bmp_width = 48*2;
  structures[REFINERY].bmp_height = 32*2;
  structures[REFINERY].bmp = (BITMAP *)gfxdata[BUILD_REFINERY].dat;
  structures[REFINERY].shadow = (BITMAP *)gfxdata[BUILD_REFINERY_SHADOW].dat;
  structures[REFINERY].fadecol = -1;
  structures[REFINERY].icon = ICON_STR_REFINERY;
  structures[REFINERY].configured = true;
  strcpy(structures[REFINERY].name, "Spice Refinery");

  // Structure    : Construction Yard
  // Description  : <none>
  structures[CONSTYARD].bmp_width = 32*2;
  structures[CONSTYARD].bmp_height = 32*2;
  structures[CONSTYARD].sight = 4;
  structures[CONSTYARD].bmp = (BITMAP *)gfxdata[BUILD_CONSTYARD].dat;
  structures[CONSTYARD].fadecol = -1;
  structures[CONSTYARD].icon = ICON_STR_CONSTYARD;
  structures[CONSTYARD].configured = true;
  strcpy(structures[CONSTYARD].name, "Construction Yard");

  // Structure    : Starport
  // Description  : You can order units from this structure
  structures[STARPORT].bmp_width = 48*2;
  structures[STARPORT].bmp_height = 48*2;
  structures[STARPORT].bmp = (BITMAP *)gfxdata[BUILD_STARPORT].dat;
  structures[STARPORT].shadow = (BITMAP *)gfxdata[BUILD_STARPORT_SHADOW].dat;
  structures[STARPORT].fadecol = -1;
  structures[STARPORT].icon  = ICON_STR_STARPORT;
  structures[STARPORT].configured = true;
  strcpy(structures[STARPORT].name, "Starport");

  // Structure    : House of IX
  // Description  : Makes it possible for the player to upgrade its Heavy Factory in order to build their special weapon
  structures[IX].bmp_width = 32*2;
  structures[IX].bmp_height = 32*2;
  structures[IX].bmp = (BITMAP *)gfxdata[BUILD_IX].dat;
  structures[IX].shadow = (BITMAP *)gfxdata[BUILD_IX_SHADOW].dat;
  structures[IX].fadecol = -1;
  structures[IX].icon  = ICON_STR_IX;
  structures[IX].configured = true;
  strcpy(structures[IX].name, "House of IX");

  // Structure    : Normal Turret
  // Description  : defence
  structures[TURRET].bmp_width = 16*2;
  structures[TURRET].bmp_height = 16*2;
  structures[TURRET].bmp = (BITMAP *)gfxdata[BUILD_TURRET].dat;
  structures[TURRET].shadow = (BITMAP *)gfxdata[BUILD_TURRET_SHADOW].dat;
  structures[TURRET].fadecol = -1;
  structures[TURRET].icon  = ICON_STR_TURRET;
  structures[TURRET].sight = 7;
  structures[TURRET].configured = true;
  strcpy(structures[TURRET].name, "Gun Turret");

  // Structure    : Rocket Turret
  // Description  : defence
  structures[RTURRET].bmp_width = 16*2;
  structures[RTURRET].bmp_height = 16*2;
  structures[RTURRET].bmp = (BITMAP *)gfxdata[BUILD_RTURRET].dat;
  structures[RTURRET].shadow = (BITMAP *)gfxdata[BUILD_RTURRET_SHADOW].dat;
  structures[RTURRET].fadecol = -1;
  structures[RTURRET].icon  = ICON_STR_RTURRET;
  structures[RTURRET].sight = 10;
  structures[RTURRET].configured = true;
  structures[RTURRET].canAttackAirUnits = true;
  strcpy(structures[RTURRET].name, "Rocket Turret");

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
  float health = (float)(flHP / flMAX);

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
	cCellCalculator * calculator = new cCellCalculator(&map);
	int result = calculator->findCloseMapBorderCellRelativelyToDestinationCel(iCll);
	delete calculator;
	return result;
}


int distanceBetweenCellAndCenterOfScreen(int iCell) {
    assert(iCell > -1);

    int centerX = mapCamera->getViewportCenterX();
    int centerY = mapCamera->getViewportCenterY();

    int cellX = mapCamera->getAbsoluteXPositionFromCell(iCell);
    int cellY = mapCamera->getAbsoluteYPositionFromCell(iCell);

    return ABS_length(centerX, centerY, cellX, cellY);
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
	if (player[0].getHouse() == HARKONNEN) {
		iType++;
	}

	if (player[0].getHouse() == ORDOS) {
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
	char *data;  // mp3 file in memory
	int len;     // length of file(also the buffer)
	FILE *f = NULL;  // file.mp3

	len = (int)file_size_ex(filename);

	data = new char[len];
	f = fopen(filename, "r");

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
    if (!game.bPlayMusic) return;

    game.iMusicType = iType;

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
      int houseIndex = player[HUMAN].getHouse();
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
          int houseIndex = player[HUMAN].getHouse();
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
 * Creates a bullet, of type, starting at *cell* and moving towards *goal_cell*. The 'unitWhichShoots' or
 * 'structureWhichShoots' is the owner of the bullet.
 * @param type
 * @param cell
 * @param goal_cell
 * @param unitWhichShoots
 * @param structureWhichShoots
 * @return
 */
int create_bullet(int type, int cell, int goal_cell, int unitWhichShoots, int structureWhichShoots) {
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
    newBullet.iCell = cell;
    newBullet.iOwnerStructure = structureWhichShoots;
    newBullet.iOwnerUnit = unitWhichShoots;

    newBullet.iGoalCell = goal_cell;

    newBullet.bAlive = true;
    newBullet.iFrame = 0;

    newBullet.iOffsetX = 8 + rnd(9);
    newBullet.iOffsetY = 8 + rnd(9);

    newBullet.iPlayer = -1;

    if (unitWhichShoots > -1 ) {
        cUnit &cUnit = unit[unitWhichShoots];
        newBullet.iPlayer = cUnit.iPlayer;
        // if an airborn unit shoots (ie Ornithopter), reveal on map
        if (cUnit.isAirbornUnit()) {
            map.clear_spot(cell, cUnit.getUnitType().sight);
        }
    }

    if (structureWhichShoots > -1) {
        cAbstractStructure *pStructure = structure[structureWhichShoots];
        newBullet.iPlayer = pStructure->getOwner();
        map.clear_spot(cell, pStructure->getS_StructuresType().sight);
    }

    if (newBullet.iPlayer < 0) {
        logbook("New bullet produced without any player!?");
    }

    // play sound (when we have one)
    if (bullets[type].sound > -1)
        play_sound_id_with_distance(bullets[type].sound, distanceBetweenCellAndCenterOfScreen(cell));

    return new_id;
}


// Make shimmer
void Shimmer(int r, int x, int y)
{
  // r = radius
  // X, Y = position

  /*
    Logic

    Each X and Y position will be taken and will be switched with a randomly choosen neighbouring
    pixel.

    Shimmer effect is done on BMP_SCREEN only.
  */

  int x1,y1;
  int nx,ny;
  int gp=0;     // Get Pixel Result
  int tc=0;

  // go around 360 fDegrees (twice as fast now)
  for (int dr=0; dr < r; dr+=3)
  {
    for (double d=0; d < 360; d++)
    {
       x1 = (x + (cos(d)*(dr)));
       y1 = (y + (sin(d)*(dr)));

       if (x1 < 0) x1=0;
       if (y1 < 0) y1=0;
       if (x1 >= game.screen_x) x1 = game.screen_x-1;
       if (y1 >= game.screen_y) y1 = game.screen_y-1;

       gp = getpixel(bmp_screen, x1,y1); // use this inline function to speed up things.
       // Now choose random spot to 'switch' with.
       nx=(x1-1) + rnd(2);
       ny=(y1-1) + rnd(2);

       if (nx < 0) nx=0;
       if (ny < 0) ny=0;
       if (nx >= game.screen_x) nx=game.screen_x-1;
       if (ny >= game.screen_y) ny=game.screen_y-1;

       tc = getpixel(bmp_screen, nx,ny);

       if (gp > -1 && tc > -1)
       {
        // Now switch colors
        putpixel(bmp_screen, nx, ny, gp);
        putpixel(bmp_screen, x1, y1, tc);
       }

    }
  }

}

// Skirmish map initialization
void INIT_PREVIEWS()
{
    for (int i=0; i < MAX_SKIRMISHMAPS; i++)
    {
        PreviewMap[i].terrain = NULL;

		// clear out name
        memset(PreviewMap[i].name , 0, sizeof(PreviewMap[i].name));

        // clear out map data
        memset(PreviewMap[i].mapdata, TERRAIN_SAND, sizeof(PreviewMap[i].mapdata));

        //sprintf(PreviewMap[i].name, "Map %d", i);
		//
        PreviewMap[i].iPlayers=0;

        PreviewMap[i].iStartCell[0]=-1;
        PreviewMap[i].iStartCell[1]=-1;
        PreviewMap[i].iStartCell[2]=-1;
        PreviewMap[i].iStartCell[3]=-1;
        PreviewMap[i].iStartCell[4]=-1;
    }

	sprintf(PreviewMap[0].name, "RANDOM MAP");
	//PreviewMap[0].terrain = (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat;
	PreviewMap[0].terrain = create_bitmap(PAN_CENTER, PAN_CENTER);
}

/**
	Function that will go through all pixels and will replace a certain color with another.
    Ie, this can be used to create the fading animation for Windtraps.
**/
void bitmap_replace_color(BITMAP *bmp, int colorToReplace, int newColor) {
	for (int x = 0; x < bmp->w; x++) {
		for (int y = 0; y < bmp->h; y++) {
			if (getpixel(bmp, x, y) == colorToReplace) {
				putpixel(bmp, x, y, newColor);
			}
		}
	}
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
