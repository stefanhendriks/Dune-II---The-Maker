/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */
#ifndef _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef D2TMH_H
#define D2TMH_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

//#include <string.h>
//#include <iostream.h>

#include <string>
#include <iostream>

#include <assert.h>

// Allegro: must be after a`ny default headers...
#include <allegro.h>

#define IDEAL_FPS	60

#ifndef UNIX
#  define ALFONT_DLL
#  define ALMP3_DLL
#endif

#include <alfont.h>
#include <almp3.h>

// Needed by fblend (fast blending techniques (3 - 10 times faster than
// default Allegro routines)
#include "fblend.h"
#include "sse.h"
#include "mmx.h"

// data file header(s)                          - THIS
#include "data/gfxdata.h"						// general gfx
#include "data/gfxaudio.h"                     	// audio (sounds and music , midi)
#include "data/gfxinter.h"                  	// sidebar/downbar gfx
#include "data/gfxworld.h"                  	// world / regions gfx
#include "data/gfxmentat.h"                  	// mentat gfx

// actual source headers                        - ORDER
#include "main.h"
#include "timers.h"							/** declare correct functions at compile time here **/
#include "cHitpointCalculator.h"			/** math for hitpoints **/
#include "d2tm_math.h"
#include "cTimeManager.h"
#include "cAbstractStructure.h"
#include "cStructureFactory.h"				// Responsible for creation of Structures
#include "cUnit.h"
#include "cAbstractUnit.h"					// Abstract Unit class (will replace cUnit.h eventually)
#include "cUnitFactory.h"					// Responsible for creation of Units
#include "cUnitUtils.h"						// Responsible for generic operations on units (ie finding a unit, etc)
#include "cUnitDrawer.h"					// Responsible for drawing units
#include "cMentat.h"
#include "cCellCalculator.h"			    /** math with cells , with handy references to structures and units **/
#include "ini.h"							// INI loading
#include "common.h"							// commonly used functions
#include "cPlayer.h"							// player data
#include "map.h"							// map data
#include "cSeedMap.h"
#include "cSeedMapGenerator.h"
#include "cParticle.h"                       // explosion animations and such
#include "bullet.h"                         // bullets
#include "regions.h"
#include "ai.h"
#include "network.h"
#include "gui.h"

#include "keyval.h"

// GUI RELATED
#include "gui\eBuildType.h"				// enum build types (UNIT/STRUCTURE)
#include "gui\cBuildingListItem.h"
#include "gui\cBuildingList.h"
#include "gui\cBuildingListDrawer.h"
#include "gui\cBuildingListFactory.h"
#include "cBuildingListUpgrader.h"
#include "gui\cSideBar.h"
#include "gui\cSideBarDrawer.h"
#include "gui\cSideBarFactory.h"


// game
#include "cGame.h"							// game class

// PROFILER
#include "stdlib.h"
// PROFILER

// Mentats
#include "mentat/cOrdosMentat.h"
#include "mentat/cHarkonnenMentat.h"
#include "mentat/cAtreidesMentat.h"

// Structures
#include "structures/cConstYard.h"
#include "structures/cStarPort.h"
#include "structures/cWindTrap.h"
#include "structures/cSpiceSilo.h"
#include "structures/cRefinery.h"
#include "structures/cOutPost.h"
#include "structures/cPalace.h"
#include "structures/cRepairFacility.h"
#include "structures/cLightFactory.h"
#include "structures/cHeavyFactory.h"
#include "structures/cHighTech.h"
#include "structures/cGunTurret.h"
#include "structures/cRocketTurret.h"
#include "structures/cWor.h"
#include "structures/cBarracks.h"
#include "structures/cIx.h"

// TODO: Units

// Extern references & variable declarations
#include "d2tmc.h"

#endif
