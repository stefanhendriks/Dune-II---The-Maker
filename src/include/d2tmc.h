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
#include <memory>
#include <SDL2/SDL_ttf.h>

constexpr int IndTrans = 223;

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
class Graphics;

struct SDL_Surface;

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

// GAME FONT
extern TTF_Font *game_font;
extern TTF_Font *bene_font;
extern TTF_Font *small_font;
extern TTF_Font *gr_bene_font;

// DATAFILES
extern std::shared_ptr<Graphics> gfxdata;		// graphics (terrain, units, structures)
extern std::shared_ptr<Graphics> gfxinter;		// interface graphics
extern std::shared_ptr<Graphics> gfxworld;		// world/pieces graphics
extern std::shared_ptr<Graphics> gfxmentat;	// mentat graphics
