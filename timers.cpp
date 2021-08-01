#include "include/d2tmh.h"

/**
	Timers lib specific implementations
**/

#ifdef ALLEGRO_H
	/** FPS timer **/
	void allegro_timerseconds() {
		allegro_timerSecond++;
	}
	END_OF_FUNCTION(allegro_timerseconds);

	/** Global timer **/
	void allegro_timergametime() {
		allegro_timerGlobal++;
	}
	END_OF_FUNCTION(allegro_timergametime);

	/** Unit timer **/
	void allegro_timerunits() {
		allegro_timerUnits++;
	}
	END_OF_FUNCTION(allegro_timerunits);
#else
	// Theoretically some other library could be used and have timer specific code here...
#endif
