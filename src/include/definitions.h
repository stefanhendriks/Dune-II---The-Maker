/**
 * @file definitions.h
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

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// #define MOUSECELL_TOPBAR  -1
// #define MOUSECELL_MINIMAP -2
// #define MOUSECELL_SIDEBAR -3

// MAP ID
inline constexpr int MAPID_UNITS = 0;
inline constexpr int MAPID_STRUCTURES = 1;
inline constexpr int MAPID_AIR = 2;
inline constexpr int MAPID_WORMS = 3;
inline constexpr int MAPID_MAX = 4; // used for clearing all

// #define MAX_REGIONS		27			// not more then 27 regions as dune 2 has

// Max length of a path (per unit)
// #define MAX_PATH_SIZE    256

// // INTRO SCENES:
// #define SCENE_NONE		-1
// #define SCENE_DUNE		0
// #define SCENE_ARRAKIS	1		// the planet arrakis... known as dune
// #define SCENE_LANDSAND	2		// land of sand (home of the spice melange)
// #define SCENE_SPICE		3		// the spice controls the empire, whoever controls dune, controls the spice
// #define SCENE_EMPEROR	4		// the emperor has proposed a challange to each of the houses
// #define SCENE_EMPTALK	5		// The House that produces the most spice, whill control dune. There are no set
// // territories and no rules of engagement
// #define SCENE_ARMIES	6		// vast armies have arrived

// // now 3 houses fight, for control of dune (no image, blank)

// #define SCENE_ATREIDES	7		// the noble atreides
// #define SCENE_ORDOS		8		// the insidious ordos
// #define SCENE_HARKONNEN	9		// the evil harkonnen
// #define SCENE_FIRE		10		// only one house will prevail...
// #define SCENE_BEGINS	11		// your battle for dune begins.. NOW!

// Indexes for keeping score
// #define INDEX_KILLS_UNITS   0
// #define INDEX_KILLS_STRUCTURES 1

// VISIBILITY (on a map)
// #define MAP_VISIBLE       0
// #define MAP_FOW           1   // fog of war
// #define MAP_FOG           2   // area is still fogged

// SCROLLING (on a map)
// #define MAP_SCROLLSPEED   TILE_SIZE_PIXELS   // pixels per frames scrolling.

// MEMORY USAGE: (units, structures, etc , more info below)
// #define MAX_UNITS        300     // max of units in the game
inline constexpr int MAX_STRUCTURES = 200; // max of structures in the game
// #define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps

inline constexpr int HUMAN = 0; // 0 = HUMAN			== ALLIES WITH FREMEN WHEN ATREIDES
inline constexpr int AI_CPU1 = 1; // 1 = CPU (Atreides)
inline constexpr int AI_CPU2 = 2; // 2 = CPU (Harkonnen)
inline constexpr int AI_CPU3 = 3; // 3 = CPU (Ordos)
inline constexpr int AI_CPU4 = 4; // 4 = CPU (Sardaukar)	== ALLIES WITH EVERYBODY (EXCEPT PLAYER)
inline constexpr int AI_CPU5 = 5; // 5 = CPU (Fremen)		== ALLIES WITH ATREIDES
inline constexpr int AI_CPU6 = 6; // 6 = WORM
inline constexpr int AI_WORM = AI_CPU6;
inline constexpr int MAX_PLAYERS = 7;

constexpr int MAX_UNIT_GROUPS = 5;

// // UNITS stuff
// #define MAX_UNITTYPES       50    // maximum of unit types

// UPGRADES stuff
//#define MAX_UPGRADETYPES       50    // maximum of upgrades

// SPECIALS (super weapons) stuff
//#define MAX_SPECIALTYPES       10    // maximum of specialInfo

// STRUCTURES stuff
inline constexpr int MAX_STRUCTURETYPES = 50; // max of building types

// Lists stuff
inline constexpr int MAX_ICONS = 30; // max of 30 icons per list
inline constexpr int MAX_ITEMS = MAX_ICONS; // because in theory the item builder should be able to build each icon simultaniously

// House id's
inline constexpr int GENERALHOUSE = 0; // no house at all, general house (gray)
inline constexpr int HARKONNEN = 1;
inline constexpr int ATREIDES = 2;
inline constexpr int ORDOS = 3;
inline constexpr int SARDAUKAR = 4;
inline constexpr int FREMEN = 5;
inline constexpr int MERCENARY = 6;
inline constexpr int CORRINO = 7; // == SARDAUKAR!!!   ---- in this code we use this 'house' for sandworms
inline constexpr int SANDHOUSE = 9;
inline constexpr int GREYHOUSE = 10;

// Structure types
inline constexpr int PALACE = 0; // cPalace
inline constexpr int WINDTRAP = 1; // cWindTrap
inline constexpr int HEAVYFACTORY = 2; // cHeavyFactory
inline constexpr int LIGHTFACTORY = 3; // cLightFactory
inline constexpr int CONSTYARD = 4; // cConstYard
inline constexpr int SILO = 5; // cSpiceSilo
inline constexpr int HIGHTECH = 6; // cHighTech
inline constexpr int IX = 7; // cIX
inline constexpr int REPAIR = 8; // cRepairFacility
inline constexpr int RADAR = 9; // = OUTPOST (cOutPost)
inline constexpr int REFINERY = 10; // cRefinery
inline constexpr int WOR = 11; // cWor
inline constexpr int BARRACKS = 12; // cBarracks
inline constexpr int STARPORT = 13; // cStarPort
inline constexpr int TURRET = 14; // cGunTurret
inline constexpr int RTURRET = 15; // cRocketTurret

// SPECIAL! -> These are not actually structures. When they are done with building the
// cell properties will be changed, this is also done in cStructureFactory::createStructure
inline constexpr int SLAB1 = 24; // 1 single slab
inline constexpr int SLAB4 = 25; // 4 slabs
inline constexpr int WALL = 26; // 1 wall

// Upgrade types
inline constexpr int UPGRADE_TYPE_CONSTYARD_SLAB4 = 0; // 4SLAB upgrade
inline constexpr int UPGRADE_TYPE_CONSTYARD_RTURRET = 1; // Rturret
inline constexpr int UPGRADE_TYPE_LIGHTFCTRY_QUAD = 2; // upgrade LightFctry to get Quad
// Heavy Factory: harkonnen / atreides / ordos
inline constexpr int UPGRADE_TYPE_HEAVYFCTRY_MVC = 3; // upgrade HeavyFctry to get MVC
// Heavy Factory: harkonnen / atreides only
inline constexpr int UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER = 4; // upgrade HeavyFctry to get Rocker Launcher
inline constexpr int UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK = 5; // upgrade HeavyFctry to get Siege Tank
// Heavy Factory: ordos only (ordos can only access Launcher via Starport)
inline constexpr int UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD = 6; // upgrade HeavyFctry to get Siege Tank (later techlevel, after MVC)
// Hi-Tech:
inline constexpr int UPGRADE_TYPE_HITECH_ORNI = 7; // upgrade HiTech to get Ornithopter
// WOR (Harkonnen & Ordos)
inline constexpr int UPGRADE_TYPE_WOR_TROOPERS = 8; // upgrade WOR to get Troopers
// Barracks (Atreides & Ordos)
inline constexpr int UPGRADE_TYPE_BARRACKS_INFANTRY = 9; // upgrade BARRACKS to get Infantry

// Heavy Factory: Sardaukar only
inline constexpr int UPGRADE_TYPE_HEAVYFCTRY_SAR_SONICTANK = 10; // upgrade for Sardaukar to get Sonic Tank
inline constexpr int UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVIATOR = 11; // upgrade for Sardaukar to get Deviator
inline constexpr int UPGRADE_TYPE_HEAVYFCTRY_SAR_DEVASTATOR = 12; // upgrade for Sardaukar to get Devastator

inline constexpr int MUSIC_WIN = 0;
inline constexpr int MUSIC_LOSE = 1;
inline constexpr int MUSIC_ATTACK = 2;
inline constexpr int MUSIC_PEACE = 3;
inline constexpr int MUSIC_MENU = 4;
inline constexpr int MUSIC_CONQUEST = 5;
inline constexpr int MUSIC_BRIEFING = 6;

// Unit types
inline constexpr int SOLDIER = 0;
inline constexpr int TROOPER = 1;
inline constexpr int INFANTRY = 2;
inline constexpr int TROOPERS = 3;
inline constexpr int TRIKE = 4;
inline constexpr int RAIDER = 5;
inline constexpr int QUAD = 6;
inline constexpr int TANK = 7;
inline constexpr int LAUNCHER = 8;
inline constexpr int SIEGETANK = 9;
inline constexpr int DEVASTATOR = 10;
inline constexpr int DEVIATOR = 11;
inline constexpr int SONICTANK = 12;
inline constexpr int HARVESTER = 13;
inline constexpr int MCV = 14; // Start with 0
inline constexpr int CARRYALL = 15; // air unit
inline constexpr int ORNITHOPTER = 16; // air unit
inline constexpr int SANDWORM = 17;

// special units
inline constexpr int SABOTEUR = 19;
inline constexpr int UNIT_FREMEN_ONE = 20;
inline constexpr int UNIT_FREMEN_THREE = 21;

// Frigate
inline constexpr int FRIGATE = 29; // air unit (should be last!!)

// Specials (super weapons)
inline constexpr int SPECIAL_SABOTEUR = 0;
inline constexpr int SPECIAL_FREMEN = 1; // special unit(s) for fremen support
inline constexpr int SPECIAL_DEATHHAND = 2;

// Bullet types
inline constexpr int ROCKET_SMALL = 0; // small rocket (trooper/troopers)
inline constexpr int ROCKET_NORMAL = 1; // normal rocket (turret/launcher/deviator)
inline constexpr int ROCKET_BIG = 2; // big rocket (palace, nuclear/deathhand/missile)
inline constexpr int BULLET_SMALL = 3; // infantry bullet
inline constexpr int BULLET_TRIKE = 4; // a trike bullet, looks like infantry but is stronger
inline constexpr int BULLET_QUAD = 5; // a quad bullet
inline constexpr int BULLET_TANK = 6; // a normal tank dot
inline constexpr int BULLET_SIEGE = 7; // a siege tank dot
inline constexpr int BULLET_DEVASTATOR = 8; // a devastator dot
inline constexpr int BULLET_GAS = 9; // a deviator shot
inline constexpr int BULLET_TURRET = 10; // a normal turret dot
inline constexpr int BULLET_SHIMMER = 11; // EXEPTION: Shimmer effect!
inline constexpr int ROCKET_SMALL_ORNI = 12; // small rocket ornithopter
inline constexpr int ROCKET_RTURRET = 13; // rocket turrets have weaker turrets!
inline constexpr int BULLET_SAB = 14; // a fake bullet , to make it easier to blow up stuff
inline constexpr int ROCKET_SMALL_FREMEN = 15; // small rocket (trooper/troopers) - FOR FREMEN

// Particle types
const int D2TM_PARTICLE_MOVE	                = 1; // particle used for indicating when move order has been received
const int D2TM_PARTICLE_ATTACK                  = 2; // particle used for indicating when attack order has been received
const int D2TM_PARTICLE_EXPLOSION_TRIKE         = 3;
const int D2TM_PARTICLE_SMOKE                   = 4;
const int D2TM_PARTICLE_SMOKE_SHADOW            = 5;
const int D2TM_PARTICLE_TRACK_DIA               = 6;
const int D2TM_PARTICLE_TRACK_HOR               = 7;
const int D2TM_PARTICLE_TRACK_VER               = 8;
const int D2TM_PARTICLE_TRACK_DIA2              = 9;
const int D2TM_PARTICLE_BULLET_PUF              = 10;
const int D2TM_PARTICLE_EXPLOSION_FIRE          = 11;
const int D2TM_PARTICLE_WORMEAT                 = 12;
const int D2TM_PARTICLE_EXPLOSION_TANK_ONE      = 13;
const int D2TM_PARTICLE_EXPLOSION_TANK_TWO      = 14;
const int D2TM_PARTICLE_EXPLOSION_STRUCTURE01   = 15;
const int D2TM_PARTICLE_EXPLOSION_STRUCTURE02   = 16;
const int D2TM_PARTICLE_EXPLOSION_GAS           = 17;
const int D2TM_PARTICLE_WORMTRAIL               = 18;
const int D2TM_PARTICLE_DEADINF01               = 19;
const int D2TM_PARTICLE_DEADINF02               = 20;
const int D2TM_PARTICLE_TANKSHOOT               = 21;
const int D2TM_PARTICLE_SIEGESHOOT              = 22;
const int D2TM_PARTICLE_SQUISH01                = 23;
const int D2TM_PARTICLE_SQUISH02                = 24;
const int D2TM_PARTICLE_SQUISH03                = 25;
const int D2TM_PARTICLE_EXPLOSION_ORNI          = 26;
const int D2TM_PARTICLE_SIEGEDIE                = 27;
const int D2TM_PARTICLE_CARRYPUFF               = 28;
const int D2TM_PARTICLE_EXPLOSION_ROCKET        = 29;
const int D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL  = 30;
const int D2TM_PARTICLE_OBJECT_BOOM01           = 31;
const int D2TM_PARTICLE_OBJECT_BOOM02           = 32;
const int D2TM_PARTICLE_OBJECT_BOOM03           = 33;
const int D2TM_PARTICLE_EXPLOSION_BULLET        = 34;
const int D2TM_PARTICLE_SMOKE_WITH_SHADOW       = 35;
const int D2TM_PARTICLE_EXPLOSION_CARRYALL      = 36;

// Particle drawing layers
// const int D2TM_RENDER_LAYER_PARTICLE_TOP        = 0;
// const int D2TM_RENDER_LAYER_PARTICLE_BOTTOM     = 1;

// smudge
// #define SMUDGE_ROCK 0
// #define SMUDGE_SAND 1
// #define SMUDGE_WALL 2

// Win/lose bit flags, borrowed from Dune Legacy's implementation
// #define WINLOSEFLAGS_AI_NO_BUILDINGS        0x01
// #define WINLOSEFLAGS_HUMAN_HAS_BUILDINGS    0x02
// #define WINLOSEFLAGS_QUOTA                  0x04
// #define WINLOSEFLAGS_TIMEOUT                0x08


/**
 * This is the palette index (in the 8 bit palette of D2TM/Dune 2) that is meant to be rendered as a 'shadow'
 */
constexpr int TransparentColorIndex = 223;

/**
 * The transparency (value between 0-255) when rendering a bitmap/texture with transparency
 */
constexpr int ShadowTrans = 160;

#endif /* DEFINITIONS_H_ */
