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
#include "game/cGame.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/cSpecialInfos.h"

#include <memory>
#include <array>
#include <SDL2/SDL_ttf.h>



class cAbstractStructure;
class SDLDrawer;
class cBullet;
class cDrawManager;
class cMapCamera;
class cUnit;
class cUnits;
class cParticle;
class cPlayer;
class cPlayers;
class Graphics;
class cGame;
class cMap;

struct SDL_Surface;


// game
extern cGame		  game;

extern SDLDrawer      *global_renderDrawer;
extern std::shared_ptr<Graphics> gfxdata;

// kinds of entities (structures, units, sUpgradeInfo, specials, bullets, reinforcements)
extern s_StructureInfo    			sStructureInfo[MAX_STRUCTURETYPES];
extern s_UnitInfo         			sUnitInfo[MAX_UNITTYPES];
extern s_UpgradeInfo            sUpgradeInfo[MAX_UPGRADETYPES];
extern s_SpecialInfo            sSpecialInfo[MAX_SPECIALTYPES];

extern cBulletInfos             bulletInfos;
