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
#include <ctime>
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

// forward declarations
class CreditsDrawer;

// actual source headers                        - ORDER
#include "enums.h"
#include "main.h"
#include "cLog.h"							// logger
#include "timers.h"							/** declare correct functions at compile time here **/

// UTILS
#include "utils\cHitpointCalculator.h"			/** math for hitpoints **/
#include "utils\d2tm_math.h"
#include "utils\cTimeManager.h"

// TODO: Finish up these structure classes
#include "structures\cAbstractStructure.h"
#include "structures\cStructureFactory.h"				// Responsible for creation of Structures

// TODO: Refactor this big-... class into smaller ones (like structures)
#include "cUnit.h"

// TODO: Finish up these classes
#include "units\cAbstractUnit.h"					// Abstract Unit class (will replace cUnit.h eventually)
#include "units\cUnitFactory.h"				// Responsible for creation of Units
#include "units\cUnitUtils.h"				// Responsible for generic operations on units (ie finding a unit, etc)
#include "units\cUnitDrawer.h"				// Responsible for drawing units

// TODO: Refactor this uber mentat class into something smaller
#include "cMentat.h"

#include "ini.h"							// INI loading

// TODO: Rethink this, perhaps other global dir is needed
#include "utils\common.h"					// commonly used functions
#include "map.h"							// map data

#include "utils\cCellCalculator.h"			    /** math with cells , with handy references to structures and units **/
#include "utils\cSeedMap.h"
#include "utils\cSeedMapGenerator.h"

#include "cParticle.h"                       // explosion animations and such
#include "bullet.h"                         // bullets
#include "regions.h"
#include "ai.h"
#include "network.h"
#include "gui.h"

// SIDEBAR
#include "sidebar\sidebarh.h"

// BUILDING LOGIC
#include "cItemBuilder.h"

// Player related
#include "player\playerh.h"

// GUI
#include "gui\guih.h"

// UPGRADE LOGIC
#include "upgrade\cBuildingListUpgrader.h"

// GAME
#include "cGame.h"

// PROFILER
#include "stdlib.h"
// PROFILER

// Mentat
// TODO: Use this mentat code
#include "mentat/cOrdosMentat.h"
#include "mentat/cHarkonnenMentat.h"
#include "mentat/cAtreidesMentat.h"

// Structures
#include "structures/structuresh.h"

// TODO: Units

// Extern references & variable declarations
#include "d2tmc.h"

#endif
