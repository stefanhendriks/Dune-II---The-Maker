#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define MOUSECELL_TOPBAR  -1
#define MOUSECELL_MINIMAP -2
#define MOUSECELL_SIDEBAR -3

#define IDEAL_FPS	60

// MAP ID
#define MAPID_UNITS  0
#define MAPID_STRUCTURES 1
#define MAPID_AIR   2
#define MAPID_WORMS  3
#define MAPID_MAX	 4 // used for clearing all

// Game modes
#define MOD_DUNE2		0
#define MOD_D2TM		1
//#define MOD_MINE		2		// own made mod

#define MAX_REGIONS		27			// not more then 27 regions as dune 2 has

// Font size
#define GAME_FONTSIZE 8

// Max length of a path (per unit)
#define MAX_PATH_SIZE    256

// INTRO SCENES:
#define SCENE_NONE		-1
#define SCENE_DUNE		0
#define SCENE_ARRAKIS	1		// the planet arrakis... known as dune
#define SCENE_LANDSAND	2		// land of sand (home of the spice melange)
#define SCENE_SPICE		3		// the spice controls the empire, whoever controls dune, controls the spice
#define SCENE_EMPEROR	4		// the emperor has proposed a challange to each of the houses
#define SCENE_EMPTALK	5		// The House that produces the most spice, whill control dune. There are no set
								// territories and no rules of engagement
#define SCENE_ARMIES	6		// vast armies have arrived

// now 3 houses fight, for control of dune (no image, blank)

#define SCENE_ATREIDES	7		// the noble atreides
#define SCENE_ORDOS		8		// the insidious ordos
#define SCENE_HARKONNEN	9		// the evil harkonnen
#define SCENE_FIRE		10		// only one house will prevail...
#define SCENE_BEGINS	11		// your battle for dune begins.. NOW!

// Indexes for keeping score
#define INDEX_KILLS_UNITS   0
#define INDEX_KILLS_STRUCTURES 1

// VISIBILITY (on a map)
#define MAP_VISIBLE       0
#define MAP_FOW           1   // fog of war
#define MAP_FOG           2   // area is still fogged

// SCROLLING (on a map)
#define MAP_SCROLLSPEED   TILE_SIZE_PIXELS   // pixels per frames scrolling.

// MEMORY USAGE: (units, structures, etc , more info below)
#define MAX_UNITS        300     // max of units in the game
#define MAX_STRUCTURES   200     // max of structures in the game
#define MAX_BULLETS      300     // max of bullets in the game at once
#define MAX_PARTICLES    600	 // max of particles in the game at once
#define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps
//#define HOUSE_SLOTS       10      // amount of House 'types'

// Game states (state machine)
#define GAME_INITIALIZE  -1      // initialize game
#define GAME_OVER         0      // game over
#define GAME_MENU         1      // in a menu
#define GAME_PLAYING      2      // playing the actual game
#define GAME_BRIEFING     3      // doing some briefing
#define GAME_EDITING      4      // running the editor
#define GAME_OPTIONS	  5		 // options menu
#define GAME_REGION		  6		 // select a region / select your next conquest
#define GAME_SELECT_HOUSE 8		 // pick a house
#define GAME_INTRO	      9
#define GAME_TELLHOUSE	 10		 // tell about the house
#define GAME_WINNING     11      // when winning, do some cool animation
#define GAME_WINBRIEF    12      // mentat chatter when won the mission
#define GAME_LOSEBRIEF   13      // briefing when losing
#define GAME_LOSING      14      // when losing, do something cool
#define GAME_SKIRMISH    15      // playing a skirmish mission!
#define GAME_SETUPSKIRMISH 16    // set up a skirmish game
#define GAME_CREDITS     17    // credits
#define GAME_MISSIONSELECT  18    // mission select
#define GAME_MAX_STATES 19

// FOR OPTIONS:

// GAME (MENU) FLAGS: (used when GAME_MENU)
// Game menu state machine

#define HUMAN			0  // 0 = HUMAN			== ALLIES WITH FREMEN WHEN ATREIDES
#define AI_CPU1         1  // 1 = CPU (Atreides)
#define AI_CPU2         2  // 2 = CPU (Harkonnen)
#define AI_CPU3         3  // 3 = CPU (Ordos)
#define AI_CPU4         4  // 4 = CPU (Sardaukar)	== ALLIES WITH EVERYBODY (EXCEPT PLAYER)
#define AI_CPU5         5  // 5 = CPU (Fremen)		== ALLIES WITH ATREIDES
#define AI_CPU6         6  // 6 = WORM
#define AI_WORM			AI_CPU6
#define MAX_PLAYERS       7

// HOUSES stuff
// #define MAX_HOUSES        12      // 12 different type of houses

// UNITS stuff
#define MAX_UNITTYPES       50    // maximum of unit types

// UPGRADES stuff
#define MAX_UPGRADETYPES       50    // maximum of upgrades

