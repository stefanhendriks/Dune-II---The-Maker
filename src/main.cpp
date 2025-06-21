/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "d2tmc.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "map/cMapEditor.h"
#include "map/cRandomMapGenerator.h"
#include "player/cPlayer.h"
#include "utils/cLog.h"
#include "drawers/SDLDrawer.hpp"

#include "utils/DataPack.hpp"
#include <string>

#include <iostream>

int	iRest = 1;	// default rest value

// the ultimate game variable(s)
cGame          				game;

cStructureUtils 			structureUtils;
cMap           				map;
cRandomMapGenerator 		randomMapGenerator;

cAbstractStructure     		*structure[MAX_STRUCTURES];
cUnit          				unit[MAX_UNITS];                // units in the game (max MAX_UNITS amount)
cMapCamera					*mapCamera;
cPlayer        				players[MAX_PLAYERS];             // player is
cParticle      				particle[MAX_PARTICLES];
cBullet        				bullet[MAX_BULLETS];
cRegion        				world[MAX_REGIONS];
cDrawManager   				*drawManager = nullptr;

SDLDrawer             *renderDrawer = nullptr;

// Structs of all kinds of objects (*info)
s_StructureInfo    			    sStructureInfo[MAX_STRUCTURETYPES];
s_UnitInfo         			    sUnitInfo[MAX_UNITTYPES];
s_UpgradeInfo                   sUpgradeInfo[MAX_UPGRADETYPES];
s_SpecialInfo                   sSpecialInfo[MAX_SPECIALTYPES];
s_BulletInfo        			sBulletInfo[MAX_BULLET_TYPES];
s_ParticleInfo        			sParticleInfo[MAX_PARTICLE_TYPES];

// palette
// PALETTE general_palette;

// bitmap(s)
SDL_Surface *bmp_screen;
SDL_Surface *bmp_backgroundMentat;
SDL_Surface *bmp_throttle;
SDL_Surface *bmp_winlose;
SDL_Surface *bmp_fadeout;

// datafile(s)
std::unique_ptr<DataPack> gfxdata;		// graphics (terrain, units, structures)
std::unique_ptr<DataPack> gfxinter;		// interface graphics
std::unique_ptr<DataPack> gfxworld;		// world/pieces graphics
std::unique_ptr<DataPack> gfxmentat;	// mentat graphics

// FONT stuff
ALFONT_FONT *game_font; 	// arrakeen.fon
ALFONT_FONT *bene_font;		// benegesserit font.
ALFONT_FONT *small_font;	// small font.

/** Allegro specific timer creation starts here **/
// volatile int allegro_timerSecond = 0;
// volatile int allegro_timerGlobal = 0;
// volatile int allegro_timerUnits = 0;


/**
	Entry point of the game
*/
int main(int argc, char **argv)
{
    game.setGameFilename("game.ini");

    if (game.handleArguments(argc, argv) < 0) {
        return 0;
    }

    try {
        if (game.setupGame()) {
            game.run();
        }

        game.shutdown();
    }
    catch (std::runtime_error &e) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_NONE, "Unknown", fmt::format("Error: {}", e.what()));
        std::cerr << fmt::format("Error: {}\n\n", e.what());
    }

    std::cout << fmt::format("Thank you for playing Dune 2 - The Maker\n");
    return 0;
}