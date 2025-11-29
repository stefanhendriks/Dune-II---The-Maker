#include "game/cGame.h"
#include "game/cTimeManager.h"
#include "utils/cSoundPlayer.h"
#include "utils/cLog.h"
#include "game/cTimeCounter.h"

#include <format>
#include <chrono>
#include <SDL2/SDL_timer.h>
#include <algorithm>

constexpr int IDEAL_FPS = 60; // ideal frames per second


cTimeManager::cTimeManager(cGame *game)
    : m_game(game)
    , m_timerFast(0)
    , m_timerNormal(0)
    , m_timerSlow(0)
    , m_timerCache(0)
    , m_gameTime(0)
{
    // we fix time to 5 100 1000
    durationTime.init(5);
    m_timeCounter = std::make_unique<cTimeCounter>();
}

std::string cTimeManager::getCurrentTime() const
{
    auto now_utc = std::chrono::system_clock::now();
    auto now_local = std::chrono::zoned_time(std::chrono::current_zone(), now_utc);
    auto local_time_seconds = std::chrono::time_point_cast<std::chrono::seconds>(now_local.get_local_time());
    auto time_of_day = std::chrono::hh_mm_ss{local_time_seconds.time_since_epoch()};
    return std::format("{:02}:{:02}:{:02}",
                       time_of_day.hours().count()%24,
                       time_of_day.minutes().count(),
                       time_of_day.seconds().count());
}

std::string cTimeManager::getCurrentTimer() const
{
    auto duration = std::chrono::seconds(m_timeCounter->getTime());
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration - hours - minutes);
    return std::format("{:02}:{:02}:{:02}",
                       hours.count(),
                       minutes.count(),
                       seconds.count());
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

    if (m_timerNormal > 10) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high unit timer ({}); capped at 10", m_timerNormal));
            m_timerNormal = 10;
        }
    }

    if (m_timerFast > 40) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high global timer ({}); capped at 40", m_timerFast));
            m_timerFast = 40;
        }
    }

    /* Taking 10 seconds to render a frame? i hope not **/
    if (m_timerSlow > 10) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high timer second ({}); capped at 10", m_timerSlow));
            m_timerSlow = 10;
        }
    }
}

void cTimeManager::handleTimerFast()
{
    // keep up with time cycles
    while (m_timerFast > 0) {
        m_game->thinkFast_fading();
        m_game->thinkFast_state();

        m_timerFast--;
    }
}

void cTimeManager::handleTimerNormal()
{
    while (m_timerNormal > 0) {
        m_game->thinkNormal_state();
        m_timerNormal--;
    }
}

void cTimeManager::handleTimerSlow()
{
    while (m_timerSlow > 0) {
        m_gameTime++;
        m_game->thinkSlow();
        m_timerSlow--; // done!
        // more practice than ask every tick or frame
        m_timerCache++;
        m_timeCounter->addTime(durationTime.slowTickDuration/1000);
    }
}

void cTimeManager::handleTimerCache()
{
    if (m_timerCache > 30) {
        m_game->thinkCache();
        m_timerCache = 0;
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
    // syncFromAllegroTimers();
    uint64_t now = SDL_GetTicks64();

    // 5 ms pour allegro_timergametime
    while (now - m_lastFastTick >= durationTime.fastTickDuration) {
        m_timerFast++;
        m_lastFastTick += durationTime.fastTickDuration;
    }

    // 100 ms pour allegro_timerunits
    while (now - m_lastNormalTick >= durationTime.normalTickDuration) {
        m_timerNormal++;
        m_lastNormalTick += durationTime.normalTickDuration;
    }

    // 1000 ms pour allegro_timerseconds
    while (now - m_lastSlowTick >= durationTime.slowTickDuration) {
        m_timerSlow++;
        m_lastSlowTick += durationTime.slowTickDuration;
    }

    capTimers();
    handleTimerSlow();
    handleTimerNormal();
    handleTimerFast();
    handleTimerCache();
}

int cTimeManager::getFps() const
{
    return m_fps;
}

void cTimeManager::waitForCPU()
{
    if (waitingTime > 0) {
        SDL_Delay(waitingTime);
    }
    frameCount++;
}

void cTimeManager::capFps()
{
    m_fps = frameCount;
    frameCount = 0;
}

void cTimeManager::adaptWaitingTime()
{
    if (m_fps > IDEAL_FPS) {
        waitingTime += 1;
    } else {
        waitingTime -= 1;
        if (waitingTime < 1) {
            waitingTime = 1; // never wait less than 1 ms
        }
    }
}

void cTimeManager::setGlobalSpeed(int speed)
{
    speed = std::clamp(speed, 1, 10);
    durationTime.init(speed);
}

void cTimeManager::startTimer()
{
    m_timeCounter->start();
}

void cTimeManager::pauseTimer()
{
    m_timeCounter->pause();
}

void cTimeManager::restartTimer()
{
    m_timeCounter->restart();
}
void cTimeManager::setGlobalSpeedVariation(int variation)
{
    int speed = durationTime.fastTickDuration;
    if (variation > 0)
        speed += 1;
    if (variation < 0)
        speed -= 1;
    setGlobalSpeed(speed);
}
