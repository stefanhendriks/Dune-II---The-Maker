/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"

using namespace std;

bool bDoDebug = false;
int	iRest = 1;	// default rest value

// Server/Client system

// the ultimate game variable(s)
cGame          				game;
cAbstractStructure     	*	structure[MAX_STRUCTURES];
cUnit          				unit[MAX_UNITS];
cMap           				map;
cMapEditor	  				mapEditor;
cRandomMapGenerator 		randomMapGenerator;
cMapCamera				*	mapCamera;
cMapUtils				*   mapUtils;
cPlayer        				player[MAX_PLAYERS];             // player is
cAIPlayer      				aiplayer[MAX_PLAYERS];           // related to aiplayer (except nr 0=human)
cParticle      				particle[MAX_PARTICLES];
cBullet        				bullet[MAX_BULLETS];
cRegion        				world[MAX_REGIONS];
cMentat		   			*	Mentat = NULL;
cTimeManager   				TimeManager;
cStructureUtils 			structureUtils;
cDrawManager   			*	drawManager = NULL;
cInteractionManager *interactionManager = NULL;

// Structs
sPreviewMap     			PreviewMap[MAX_SKIRMISHMAPS];        // max of 100 maps in skirmish directory
s_House         			houses[MAX_HOUSES];
s_Structures    			structures[MAX_STRUCTURETYPES];    // structure types
s_UnitP         			units[MAX_UNITTYPES];              // unit types
s_Bullet        			bullets[MAX_BULLET_TYPES];          // bullet slots
sReinforcement  			reinforcements[MAX_REINFORCEMENTS];  // reinforcement slots

// palette
PALETTE general_palette;

// bitmap(s)
BITMAP *bmp_screen;
BITMAP *bmp_throttle;
BITMAP *bmp_winlose;
BITMAP *bmp_fadeout;

// datafile(s)
DATAFILE *gfxdata;		// graphics (terrain, units, structures)
DATAFILE *gfxaudio;		// audio
DATAFILE *gfxinter;		// interface graphics
DATAFILE *gfxworld;		// world/pieces graphics
DATAFILE *gfxmentat;	// mentat graphics
DATAFILE *gfxmovie;     // movie to be played (= scene = data file)

// FONT stuff
ALFONT_FONT *game_font; 	// arrakeen.fon
ALFONT_FONT *bene_font;		// benegesserit font.
ALFONT_FONT *small_font;	// small font.

// MP3 STUFF
ALMP3_MP3   *mp3_music; // pointer to mp3 music

// Mouse information
int mouse_co_x1;      // coordinates
int mouse_co_y1;      // of
int mouse_co_x2;      // the
int mouse_co_y2;      // mouse border

int mouse_status;     // status of the mouse (see main.h)
int mouse_tile;       // mouse picture in gfxdata

// Timers
int frame_count, fps;  // fps and such

/** Allegro specific timer creation starts here **/
#ifdef ALLEGRO_H
	volatile int allegro_timerSecond = 0;
	volatile int allegro_timerGlobal = 0;
	volatile int allegro_timerUnits = 0;
#endif

int handleArguments(int argc, char *argv[]) {

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			string command = argv[i];
			if (command.compare("-game") == 0) {
				if ((i + 1) < argc) {
					i++;
					game.game_filename = string(argv[i]);
				}
			}
		}
	} // arguments passed

	return 0;
}


/**
	Entry point of the game
*/
int main(int argc, char **argv) {
	game.game_filename = "game.ini";

	if (handleArguments(argc, argv) > 0) {
		return 0;
	}

	game.init();

	if (game.setupGame()) {
		game.run();
	}

	game.shutdown();

	return 0;
}
END_OF_MAIN();

