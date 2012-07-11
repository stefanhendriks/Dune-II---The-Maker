/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */
#ifndef GAME_H_H
#define GAME_H_H

#include "movie/cMoviePlayer.h"
#include "utils/cSoundPlayer.h"

#include "gameobjects/ScreenResolution.h"

#include "include/GameState.h"

#include "states/State.h"

class cGame {

	public:

		cGame();
		~cGame();

		std::string game_filename;

		bool windowed;
		char version[15];
		char revision[15];

		// init functions
		bool setupGame();
		void init();

		// main run function
		void run();

		// get / set
		cSoundPlayer *getSoundPlayer() {
			return soundPlayer;
		}

		cMoviePlayer *getMoviePlayer() {
			return moviePlayer;
		}

		void setMoviePlayer(cMoviePlayer * value) {
			moviePlayer = value;
		}

		ScreenResolution * getScreenResolution() {
			return screenResolution;
		}

		void setScreenResolution(ScreenResolution * newScreenResolution) {
			if (screenResolution) {
				delete screenResolution;
			}
			if (newScreenResolution) {
				screenResolution = newScreenResolution;
			} else {
				screenResolution = NULL;
			}
		}

		ScreenResolution *getScreenResolutionFromIni() {
			return screenResolutionFromIni;
		}

		void setScreenResolutionFromIni(ScreenResolution * newScreenResolution) {
			screenResolutionFromIni = newScreenResolution;
		}

		void shutdown();

		State * getState() { return state; }

	private:
		// variables
		GameState gameStateEnum;
		State * state;

		ScreenResolution * screenResolution;
		ScreenResolution * screenResolutionFromIni;

		cSoundPlayer * soundPlayer;
		cMoviePlayer * moviePlayer;

};

#endif
