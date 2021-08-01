/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#ifndef TIMERS_H
	void allegro_timerseconds();	/** FPS timer **/
	void allegro_timergametime(); /** Global timer **/
	void allegro_timerunits();	/** Unit timer **/

	extern volatile int allegro_timerSecond;
	extern volatile int allegro_timerGlobal;
	extern volatile int allegro_timerUnits;
#endif
