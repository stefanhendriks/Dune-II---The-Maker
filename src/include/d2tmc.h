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

// #include "game/cGame.h"
#include "definitions.h"
//#include "map/cMap.h"
#include "structs.h"
#include "utils/cStructureUtils.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/bullet.h"

//#include "utils/cDataPack.hpp"
#include <memory>
#include <array>
#include <SDL2/SDL_ttf.h>

/**
 * This is the palette index (in the 8 bit palette of D2TM/Dune 2) that is meant to be rendered as a 'shadow'
 */
constexpr int TransparentColorIndex = 223;

/**
 * The transparency (value between 0-255) when rendering a bitmap/texture with transparency
 */
constexpr int ShadowTrans = 160;

class cAbstractStructure;
class SDLDrawer;
class cBullet;
class cDrawManager;
class cMapCamera;
// class cRandomMapGenerator;
class cUnit;
class cParticle;
class cPlayer;
// class cRegion;
class Graphics;
class cGame;
class cMap;

struct SDL_Surface;

// Process 'extern' stuff, so we can access our classes

// game
extern cGame		  game;

extern cMap       global_map;

extern cAbstractStructure     *g_pStructure[MAX_STRUCTURES];
extern cUnit          g_Unit[MAX_UNITS];
extern cMapCamera	  *global_mapCamera;
extern cPlayer        g_Player[MAX_PLAYERS];
//extern cParticle      g_Particle[MAX_PARTICLES];
extern cParticles      g_Particles;
extern cBullet        g_Bullet[MAX_BULLETS];
// extern std::array<cRegion, MAX_REGIONS> world;
extern cStructureUtils structureUtils;
extern cDrawManager   *global_drawManager;
extern SDLDrawer      *global_renderDrawer;

// kinds of entities (structures, units, sUpgradeInfo, specials, bullets, reinforcements)
extern s_StructureInfo    			sStructureInfo[MAX_STRUCTURETYPES];
extern s_UnitInfo         			sUnitInfo[MAX_UNITTYPES];
extern s_UpgradeInfo                sUpgradeInfo[MAX_UPGRADETYPES];
extern s_SpecialInfo                sSpecialInfo[MAX_SPECIALTYPES];
// extern s_BulletInfo        			sBulletInfo[MAX_BULLET_TYPES];
// extern s_ParticleInfo        		sParticleInfo[MAX_PARTICLE_TYPES];
extern cBulletInfos             bulletInfos;
extern cParticleInfos					  particleInfos;

// DATAFILES
extern std::shared_ptr<Graphics> gfxdata;		// graphics (terrain, units, structures)