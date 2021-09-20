/*
 * structs.h
 *
 *  Created on: 7-nov-2010
 *      Author: Stefan
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <vector>
#include "enums.h"

// Unit properties
// the s_UnitP struct is holding all properties for a specific unit type.
// note: The unit properties are actually set in common.cpp , like the houses!
// note: Some values are now read via INI.CPP (GAME.INI reader).
struct s_UnitP {
  BITMAP  *bmp;         // each unit has a reference to his 8 bit bitmap.
  BITMAP  *top;         // top of unit
  BITMAP  *shadow;      // shadow of unit (24 bit)

  int    bmp_width;     // width of each unit frame (in pixels).
  int    bmp_height;    // Height of the unit, for each animating frame.

  int    bmp_frames;

  int    bmp_startpixel; // The X axis starting pixel (i.e. for the harvester)

  int    hp;             // Amount of hitpoints at maximum
  int    speed;          // speed (0 being super fast, the higher the number, the slower).
  int    turnspeed;
  int    attack_frequency;  // timer for attacking

  int    build_time;        // how long it takes for building this thing

  int    bulletType;           // type of bullet
  int    max_bullets;       // Maximum of 'bullets' it can fire at a time

  int    sight;             // Sight of a unit (in radius)
  int    range;             // Range of a unit (in radius)

  int    icon;              // icon id (taken from gfxInter data file)

  int    cost;              // cost/price

  bool airborn;             // does this unit fly?
  bool infantry;            // legged unit? (infantry)
  bool free_roam;           // (airborn only) - does it free roam?

  bool fireTwice;         // can shoot 2 times?

  bool squish;              // can this unit squish infantry?
  bool canBeSquished;       // can this unit be squished (is this a squishable unit)

  char name[64];            // name of unit

  int structureTypeItLeavesFrom; // the type of structure this unit will 'leave' from when it is built. (ie Quad from Light Factory, etc)

  bool queuable;        // can this item be queued in the buildList? (default = true for units)

  // UNIT : HARVESTER specific types
  int credit_capacity;      // max credits capacity
  int harvesting_speed;     // speed rate for harvesting (time)
  int harvesting_amount;    // value rate for harvesting (amount)

  // house specific rates

  // list related
  int listId;               // ie LIST_FOOT_UNITS or LIST_UNITS
  int subListId;            // ie SUBLIST_LIGHTFCTRY

  // attack related
  bool canAttackAirUnits;   // ie for rocket typed units

  bool canEnterAndDamageStructure;  // can this unit enter a structure and damage it? (and eventually capture?)
  bool attackIsEnterStructure;      // for saboteur only really
  float damageOnEnterStructure;     // the damage inflicted to a structure when entered

};

// Structure types
struct s_Structures {
  // Structures
  BITMAP *bmp;         // a pointer to the 8 bit bitmap
  BITMAP *shadow;	   // a pointer to the shadow bitmap

  int bmp_width;       // width structure (in pixels)
  int bmp_height;      // height structure (in pixels)

  int list;            // list this structure has!

  int hp;              // max hp
  int fixhp;	   	   // how much you get for fixing (reparing) this thing

  int bullets;         // type of bullet

  int build_time;      // how long it takes for building this structure


  int power_drain;       // the power that this building drains...
  int power_give;        // the power that this building gives...

  int sight;

  int fadecol;         // Fading color (if needed), -1 is none, > -1 means starting color
  int fademax;         // Max fading color (assuming fadecol < fademax)

  int icon;            // icon id

  int cost;            // price

  char name[64];       // name

  bool queuable;        // can this item be queued in the buildList? (default = false)

  bool configured;     // is this structure configured? (poor man solution)

  bool canAttackAirUnits; // for turrets
};

/**
 * Upgrades are tied to structures (ie this is used to upgrade a structure type). After
 * the upgrade is completed, the result is always an increased upgrade count.
 *
 * Then, also, this struct provides which item and what kind of item will be made available.
 */
struct s_Upgrade {
    bool enabled;        // set to true to use this upgrade logic

    int icon;            // icon id

