/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "d2tmh.h" 

bool bDoDebug = false;
int	iRest = 1;	// rest value

// Server/Client system

// the ultimate game variable(s)
cGame          game;
cStructure     *structure[MAX_STRUCTURES];
cUnit          unit[MAX_UNITS];
cMap           map;
cPlayer        player[MAX_PLAYERS];             // player is 
cAIPlayer      aiplayer[MAX_PLAYERS];           // related to aiplayer (except nr 0=human)
cParticle      particle[MAX_PARTICLES];
cBullet        bullet[MAX_BULLETS];
cRegion        world[MAX_REGIONS];
cMentat		   *Mentat;
cTimeManager   TimeManager;

// Structs
sPreviewMap     PreviewMap[MAX_SKIRMISHMAPS];        // max of 100 maps in skirmish directory
s_House         houses[MAX_HOUSES];
s_Structures    structures[MAX_STRUCTURETYPES];    // structure types
s_UnitP         units[MAX_UNITTYPES];              // unit types
s_Bullet        bullets[MAX_BULLET_TYPES];          // bullet slots
sReinforcement  reinforcements[MAX_REINFORCEMENTS];  // reinforcement slots

int MAXVOICES = 8;            // USE THIS FOR DETERMINING VOICES

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
ALFONT_FONT *game_font; // arrakeen.fon
ALFONT_FONT *bene_font;	// benegesserit font.

// MP3 STUFF
ALMP3_MP3   *mp3_music; // pointer to mp3 music

// Mouse information
int mouse_co_x1;      // coordinates
int mouse_co_y1;      // of
int mouse_co_x2;      // the
int mouse_co_y2;      // mouse border

int mouse_status;     // status of the mouse (see main.h)
int mouse_tile;       // mouse picture in gfxdata

/** Allegro specific timer creation starts here **/

#ifdef ALLEGRO_H 
	cMultiMediaEngine *MMEngine = new cMMEAllegro;
	

	volatile int allegro_timerSecond = 0;
	volatile int allegro_timerGlobal = 0;
	volatile int allegro_timerUnits = 0;	
#endif

/**
	Entry point of the game
*/
int main() {

	game.init();

	if (game.setupGame()) {
		game.run();
	}

	game.shutdown();
	
	return 0;
}
END_OF_MAIN();

