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

#include "game/cGame.h"


#include <memory>

class SDLDrawer;
class Graphics;
class cGame;

struct SDL_Surface;


// game
extern cGame		  game;

extern SDLDrawer      *global_renderDrawer;
extern std::shared_ptr<Graphics> gfxdata;
