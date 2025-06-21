/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

/**
	These are the globals used by D2TM. Global variables are considered 'evil'. They should
	be moved to their respective context/classes. Ie, the game bitmap should be either in a Screen
	class which is eventually used in a Game class.
**/
#pragma once

#include "cGame.h"
#include "definitions.h"
#include "map/cMap.h"
#include "structs.h"
#include "utils/cStructureUtils.h"

#include "utils/DataPack.hpp"

class cAbstractStructure;
class SDLDrawer;
class cBullet;
class cDrawManager;
class cMapCamera;
class cRandomMapGenerator;
class cUnit;
class cParticle;
class cPlayer;
class cRegion;
class DataPack;

struct ALFONT_FONT;
struct BITMAP;
struct DATAFILE;

// Process 'extern' stuff, so we can access our classes
extern int		iRest;	// rest value

// game
extern cGame		  game;

extern cMap           map;

extern cAbstractStructure     *structure[MAX_STRUCTURES];
extern cUnit          unit[MAX_UNITS];
extern cRandomMapGenerator randomMapGenerator;
extern cMapCamera	  *mapCamera;
extern cPlayer        players[MAX_PLAYERS];
extern cParticle      particle[MAX_PARTICLES];
extern cBullet        bullet[MAX_BULLETS];
extern cRegion        world[MAX_REGIONS];
extern cStructureUtils structureUtils;
extern cDrawManager   *drawManager;
extern SDLDrawer      *renderDrawer;

// kinds of entities (structures, units, sUpgradeInfo, specials, bullets, reinforcements)
extern s_StructureInfo    			sStructureInfo[MAX_STRUCTURETYPES];
extern s_UnitInfo         			sUnitInfo[MAX_UNITTYPES];
extern s_UpgradeInfo                sUpgradeInfo[MAX_UPGRADETYPES];
extern s_SpecialInfo                sSpecialInfo[MAX_SPECIALTYPES];
extern s_BulletInfo        			sBulletInfo[MAX_BULLET_TYPES];
extern s_ParticleInfo        		sParticleInfo[MAX_PARTICLE_TYPES];

// PALETTE
extern PALETTE general_palette;

// GAME FONT
extern ALFONT_FONT *game_font;
extern ALFONT_FONT *bene_font;
extern ALFONT_FONT *small_font;

// DATAFILES
extern DataPack *gfxdata;
extern DataPack *gfxinter;
extern DataPack *gfxworld;
extern DataPack *gfxmentat;

// BITMAPS
extern SDL_Surface *bmp_screen;
extern SDL_Surface *bmp_backgroundMentat;
extern SDL_Surface *bmp_fadeout;
extern SDL_Surface *bmp_throttle;
extern SDL_Surface *bmp_winlose;


