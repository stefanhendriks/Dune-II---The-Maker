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
    int getWaitingTime() const { return m_waitingTime; }
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
    uint16_t getGlobalSpeed() const { return m_timerGameTime.tickDuration; }
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

    // freeze time when focus is lost, and catch up when focus is regained
    void onWindowFocusLost();
    // catch up time when focus is regained
    void onWindowFocusGained();

private:
    // gametime timer is called every 5 ms, try to keep up with that.
    void handleTimerFast();
    // gametime timer is called every 100 ms, try to keep up with that.
    void handleTimerNormal();
    // gametime timer is called every 1000 ms, try to keep up with that.
    void handleTimerSlow();
    // system capping to avoid extremely high timers
    void capTimers();
    // start every 60000 ms
    void handleTimerCache();
    // reset or set the tick duration based on the global speed
    void initTimers(int baseSpeed);

    cGame *m_game;

    struct Timer {
        int count = 0;                  // number of cycles to process
        uint64_t lastTick = 0;          // last tick processed
        uint64_t tickDuration = 0;      // duration of a tick (in ms)
    };

    Timer m_timerUnits;
    Timer m_timerGameTime;
    Timer m_timerSecond;
    Timer m_timerMinute;

    int m_gameTime = 0;         // Definition of game time (= in seconds)
    int m_fps = 0;			    // Frames per second
    int m_frameCount = 0;	    // Frame count for FPS calculation
    int m_waitingTime = 10;	    // Waiting time in ms, used to adapt FPS
    int m_focusLostTime = 0;    // Time when focus was lost, used to calculate time to catch up when focus is regained

    std::unique_ptr<cTimeCounter> m_timeCounter;
};