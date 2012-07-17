/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2012 (c) code by Stefan Hendriks

 */

#include <string.h>

#include "Game.h"

#include "states/MainMenuState.h"

#include "utils/FileReader.h"
#include "utils/StringUtils.h"

#include "include/allegroh.h"

using namespace std;

Game::Game(State * state, Version * version) {
	this->state = state;
	this->version = version;
	windowed = true;
	gameStateEnum = MAINMENU;
}

Game::~Game() {
}

void Game::run() {
	set_trans_blender(0, 0, 0, 128);
	while (!state->shouldQuitGame()) {
		state->run();
	}
}

void Game::shutdown() {

}