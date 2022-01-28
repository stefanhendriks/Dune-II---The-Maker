#include "timers.h"

#include <allegro/base.h>
/**
	Timers lib specific implementations
**/

/** FPS timer **/
void allegro_timerseconds() {
  allegro_timerSecond = allegro_timerSecond + 1;
}
END_OF_FUNCTION(allegro_timerseconds);

/** Global timer **/
void allegro_timergametime() {
  allegro_timerGlobal = allegro_timerGlobal + 1;
}
END_OF_FUNCTION(allegro_timergametime);

/** Unit timer **/
void allegro_timerunits() {
  allegro_timerUnits = allegro_timerUnits + 1;
}
END_OF_FUNCTION(allegro_timerunits);
