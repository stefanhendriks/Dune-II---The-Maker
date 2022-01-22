#include "../include/d2tmh.h"

#include "timers.h"
#include "utils/cSoundPlayer.h"

cTimeManager::cTimeManager() {
    timerUnits = 0;
    timerGlobal = 0;
    timerSecond = 0;

    gameTime = 0;
}

/**
	In case the system locks up, or the computer is on heavy duty. The capping
	makes sure the computer will not cause a chainreaction (getting extremely high timers
	and doing a lot of loops, sucking mure cpu power).

	In most coses this is not nescesary.
**/

void cTimeManager::capTimers() {
    if (timerUnits > 10) {
        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "WARNING: Exeptional high unit timer (%d); capped at 10", timerUnits);
            logbook(msg);
            timerUnits = 10;
        }
    }

    if (timerGlobal > 40) {
        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "WARNING: Exeptional high global timer (%d); capped at 40", timerGlobal);
            logbook(msg);
            timerGlobal = 40;
        }
    }

    /* Taking 10 seconds to render a frame? i hope not **/
    if (timerSecond > 10) {
        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "WARNING: Exeptional high timer second (%d); capped at 40", timerSecond);
            logbook(msg);
            timerSecond = 10;
        }
    }
}

/**
 * timerseconds timer is called every 1000 ms, try to keep up with that.
 */
void cTimeManager::handleTimerAllegroTimerSeconds() {
    while (timerSecond > 0) {
        gameTime++;
        game.thinkSlow();
        timerSecond--; // done!
    }

}

/**
 * gametime timer is called every 5 ms, try to keep up with that.
 */
void cTimeManager::handleTimerGameTime() {
    // keep up with time cycles
    while (timerGlobal > 0) {
        game.think_fading();
        game.thinkFast_state();

        timerGlobal--;
    }
}

/**
 * units timer is called every 100 ms, try to keep up with that.
 */
void cTimeManager::handleTimerUnits() {
    while (timerUnits > 0) {
        game.think_state();
        timerUnits--;
    }
}

/**
	This function is called by the game class in the run() function.

	It is important that this function is called first, as it will make sure
	the timers state is updated (since time passed between the previous
	and the current frame), therefore any think() function is relying on the data
	that eventually is set by this function.

    Do note this, is basically just an "update" call, but takes amount of update cycles into account, so we make sure
    we do not skip or get sloppy with update loops. Although, the entire 'update' in this game is a bit hairy, you would
    expect this function to be called by the main update function. Heck, even per state this would be different - which
    is now not the case.
*/
void cTimeManager::processTime() {
#ifdef ALLEGRO_H
    syncFromAllegroTimers();
#endif

    capTimers();
    handleTimerAllegroTimerSeconds();
    handleTimerUnits();
    handleTimerGameTime();

#ifdef ALLEGRO_H
    syncAllegroTimers();
#endif
}

/** allegro specific routine **/
void cTimeManager::syncFromAllegroTimers() {
    timerSecond = allegro_timerSecond;
    timerGlobal = allegro_timerGlobal;
    timerUnits = allegro_timerUnits;
}

/** allegro specific routine **/
void cTimeManager::syncAllegroTimers() {
    allegro_timerSecond = timerSecond;
    allegro_timerGlobal = timerGlobal;
    allegro_timerUnits = timerUnits;
}
