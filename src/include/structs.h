/*
 * structs.h
 *
 *  Created on: 7-nov-2010
 *      Author: Stefan
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_


// Unit properties
// note: This is NOT the unit class, this is simply a 'prefix' for a unit!
// note: The unit properties are actually set in common.cpp , like the houses!
// note: Some values are now read via INI.CPP (GAME.INI reader).
struct s_UnitP
{
  BITMAP  *bmp;         // each unit has a reference to his 8 bit bitmap.
  BITMAP  *top;         // top of unit
  BITMAP  *shadow;      // shadow of unit (24 bit)

  int    bmp_width;     // width of each unit frame (in pixels).
  int    bmp_height;    // Height of the unit, for each animating frame.

  int    bmp_frames;

  int    bmp_startpixel; // The X axis starting pixel (i.e. for the harvester)

  int    hp;             // Amount of hitpoints at maximum
  int    speed;          // speed (0 being uberly slow, 1000 being super fast).
  int    turnspeed;
  int    attack_frequency;  // timer for attacking

  int    build_time;        // how long it takes for building this thing

  int    bullets;           // type of bullet
  int    max_bullets;       // Maximum of 'bullets' it can fire at a time

  int    type;              // type of unit,

  int    sight;             // Sight of a unit (in radius)
  int    range;             // Range of a unit (in radius)

  int    icon;              // icon id

  int    cost;              // cost/price

  bool airborn;             // does this unit fly?
  bool infantry;            // legged unit? (infantry)
  bool free_roam;           // (airborn only) - does it free roam?

  bool second_shot;         // can shoot 2 times?

  bool squish;              // can this unit squish infantry?

  char name[50];            // name of unit

  int structureTypeItLeavesFrom; // the type of structure this unit will 'leave' from when it is built. (ie Quad from Light Factory, etc)

  // UNIT : HARVESTER specific types
  int credit_capacity;      // max credits capacity
  int harvesting_speed;     // speed rate for harvesting (time)
  int harvesting_amount;    // value rate for harvesting (amount)
  // house specific rates
};


// Structure types
struct s_Structures
{
  // Structures
  BITMAP *bmp;         // a reference to his 8 bit bitmap. (id in graphics)
  BITMAP *shadow;	   // shadow

  int bmp_width;
  int bmp_height;

  int list;            // list this structure has!

  int hp;              // max hp
  int fixhp;	   	   // how much you get for fixing this thing
  int bullets;         // type of bullet

  int build_time;      // how long it takes for building this thing


  int    power_drain;       // the power that this building drains...
  int    power_give;        // the power that this building gives...

  int sight;

  int fadecol;         // Fading color (if needed), -1 is none, > -1 means starting color
  int fademax;         // Max fading color (assuming fadecol < fademax)

  int icon;            // icon id

  int cost;            // price of this thing

  char name[64];       // name
};

// House properties
struct s_House
{
  int swap_color;           // color index to start swapping with.
  int minimap_color;        // rgb value on minimap
};


struct s_Bullet
{

  BITMAP *bmp;            // a reference to its bitmap. (16 bits here!)
  int deadbmp;        // when the bullet dies, it will use this bmp to show its animation
  int bmp_width;      // how much pixels a bullet is (wide)

  int damage;         // how much damage it does -> vehicles
  int damage_inf;     // how much damage it does -> infantry
  int max_frames;     // when > 0 it animates automaticly
  int max_deadframes; // max dead frames

  int sound;          // Sound produced when 'shot' (or explosion sound)

};

// SKIRMISH MAP PREVIEW DATA
struct sPreviewMap {
     BITMAP *terrain;
     int terrainType[MAX_CELLS];
     int iPlayers;           // players
     int iStartCell[5];      // starting locations
     char name[80];          // name
};

// SETTINGS
struct sGameSettings {
    int iIdealFPS;          // ideal fps to reach
    bool bUseRest;          // use Rest technique
    int iScollSpeed;        // scroll speed
    bool bWindowed;         // launch default as windowed or not?
};

#endif /* STRUCTS_H_ */
