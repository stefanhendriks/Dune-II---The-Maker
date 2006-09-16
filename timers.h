#ifndef TIMERS_H

/**
	Timers lib specific definitions
**/

#ifdef ALLEGRO_H

	void allegro_timerfps();	/** FPS timer **/
	void allegro_timerglobal(); /** Global timer **/	
	void allegro_timerunits();	/** Unit timer **/

	extern volatile int allegro_timerSecond;
	extern volatile int allegro_timerGlobal;
	extern volatile int allegro_timerUnits;

#else 
	// Here comes any other lib, eventually the 3 TimerManager functions
	// should be called to make this thing work
#endif


#endif