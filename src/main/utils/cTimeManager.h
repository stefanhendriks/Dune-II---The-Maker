/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

/**
 Time management is done in this class
 */

#ifndef CTIMEMANAGER_H
#define CTIMEMANAGER_H

class cTimeManager {

	private:

		int timerUnits; /** !!Specificly!! used for units **/
		int timerSecond;
		int timerGlobal;

		void handleTimerUnits();
		void handleTimerFPS();
		void handleTimerGlobal();

		void capTimers();

		/** Allegro specific routines to handle timers **/
		void syncFromAllegroTimers();
		void syncAllegroTimers();

	public:

		cTimeManager();

		int gameTime; /** Definition of game time (= in seconds) **/

		void processTime();

		/** This function sets the timers; its an ugly hack but Allegro does not
		 like to directly call the increase.. functions (you need to lock the
		 entire thing which is not what we want. */

		void increaseTimerForUnits();
		void increaseTimerForFPS();
		void increaseTimerGlobal();
};

#endif
