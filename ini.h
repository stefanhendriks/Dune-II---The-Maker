/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

// Sections
#define INI_NONE       -1
#define INI_HOUSES     0
#define INI_UNITS      1
#define INI_STRUCTURES 2
#define INI_GENERAL    3
#define INI_MAP        4

//#define INI_PLAYER     5
//#define INI_ICONS      6
//#define INI_BITMAPS    7
#define INI_NEWID      10
#define INI_GAME       11       // mod.ini / game.ini
//#define INI_MOD        12       // mod.ini
//#define INI_MENU       13       // game.ini
#define INI_TEAMS      14       // game.ini
//#define INI_MOUSE      15       // game.ini
//#define INI_SIDEBAR    16       // game.ini
#define INI_BULLETS    17       // game.ini
#define INI_EXPLOSIONS 18
//#define INI_TERRAIN	   19
#define INI_BASIC	   20
#define INI_SKIRMISH   21

#define INI_HOUSEATREIDES 21
#define INI_HOUSEORDOS 22
#define INI_HOUSEHARKONNEN 23
#define INI_HOUSESARDAUKAR 24
#define INI_HOUSEFREMEN 25
#define INI_HOUSEMERCENARY 26

#define INI_REINFORCEMENTS 27

#define INI_SCEN        28
#define INI_BRIEFING    29
#define INI_ADVICE      30
#define INI_WIN         31
#define INI_LOSE        32
#define INI_DESCRIPTION 33
#define INI_SETTINGS    34

#define SEC_REGION		198

// 'Words'
#define WORD_NONE       -1
#define WORD_MAPHEIGHT   0
#define WORD_MAPWIDTH    1
#define WORD_ICONNAME    2
#define WORD_DESCRIPTION 3
#define WORD_SWAPCOLOR   4
#define WORD_ICONID      5
#define WORD_FADECOLOR   6
#define WORD_FADEMAX     7
#define WORD_BITMAPID    8
#define WORD_PREBUILD    9
#define WORD_POWER       10
#define WORD_MODID       11     // mod.ini
#define WORD_TITLE       12     // mod.ini
#define WORD_VERSION     13     // mod.ini
#define WORD_DIR         14     // mod.ini
#define WORD_TITLEBITMAP 15     // game.ini
#define WORD_MENUCLICKSOUND 16  // game.ini
#define WORD_MOUSENORMAL  17
#define WORD_MOUSEMOVE    18
#define WORD_MOUSEATTACK  19
#define WORD_MOUSEDEPLOY  20
#define WORD_MOUSENOMOVE  21    // game.ini
#define WORD_RED          22    // game.ini
#define WORD_GREEN        23    // game.ini
#define WORD_BLUE         24    // game.ini
#define WORD_HOUSE_RED    25    // game.ini
#define WORD_HOUSE_GREEN  26    // game.ini
#define WORD_HOUSE_BLUE   27    // game.ini
#define WORD_BITMAP       28    // game.ini
#define WORD_BITMAP_WIDTH 29    // game.ini
#define WORD_BITMAP_HEIGHT 30     // game.ini
#define WORD_BITMAP_FRAMES 31     // game.ini
#define WORD_BITMAP_DEADFRAMES 32 // game.ini
#define WORD_DAMAGE       33    // game.ini
#define WORD_DEFINITION   34    // game.ini
#define WORD_BITMAP_DEAD  35    // game.ini
#define WORD_BITMAP_TOP   36    // game.ini
#define WORD_ICON         37    // game.ini
#define WORD_HITPOINTS    38    // game.ini
#define WORD_COST         40    // game.ini
#define WORD_BULLETTYPE   41    // game.ini
#define WORD_ATTACKFREQ   42    // game.ini (attack freq-uency)
#define WORD_SIGHT        43    // game.ini (sight)
#define WORD_RANGE        44    // game.ini (range)
#define WORD_BUILDTIME    45    // game.ini (build time for unit)
#define WORD_MOVESPEED    46    // game.ini (move speed)
#define WORD_TURNSPEED    47    // game.ini (turn speed)
#define WORD_SOUND        48    // game.ini (sound)
#define WORD_ISHARVESTER  49    // game.ini (IsHarvester, is it a harvester or not?)
#define WORD_SECONDSHOT   50    // game.ini (SecondShot, makes unit shoot twice?)
#define WORD_ISAIRBORN    51    // game.ini (Unit is airborn?)
#define WORD_ABLETOCARRY  52    // game.ini (Able to transport units, aka CarryAll)
#define WORD_FREEROAM     53    // game.ini (Will freely roam around the map?)
#define WORD_ISINFANTRY   54    // game.ini (Act as infantry? squisable...)
#define WORD_HARVESTAMOUNT 55   // game.ini (How much a harvester takes every step)
#define WORD_HARVESTSPEED  56   // game.ini (How many ticks needed per step)
#define WORD_HARVESTLIMIT  57   // game.ini (Max a harvester can hold)
#define WORD_PRODUCER	88		// game.ini (Producing structure of unit)

//
#define WORD_HOUSE			60
#define WORD_FOCUS			61
#define WORD_CREDITS		62
#define WORD_MAPSEED		63
#define WORD_MAPBLOOM       68   // scen.ini (spice blooms for DUNE 2 maps)
#define WORD_MAPFIELD       78   // scen.ini (spice fields for DUNE 2 maps)
#define WORD_QUOTA          79

#define WORD_BRIEFPICTURE   80

#define WORD_BRAIN			64
#define WORD_TEAM			65
#define WORD_SKILL			66
#define WORD_FIXHP			67	// fix hp

#define WORD_POWERDRAIN			68	// power
#define WORD_POWERGIVE			69	// power



// HOUSE SPECIFIC
#define WORD_FIREPOWER		70
#define WORD_FIRERATE		71
#define WORD_STRUCTPRICE	72
#define WORD_UNITPRICE		73
#define WORD_SPEED			74
#define WORD_BUILDSPEED		75
//#define WORD_HARVESTSPEED	76
#define WORD_DUMPSPEED		77


// REGION SPECIFIC
#define WORD_REGION			80
#define WORD_REGIONHOUSE	81
#define WORD_REGIONTEXT		82
#define WORD_REGIONSELECT	83
#define WORD_REGIONCONQUER	84

#define WORD_NUMBER         85

#define WORD_MAPNAME        86
#define WORD_STARTCELL      87

// SETTINGS SPECIFIC

#define WORD_FULLSCREEN	    90			// fullscreen settings in game.ini
#define WORD_SCREENWIDTH	91			// horizontal screen resolution in game.ini
#define WORD_SCREENHEIGHT	92			// vertical screen resolution in game.ini

#define WORD_MP3MUSIC		93			// MP3Music settings in game.ini

#define MAX_LINE_LENGTH     256

// Scenario loading
//void INI_Load_scenario(bool bOrDune, char filename[30] );
void INI_Load_scenario(int iHouse, int iRegion);
void INI_Load_seed(int seed);

void INI_Install_Game(std::string filename);
//void LOAD_BRIEFING(char filename[35]);
void INI_LOAD_BRIEFING(int iHouse, int iScenarioFind, int iSectionFind);
void INI_Load_Regionfile(int iHouse, int iMission);

int INI_SectionType(char section[30], int last);
void INI_WordValueSENTENCE(char result[MAX_LINE_LENGTH], char value[256]);
void INI_PRESCAN_SKIRMISH();

int getHouseFromChar(char chunk[25]);
int getUnitTypeFromChar(char chunk[25]);
int INI_GetPositionOfCharacter(char result[MAX_LINE_LENGTH], char c);
