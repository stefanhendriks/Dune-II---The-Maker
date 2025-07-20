/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */
#pragma once

#include <string>

class cReinforcements;
class AbstractMentat;
class cSelectYourNextConquestState;

// Sections
#define INI_NONE       -1
#define INI_HOUSES     0
#define INI_UNITS      1
#define INI_STRUCTURES 2
#define INI_GENERAL    3
#define INI_MAP        4

#define INI_NEWID      10
#define INI_GAME       11       // mod.ini / game.ini
#define INI_BULLETS    17       // game.ini
#define INI_EXPLOSIONS 18
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

#define SEC_REGION		198

// 'Words'
#define WORD_NONE       -1
#define WORD_MAPHEIGHT   0
#define WORD_MAPWIDTH    1
#define WORD_DESCRIPTION 3
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
#define WORD_FIRETWICE   50    // game.ini (SecondShot, makes unit shoot twice?)
#define WORD_ISAIRBORN    51    // game.ini (Unit is airborn?)
#define WORD_ABLETOCARRY  52    // game.ini (Able to transport units, aka CarryAll)
#define WORD_FREEROAM     53    // game.ini (Will freely roam around the map?)
#define WORD_ISINFANTRY   54    // game.ini (Act as infantry?)
#define WORD_HARVESTAMOUNT 55   // game.ini (How much a harvester takes every step)
#define WORD_HARVESTSPEED  56   // game.ini (How many ticks needed per step)
#define WORD_HARVESTLIMIT  57   // game.ini (Max a harvester can hold)
#define WORD_NEXTATTACKFREQ   59   // game.ini (next attack frequency)
#define WORD_PRODUCER	88		// game.ini (Producing structure of unit)
#define WORD_ISSQUISHABLE 189    // game.ini (Is unit squishable?)
#define WORD_CANSQUISH    190    // game.ini (Can squish unit?)
#define WORD_APPETITE     191    // game.ini (How much appetite has a sandworm?)

//
#define WORD_HOUSE			60
#define WORD_FOCUS			61
#define WORD_CREDITS		62
#define WORD_MAPSEED		63
#define WORD_MAPBLOOM       68   // scen.ini (spice blooms for DUNE 2 maps)
#define WORD_MAPFIELD       78   // scen.ini (spice fields for DUNE 2 maps)
#define WORD_QUOTA          79
#define WORD_WINFLAGS       192
#define WORD_LOSEFLAGS      193

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

#define MAX_LINE_LENGTH     256

#define INI_UNITS_PART_CONTROLLER   0
#define INI_UNITS_PART_TYPE         1
#define INI_UNITS_PART_HP           2
#define INI_UNITS_PART_CELL         3
#define INI_UNITS_PART_FACING_BODY  4
#define INI_UNITS_PART_FACING_HEAD  5

// Scenario loading
//void INI_Load_scenario(bool bOrDune, char filename[30] );
void INI_Load_scenario(int iHouse, int iRegion, AbstractMentat *pMentat,cReinforcements *reinforcements);
void INI_Load_seed(int seed);

void INI_Install_Game(std::string filename);
void INI_LOAD_BRIEFING(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat);
void INI_Load_Regionfile(int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState);
