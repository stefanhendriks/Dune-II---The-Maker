/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2010 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"

using namespace std;

bool bDoDebug = false;
int	iRest = 1;	// default rest value

// Server/Client system

// the ultimate game variable(s)
cGame          				game;
cGameState                  *gameState;
cSelectYourNextConquestState *selectYourNextConquestState; // there is only 1 instance of this

cTimeManager   				TimeManager;
cStructureUtils 			structureUtils;
cMap           				map;
cMapEditor	  				mapEditor;
cRandomMapGenerator 		randomMapGenerator;

cAbstractStructure     	*	structure[MAX_STRUCTURES];
cUnit          				unit[MAX_UNITS];                // units in the game (max MAX_UNITS amount)
cMapCamera				*	mapCamera;
cPlayer        				players[MAX_PLAYERS];             // player is
cAIPlayer      				aiplayer[MAX_PLAYERS];           // related to aiplayer (except nr 0=human)
cParticle      				particle[MAX_PARTICLES];
cBullet        				bullet[MAX_BULLETS];
cRegion        				world[MAX_REGIONS];
cDrawManager   			*	drawManager = NULL;

cAllegroDrawer          *   allegroDrawer = nullptr;

// Structs
s_PreviewMap     			PreviewMap[MAX_SKIRMISHMAPS];        // max of 100 maps in skirmish directory
s_House         			houses[MAX_HOUSES];
s_Structures    			structures[MAX_STRUCTURETYPES];         // structure types
s_UnitP         			units[MAX_UNITTYPES];              // unit types
s_Upgrade                   upgrades[MAX_UPGRADETYPES];        // upgrade types
s_Special                   specials[MAX_SPECIALTYPES];        // special types
s_Bullet        			bullets[MAX_BULLET_TYPES];          // bullet slots
sReinforcement  			reinforcements[MAX_REINFORCEMENTS];  // reinforcement slots

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

int mouse_mv_x1;
int mouse_mv_y1;
int mouse_mv_x2;
int mouse_mv_y2;

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
    game.bDisableAI = false;
    game.bOneAi = false;
    game.windowed = false;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			string command = argv[i];
			if (command.compare("-game") == 0) {
				if ((i + 1) < argc) {
					i++;
					game.game_filename = string(argv[i]);
				}
			} else if (command.compare("-windowed") == 0) {
				// Windowed flag passed, so use that
				game.windowed = true;
			} else if (command.compare("-nomusic") == 0) {
				game.bPlayMusic = false;
			} else if (command.compare("-nosound") == 0) {
			    // disable all sound effects
				game.bPlayMusic = false;
				game.bPlaySound = false;
			} else if (command.compare("-debug") == 0) {
			    // debugging enabled
                bDoDebug = true;
			} else if (command.compare("-noai") == 0) {
                game.bDisableAI = true;
            } else if (command.compare("-oneai") == 0) {
                game.bOneAi = true;
            }
		}
	} // arguments passed

	return 0;
}

/**
 * This stuff initializes only at the beginning, a plug point/method before moving to actual constructor, but
 * atleast moving out the big game.init and game.setup_players methods.
 */
void initializingOnlyOnce() {
    gameState = nullptr;
    selectYourNextConquestState = new cSelectYourNextConquestState(game);

    mouse_co_x1 = -1;      // coordinates
    mouse_co_y1 = -1;      // of
    mouse_co_x2 = -1;      // the
    mouse_co_y2 = -1;      // mouse border

    mouse_mv_x1 = -1;
    mouse_mv_y1 = -1;
    mouse_mv_x2 = -1;
    mouse_mv_y2 = -1;
}

/**
	Entry point of the game
*/
int main(int argc, char **argv) {
	game.game_filename = "game.ini";
    set_config_file("d2tm.cfg");

    if (handleArguments(argc, argv) > 0) {
        return 0;
    }

    initializingOnlyOnce();

    game.init();

    if (game.setupGame()) {
		game.run();
	}

	game.shutdown();

	return 0;
}
END_OF_MAIN();

