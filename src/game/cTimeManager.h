/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */


#pragma once
#include <stdint.h>
#include <string>
#include <memory>

class cGame;
class cTimeCounter;

/**
  Time management is done in this class
*/
class cTimeManager {
public:
    explicit cTimeManager(cGame *game);
    void processTime();

    // returns the current FPS
    int getFps() const;
    // return time to wait for CPU
    int getWaitingTime() const { return waitingTime; }
    // wait for CPU to catch up
    void waitForCPU();
    // get after 1s the FPS
    void capFps();
    // adapt waiting time based on FPS
    void adaptWaitingTime();
    // change global speed
    void setGlobalSpeed(int speed);
    void setGlobalSpeedVariation(int variation);
    // get global speed
    uint16_t getGlobalSpeed() const { return durationTime.fastTickDuration; }
    // get current local Time
    std::string getCurrentTime() const;
    // returns the stored timer value on time format
    std::string getCurrentTimer() const;

    // start timer control
    void startTimer();
    // take a pause
    void pauseTimer();
    // restart Timer
    void restartTimer();

private:
    // gametime timer is called every 5 ms, try to keep up with that.
    void handleTimerFast();
    // gametime timer is called every 100 ms, try to keep up with that.
    void handleTimerNormal();
    // gametime timer is called every 1000 ms, try to keep up with that.
    void handleTimerSlow();
    // system capping to avoid extremely high timers
    void capTimers();
    // called every X s
    void handleTimerCache();

    cGame *m_game;
    int m_timerFast;
    int m_timerNormal;
    int m_timerSlow;
    int m_timerCache;
    int m_gameTime;		/** Definition of game time (= in seconds) **/

    int m_fps = 0;			/** Frames per second **/
    int frameCount = 0;		/** Frame count for FPS calculation **/
    int waitingTime = 10;	/** Waiting time in ms, used to adapt FPS **/
    uint64_t m_lastNormalTick = 0;
    uint64_t m_lastFastTick = 0;
    uint64_t m_lastSlowTick = 0;

    struct DurationTime {
        uint64_t fastTickDuration;  // 5
        uint64_t normalTickDuration;  // 100
        uint64_t slowTickDuration; // 1000

        void init(int value) {
            fastTickDuration = value;
            normalTickDuration = value * 20;
            slowTickDuration = value * 200;
        }
    };

    std::unique_ptr<cTimeCounter> m_timeCounter;
    DurationTime durationTime;
};
