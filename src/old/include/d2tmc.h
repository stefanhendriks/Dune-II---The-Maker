/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2011 (c) code by Stefan Hendriks

  */

/**
	These are the globals used by D2TM. Global variables are considered 'evil'. They should
	be moved to their respective context/classes. Ie, the game bitmap should be either in a Screen
	class which is eventually used in a Game class.
**/

#ifndef D2TMC_H

void timer_units();
void timer_global();
void timer_fps();

// Process 'extern' stuff, so we can access our classes
extern bool		bDoDebug;

// game
// cGame		  game;
cAbstractStructure     *structure[MAX_STRUCTURES];
cUnit          unit[MAX_UNITS];
cMap           * map;
cMapEditor	  mapEditor;
cMapCamera	  *mapCamera;
cMapUtils	  *mapUtils;
cPlayer        player[MAX_PLAYERS];
Particle      particle[MAX_PARTICLES];
Projectile        bullet[MAX_BULLETS];
cRegion        world[MAX_REGIONS];
cMentat		  *Mentat;
cStructureUtils structureUtils;
GameDrawer   * gameDrawer;

// ...
sPreviewMap     PreviewMap[MAX_SKIRMISHMAPS];                   // max of 100 maps in skirmish directory
s_House         houses[MAX_HOUSES];
s_Structures    structures[MAX_STRUCTURETYPES];    // structure types
s_UnitP         units[MAX_UNITTYPES];              // unit types
s_Bullet        bullets[MAX_BULLET_TYPES];          // bullet slots
sReinforcement  reinforcements[MAX_REINFORCEMENTS];  // reinforcement slots

// MP3 Music support
extern ALMP3_MP3 *mp3_music;

// PALETTE
extern PALETTE general_palette;

// GAME FONT
extern ALFONT_FONT *game_font;
extern ALFONT_FONT *bene_font;
extern ALFONT_FONT *small_font;

// DATAFILES
extern DATAFILE *gfxdata;
extern DATAFILE *gfxinter;
extern DATAFILE *gfxworld;
extern DATAFILE *gfxmentat;

// BITMAPS
extern BITMAP *bmp_screen;
extern BITMAP *bmp_fadeout;
extern BITMAP *bmp_throttle;
extern BITMAP *bmp_winlose;

// VARIABLES
extern int fps, frame_count;

#endif


