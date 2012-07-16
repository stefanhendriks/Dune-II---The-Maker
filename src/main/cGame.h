/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */
#ifndef GAME_H_H
#define GAME_H_H

#include <string>

#include "domain/Version.h"

#include "domain/ScreenResolution.h"

#include "include/GameState.h"

#include "states/State.h"


class cGame {

	public:
																																																																																																																																										
		cGame(State * state, Version * version);
		~cGame();

		std::string rules;

		bool windowed;

		Version * version;
		int revision;

		// init functions
		bool setupGame();
		void init();

		// main run function
		void run();

		void shutdown();

		State * getState() { return state; }

		void setGameRules(std::string filename) {
			this->rules = filename;
		}

	private:

		// variables
		GameState gameStateEnum;
		State * state;
		ScreenResolution * screenResolution;

};

#endif