// SPECIALS (super weapons) stuff
#define MAX_SPECIALTYPES       10    // maximum of specialInfo

// STRUCTURES stuff
#define MAX_STRUCTURETYPES  50    // max of building types

// BULLETS stuff
#define MAX_BULLET_TYPES      30

// PARTICLES
#define MAX_PARTICLE_TYPES    50

// Lists stuff
#define MAX_ICONS       30          // max of 30 icons per list
#define MAX_ITEMS		MAX_ICONS   // because in theory the item builder should be able to build each icon simultaniously

// #TIMERS
#define TIMER_GUARD         10
#define TIMER_UNIT_BULLET   5

// FACING
#define FACE_UP           0
#define FACE_UPRIGHT      1
#define FACE_RIGHT        2
#define FACE_DOWNRIGHT    3
#define FACE_DOWN         4
#define FACE_DOWNLEFT     5
#define FACE_LEFT         6
#define FACE_UPLEFT       7

// House id's
#define GENERALHOUSE   0     // no house at all, general house (gray)
#define HARKONNEN 1
#define ATREIDES  2
#define ORDOS     3
#define SARDAUKAR 4
#define FREMEN 	  5
#define MERCENARY 6
#define CORRINO   7         // == SARDAUKAR!!!   ---- in this code we use this 'house' for sandworms
#define SANDHOUSE 9
#define GREYHOUSE	10

// Structure types
#define PALACE            0         // cPalace
#define WINDTRAP          1         // cWindTrap
#define HEAVYFACTORY      2         // cHeavyFactory
#define LIGHTFACTORY      3         // cLightFactory
#define CONSTYARD         4         // cConstYard
#define SILO              5         // cSpiceSilo
#define HIGHTECH          6         // cHighTech
#define IX                7         // cIX
#define REPAIR            8         // cRepairFacility
#define RADAR             9         // = OUTPOST (cOutPost)
#define REFINERY          10        // cRefinery
#define WOR               11        // cWor
#define BARRACKS          12        // cBarracks
#define STARPORT          13        // cStarPort
#define TURRET            14        // cGunTurret
#define RTURRET           15        // cRocketTurret

// SPECIAL! -> These are not actually structures. When they are done with building the
// cell properties will be changed, this is also done in cStructureFactory::createStructure
#define SLAB1             24      // 1 single slab
#define SLAB4             25      // 4 slabs
#define WALL              26      // 1 wall

// Upgrade types
#define UPGRADE_TYPE_CONSTYARD_SLAB4              0 // 4SLAB upgrade
#define UPGRADE_TYPE_CONSTYARD_RTURRET            1 // Rturret
#define UPGRADE_TYPE_LIGHTFCTRY_QUAD              2 // upgrade LightFctry to get Quad

// Heavy Factory: harkonnen / atreides / ordos
#define UPGRADE_TYPE_HEAVYFCTRY_MVC               3 // upgrade HeavyFctry to get MVC
// Heavy Factory: harkonnen / atreides only
#define UPGRADE_TYPE_HEAVYFCTRY_LAUNCHER          4 // upgrade HeavyFctry to get Rocker Launcher
#define UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK         5 // upgrade HeavyFctry to get Siege Tank
// Heavy Factory: ordos only (ordos can only access Launcher via Starport)
#define UPGRADE_TYPE_HEAVYFCTRY_SIEGETANK_ORD     6 // upgrade HeavyFctry to get Siege Tank (later techlevel, after MVC)

// Hi-Tech:
#define UPGRADE_TYPE_HITECH_ORNI                  7 // upgrade HiTech to get Ornithopter

// WOR (Harkonnen & Ordos)
#define UPGRADE_TYPE_WOR_TROOPERS                 8 // upgrade WOR to get Troopers

// Barracks (Atreides & Ordos)
#define UPGRADE_TYPE_BARRACKS_INFANTRY            9 // upgrade BARRACKS to get Infantry

#define MUSIC_WIN           0
#define MUSIC_LOSE          1
#define MUSIC_ATTACK        2
#define MUSIC_PEACE         3
#define MUSIC_MENU          4
#define MUSIC_CONQUEST      5
#define MUSIC_BRIEFING      6

// Unit types
#define SOLDIER           0
#define TROOPER           1
#define INFANTRY          2
#define TROOPERS          3
#define TRIKE             4
#define RAIDER            5
#define QUAD              6
#define TANK              7
#define LAUNCHER          8
#define SIEGETANK         9
#define DEVASTATOR        10
#define DEVIATOR          11
#define SONICTANK         12
#define HARVESTER         13
#define MCV               14   // Start with 0
#define CARRYALL          15  // air unit
#define ORNITHOPTER       16  // air unit
#define SANDWORM		  17

// special units
#define SABOTEUR		    19
#define UNIT_FREMEN_ONE     20
#define UNIT_FREMEN_THREE   21

// Frigate
#define FRIGATE           29  // air unit (should be last!!)

