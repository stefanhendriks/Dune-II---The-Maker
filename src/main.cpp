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

#include "utils/Graphics.hpp"

#include "utils/cHandleArgument.h"
#include "utils/common.h"
#include <string>

#include <iostream>
#include <SDL2/SDL_ttf.h>
int	iRest = 1;	// default rest value

// the ultimate game variable(s)
cGame          				game;

cStructureUtils 			structureUtils;
cMap           				global_map;
cRandomMapGenerator 		randomMapGenerator;

cAbstractStructure     		*structure[MAX_STRUCTURES];
cUnit          				unit[MAX_UNITS];                // units in the game (max MAX_UNITS amount)
cMapCamera					*mapCamera;
cPlayer        				players[MAX_PLAYERS];             // player is
cParticle      				particle[MAX_PARTICLES];
cBullet        				bullet[MAX_BULLETS];
cRegion        				world[MAX_REGIONS];
cDrawManager   				*drawManager = nullptr;

SDLDrawer                   *renderDrawer = nullptr;

// Structs of all kinds of objects (*info)
s_StructureInfo    			    sStructureInfo[MAX_STRUCTURETYPES];
s_UnitInfo         			    sUnitInfo[MAX_UNITTYPES];
s_UpgradeInfo                   sUpgradeInfo[MAX_UPGRADETYPES];
s_SpecialInfo                   sSpecialInfo[MAX_SPECIALTYPES];
s_BulletInfo        			sBulletInfo[MAX_BULLET_TYPES];
s_ParticleInfo        			sParticleInfo[MAX_PARTICLE_TYPES];

// datafile(s)
std::shared_ptr<Graphics> gfxdata;		// graphics (terrain, units, structures)
std::shared_ptr<Graphics> gfxinter;		// interface graphics
std::shared_ptr<Graphics> gfxworld;		// world/pieces graphics
std::shared_ptr<Graphics> gfxmentat;	// mentat graphics

// FONT stuff
TTF_Font *game_font; 	// arrakeen.fon
TTF_Font *bene_font;	// benegesserit font.
TTF_Font *small_font;	// small font.
TTF_Font *gr_bene_font; // benegesserit font for size XXL

/**
	Entry point of the game
*/
int main(int argc, char **argv)
{
    std::unique_ptr<GameSettings> settings = loadSettingsFromIni("settings.ini");
    
    // read command-line arguments, can override settings.ini
    cHandleArgument handleArg;
    try {
        int parseResult = handleArg.handleArguments(argc, argv, settings.get());
        if (parseResult < 0) {
            cLogger::getInstance()->log(LOG_ERROR, COMP_GAMERULES, "HandleArgument", "Incorrect arguments");
            return 1;
        }
    }
    catch (std::runtime_error &e) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_GAMERULES, "HandleArgument", std::format("Runtime_error on parsing : {}", e.what()));
        std::cerr << std::format("Error: {}\n\n", e.what());
        return 1;
    }

    game.applySettings(settings.get());

    try {
        if (game.setupGame()) {
            game.run();
        }

        game.shutdown();
    }
    catch (std::runtime_error &e) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_NONE, "Unknown", std::format("Error: {}", e.what()));
        std::cerr << std::format("Error: {}\n\n", e.what());
    }

    std::cout << std::format("Thank you for playing Dune 2 - The Maker\n");
    return 0;
}