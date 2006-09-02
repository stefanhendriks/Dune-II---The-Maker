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

// Allegro: must be after any default header...
#include <allegro.h>
//#include <OpenLayer.hpp> 

#define ALFONT_DLL
#define ALMP3_DLL

#include <alfont.h>
#include <almp3.h>
#include "fblend.h"             // fast blending
#include "sse.h"
#include "mmx.h"

// Make sure we refer to our own sub dir and not a global one
#include "Raknet\\RakClientInterface.h"
#include "Raknet\\RakNetworkFactory.h"
#include "Raknet\\RakServerInterface.h"

// data file header(s)                          - THIS
#include "data\\gfxdata.h"						// general gfx
#include "data\\gfxaudio.h"                     // audio 
#include "data\\gfxinter.h"                  // sidebar/downbar gfx
#include "data\\gfxworld.h"                  // world / regions gfx
#include "data\\gfxmentat.h"                  // mentat gfx

// actual source headers                        - ORDER
//                                                (in every file!) 
#include "main.h"                           // main
#include "math.h"                           // mathematics
#include "cStructure.h"						// structures
#include "cUnit.h"							// units	
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

#define PROFILE	&Profile
#include "HWProf.h"

// PROFILER


// Structures
#include "structures\\cConstYard.h"
#include "structures\\cStarPort.h"
#include "structures\\cWindTrap.h"
#include "structures\\cSpiceSilo.h"
#include "structures\\cRefinery.h"
#include "structures\\cOutPost.h"
#include "structures\\cPalace.h"
#include "structures\\cRepairFacility.h"
#include "structures\\cLightFactory.h"
#include "structures\\cHeavyFactory.h"
#include "structures\\cHighTech.h"
#include "structures\\cGunTurret.h"
#include "structures\\cRocketTurret.h"
#include "structures\\cWor.h"
#include "structures\\cBarracks.h"
#include "structures\\cIx.h"

// TODO: Units

#endif