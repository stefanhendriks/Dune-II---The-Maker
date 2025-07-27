#include "cTimeManager.h"

#include "cGame.h"
// #include "timers.h"
#include "utils/cSoundPlayer.h"
#include "utils/cLog.h"

#include <format>
#include <SDL2/SDL_timer.h>
cTimeManager::cTimeManager(cGame *game)
    : m_timerUnits(0)
    , m_timerSecond(0)
    , m_timerGlobal(0)
    , m_game(game)
    , m_gameTime(0)
{
}

/**
	In case the system locks up, or the computer is on heavy duty. The capping
	makes sure the computer will not cause a chainreaction (getting extremely high timers
	and doing a lot of loops, sucking mure cpu power).

	In most cases this is not nescesary.
**/
void cTimeManager::capTimers()
{
    auto logger = cLogger::getInstance();

    if (m_timerUnits > 10) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high unit timer ({}); capped at 10", m_timerUnits));
            m_timerUnits = 10;
        }
    }

    if (m_timerGlobal > 40) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high global timer ({}); capped at 40", m_timerGlobal));
            m_timerGlobal = 40;
        }
    }

    /* Taking 10 seconds to render a frame? i hope not **/
    if (m_timerSecond > 10) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high timer second ({}); capped at 10", m_timerSecond));
            m_timerSecond = 10;
        }
    }
}

/**
 * timerseconds timer is called every 1000 ms, try to keep up with that.
 */
void cTimeManager::handleTimerAllegroTimerSeconds()
{
    while (m_timerSecond > 0) {
        m_gameTime++;
        m_game->thinkSlow();
        m_timerSecond--; // done!
    }

}

/**
 * gametime timer is called every 5 ms, try to keep up with that.
 */
void cTimeManager::handleTimerGameTime()
{
    // keep up with time cycles
    while (m_timerGlobal > 0) {
        m_game->think_fading();
        m_game->thinkFast_state();

        m_timerGlobal--;
    }
}

/**
 * units timer is called every 100 ms, try to keep up with that.
 */
void cTimeManager::handleTimerUnits()
{
    while (m_timerUnits > 0) {
        m_game->think_state();
        m_timerUnits--;
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
void cTimeManager::processTime()
{
//    syncFromAllegroTimers();
    uint64_t now = SDL_GetTicks64();

    // 100 ms pour allegro_timerunits
    while (now - m_lastUnitsTick >= 100) {
        m_timerUnits++;
        m_lastUnitsTick += 100;
    }

    // 5 ms pour allegro_timergametime
    while (now - m_lastGameTimeTick >= 5) {
        m_timerGlobal++;
        m_lastGameTimeTick += 5;
    }

    // 1000 ms pour allegro_timerseconds
    while (now - m_lastSecondsTick >= 1000) {
        m_timerSecond++;
        m_lastSecondsTick += 1000;
    }
    capTimers();
    handleTimerAllegroTimerSeconds();
    handleTimerUnits();
    handleTimerGameTime();
}