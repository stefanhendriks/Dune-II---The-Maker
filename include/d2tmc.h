/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */

/**
	These are the globals used by D2TM. Global variables are considered 'evil'. They should
	be moved to their respective context/classes. Ie, the game bitmap should be either in a Screen
	class which is eventually used in a Game class.
**/

#ifndef D2TMC_H

// Process 'extern' stuff, so we can access our classes
extern bool		bDoDebug;
extern int		iRest;	// rest value

// game
extern cGame		  game;
extern cGameState     *gameState;
extern cSelectYourNextConquestState *selectYourNextConquestState;

extern cMap           map;
extern cMapEditor	  mapEditor;

extern cAbstractStructure     *structure[MAX_STRUCTURES];
extern cUnit          unit[MAX_UNITS];
extern cRandomMapGenerator randomMapGenerator;
extern cMapCamera	  *mapCamera;
extern cPlayer        players[MAX_PLAYERS];
extern cParticle      particle[MAX_PARTICLES];
extern cBullet        bullet[MAX_BULLETS];
extern cRegion        world[MAX_REGIONS];
extern cTimeManager   TimeManager;
extern cStructureUtils structureUtils;
extern cDrawManager   *drawManager;
extern cAllegroDrawer      *allegroDrawer;

// kinds of entities (sHouseInfo, structures, units, sUpgradeInfo, specials, bullets, reinforcements)
extern s_PreviewMap     		    PreviewMap[MAX_SKIRMISHMAPS];        // max of 100 maps in skirmish directory
extern s_HouseInfo         			sHouseInfo[MAX_HOUSES];
extern s_StructureInfo    			sStructureInfo[MAX_STRUCTURETYPES];
extern s_UnitInfo         			sUnitInfo[MAX_UNITTYPES];
extern s_UpgradeInfo                sUpgradeInfo[MAX_UPGRADETYPES];
extern s_SpecialInfo                sSpecialInfo[MAX_SPECIALTYPES];
extern s_BulletInfo        			sBulletInfo[MAX_BULLET_TYPES];
extern s_ParticleInfo        		sParticleInfo[MAX_PARTICLE_TYPES];
extern sReinforcement  			    reinforcements[MAX_REINFORCEMENTS];

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
extern DATAFILE *gfxaudio;
extern DATAFILE *gfxinter;
extern DATAFILE *gfxworld;
extern DATAFILE *gfxmentat;

//extern DATAFILE *gfxmovie;     // movie to be played (= scene = data file)

// BITMAPS
extern BITMAP *bmp_screen;
extern BITMAP *bmp_backgroundMentat;
extern BITMAP *bmp_fadeout;
extern BITMAP *bmp_throttle;
extern BITMAP *bmp_winlose;

// VARIABLES
extern int fps, frame_count;
extern int mouse_tile;
extern int mouse_status;     // status of the mouse (see main.h)

extern int mouse_co_x1;      // coordinates
extern int mouse_co_y1;      // of
extern int mouse_co_x2;      // the
extern int mouse_co_y2;      // mouse border (box selecting)

extern int mouse_mv_x1;      // coordinates
extern int mouse_mv_y1;      // of
extern int mouse_mv_x2;      // the
extern int mouse_mv_y2;      // mouse for scrolling with right click holding

#endif


