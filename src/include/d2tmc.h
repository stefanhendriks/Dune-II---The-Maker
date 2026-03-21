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

#include "definitions.h"
#include "structs.h"
#include "utils/cStructureUtils.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/bullet.h"

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
class cUnit;
class cParticle;
class cPlayer;
class cPlayers;
class Graphics;
class cGame;
class cMap;

struct SDL_Surface;

// Process 'extern' stuff, so we can access our classes

// game
extern cGame		  game;
extern cMap       global_map;
extern cAbstractStructure     *g_pStructure[MAX_STRUCTURES];
extern cStructureUtils structureUtils;
extern cDrawManager    *global_drawManager;
extern SDLDrawer       *global_renderDrawer;

extern cUnit          g_Unit[MAX_UNITS];
extern cMapCamera	    *global_mapCamera;

extern cPlayers       g_Players;
extern cParticles     g_Particles;
extern cBullets       g_Bullets;

// kinds of entities (structures, units, sUpgradeInfo, specials, bullets, reinforcements)
extern s_StructureInfo    			sStructureInfo[MAX_STRUCTURETYPES];
extern s_UnitInfo         			sUnitInfo[MAX_UNITTYPES];
extern s_UpgradeInfo            sUpgradeInfo[MAX_UPGRADETYPES];
extern s_SpecialInfo            sSpecialInfo[MAX_SPECIALTYPES];

extern cBulletInfos             bulletInfos;
extern cParticleInfos					  particleInfos;

// DATAFILES
extern std::shared_ptr<Graphics> gfxdata;		// graphics (terrain, units, structures)