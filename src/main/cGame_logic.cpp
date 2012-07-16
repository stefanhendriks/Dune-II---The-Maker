/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2012 (c) code by Stefan Hendriks

 */

#include <string.h>

#include "cGame.h"

#include "states/MainMenuState.h"

#include "utils/FileReader.h"
#include "utils/StringUtils.h"

#include "include/allegroh.h"

using namespace std;

cGame::cGame(State * state, Version * version) {
	this->state = state;
	this->version = version;
	windowed = true;
	gameStateEnum = MAINMENU;
}

cGame::~cGame() {
	delete screenResolution;
	screenResolution = NULL;
	delete state;
	state = NULL;
	delete version;
	version = NULL;
}

/**
 Main game loop
 */
void cGame::run() {
	set_trans_blender(0, 0, 0, 128);
	while (!state->shouldQuitGame()) {
		state->run();
	}
}

/**
 Shutdown the game
 */
void cGame::shutdown() {
	Logger *logger = Logger::getInstance();
	logger->logHeader("SHUTDOWN");
	logger->debug("Starting shutdown");

// 	destroy_bitmap(bmp_fadeout);
// 	destroy_bitmap(bmp_screen);
// 	destroy_bitmap(bmp_throttle);
// 	destroy_bitmap(bmp_winlose);
// 	logger->debug("Destroyed bitmaps");

	// Destroy font of Allegro FONT library
	// this somehow seems to crash!
// 	if (game_font != NULL) alfont_destroy_font(game_font);
// 	if (bene_font != NULL) alfont_destroy_font(bene_font);
	logger->debug("Destroyed fonts");

	// Exit the font library (must be first)
 	alfont_exit();
	logger->debug("Allegro FONT library shut down.");

// 	if (mp3_music != NULL) {
// 		almp3_stop_autopoll_mp3(mp3_music);
// 		almp3_destroy_mp3(mp3_music);
// 	}

	logger->debug("Allegro MP3 library shut down.");

	// Now we are all neatly closed, we exit Allegro and return to the OS
	allegro_exit();
	logger->debug("Allegro shut down.");
	logger->debug("Thanks for playing.");
}