/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */
#ifndef D2TMH_H

// include this stuff
#include <time.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

// Allegro: must be after any default headers...
#include <allegro.h>

#define IDEAL_FPS	60

#ifdef ALLEGRO_H 
	// note: timer specific declarations are done in timer.h
	
#ifndef UNIX
#  define ALFONT_DLL
#  define ALMP3_DLL
#endif

#endif

#include <alfont.h>
#include <almp3.h>
#include "fblend.h"             // fast blending
#include "sse.h"
#include "mmx.h"

// Make sure we refer to our own sub dir and not a global one
#include "raknet/RakClientInterface.h"
#include "raknet/RakNetworkFactory.h"
#include "raknet/RakServerInterface.h"

// data file header(s)                          - THIS
#include "data/gfxdata.h"						// general gfx
#include "data/gfxaudio.h"                     // audio 
#include "data/gfxinter.h"                  // sidebar/downbar gfx
#include "data/gfxworld.h"                  // world / regions gfx
#include "data/gfxmentat.h"                  // mentat gfx

// actual source headers                        - ORDER
#include "main.h"
#include "timers.h"							/** declare correct functions at compile time here **/
#include "d2tm_math.h"
#include "cMultiMediaEngine.h"

// Any MME layer is defined here
#include "cMMEAllegro.h"

#include "cLog.h"
#include "cMouse.h"
#include "cMentatFactory.h"
#include "cStructureFactory.h"
#include "cUnitFactory.h"
#include "cEventManager.h"
#include "cTimeManager.h"
#include "cDrawManager.h"
#include "cStructure.h"
#include "cUnit.h"
#include "cMentat.h"
#include "ini.h"							// INI loading
#include "cGame.h"							// game class
#include "common.h"							// commonly used functions
#include "cPlayer.h"							// player data
#include "map.h"							// map data
#include "cParticle.h"                       // explosion animations and such
#include "bullet.h"                         // bullets
#include "regions.h"
#include "ai.h"
#include "network.h"
#include "gui.h"

#include "keyval.h"


// PROFILER
#include "stdlib.h"
// PROFILER

// Mentats
#include "mentat/cOrdosMentat.h"
#include "mentat/cHarkonnenMentat.h"
#include "mentat/cAtreidesMentat.h"
#include "mentat/cBenegesMentat.h"

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
