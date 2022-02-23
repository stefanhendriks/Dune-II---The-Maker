/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"

#include "map/cMapEditor.h"
#include "map/cRandomMapGenerator.h"

#include <iostream>

int	iRest = 1;	// default rest value

// the ultimate game variable(s)
cGame          				game;

cStructureUtils 			structureUtils;
cMap           				map;
cRandomMapGenerator 		randomMapGenerator;

cAbstractStructure     	*	structure[MAX_STRUCTURES];
cUnit          				unit[MAX_UNITS];                // units in the game (max MAX_UNITS amount)
cMapCamera				*	mapCamera;
cPlayer        				players[MAX_PLAYERS];             // player is
cParticle      				particle[MAX_PARTICLES];
cBullet        				bullet[MAX_BULLETS];
cRegion        				world[MAX_REGIONS];
cDrawManager   			*	drawManager = nullptr;

cAllegroDrawer          *   allegroDrawer = nullptr;

// Structs of all kinds of objects (*info)
s_PreviewMap     			    PreviewMap[MAX_SKIRMISHMAPS];
s_HouseInfo         			sHouseInfo[MAX_HOUSES];
s_StructureInfo    			    sStructureInfo[MAX_STRUCTURETYPES];
s_UnitInfo         			    sUnitInfo[MAX_UNITTYPES];
s_UpgradeInfo                   sUpgradeInfo[MAX_UPGRADETYPES];
s_SpecialInfo                   sSpecialInfo[MAX_SPECIALTYPES];
s_BulletInfo        			sBulletInfo[MAX_BULLET_TYPES];
s_ParticleInfo        			sParticleInfo[MAX_PARTICLE_TYPES];
sReinforcement  			    reinforcements[MAX_REINFORCEMENTS];

// palette
PALETTE general_palette;

// bitmap(s)
BITMAP *bmp_screen;
BITMAP *bmp_backgroundMentat;
BITMAP *bmp_throttle;
BITMAP *bmp_winlose;
BITMAP *bmp_fadeout;

// datafile(s)
DATAFILE *gfxdata;		// graphics (terrain, units, structures)
DATAFILE *gfxinter;		// interface graphics
DATAFILE *gfxworld;		// world/pieces graphics
DATAFILE *gfxmentat;	// mentat graphics

// FONT stuff
ALFONT_FONT *game_font; 	// arrakeen.fon
ALFONT_FONT *bene_font;		// benegesserit font.
ALFONT_FONT *small_font;	// small font.

/** Allegro specific timer creation starts here **/
volatile int allegro_timerSecond = 0;
volatile int allegro_timerGlobal = 0;
volatile int allegro_timerUnits = 0;


/**
	Entry point of the game
*/
int main(int argc, char **argv) {
	game.setGameFilename("game.ini");

  if (game.handleArguments(argc, argv) > 0) {
    return 0;
  }

  if (game.setupGame()) {
		game.run();
	}

	game.shutdown();

	return 0;
}
END_OF_MAIN();

