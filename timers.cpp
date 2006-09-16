#include "d2tmh.h"

/**
	Timers lib specific implementations
**/

#ifdef ALLEGRO_H
	/** FPS timer **/
	void allegro_timerfps() {
		allegro_timerSecond++;
	}
	END_OF_FUNCTION(allegro_timerfps);

	/** Global timer **/
	void allegro_timerglobal() {
		allegro_timerGlobal++;
	}
	END_OF_FUNCTION(allegro_timerglobal);

	/** Unit timer **/
	void allegro_timerunits() {
		allegro_timerUnits++;
	}
	END_OF_FUNCTION(allegro_timerunits);
#else 
	// Here comes any other lib, eventually the 3 TimerManager functions
	// should be called to make this thing work
#endif
