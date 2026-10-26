/**
 * @file iniWords.h
 *
 * INI word IDs used across the parser sections.
 */

#pragma once

enum class eIniWord {
    NONE = -1,
    MAPHEIGHT,
    MAPWIDTH,
    DESCRIPTION,
    PREBUILD,
    POWER,
    MODID,
    TITLE,
    VERSION,
    DIR,
    TITLEBITMAP,
    MENUCLICKSOUND,
    MOUSENORMAL,
    MOUSEMOVE,
    MOUSEATTACK,
    MOUSEDEPLOY,
    MOUSENOMOVE,
    RED,
    GREEN,
    BLUE,
    BITMAP,
    BITMAP_WIDTH,
    BITMAP_HEIGHT,
    BITMAP_FRAMES,
    BITMAP_DEADFRAMES,
    DAMAGE_VEHICLE,
    DEFINITION,
    BITMAP_DEAD,
    BITMAP_TOP,
    ICON,
    HITPOINTS,
    COST,
    BULLETTYPE,
    ATTACKFREQ,  // (attack freq-uency)
    SIGHT,  // (sight)
    RANGE,  // (range)
    BUILDTIME,  // (build time for unit)
    MOVESPEED,  // (move speed)
    TURNSPEED,  // (turn speed)
    SOUND,  // (sound)
    ISHARVESTER,  // (IsHarvester, is it a harvester or not?)
    FIRETWICE,  // (SecondShot, makes unit shoot twice?)
    ISAIRBORN,  // (Unit is airborn?)
    ABLETOCARRY,  // (Able to transport units, aka CarryAll)
    FREEROAM,  // (Will freely roam around the map?)
    ISINFANTRY,  // (Act as infantry?)
    HARVESTAMOUNT,  // (How much a harvester takes every step)
    HARVESTSPEED,  // (How many ticks needed per step)
    HARVESTLIMIT,  // (Max a harvester can hold)
    NEXTATTACKFREQ,  // (next attack frequency)
    PRODUCER,  // (Producing structure of unit)
    ISSQUISHABLE,  // (Is unit squishable?)
    CANSQUISH,  // (Can squish unit?)
    APPETITE,  // (How much appetite has a sandworm?)
    SMOKEHFACTOR,  // (Smoke HP Factor)
    SQUISH,  // (Can squish infantry?)
    CANATTACKAIRUNITS,  // (Can attack air units?)
    CANATTACKUNITS,  // (Can attack ground units?)
    DAMAGE_INFANTRY,  // (Damage to infantry)
    EXPLOSION_SIZE,  // (Explosion size)
    DEVIATE_PROBABILITY,  // (Deviation probability)
    GROUND_BULLET,  // (blocked by mountains, walls, structures)
    UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MIN,  // (structures, how many units to spawn if destroyed? (min))
    UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MAX,  // (structures, how many units to spawn if destroyed? (max))
    UPON_DESTRUCTION_SPAWN_UNIT_TYPE,  // (structures, which type of unit to spawn if destroyed?)
    ATTACK_IS_ENTER_STRUCTURE,  // (unit attacks by entering a structure?)
    DAMAGE_ON_ENTER_STRUCTURE,  // (damage dealt to structure on entry)
    BLOOMTIMERDURATION,  // BLOOM TIMER DURATION
    TERRAIN_MINSPICE,  // minimal spice on SPICETERRAIN
    TERRAIN_MAXSPICE,  // maximal spice on SPICETERRAIN
    TERRAINHILL_MINSPICE,  // minimal spice on SPICEHILL
    TERRAINHILL_MAXSPICE,  // maximal spice on SPICEHILL
    TERRAINWALL_HP,  // HP of TERRAINWALL
    HOUSE,
    FOCUS_POS,
    CREDITS,
    MAPSEED,
    MAPBLOOM,  // scen.ini (spice blooms for DUNE 2 maps)
    MAPFIELD,  // scen.ini (spice fields for DUNE 2 maps)
    QUOTA,
    WINFLAGS,
    LOSEFLAGS,
    BRIEFPICTURE,
    BRAIN,
    TEAM,
    SKILL,
    FIXHP,  // fix hp
    POWERDRAIN,  // power
    POWERGIVE,  // power
    HAS_CONCRETE,  // has concrete
    FIREPOWER,
    FIRERATE,
    STRUCTPRICE,
    UNITPRICE,
    SPEED,
    BUILDSPEED,
    DUMPSPEED,
    REGION,
    REGIONHOUSE,
    REGIONTEXT,
    REGIONSELECT,
    REGIONCONQUER,
    NUMBER,
    MAPNAME,
    STARTCELL,
};

constexpr int MAX_LINE_LENGTH = 256;
