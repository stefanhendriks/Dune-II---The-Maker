/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  TODO: Reduce this crap to minimum

  */
#define DEBUGGING bDoDebug

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

#define Y_DRAWOFFSET	24

#define MAX_WORLD		4			// 4 worlds is enough for now
#define MAX_REGIONS		27			// not more then 27 regions as dune 2 has

// MATH
#define PI      3.1415926535897932384626433832795   // a definition of PI

// Font size
#define GAME_FONTSIZE 8

// Buffer lenght of MP3 playback
#define MP3_BUFFER_LEN  16384

// Max length of a path (per unit)
#define MAX_PATH    256

// maximum of 99 orders
#define MAX_QUE 99

#define MAP_W_MAX        64     // max of width
#define MAP_H_MAX        64     // max of height

#define MAX_CELLS    4096

// SCENES USED FOR GIF PLAYBACK:
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

// List ID's corresponding buttons
#define LIST_NONE		0
#define LIST_CONSTYARD	1
#define LIST_INFANTRY	2
#define LIST_LIGHTFC	3
#define LIST_HEAVYFC	4
#define LIST_ORNI		5
#define LIST_STARPORT	6
#define LIST_PALACE		7
#define LIST_MAX		8			// max amount of lists

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
#define MAX_BULLETS      600     // max of bullets in the game at once
#define MAX_PARTICLES    1200	 // max of particles at once
#define MAX_REINFORCEMENTS 50    // max of 50 reinforcements
#define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps
//#define HOUSE_SLOTS       10      // amount of House 'types'

// Game fases (state machine)
#define GAME_OVER         0      // game over
#define GAME_MENU         1      // in a menu
#define GAME_PLAYING      2      // playing the actual game
#define GAME_BRIEFING     3      // doing some briefing
#define GAME_EDITING      4      // running the editor
#define GAME_OPTIONS	  5		 // options menu
#define GAME_REGION		  6		 // select a region
#define GAME_HOUSE		  8		 // pick a house
#define GAME_INTRO	      9
#define GAME_TELLHOUSE	 10		 // tell about the house
#define GAME_WINNING     11      // when winning, do some cool animation
#define GAME_WINBRIEF    12      // mentat chatter when won the mission
#define GAME_LOSEBRIEF   13      // briefing when losing
#define GAME_LOSING      14      // when  losing, do something cool
#define GAME_SKIRMISH    15      // playing a skirmish mission!
#define GAME_SETUPSKIRMISH 16    // set up a skirmish game

// FOR OPTIONS:

// GAME (MENU) FLAGS: (used when GAME_MENU)
// Game menu state machine


// PLAYER/AI stuff
// 0 = HUMAN			== ALLIES WITH FREMEN WHEN ATREIDES
// 1 = CPU (Atreides)	
// 2 = CPU (Harkonnen)
// 3 = CPU (Ordos)		
// 4 = CPU (Sardaukar)	== ALLIES WITH EVERYBODY
// 5 = CPU (Fremen)		== ALLIES WITH ATREIDES
// 6 = WORM
#define MAX_PLAYERS       7 

#define AI_WORM			  6

#define THINK_PLAYER      1      // real player is thinking for this one
#define THINK_CLIENT      2      // this ID is a client; so this is updated through the network
#define THINK_SERVER      3      // this ID is the server (??)
#define THINK_NONE        4      // un-used slot
#define THINK_CPU         99     // computer thinks for this player

// HOUSES stuff
#define MAX_HOUSES        12      // 12 different type of houses

// UNITS stuff
#define MAX_UNIT_TYPES    25     // maximum of unit types

// STRUCTURES stuff
#define MAX_BUILDING_TYPES 25    // max of building types

// BULLETS stuff
#define MAX_BULLET_TYPES      30    // max of 30 types of bullets

// Lists stuff
#define MAX_ICONS       30      // max of 30 icons per list
//#define MAX_LISTS       25      // max of 25 lists

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
#define FREMEN	  5
#define MERCENARY 6
#define CORRINO   7         // == SARDAUKAR!!!   ---- in this code we use this 'house' for sandworms
#define SANDHOUSE 9
#define GREYHOUSE	10

#define ATREIDES_TALK	20
#define HARKONNEN_TALK	21
#define ORDOS_TALK		22
// .. add another id for another house