// Specials (super weapons)
#define SPECIAL_SABOTEUR		0
#define SPECIAL_FREMEN          1  // special unit(s) for fremen support
#define SPECIAL_DEATHHAND       2

// Bullet types
#define ROCKET_SMALL      0   // small rocket (trooper/troopers)
#define ROCKET_NORMAL     1   // normal rocket (turret/launcher/deviator)
#define ROCKET_BIG        2   // big rocket (palace, nuclear/deathhand/missile)
#define BULLET_SMALL      3   // infantry bullet
#define BULLET_TRIKE      4   // a trike bullet, looks like infantry but is stronger
#define BULLET_QUAD       5   // a quad bullet
#define BULLET_TANK       6   // a normal tank dot
#define BULLET_SIEGE      7   // a siege tank dot
#define BULLET_DEVASTATOR 8   // a devastator dot
#define BULLET_GAS        9   // a deviator shot
#define BULLET_TURRET     10  // a normal turret dot
#define BULLET_SHIMMER    11  // EXEPTION: Shimmer effect!
#define ROCKET_SMALL_ORNI 12   // small rocket ornithopter
#define ROCKET_RTURRET	  13	// rocket turrets have weaker turrets!
#define BULLET_SAB        14    // a fake bullet , to make it easier to blow up stuff
#define ROCKET_SMALL_FREMEN     15   // small rocket (trooper/troopers) - FOR FREMEN

// Special bullets (explosions) (Particle types)
#define EXPL_ONE          21  // 2 types of explosion
#define EXPL_TWO          22  // for structures
#define EXPL_TRIKE        23  // explosion for trike/quad
#define EXPL_TANK         24  // explosion for a tank/siege/devastator
#define EXPL_TANK_TWO     25  // another explosion of a tank etc

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

// Particle drawing layers
const int D2TM_RENDER_LAYER_PARTICLE_TOP        = 0;
const int D2TM_RENDER_LAYER_PARTICLE_BOTTOM     = 1;

// Bitmap types
const int D2TM_BITMAP_PARTICLE_MOVE	                   = 1;
const int D2TM_BITMAP_PARTICLE_ATTACK                  = 2;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_TRIKE         = 3;
const int D2TM_BITMAP_PARTICLE_SMOKE                   = 4;
const int D2TM_BITMAP_PARTICLE_SMOKE_SHADOW            = 5;
const int D2TM_BITMAP_PARTICLE_TRACK_DIA               = 6;
const int D2TM_BITMAP_PARTICLE_TRACK_HOR               = 7;
const int D2TM_BITMAP_PARTICLE_TRACK_VER               = 8;
const int D2TM_BITMAP_PARTICLE_TRACK_DIA2              = 9;
const int D2TM_BITMAP_PARTICLE_BULLET_PUF              = 10;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_FIRE          = 11;
const int D2TM_BITMAP_PARTICLE_WORMEAT                 = 12;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_TANK_ONE      = 13;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_TANK_TWO      = 14;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_STRUCTURE01   = 15;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_STRUCTURE02   = 16;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_GAS           = 17;
const int D2TM_BITMAP_PARTICLE_WORMTRAIL               = 18;
const int D2TM_BITMAP_PARTICLE_DEADINF01               = 19;
const int D2TM_BITMAP_PARTICLE_DEADINF02               = 20;
const int D2TM_BITMAP_PARTICLE_TANKSHOOT               = 21;
const int D2TM_BITMAP_PARTICLE_SIEGESHOOT              = 22;
const int D2TM_BITMAP_PARTICLE_SQUISH01                = 23;
const int D2TM_BITMAP_PARTICLE_SQUISH02                = 24;
const int D2TM_BITMAP_PARTICLE_SQUISH03                = 25;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_ORNI          = 26;
const int D2TM_BITMAP_PARTICLE_SIEGEDIE                = 27;
const int D2TM_BITMAP_PARTICLE_CARRYPUFF               = 28;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_ROCKET        = 29;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_ROCKET_SMALL  = 30;
const int D2TM_BITMAP_PARTICLE_OBJECT_BOOM01           = 31;
const int D2TM_BITMAP_PARTICLE_OBJECT_BOOM02           = 32;
const int D2TM_BITMAP_PARTICLE_OBJECT_BOOM03           = 33;
const int D2TM_BITMAP_PARTICLE_EXPLOSION_BULLET        = 34;


// smudge
#define SMUDGE_ROCK 0
#define SMUDGE_SAND 1
#define SMUDGE_WALL 2

// Win/lose bit flags, borrowed from Dune Legacy's implementation
#define WINLOSEFLAGS_AI_NO_BUILDINGS        0x01
#define WINLOSEFLAGS_HUMAN_HAS_BUILDINGS    0x02
#define WINLOSEFLAGS_QUOTA                  0x04
#define WINLOSEFLAGS_TIMEOUT                0x08

#endif /* DEFINITIONS_H_ */
