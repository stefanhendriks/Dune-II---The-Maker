#include "game/cGame.h"
#include "game/cTimeManager.h"
#include "utils/cSoundPlayer.h"
#include "utils/cLog.h"
#include "game/cTimeCounter.h"

#include <format>
#include <chrono>
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <platform.h>
#include <cassert>

constexpr int IDEAL_FPS = 60; // ideal frames per second


cTimeManager::cTimeManager(cGame *game)
    : m_game(game)
    , m_gameTime(0)
{
    assert(game != nullptr);
    // we fix time to 5 100 1000
    initTimers(5);
    m_timeCounter = std::make_unique<cTimeCounter>();
}

std::string cTimeManager::getCurrentTime() const
{
#if D2TM_CLANG
    auto duration = std::chrono::seconds(m_timeCounter->getTime());
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration - hours - minutes);
    return std::format("{:02}:{:02}:{:02}",
                       hours.count(),
                       minutes.count(),
                       seconds.count());
#else
    auto now_utc = std::chrono::system_clock::now();
    auto now_local = std::chrono::zoned_time(std::chrono::current_zone(), now_utc);
    auto local_time_seconds = std::chrono::time_point_cast<std::chrono::seconds>(now_local.get_local_time());
    auto time_of_day = std::chrono::hh_mm_ss{local_time_seconds.time_since_epoch()};
    return std::format("{:02}:{:02}:{:02}",
                       time_of_day.hours().count()%24,
                       time_of_day.minutes().count(),
                       time_of_day.seconds().count());
#endif
}

std::string cTimeManager::getCurrentTimer() const
{
#if D2TM_CLANG
    auto now = std::chrono::system_clock::now();
    std::time_t now_tt = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm;
    localtime_r(&now_tt, &local_tm);

    return std::format("{:02}:{:02}:{:02}",
                       local_tm.tm_hour,
                       local_tm.tm_min,
                       local_tm.tm_sec);
#else
    auto total_seconds = m_timeCounter->getTime();

    int hours = static_cast<int>(total_seconds / 3600);
    int minutes = static_cast<int>((total_seconds % 3600) / 60);
    int seconds = static_cast<int>(total_seconds % 60);

    return std::format("{:02}:{:02}:{:02}", hours, minutes, seconds);
#endif
}

void cTimeManager::onWindowFocusLost()
{
    m_focusLostTime = SDL_GetTicks();
}

void cTimeManager::onWindowFocusGained()
{
    if (m_focusLostTime > 0) {
        int lostTime = SDL_GetTicks() - m_focusLostTime;
        // std::cout << "Focus regained, lost time: " << lostTime << " ms" << std::endl;
        m_focusLostTime = 0;

        // std::cout << "Before : " << m_timerGameTime.lastTick << std::endl;
        m_timerGameTime.lastTick += lostTime;
        m_timerUnits.lastTick += lostTime;
        m_timerSecond.lastTick += lostTime;
        m_timerMinute.lastTick += lostTime;
        // std::cout << "After : " << m_timerGameTime.lastTick << std::endl;
        // std::cout << "yet : " << SDL_GetTicks() << std::endl;
    }
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

    if (m_timerUnits.count > 10) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high unit timer ({}); capped at 10", m_timerUnits.count));
            m_timerUnits.count = 10;
        }
    }

    if (m_timerGameTime.count > 40) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high global timer ({}); capped at 40", m_timerGameTime.count));
            m_timerGameTime.count = 40;
        }
    }

    /* Taking 10 seconds to render a frame? i hope not **/
    if (m_timerSecond.count > 10) {
        if (m_game->isDebugMode()) {
            logger->log(LOG_WARN, COMP_NONE, "Timer", std::format("WARNING: Exeptional high timer second ({}); capped at 10", m_timerSecond.count));
            m_timerSecond.count = 10;
        }
    }
}

/**
 * timerseconds timer is called every 1000 ms, try to keep up with that.
 */
void cTimeManager::handleTimerSlow()
{
    while (m_timerSecond.count > 0) {
        m_gameTime++;
        m_game->thinkSlow();
        m_timerSecond.count--;
        m_timeCounter->addTime(m_timerSecond.tickDuration / 1000);
    }
}

/**
 * gametime timer is called every 5 ms, try to keep up with that.
 */
void cTimeManager::handleTimerFast()
{
    while (m_timerGameTime.count > 0) {
        m_game->thinkFast();
        m_timerGameTime.count--;
    }
}

/**
 * units timer is called every 100 ms, try to keep up with that.
 */
void cTimeManager::handleTimerNormal()
{
    while (m_timerUnits.count > 0) {
        m_game->thinkNormal();
        m_timerUnits.count--;
    }
}

void cTimeManager::handleTimerCache()
{
    while (m_timerMinute.count > 0) {
        m_game->thinkCache();
        m_timerMinute.count--;
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
    uint64_t now = SDL_GetTicks64();

    while (now - m_timerUnits.lastTick >= m_timerUnits.tickDuration) {
        m_timerUnits.count++;
        m_timerUnits.lastTick += m_timerUnits.tickDuration;
    }

    while (now - m_timerGameTime.lastTick >= m_timerGameTime.tickDuration) {
        m_timerGameTime.count++;
        m_timerGameTime.lastTick += m_timerGameTime.tickDuration;
    }

    while (now - m_timerSecond.lastTick >= m_timerSecond.tickDuration) {
        m_timerSecond.count++;
        m_timerSecond.lastTick += m_timerSecond.tickDuration;
    }

    while (now - m_timerMinute.lastTick >= m_timerMinute.tickDuration) {
        m_timerMinute.count++;
        m_timerMinute.lastTick += m_timerMinute.tickDuration;
    }

    capTimers();
    handleTimerFast();
    handleTimerNormal();
    handleTimerSlow();
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
    initTimers(speed);
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
    int speed = m_timerGameTime.tickDuration;
    if (variation > 0)
        speed += 1;
    if (variation < 0)
        speed -= 1;
    setGlobalSpeed(speed);
}

void cTimeManager::initTimers(int baseSpeed) {
    m_timerGameTime.tickDuration = baseSpeed;
    m_timerUnits.tickDuration = baseSpeed * 20;
    m_timerSecond.tickDuration = baseSpeed * 200;
    m_timerMinute.tickDuration = 60000;
}
