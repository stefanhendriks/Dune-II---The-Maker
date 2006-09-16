/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#ifndef D2TMC_H


// Process 'extern' stuff, so we can access our classes
extern bool		bDoDebug;
extern int		iRest;	// rest value

// game
extern cGame		  game;
extern cStructure     *structure[MAX_STRUCTURES];
extern cUnit          unit[MAX_UNITS];
extern cMap           map;
extern cPlayer        player[MAX_PLAYERS];
extern cAIPlayer      aiplayer[MAX_PLAYERS];           // related to aiplayer (except nr 0=human)
extern cParticle      particle[MAX_PARTICLES];
extern cBullet        bullet[MAX_BULLETS];
extern cRegion        world[MAX_REGIONS];
extern cMentat		  *Mentat;
extern cTimeManager   TimeManager;

extern cMultiMediaEngine *MMEngine;

extern int MAXVOICES;            // USE THIS FOR DETERMINING VOICES

// ...
extern sPreviewMap     PreviewMap[MAX_SKIRMISHMAPS];                   // max of 100 maps in skirmish directory
extern s_House         houses[MAX_HOUSES];
extern s_Structures    structures[MAX_STRUCTURETYPES];    // structure types
extern s_UnitP         units[MAX_UNITTYPES];              // unit types
extern s_Bullet        bullets[MAX_BULLET_TYPES];          // bullet slots
extern sReinforcement  reinforcements[MAX_REINFORCEMENTS];  // reinforcement slots

// MP3 Music support
extern ALMP3_MP3 *mp3_music;

// PALETTE
extern PALETTE general_palette;

// GAME FONT
extern ALFONT_FONT *game_font;
extern ALFONT_FONT *bene_font;

// DATAFILES
extern DATAFILE *gfxdata;
extern DATAFILE *gfxaudio;
extern DATAFILE *gfxinter;
extern DATAFILE *gfxworld;
extern DATAFILE *gfxmentat;

extern DATAFILE *gfxmovie;     // movie to be played (= scene = data file)

// BITMAPS
extern BITMAP *bmp_screen;
extern BITMAP *bmp_fadeout;
extern BITMAP *bmp_throttle;
extern BITMAP *bmp_winlose;


// VARIABLES
extern int mouse_tile;
extern int mouse_status;     // status of the mouse (see main.h)

extern int mouse_co_x1;      // coordinates
extern int mouse_co_y1;      // of
extern int mouse_co_x2;      // the
extern int mouse_co_y2;      // mouse border

#endif