    int cost;            // price

    char description[64]; // ie: "Upgrade to 4slab"

    int buildTime;     // how long it takes to upgrade/build

    unsigned char house; // 8-bit-flag for which house this upgrade applies.

    int techLevel;      // the minimum techlevel required for this upgrade
    int atUpgradeLevel; // linear upgradeLevel per structure type, this is the nr where this upgrade is offered. (0 = start)
    int structureType;  // if > -1 then increase upgradeLevel for structureType (this is the structureType being 'upgraded')
    int needsStructureType; // the upgrade is only available when this structure is available, this is additional to the structureType property
                        // above, so if you require CONSTYARD *and* RADAR you set structureType=CONSTYARD and needsStructure=RADAR, keep -1
                        // if you don't require any additional structure

    eBuildType providesType;    // UNIT or STRUCTURE, etc

    int providesTypeId;   // upgrade results into getting this typeId (type depends on 'providesType') (points to s_Unit/s_Structure)
    int providesTypeList; // into which list will this type be made available?
    int providesTypeSubList; // and sublist

};

/**
 * Upgrades are tied to structures (ie this is used to upgrade a structure type). After
 * the upgrade is completed, the result is always an increased upgrade count.
 *
 * Then, also, this struct provides which item and what kind of item will be made available.
 */
struct s_Special {
    int icon;            // icon id

    char description[64]; // ie: "Upgrade to 4slab"

    int buildTime;     // how long it takes to 'build' (ie wait before ready)

    eBuildType providesType;
    int providesTypeId;   // slot of type it points to (typeId)

    int units;   // amount of units to spawn at once

    eDeployFromType deployFrom; // for Fremen, deploys unit to random cell

    eDeployTargetType deployTargetType; // for ie DeathHand, how 'accurate' the deploy target will become

    /**
     * how accurate (for deployTargetType == INACCURATE), ie the amount of cells it can be off.
     * A 0 means 100% accurate (ie nothing to fudge), 1 means 1 cell off, etc.
     */
    int deployTargetPrecision;

    unsigned char house; // which house(s) have access to this?

    bool autoBuild; // if true, then this item is built automatically

    int deployAtStructure; // if deployFrom == STRUCTURE, then this is the structure type to deploy at

    int listId;
    int subListId;
};

// House properties
struct s_House {
  int swap_color;           // color index to start swapping with.
  int minimap_color;        // rgb value on minimap
};


struct s_Particle {
    BITMAP *bmp;         // a pointer to a BITMAP to draw
};

struct s_Bullet {
    BITMAP *bmp;        // a reference to its bitmap. (16 bits here!)
    int deadbmp;        // when the bullet dies, it will use this bmp to show its animation
    int bmp_width;      // how much pixels a bullet is (wide)

    int damage;         // how much damage it does to vehicles
    int damage_inf;     // how much damage it does to infantry
    int explosionSize;  // square like explosion, defaults 1 (=1x1 tile), 2 means 2x2, etc.
    int smokeParticle;   // spawn smoke particles while traveling? (if > -1)
    int max_frames;     // when > 0 it animates automatically so many frames. If you want to flicker between frame 0 and 1, max_frames should be 1
    int max_deadframes; // max dead frames

    int sound;          // Sound produced when 'shot' (or explosion sound)

    int deviateProbability;   // how high probability it deviates a unit (changes sides) (0 = no chance, 100 = 100% chance)

    bool groundBullet;   // if true, then it gets blocked by walls, mountains or structures. False == flying bullets, ie, rockets
    bool canDamageAirUnits;   // if true, then upon impact the bullet can also damage air units

    char description[64]; // ie: "bullet"

    bool canDamageGround; // when true, this bullet can damage ground (ie walls, concrete, etc)
};

// SKIRMISH MAP PREVIEW DATA
struct s_PreviewMap {
     BITMAP *terrain;        // terrain
     int width, height;      // width & height of map
     std::vector<int> mapdata;
     int iPlayers;           // players
     int iStartCell[5];      // starting locations
     char name[80];          // name
};

#endif /* STRUCTS_H_ */
