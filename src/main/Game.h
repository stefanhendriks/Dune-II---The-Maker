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

#include "game/Version.h"

#include "game/states/State.h"

class Game {

	public:
																																																																																																																																										
		Game(State * state, Version * version);
		~Game();
		
		void run();

		void shutdown();

		State * getState() { return state; }

		void setGameRules(std::string filename) {
			this->rules = filename;
		}

	private:

		State * state;

		std::string rules;
		bool windowed;
		Version * version;
};

#endif
