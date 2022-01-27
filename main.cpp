/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"

bool bDoDebug = false;
int	iRest = 1;	// default rest value

// the ultimate game variable(s)
cGame          				game;

cTimeManager   				TimeManager;
cStructureUtils 			structureUtils;
cMap           				map;
cMapEditor	  				mapEditor;
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

int handleArguments(int argc, char *argv[]) {
    game.m_disableAI = false;
    game.m_disableReinforcements = false;
    game.m_drawUsages = false;
    game.m_drawUnitDebug = false;
    game.m_oneAi = false;
    game.m_windowed = false;
    game.m_noAiRest = false;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
            std::string command = argv[i];
			if (command.compare("-game") == 0) {
				if ((i + 1) < argc) {
					i++;
					game.m_gameFilename = std::string(argv[i]);
				}
			} else if (command.compare("-windowed") == 0) {
				// Windowed flag passed, so use that
				game.m_windowed = true;
			} else if (command.compare("-nomusic") == 0) {
				game.m_playMusic = false;
			} else if (command.compare("-nosound") == 0) {
			    // disable all sound effects
				game.m_playMusic = false;
				game.m_playSound = false;
			} else if (command.compare("-debug") == 0) {
			    // generic debugging enabled
                bDoDebug = true;
			} else if (command.compare("-debug-units") == 0) {
                // unit debugging enabled
                game.m_drawUnitDebug = true;
			} else if (command.compare("-noai") == 0) {
                game.m_disableAI = true;
            } else if (command.compare("-oneai") == 0) {
                game.m_oneAi = true;
            } else if (command.compare("-noreinforcements") == 0) {
                game.m_disableReinforcements = true;
            } else if (command.compare("-noairest") == 0) {
                game.m_noAiRest = true;
            } else if (command.compare("-usages") == 0) {
                game.m_drawUsages = true;
            }
		}
	} // arguments passed

	return 0;
}

/**
	Entry point of the game
*/
int main(int argc, char **argv) {
	game.m_gameFilename = "game.ini";

    if (handleArguments(argc, argv) > 0) {
        return 0;
    }

    if (game.setupGame()) {
		game.run();
	}

	game.shutdown();

	return 0;
}
END_OF_MAIN();