// BRAINS:
#define BRAIN_AI        1
#define BRAIN_CLIENT    2
#define BRAIN_NONE      3
#define BRAIN_AI_WORM   4


#define MAX_UNITTYPES       50
#define MAX_STRUCTURETYPES  50

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
// cell properties will be changed, this is also done in CREATE_STRUCTURE()
#define SLAB1             24      // 1 single slab
#define SLAB4             25      // 4 slabs
#define WALL              26      // 1 wall


#define MUSIC_WIN           0
#define MUSIC_LOSE          1
#define MUSIC_ATTACK        2
#define MUSIC_PEACE         3
#define MUSIC_MENU          4
#define MUSIC_CONQUEST      5
#define MUSIC_BRIEFING      6

// Unit types
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
#define SABOTEUR		  19  // special unit
#define UNIT_FREMEN       20  // special unit(s) for fremen support

// Frigate
#define FRIGATE           29  // air unit (should be last!!)

// UPGRADES AS UNIT TYPES
#define UPGR_FREMEN		  30 // Use fremen intelligence
#define UPGR_SANDWORM	  31 // see sandworms on minimap
#define UPGR_MISSILE      32 // dead-hand

// Bullet types
#define ROCKET_SMALL      0   // small rocket (trooper/troopers) 
#define ROCKET_NORMAL     1   // normal rocket (turret/launcher/deviator)
#define ROCKET_BIG        2   // big rocket (palace, nuclear)
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
#define PARTYPE_SMOKE		1
#define PARTYPE_REPAIRBLINK 2

// smudge
#define SMUDGE_ROCK 0
#define SMUDGE_SAND 1
#define SMUDGE_WALL 2


// Unit properties
// note: This is NOT the unit class, this is simply a 'prefix' for a unit!
// note: The unit properties are actually set in common.cpp , like the houses!
// note: Some values are now read via INI.CPP (GAME.INI reader).
struct s_UnitP {
	BITMAP  *bmp;			// each unit has a reference to his 8 bit bitmap.
	BITMAP  *top;			// top of unit
	BITMAP  *shadow;		// shadow of unit (24 bit)
	int bmp_width;			// width of each unit frame (in pixels).
	int bmp_height;			// Height of the unit, for each animating frame.
	int bmp_frames;
	int bmp_startpixel;		// The X axis starting pixel (i.e. for the harvester)
	int hp;					// Amount of hitpoints at maximum
	int speed;				// speed (0 being uberly slow, 1000 being super fast).
	int turnspeed;
	int attack_frequency;	// timer for attacking
	int build_time;			// how long it takes for building this thing
	int bullets;			// type of bullet
	int max_bullets;		// Maximum of 'bullets' it can fire at a time
	int type;				// type of unit, 
	int sight;				// Sight of a unit (in radius)
	int range;				// Range of a unit (in radius)
	int icon;				// icon id  
	int cost;				// cost/price
	bool airborn;           // does this unit fly?
	bool infantry;          // legged unit? (infantry)
	bool free_roam;         // (airborn only) - does it free roam?
	bool second_shot;       // can shoot 2 times?
	bool squish;            // can this unit squish infantry?
	char name[50];          // name of unit
	int credit_capacity;    // max credits capacity
	int harvesting_speed;   // speed rate for harvesting (time)
	int harvesting_amount;  // value rate for harvesting (amount)
};


// Structure types
struct s_Structures {	
	BITMAP *bmp;			// a reference to his 8 bit bitmap. (id in graphics)
	BITMAP *shadow;			// shadow
	int bmp_width;			// width
	int bmp_height;			// height
	int list;				// list this structure has!
	int hp;					// max hp
	int fixhp;				// how much you get for fixing this thing
	int bullets;			// type of bullet
	int build_time;			// how long it takes for building this thing
	int power_drain;		// the power that this building drains...
	int power_give;			// the power that this building gives...
	int sight;				// sight in cells
	int fadecol;			// Fading color (if needed), -1 is none, > -1 means starting color
	int fademax;			// Max fading color (assuming fadecol < fademax)
	int icon;				// icon id
	int cost;				// price of this thing
	char name[64];			// name
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
     BITMAP *terrain;        // terrain
     int mapdata[MAX_CELLS]; // map data
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

